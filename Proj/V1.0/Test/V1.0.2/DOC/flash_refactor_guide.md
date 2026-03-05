# Flash参数存储架构重构指南

## 📋 概述

本次重构将Flash操作分为**驱动层**和**应用层**，实现职责分离：
- **驱动层（bsp_flash）**：通用Flash读写擦除，无业务限制
- **应用层（task_flash）**：参数存储策略（双页备份、CRC校验、版本管理）

---

## 🏗️ 架构设计

### 分层架构图
```
┌─────────────────────────────────────────────────┐
│         应用层（task_flash）                     │
│  - 双页Ping-Pong存储                             │
│  - Magic + CRC32校验                             │
│  - 序列号版本管理                                │
│  - 参数加载/保存策略                             │
└─────────────────┬───────────────────────────────┘
                  │ 调用
┌─────────────────▼───────────────────────────────┐
│         驱动层（bsp_flash）                      │
│  - 擦除任意页                                    │
│  - 写入任意地址（8字节对齐）                     │
│  - 读取Flash                                     │
│  - CRC32工具                                     │
│  - 写后校验                                      │
└─────────────────────────────────────────────────┘
```

---

## 🔧 驱动层 API（bsp_flash）

### 1. 擦除页
```c
bool BSP_Flash_ErasePage(uint32_t page_index);
```
- **功能**：擦除指定页（通用接口，无业务限制）
- **参数**：`page_index` - 页号（0-63）
- **返回**：`true`=成功, `false`=失败
- **注意**：调用者需确保页号有效（如避免擦除程序区）

### 2. 写入数据
```c
bool BSP_Flash_Write(uint32_t addr, const uint8_t *data, uint32_t len);
```
- **功能**：写入数据到Flash
- **参数**：
  - `addr` - 目标地址（**必须8字节对齐**）
  - `data` - 源数据缓冲区
  - `len` - 数据长度（自动补齐到8字节倍数）
- **返回**：`true`=成功, `false`=失败

### 3. 读取数据
```c
void BSP_Flash_Read(uint32_t addr, uint8_t *data, uint32_t len);
```
- **功能**：从Flash读取数据
- **参数**：标准读取参数

### 4. 写后校验
```c
bool BSP_Flash_Verify(uint32_t addr, const uint8_t *data, uint32_t len);
```
- **功能**：验证Flash内容与期望数据是否一致
- **推荐**：每次写入后调用，确保数据完整性

### 5. CRC32校验
```c
uint32_t BSP_Flash_CRC32(const uint8_t *data, uint32_t len);
```
- **功能**：计算标准CRC32校验和

---

## 📦 应用层 API（task_flash）

### 核心接口（4个）

#### 1. 参数初始化
```c
void Param_Init(void);
```
- **功能**：上电时调用，从Flash加载参数到RAM
- **策略**：
  - 从A/B页读取记录
  - 使用Magic + CRC32校验有效性
  - 两页都有效 → 选序列号更大的
  - 都无效 → 使用默认参数（`PARAM_CONFIG_USER`）
- **调用时机**：`main()`中的`Task_Init()`

#### 2. 获取参数（只读）
```c
const param_config_t* Param_Get(void);
```
- **功能**：获取当前参数指针
- **返回**：指向`g_param_cfg`的常量指针
- **用法**：
  ```c
  const param_config_t *cfg = Param_Get();
  uint16_t v_set = cfg->app.v_set;
  ```

#### 3. 设置参数
```c
void Param_Set(const param_config_t *cfg);
```
- **功能**：整体覆盖当前参数（仅修改RAM）
- **注意**：需手动调用`Param_Save()`才写入Flash
- **用法**：
  ```c
  param_config_t new_cfg = *Param_Get();
  new_cfg.app.v_set = 450;
  Param_Set(&new_cfg);
  Param_Save();  // 保存到Flash
  ```

#### 4. 保存参数
```c
bool Param_Save(void);
```
- **功能**：保存当前参数到Flash
- **返回**：`true`=成功, `false`=失败
- **机制**：
  - 自动Ping-Pong写入另一个页
  - 递增序列号（`seq++`）
  - 写后读回校验
  - 更新内部状态

---

## 💾 存储结构

