# Flash 参数管理 - 精简版说明

## 📋 方案选择

### 单页 vs A/B 双页

| 方案 | 优点 | 缺点 | 适用场景 |
|-----|------|------|---------|
| 单页 | 代码最简单，节省 1KB Flash | 擦除/写入过程掉电会丢失所有数据 | 对掉电不敏感的场景 |
| **A/B 双页** | **任何时刻掉电都有至少一份完整数据** | 多占用 1KB Flash，代码稍复杂 | **推荐方案** |

**本实现选择：A/B 双页备份**
- 理由：掉电风险真实存在，A/B 双页只需 2KB，代码增加很少
- 实现简化：只保留 seq 序号判断新旧，无复杂状态机

---

## 🏗️ 数据结构

### 1. 参数结构体（用户可修改）

```c
typedef struct {
    uint8_t  pls;                   /* 发射脉冲幅值 */
    uint16_t fine_gain;             /* 细调增益 */
    uint8_t  coarse_gain1;          /* 粗调增益1 */
    uint8_t  work_mode;             /* 工作模式 */
    uint8_t  sample_rate;           /* 采样率 */
    uint16_t threshold_high;        /* 高阈值 */
    uint16_t threshold_low;         /* 低阈值 */
} sensor_param_t;  /* 11 字节 */
```

### 2. NVM 存储记录

```c
typedef struct {
    uint32_t magic;                 /* 0x5A5AA55A */
    uint32_t seq;                   /* 序号，越大越新 */
    sensor_param_t param;           /* 参数数据 (11 字节) */
    uint32_t crc32;                 /* CRC 校验 */
    uint8_t  padding[5];            /* 补齐到 24 字节 */
} nvm_record_t;  /* 24 字节，8 的倍数 */
```

**注意：** 新增/删减 `sensor_param_t` 字段时，调整 `padding` 大小保证总大小是 8 的倍数。

---

## 🚀 API 使用

### 4 个精简 API

| 函数 | 功能 | 返回值 |
|-----|------|--------|
| `nvm_param_init()` | 初始化（上电加载或使用默认值） | 0: 成功, -1: 失败 |
| `nvm_param_get(param)` | 读取当前参数到 `param` | 0: 成功, -1: 失败 |
| `nvm_param_set(param)` | 保存参数到 Flash | 0: 成功, -1: 失败 |
| `nvm_param_restore_default()` | 恢复出厂默认值 | 0: 成功, -1: 失败 |

---

## 💻 使用示例

```c
#include "nvm_param_simple.h"

int main(void)
{
    sensor_param_t param;
    
    /* 1. 初始化（上电加载） */
    nvm_param_init();
    
    /* 2. 读取当前参数 */
    nvm_param_get(&param);
    printf("pls=%u, fine_gain=%u\n", param.pls, param.fine_gain);
    
    /* 3. 修改参数并保存 */
    param.pls = 9;
    param.fine_gain = 600;
    nvm_param_set(&param);
    
    /* 4. 恢复出厂默认（可选） */
    // nvm_param_restore_default();
    
    while(1) {
        /* 主循环 */
    }
}
```

---

## 🔧 如何新增/修改参数

### 步骤：

1. **修改 `sensor_param_t`**
   ```c
   typedef struct {
       // ... 原有字段 ...
       uint8_t new_field;   /* 新增字段 */
   } sensor_param_t;
   ```

2. **调整 `nvm_record_t` 的 `padding`**
   - 计算总大小：`sizeof(magic) + sizeof(seq) + sizeof(param) + sizeof(crc32) + sizeof(padding)`
   - 确保总大小是 8 的倍数
   - 示例：如果 `sensor_param_t` 变成 12 字节，padding 调整为 4 字节

3. **更新 `FACTORY_DEFAULT`**
   ```c
   const sensor_param_t FACTORY_DEFAULT = {
       // ... 原有字段 ...
       .new_field = 默认值,
   };
   ```

4. **重新编译烧录**

---

## 📐 掉电保护原理

### A/B 双页写入流程

```
初始状态：A 页有效(seq=10)，B 页有效(seq=9)
写入新数据：
  1. 识别 B 页是旧页(seq=9)
  2. 擦除 B 页
  3. 更新 seq=11
  4. 计算 CRC32
  5. 写入 B 页
```

### 掉电场景

| 掉电时刻 | A 页 | B 页 | 恢复结果 |
|---------|------|------|---------|
| 擦除前 | 有效(seq=10) | 有效(seq=9) | 加载 A 页 |
| 擦除中 | 有效(seq=10) | 已擦除 | 加载 A 页 |
| 写入中 | 有效(seq=10) | 部分写入 | CRC 失败，加载 A 页 |
| 写入后 | 有效(seq=10) | 有效(seq=11) | 加载 B 页 |

**结论：任何时刻掉电，至少有一页完整数据可用。**

---

## 📏 8 字节对齐处理

### Flash 写入约束

- GD32C231 必须按 **8 字节（doubleword）** 写入
- 地址必须 **8 字节对齐**
- 不足 8 字节的补 `0xFF`

### 实现细节

```c
/* 自动补齐到 8 字节倍数 */
uint32_t aligned_len = ((len + 7) / 8) * 8;
memcpy(buffer, data, len);
memset(buffer + len, 0xFF, aligned_len - len);  /* 补 0xFF */
```

---

## 📦 文件清单

| 文件 | 说明 |
|-----|------|
| `bsp_flash_simple.h` | Flash 底层驱动头文件 |
| `bsp_flash_simple.c` | Flash 底层驱动实现 |
| `nvm_param_simple.h` | NVM 参数管理头文件 |
| `nvm_param_simple.c` | NVM 参数管理实现 |

---

## ⚠️ 注意事项

1. **结构体大小**：修改 `sensor_param_t` 后，务必调整 `padding` 保证 8 字节对齐
2. **出厂默认值**：在 `FACTORY_DEFAULT` 中设置合理的默认参数
3. **CRC 范围**：CRC 不包含 `crc32` 和 `padding` 字段
4. **写入频率**：Flash 擦写次数有限（典型 10 万次），避免过于频繁保存

---

## 🔍 与原版本对比

| 特性 | 原版本 | 精简版 |
|-----|--------|--------|
| 代码行数 | ~800 行 | ~250 行 |
| API 数量 | 8 个 | 4 个 |
| 参数存储 | 拆分 a/b/c | 直接存完整结构体 |
| 版本迁移 | 复杂框架 | 无（注释说明） |
| 参数验证 | 8 项范围检查 | 无（简化） |
| 调试输出 | 详细日志 | 无 |
| 掉电保护 | A/B 双页 | A/B 双页（简化） |

**精简版适合：** 代码量敏感、功能要求不复杂的项目
**原版本适合：** 需要详细日志、严格验证、版本迁移的项目

---

**创建日期：** 2026-02-02  
**版本：** 1.0（精简版）