### Flash布局
```
Page 62 (0x0800F800)  ─┐
                       ├─ A/B双页备份
Page 63 (0x0800FC00)  ─┘
```

### 记录格式
```c
typedef struct {
    param_header_t   header;   // 16字节
    param_config_t   config;   // 21字节（isl700_config_t 15B + app_config_t 6B）
} param_record_t;  // 总计37字节
```

### 头部结构
```c
typedef struct {
    uint32_t magic;         // 魔数：0x48503650 ("HP6P")
    uint32_t seq;           // 序列号（递增）
    uint16_t payload_len;   // 载荷长度：21
    uint16_t reserved;      // 保留字段
    uint32_t crc32;         // payload的CRC32
} param_header_t;
```

---

## 📖 使用示例

### 示例1：上电初始化
```c
int main(void)
{
    BSP_Init();
    Task_Init();  // 内部调用 Param_Init()
    
    // 参数已加载到 g_param_cfg
    ISL700_Init(&g_param_cfg.isl700);
    
    while (1) {
        // 主循环
    }
}
```

### 示例2：读取参数
```c
void OutputTask(void)
{
    const param_config_t *cfg = Param_Get();
    uint16_t v_set = cfg->app.v_set;
    uint16_t v_hys = cfg->app.v_hys;
    
    if (V_OUT >= v_set) {
        // 物体检测逻辑
    }
}
```

### 示例3：修改并保存参数
```c
void UpdateThreshold(uint16_t new_vset)
{
    // 方式1：直接修改全局变量
    g_param_cfg.app.v_set = new_vset;
    
    // 方式2：通过Param_Set（整体覆盖）
    // param_config_t new_cfg = *Param_Get();
    // new_cfg.app.v_set = new_vset;
    // Param_Set(&new_cfg);
    
    // 保存到Flash
    if (Param_Save()) {
        printf("参数保存成功\r\n");
    } else {
        printf("参数保存失败\r\n");
    }
}
```

### 示例4：恢复出厂设置
```c
void ResetToFactory(void)
{
    Param_RestoreFactory();  // 自动保存
    printf("已恢复出厂设置\r\n");
}
```

### 示例5：上位机批量下发参数
```c
void OnReceiveParamPacket(const uint8_t *data, uint16_t len)
{
    if (len == sizeof(param_config_t)) {
        param_config_t *new_cfg = (param_config_t *)data;
        
        // 整体覆盖
        Param_Set(new_cfg);
        
        // 保存到Flash
        if (Param_Save()) {
            printf("参数更新成功\r\n");
        }
    }
}
```

---

## 🔄 Ping-Pong机制详解

### 工作原理
```
初始状态：Page A有效（seq=1）, Page B空

第1次保存：
  - 写入 Page B (seq=2)
  - 现在 A=seq1, B=seq2 ✅

第2次保存：
  - 写入 Page A (seq=3)
  - 现在 A=seq3 ✅, B=seq2

第3次保存：
  - 写入 Page B (seq=4)
  - 现在 A=seq3, B=seq4 ✅
```

### 掉电保护
```
场景1：写入中掉电
  - 目标页部分写入（Magic或CRC校验失败）
  - 上电后从另一页恢复 ✅

场景2：擦除中掉电
  - 目标页全FF（无Magic）
  - 上电后从另一页恢复 ✅

场景3：写入完成但未更新状态
  - 两页都有效，选seq更大的 ✅
```

---

## ⚙️ 配置参数

### 页号配置（task_flash.h）
```c
#define PARAM_PAGE_A    62U
#define PARAM_PAGE_B    63U
```

### 魔数配置
```c
#define PARAM_MAGIC     0x48503650UL  // "HP6P"
```

### 默认参数（param_config.h）
```c
#define PARAM_CONFIG_USER {         \
    .isl700 = ISL700_CONFIG_USER,   \
    .app    = APP_CONFIG_USER,      \
}
```

---

## 🐛 调试与测试

### 测试用例1：首次上电
```c
// 预期：Flash为空，使用默认参数
Param_Init();
assert(g_param_cfg.app.v_set == 400);
```

### 测试用例2：参数持久化
```c
// 修改参数
g_param_cfg.app.v_set = 500;
Param_Save();

// 重启后验证
Param_Init();
assert(g_param_cfg.app.v_set == 500);
```

### 测试用例3：双页冲突
```c
// 人为写入两页不同seq
write_record(PARAM_PAGE_A, &cfg1, seq=10);
write_record(PARAM_PAGE_B, &cfg2, seq=20);

// 上电应选seq=20的B页
Param_Init();
assert(memcmp(&g_param_cfg, &cfg2, sizeof(param_config_t)) == 0);
```

### 测试用例4：CRC错误
```c
// 破坏A页CRC
corrupt_page_crc(PARAM_PAGE_A);

// 上电应从B页恢复
Param_Init();
assert(is_valid_config(&g_param_cfg));
```

---

## ⚠️ 注意事项

### 驱动层
1. ✅ **地址对齐**：`BSP_Flash_Write()`必须8字节对齐
2. ✅ **页号检查**：`BSP_Flash_ErasePage()`仅检查物理范围（0-63）
3. ✅ **写后校验**：强烈建议每次写入后调用`BSP_Flash_Verify()`

### 应用层
1. ✅ **初始化顺序**：`Param_Init()`必须在使用`g_param_cfg`前调用
2. ✅ **保存时机**：修改参数后需手动调用`Param_Save()`
3. ✅ **序列号溢出**：使用有符号差值比较，支持溢出（假设不会相差>2^31）
4. ✅ **RAM修改**：可直接修改`g_param_cfg`，但需调用`Param_Save()`持久化

---

## 🔧 扩展建议

### 1. 添加单字段修改接口
```c
bool Param_SetVset(uint16_t v_set)
{
    g_param_cfg.app.v_set = v_set;
    return Param_Save();
}
```

### 2. 添加脏标记机制
```c
static bool s_param_dirty = false;

void Param_MarkDirty(void)
{
    s_param_dirty = true;
}

void Param_SaveIfDirty(void)
{
    if (s_param_dirty) {
        Param_Save();
        s_param_dirty = false;
    }
}
```

### 3. 添加参数导出/导入
```c
void Param_Export(uint8_t *buffer, uint16_t *len)
{
    memcpy(buffer, &g_param_cfg, sizeof(param_config_t));
    *len = sizeof(param_config_t);
}

bool Param_Import(const uint8_t *buffer, uint16_t len)
{
    if (len == sizeof(param_config_t)) {
        Param_Set((const param_config_t *)buffer);
        return Param_Save();
    }
    return false;
}
```

---

## 📊 性能指标

| 操作 | 时间 | 说明 |
|-----|------|------|
| `Param_Init()` | ~10ms | 读取两页 + 校验 |
| `Param_Save()` | ~50ms | 擦除 + 写入 + 校验 |
| `Param_Get()` | <1μs | 直接返回指针 |
| `Param_Set()` | <1ms | 内存拷贝 |

---

## ✅ 重构检查清单

- [x] bsp_flash驱动层：去掉业务限制（Page A/B限制）
- [x] bsp_flash驱动层：改用bool返回值
- [x] bsp_flash驱动层：添加BSP_Flash_Verify()
- [x] task_flash应用层：实现双页Ping-Pong
- [x] task_flash应用层：Magic + CRC32校验
- [x] task_flash应用层：序列号版本管理
- [x] task_flash应用层：简洁的4个API
- [x] main.c：移除g_param_cfg本地初始化
- [x] task.c：Task_Init()中调用Param_Init()
- [x] 文档：使用指南和设计说明

---

## 📝 版本历史

- **V1.0.2** (2026-02-28)
  - 重构Flash架构，分离驱动层和应用层
  - 实现双页Ping-Pong存储机制
  - 添加完整的参数管理API

---

## 🔗 相关文件

- `BSP/inc/bsp_flash.h` - 驱动层头文件
- `BSP/src/bsp_flash.c` - 驱动层实现
- `App/inc/task_flash.h` - 应用层头文件
- `App/src/task_flash.c` - 应用层实现
- `App/inc/param_config.h` - 参数结构定义
- `App/src/main.c` - 主程序
- `App/src/task.c` - 任务初始化

---

**作者**: 系统架构设计  
**日期**: 2026年2月28日
