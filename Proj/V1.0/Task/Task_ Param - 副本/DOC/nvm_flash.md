# GD32C231G8 Flash 掉电保存完整指南

## 1. 概述

### 问题背景
本模块为 GigaDevice GD32C231G8 微控制器提供 **Flash 掉电保存机制**，用于在系统重启或掉电后恢复关键参数（如传感器初始化配置、校准参数等）。

### 为什么需要 A/B 双页 + seq + CRC？

#### 传统单页方案的问题
如果仅使用一个 Flash 页存储数据：
- 若在擦除或写入过程中**掉电**，该页面会被损坏
- 系统重启后无法恢复任何数据

#### 本模块的解决方案
采用 **A/B 双页机制**：
- **Page A（页62）**：主存储页
- **Page B（页63）**：备份页
- 每次保存时，只对其中一页进行擦写，**另一页保留作为备份**
- 如果保存过程中掉电，系统可从另一页恢复旧数据
- 使用 **seq（序列号）** 区分新旧版本，启动时自动选择最新有效数据
- 使用 **CRC32** 校验数据完整性，检测位翻或损坏

#### 掉电保护的工作原理
```
保存流程：
  1. 读取 A、B 两页，比较 seq 判断哪页更旧
  2. 选择较旧页作为目标
  3. 只对目标页进行 [擦除] → [写入]
  4. 另一页保持不变

掉电场景 1：写入目标页中间掉电
  → 目标页损坏（全 0xFF）
  → 系统启动时读取另一页（仍有效）
  → 恢复成功 ✓

掉电场景 2：写入完成后掉电
  → 目标页有效且为最新版本
  → 系统启动时读取新数据
  → 恢复成功 ✓
```

---

## 2. Flash / NVM 基础概念

### Flash 存储特性（面向新手）

| 特性 | 说明 |
|------|------|
| **擦除粒度** | 以**页**为单位，1KB/页。不能擦除部分字节 |
| **写入粒度** | 以**双字（doubleword）**为单位，8 字节/次。地址必须 8 字节对齐 |
| **写入规则** | 只能将 1 写成 0，**不能直接将 0 改回 1**（需先擦除整页） |
| **特点** | 掉电安全，断电不丢失，适合存储关键参数 |

### GD32C231G8 Flash 总体布局

```
┌─────────────────────────────────┐
│    Flash 总容量：64KB            │
│    (0x08000000 ~ 0x0800FFFF)    │
├─────────────────────────────────┤
│ 页 0 ~ 页 61：应用程序区         │
│ (0x08000000 ~ 0x0800F7FF)        │
│ 共 62 页 = 62KB                  │
├─────────────────────────────────┤
│ 页 62：NVM Page A（1KB）          │
│ (0x0800F800 ~ 0x0800FBFF)        │
├─────────────────────────────────┤
│ 页 63：NVM Page B（1KB）          │
│ (0x0800FC00 ~ 0x0800FFFF)        │
└─────────────────────────────────┘
```

### 地址与页号对应表

| 功能 | 页索引 | 起始地址 | 结束地址 | 容量 |
|------|--------|---------|---------|------|
| NVM Page A | 62 | 0x0800F800 | 0x0800FBFF | 1KB |
| NVM Page B | 63 | 0x0800FC00 | 0x0800FFFF | 1KB |

### 重要提示

⚠️ **链接脚本配置**  
编译时确保程序代码**只编译到页 61 为止**，不要跨越 0x0800F800 边界。否则程序会覆盖 NVM 区域。

**Keil 工程中的做法：**
- 打开 `Target Options → Linker → Scatter File`
- 编辑 `.scf` 文件，限制代码段的最大地址为 `0x0800F7FF`

---

## 3. 数据格式说明

### flash_data_t 结构体定义

```c
typedef struct {
    uint32_t magic;      /* 魔数：0x5A5AA55A，标记数据有效性 */
    uint32_t seq;        /* 序列号：单调递增，用于判断 A/B 哪份最新 */
    uint32_t a;          /* 用户参数：示例变量 a */
    uint32_t b;          /* 用户参数：示例变量 b */
    uint32_t c;          /* 用户参数：示例变量 c */
    uint32_t reserved;   /* 预留字段：保证 8 字节对齐 */
    uint32_t crc32;      /* CRC32 校验值 */
} flash_data_t;         /* 总长度：28 字节（7×uint32_t） */
```

### 字段详解

| 字段 | 类型 | 长度 | 说明 |
|------|------|------|------|
| `magic` | uint32_t | 4B | 固定值 0x5A5AA55A，用于快速判断该页是否包含有效数据 |
| `seq` | uint32_t | 4B | 序列号，从 1 开始，每次保存递增。启动时对比 A/B 的 seq，选最大的 |
| `a/b/c` | uint32_t | 12B | 实际参数存储（示例）。用户可根据需要修改名称和数量 |
| `reserved` | uint32_t | 4B | 填充字段，确保整个结构体长度是 8 的倍数（必须！） |
| `crc32` | uint32_t | 4B | CRC32 值，保存前计算，加载前校验 |

### CRC32 覆盖范围

⚠️ **重点**：CRC32 **不包含** `crc32` 字段本身。

```c
/* CRC32 计算覆盖的字节范围 */
crc32_value = flash_crc32(
    &magic,                     /* 起点 */
    sizeof(flash_data_t) - sizeof(uint32_t)  /* 长度 = 24 字节 */
);
/* 即：magic(4B) + seq(4B) + a(4B) + b(4B) + c(4B) + reserved(4B) = 24B */
```

**为什么这样设计？**  
计算 CRC 时不能包含 CRC 值本身，否则陷入循环定义。校验时重新计算前 24 字节的 CRC，与存储的 crc32 字段比对。

---

## 4. 擦写流程（底层操作）

### 单次写入流程

```
Flash 写入操作步骤（以 flash_write() 为例）

1. 参数检查
   ├─ buffer 非 NULL？
   ├─ length > 0？
   └─ address % 8 == 0？（8 字节对齐）
      ↓ [否] 返回 NVM_ERR_ALIGN
      
2. 解锁 Flash 控制器
   └─ flash_unlock()
   
3. 清除标志位
   └─ flash_clear_flags()
      清除 FMC_STAT 寄存器中的所有错误标志
      （ENDF, WPERR, PGERR, PGAERR 等）
   
4. 分包写入（每次 8 字节）
   ├─ 从 buffer 读 8 字节
   ├─ 不足 8 字节的部分补 0xFF（Flash 默认值）
   ├─ 调用 fmc_doubleword_program(address, data64)
   └─ 重复直到全部数据写入

5. 上锁 Flash 控制器
   └─ flash_lock()

6. 返回
   └─ 如果任何步骤失败，返回对应错误码
```

### 页擦除流程

```
Flash 页擦除操作步骤（以 flash_erase_page() 为例）

1. 参数检查
   └─ page_index < 64？
      ↓ [否] 返回 NVM_ERR_PAGE_IDX
      
2. 解锁 → 清标志 → 擦除 → 上锁
   └─ flash_unlock()
   └─ flash_clear_flags()
   └─ fmc_page_erase(page_index)
   └─ flash_lock()

3. 返回
   └─ NVM_OK 或 NVM_ERR_ERASE
```

### 对齐与补齐策略

**为什么要 8 字节对齐？**  
GD32C231G8 的 FMC 硬件只支持 doubleword（64-bit）编程，地址必须对齐。

**不足 8 字节如何处理？**  
本模块在 `flash_write()` 内部会自动补齐：
```c
uint8_t temp[8];
memset(temp, 0xFF, 8);           /* 全部填充为 0xFF */
if (remain >= 8) {
    memcpy(temp, &buffer[offset], 8);
} else {
    memcpy(temp, &buffer[offset], remain);  /* 只复制 remain 字节 */
}
/* temp 中剩余部分仍为 0xFF，一起写入 Flash */
```

---

## 5. 掉电保存流程（高层业务逻辑）

### 保存数据：flash_save_data()

```
save_data() 的执行步骤

输入：const flash_data_t *data（待保存的数据）

1. 参数检查
   └─ data != NULL？

2. 智能选择目标页（读 A、读 B，比较 seq）
   ├─ 读 Page A → 计算 CRC → 检查 magic
   ├─ 读 Page B → 计算 CRC → 检查 magic
   ├─ 判断有效性与 seq 大小
   │  ├─ [A 有效，B 有效] → 写较旧页（seq 小的那页）
   │  ├─ [仅 A 有效]      → 写 Page B
   │  ├─ [仅 B 有效]      → 写 Page A
   │  └─ [都无效]         → 写 Page A，seq 从 1 开始
   └─ 确定目标地址与新 seq 值

3. 准备数据
   ├─ 复制 data 到临时容器
   ├─ 将目标 seq 值填入（比旧值 +1）
   └─ 计算 CRC32（覆盖 magic~reserved 的 24 字节）

4. 擦除目标页
   └─ flash_erase_page(target_page_index)
      [失败] → 返回 NVM_ERR_ERASE

5. 写入目标页
   └─ flash_write(target_address, data, 28 字节)
      [失败] → 返回 NVM_ERR_WRITE

6. 写后校验（关键步骤！）
   ├─ 读回目标页数据
   ├─ 重新计算 CRC
   └─ 如果 CRC 不匹配或 magic 无效
      → 返回 NVM_ERR_VERIFY
      → 另一页的备份仍保留（掉电恢复点）

7. 返回 NVM_OK（保存成功）
```

### 加载数据：flash_load_data()

```
load_data() 的执行步骤

输入：flash_data_t *data（用于接收加载的数据）

1. 参数检查
   └─ data != NULL？

2. 读取两页数据
   ├─ 读 Page A（28 字节）→ 计算 CRC
   ├─ 读 Page B（28 字节）→ 计算 CRC
   └─ 检查双方的 magic 是否为 0x5A5AA55A

3. 有效性判断
   ├─ A_valid = (A 的计算 CRC == 存储 CRC) && (A 的 magic 有效)
   ├─ B_valid = (B 的计算 CRC == 存储 CRC) && (B 的 magic 有效)
   └─ 记录双方的 seq 值

4. 选择数据源（优先级：seq 最大的有效页）
   ├─ [A 有效，B 有效]
   │  └─ seq_A >= seq_B？使用 A：使用 B
   ├─ [仅 A 有效]
   │  └─ 使用 A（B 已损坏，但有备份）
   ├─ [仅 B 有效]
   │  └─ 使用 B（A 已损坏，但有备份）
   └─ [都无效]
      ├─ 返回 NVM_ERR_INVALID
      ├─ data 全部填充 0xFF
      └─ 调用者需判断 magic 并考虑使用默认值

5. 复制数据
   └─ memcpy(data, 选中的页内容, 28 字节)

6. 返回
   └─ NVM_OK（成功加载）或 NVM_ERR_INVALID（无有效备份）
```

### 掉电场景模拟图

```
时间线示例（假设 seq_A=5，seq_B=3）

【正常保存情况】
Step 1: 选择目标 Page A（seq_A 更大，写它的对手 Page B）
Step 2: 擦除 Page B → OK
Step 3: 写入 Page B，更新为 seq=6
Step 4: 写后校验 → OK
Step 5: 返回成功
结果：Page A seq=5，Page B seq=6（最新）

【掉电在 Step 3 中间】
Outcome: Page B 损坏（部分 0xFF 或不完整）
启动后加载：
  - 读 Page A seq=5 ✓ CRC OK
  - 读 Page B 失败（CRC bad）
  - 选择 Page A（seq=5）
  - 恢复成功 ✓

【掉电在 Step 4 后（Write Success, Before Verify）】
Outcome: Page B 写入完成，seq=6 ✓
启动后加载：
  - 读 Page A seq=5 ✓ CRC OK
  - 读 Page B seq=6 ✓ CRC OK（概率大）
  - seq_B > seq_A，选择 Page B（最新版本）
  - 恢复最新数据 ✓
```

---

## 6. A/B 页轮换策略详解

### 典型场景分析

#### 场景 1：首次启动（两页都是 0xFFFFFFFF）

```
启动时：
  Page A: 全 0xFF（无效）
  Page B: 全 0xFF（无效）

首次保存（例如初始化参数）：
  ├─ 读 A、B 都无效
  ├─ 决策：写 Page A，seq=1
  └─ 结果：Page A seq=1（新）Page B（空）

第二次保存：
  ├─ 读 A seq=1 有效，B 无效
  ├─ 决策：写 Page B，seq=2
  └─ 结果：Page A seq=1（旧）Page B seq=2（新）

第三次保存：
  ├─ 读 A seq=1 有效，B seq=2 有效
  ├─ 决策：B 较新，写 A，seq=3
  └─ 结果：Page A seq=3（新）Page B seq=2（旧）

第四次保存：
  ├─ 读 A seq=3 有效，B seq=2 有效
  ├─ 决策：A 较新，写 B，seq=4
  └─ 结果：Page A seq=3（旧）Page B seq=4（新）

...（以此类推，A/B 轮流更新）
```

#### 场景 2：Page A 有效，Page B 无效

```
初始状态：
  Page A: 有效，seq=10
  Page B: 无效（0xFF）

保存操作：
  ├─ 读 A 有效，B 无效
  ├─ 决策：写 B，seq=11（A 的 seq + 1）
  └─ 结果：A seq=10 被保留，B seq=11 是新版本
```

#### 场景 3：Page B 有效，Page A 无效（某次保存失败）

```
初始状态：
  Page A: 无效
  Page B: 有效，seq=5

保存操作：
  ├─ 读 A 无效，B 有效
  ├─ 决策：写 A，seq=6（B 的 seq + 1）
  └─ 结果：B seq=5 被保留为备份，A seq=6 是新版本
```

#### 场景 4：两页都无效（异常情况，如遇干扰）

```
初始状态：
  Page A: 无效或 CRC 损坏
  Page B: 无效或 CRC 损坏

保存操作：
  ├─ 决策：写 Page A，seq=1（从头开始）
  └─ 结果：Page A seq=1，Page B 保持 0xFF

下次保存：
  └─ Page A seq=1 有效，Page B 无效 → 写 B seq=2
```

### 掉电恢复的核心原理

```
关键点：保存时只改动"另一页"，当前有数据的页不动

场景：正在更新 seq，掉电发生

Timeline:
┌─────────────────────────────────────────────────┐
│ Page A seq=10 (有效)  Page B seq=8 (有效)       │
└─────────────────────────────────────────────────┘
                  ↓ 决定写 Page B（较旧的）
┌─────────────────────────────────────────────────┐
│ 操作: 擦除 Page B                                 │
└─────────────────────────────────────────────────┘
        ↓ 成功 [Page A 仍为 seq=10]
┌─────────────────────────────────────────────────┐
│ 操作: 写入 Page B 新数据 seq=11                   │
│ [掉电发生在此] →                                   │
│ Page B 可能：完全成功 / 部分成功 / 失败            │
└─────────────────────────────────────────────────┘
        ↓ 重启
┌─────────────────────────────────────────────────┐
│ Page A seq=10 仍有效 ✓                           │
│ Page B 可能无效或不完整 ✗                        │
│ → 选择 Page A（seq=10），恢复成功               │
└─────────────────────────────────────────────────┘
```

**为什么能恢复？**  
- A/B 不同时修改，至少一页总是完整的
- seq 最大的有效页是最新数据
- 即使 seq 相同，CRC 校验会发现损坏

---

## 7. 最小使用示例

### 完整演示代码

本示例展示如何在 `main()` 中集成掉电保存机制，包括首次初始化、保存、加载及错误处理。

```c
#include "main.h"
#include "bsp_flash.h"

int main(void)
{
    flash_data_t nvm_data;
    int ret;

    /* ===== 初始化硬件 ===== */
    systick_config();
    GPIO_UserInit();

    /* ===== 阶段 1：尝试从 Flash 加载保存的数据 ===== */
    printf("\n=== 启动：加载 Flash 数据 ===\n");
    ret = flash_load_data(&nvm_data);
    
    if (ret == NVM_OK) {
        /* 数据有效，继续使用 */
        printf("✓ Flash 数据加载成功\n");
        printf("  seq=%u, a=%u, b=%u, c=%u\n", 
               nvm_data.seq, nvm_data.a, nvm_data.b, nvm_data.c);
        
    } else if (ret == NVM_ERR_INVALID) {
        /* 两页都无效或损坏，使用默认值 */
        printf("✗ Flash 数据全部无效，使用默认值\n");
        nvm_data.magic = 0x5A5AA55AU;
        nvm_data.seq = 0;           /* seq 从 0 开始，save 时会递增 */
        nvm_data.a = 1U;            /* 默认值示例 */
        nvm_data.b = 2U;
        nvm_data.c = 3U;
        nvm_data.reserved = 0U;
        
        /* 保存初始化参数到 Flash */
        printf("→ 保存默认参数到 Flash...\n");
        ret = flash_save_data(&nvm_data);
        if (ret == NVM_OK) {
            printf("✓ 默认参数保存成功 (seq=%u)\n", nvm_data.seq);
        } else {
            printf("✗ 参数保存失败，错误码=%d\n", ret);
            /* 错误码含义：
               -2 = NVM_ERR_ERASE   (页擦除失败)
               -3 = NVM_ERR_WRITE   (写入失败)
               -4 = NVM_ERR_ALIGN   (地址未对齐)
               -5 = NVM_ERR_VERIFY  (写后 CRC 校验失败)
            */
        }
    } else {
        printf("✗ 其他错误，错误码=%d\n", ret);
    }

    /* ===== 阶段 2：应用参数到系统 ===== */
    printf("\n=== 应用参数 ===\n");
    printf("使用参数：a=%u, b=%u, c=%u\n", 
           nvm_data.a, nvm_data.b, nvm_data.c);
    /* 在此初始化传感器或其他硬件 */

    /* ===== 阶段 3：演示参数修改与保存 ===== */
    printf("\n=== 修改参数演示 ===\n");
    nvm_data.a = 10U;  /* 修改参数 */
    nvm_data.b = 20U;
    
    printf("→ 保存修改的参数...\n");
    ret = flash_save_data(&nvm_data);
    if (ret == NVM_OK) {
        printf("✓ 参数保存成功 (seq=%u)\n", nvm_data.seq);
    } else {
        printf("✗ 参数保存失败，错误码=%d\n", ret);
    }

    /* ===== 主循环 ===== */
    printf("\n=== 进入主循环 ===\n");
    while(1) {
        LedSet(kLedYellow);
        delay_1ms(1000);
        LedClr(kLedYellow);
        delay_1ms(1000);
        
        /* 在此位置可以检查是否需要保存参数 */
        /* 示例：定时保存或响应外部命令 */
    }
}

/* RTT 重定向 printf */
int fputc(int ch, FILE *f)
{
    SEGGER_RTT_PutChar(0, (char)ch);
    return ch;
}
```

### 关键点解析

#### 错误处理

```c
ret = flash_load_data(&nvm_data);

if (ret == NVM_OK) {
    /* 数据有效 */
    
} else if (ret == NVM_ERR_INVALID) {
    /* 特别处理：两页都无效 */
    /* 应初始化为默认值并保存 */
    
} else {
    /* 其他错误（一般不发生） */
}
```

#### 如何快速判断 Flash 是否已初始化？

```c
/* 方法 1：检查返回值 */
if (flash_load_data(&nvm_data) == NVM_OK) {
    /* 已初始化 */
} else {
    /* 未初始化，需要设置默认值 */
}

/* 方法 2：检查 magic 字段 */
if (nvm_data.magic == 0x5A5AA55AU) {
    /* 数据有效 */
} else {
    /* 数据无效 */
}
```

#### 保存频率建议

```c
/* 方案 1：需要时立即保存（简单但频繁） */
nvm_data.a = 100;
flash_save_data(&nvm_data);  /* 每次修改都保存 */

/* 方案 2：标记"脏"数据，定时保存（推荐） */
volatile int param_dirty = 0;

void update_param(uint32_t new_a) {
    nvm_data.a = new_a;
    param_dirty = 1;  /* 标记为需要保存 */
}

void task_save_if_dirty(void) {
    if (param_dirty) {
        flash_save_data(&nvm_data);
        param_dirty = 0;
    }
}

/* 在主循环或定时器中调用 task_save_if_dirty() */
```

---

## 8. 常见问题与排查

### Q1：收到 NVM_ERR_ALIGN (-4)

**原因**  
调用 `flash_write()` 时，地址不是 8 字节对齐。

**检查清单**  
```c
/* 错误示例 */
flash_write(0x0800F801, buffer, 28);  /* 地址 0xF801，地址 % 8 != 0 */

/* 正确示例 */
flash_write(0x0800F800, buffer, 28);  /* 0xF800 % 8 == 0 ✓ */
```

**注意**  
本模块的 `flash_save_data()` 和 `flash_load_data()` 会自动使用正确的地址，用户不需手动调用 `flash_write()`。

### Q2：收到 NVM_ERR_VERIFY (-5)

**原因**  
写入后 CRC 校验失败，表示数据在写入过程中被破坏。

**可能原因**  
1. Flash 存储位置坏块
2. 电压不稳定导致数据错误
3. EMI 干扰

**排查步骤**  
```c
/* 增加调试输出 */
#define NVM_DEBUG_ENABLE 1   /* 启用调试打印 */

ret = flash_save_data(&nvm_data);
if (ret == NVM_ERR_VERIFY) {
    printf("写后校验失败！\n");
    /* 尝试再次保存到另一页 */
    ret = flash_save_data(&nvm_data);
}
```

**恢复方案**  
- 重试保存（自动使用另一页）
- 如果反复失败，可能是硬件问题

### Q3：CRC 不匹配导致数据无效

**现象**  
`flash_load_data()` 返回 `NVM_ERR_INVALID`，两页数据都被判为无效。

**排查**  
```c
/* 手动检查 CRC 计算 */
uint32_t calc_crc = flash_crc32(&nvm_data, 24);  /* 注意：长度是 24，不是 28 */
printf("Stored CRC: 0x%08X\n", nvm_data.crc32);
printf("Calc CRC:   0x%08X\n", calc_crc);
if (calc_crc != nvm_data.crc32) {
    printf("CRC 不匹配，数据已损坏\n");
}
```

**常见错误**  
```c
/* ❌ 错误：计算长度包含 crc32 本身 */
flash_crc32(&nvm_data, sizeof(flash_data_t));  /* 28 字节 */

/* ✓ 正确：只计算前 24 字节 */
flash_crc32(&nvm_data, sizeof(flash_data_t) - sizeof(uint32_t));  /* 24 字节 */
```

### Q4：Flash 写保护错误（WPERR）

**现象**  
返回 `NVM_ERR_ERASE` 或 `NVM_ERR_WRITE`。

**原因**  
页面被写保护。

**解决方案**  
1. 检查 Flash 控制寄存器配置
2. 确保 `flash_unlock()` 正确调用
3. 确认没有其他代码同时访问 Flash

**代码检查**  
```c
/* 本模块内部已处理，用户无需干预 */
static void flash_unlock(void) {
    fmc_unlock();  /* 调用 GD32 库函数解锁 */
}
```

### Q5：频繁保存导致 Flash 寿命问题

**背景**  
Flash 擦除次数有限制（约 100K 次），频繁保存会加速老化。

**建议方案**  
```c
/* 方案：使用"脏标志"延迟保存 */
volatile int param_changed = 0;

void on_param_update(void) {
    param_changed = 1;  /* 标记为有更改 */
}

void task_periodic_save(void) {
    if (param_changed) {
        flash_save_data(&nvm_data);
        param_changed = 0;
    }
}

/* 在定时器中以固定间隔（如 1s）调用 task_periodic_save() */
```

**估算寿命**  
```
假设：
  - Flash 擦除寿命：100K 次
  - A/B 页轮换使用
  - 每次保存 1 个 A/B 页

方案 1（每次修改都保存）：
  - 如果 1s 内修改 1 次，运行 1 年 → 365×86400 = 3153.6M 次修改
  - 超过 100K 很多倍！不可行
  
方案 2（定时保存，1min 一次）：
  - 1 年：365×24×60 = 525600 次保存
  - 每 2 个页轮换一次：525600/2 = 262800 次
  - 仍高于 100K！考虑更长周期
  
方案 3（定时保存，10min 一次）：
  - 1 年：365×24×6 = 52560 次保存
  - 每 2 个页轮换一次：52560/2 = 26280 次 < 100K ✓
```

### Q6：如何恢复为默认参数？

**需求**  
用户想清除已保存的参数，恢复出厂设置。

**方案**  
```c
void restore_default_params(void) {
    flash_data_t default_data;
    
    /* 填充默认值 */
    default_data.magic = 0x5A5AA55AU;
    default_data.seq = 0;
    default_data.a = 1U;
    default_data.b = 2U;
    default_data.c = 3U;
    default_data.reserved = 0U;
    
    /* 保存到 Flash */
    printf("恢复默认参数...\n");
    int ret = flash_save_data(&default_data);
    if (ret == NVM_OK) {
        printf("✓ 默认参数恢复完成\n");
    } else {
        printf("✗ 恢复失败，错误码=%d\n", ret);
    }
}
```

---

## 9. 扩展建议

### 增加 version / length 字段（结构体升级兼容）

**场景**  
现在有参数结构体 V1，后来需要添加新字段变成 V2，但已有设备存储的是 V1 数据。

**解决方案**  
在 `flash_data_t` 中添加版本字段：

```c
typedef struct {
    uint32_t magic;      /* 0x5A5AA55A */
    uint32_t version;    /* 参数表版本：1=V1, 2=V2 */
    uint32_t seq;        /* 序列号 */
    
    /* V1 字段 */
    uint32_t a, b, c;
    
    /* V2 新增字段 */
    uint32_t d;          /* 新增参数 */
    uint32_t e;
    
    uint32_t reserved;
    uint32_t crc32;
} flash_data_t_v2;
```

**加载时处理**  
```c
void load_with_migration(flash_data_t_v2 *data) {
    flash_load_data((flash_data_t *)data);
    
    switch (data->version) {
        case 1:
            /* V1 → V2 迁移 */
            data->d = 0;  /* 新字段设默认值 */
            data->e = 0;
            data->version = 2;
            flash_save_data(data);  /* 保存为 V2 */
            break;
        case 2:
            /* 已是 V2，无需迁移 */
            break;
    }
}
```

### 日志式磨损均衡（参数变化极频繁的场景）

**场景**  
参数更新很频繁（如每秒多次），无法用延迟保存解决。

**方案**  
使用循环日志：分配多个页，轮流写入。

```c
#define NVM_PAGE_COUNT 4  /* 使用 4 个页实现磨损均衡 */

struct {
    uint32_t page_index;
} nvm_log_state = { .page_index = 0 };

void save_with_wear_leveling(const flash_data_t *data) {
    /* 轮流使用 4 个页 */
    uint32_t target_page = (nvm_log_state.page_index++) % 4;
    uint32_t target_addr = NVM_BASE_ADDR + target_page * 0x400;
    
    flash_erase_page(target_page + NVM_PAGE_START);
    flash_write(target_addr, (uint8_t *)data, sizeof(flash_data_t));
}

void load_with_wear_leveling(flash_data_t *data) {
    /* 查找最新有效的数据 */
    uint32_t max_seq = 0;
    int best_page = 0;
    
    for (int i = 0; i < 4; i++) {
        flash_data_t temp;
        flash_read(NVM_BASE_ADDR + i * 0x400, (uint8_t *)&temp, sizeof(flash_data_t));
        if (temp.magic == 0x5A5AA55AU && temp.seq > max_seq) {
            max_seq = temp.seq;
            best_page = i;
        }
    }
    
    flash_read(NVM_BASE_ADDR + best_page * 0x400, (uint8_t *)data, sizeof(flash_data_t));
}
```

### 使用硬件 CRC 外设替代查表法（可选优化）

**优势**  
- 速度快
- 节省 Flash 空间（查表法占 1KB）

**缺点**  
- 硬件 CRC 多项式可能与软件标准不一致
- 需要验证与 PC 端计算工具兼容性

**实现参考**  
```c
/* 调用 GD32 的硬件 CRC 外设 */
#include "gd32c2x1_crc.h"

uint32_t flash_crc32_hw(const void *data, uint32_t len) {
    crc_deinit();
    
    const uint32_t *p = (const uint32_t *)data;
    uint32_t words = len / 4;
    
    while (words--) {
        crc_single_data_input(*p++);
    }
    
    return crc_result_get();
}
```

**兼容性检查**  
```c
/* 启动时验证硬件 CRC 与软件 CRC 一致 */
uint32_t test_data[] = { 0x12345678, 0xABCDEF00 };
uint32_t sw_crc = flash_crc32(test_data, 8);
uint32_t hw_crc = flash_crc32_hw(test_data, 8);

if (sw_crc == hw_crc) {
    printf("硬件 CRC 兼容 ✓\n");
} else {
    printf("硬件 CRC 不兼容，使用软件版本\n");
}
```

---

## 10. 快速参考

### 函数列表

| 函数 | 用途 | 返回值 |
|------|------|--------|
| `flash_load_data(data)` | 从 Flash 加载数据 | NVM_OK / NVM_ERR_INVALID |
| `flash_save_data(data)` | 保存数据到 Flash | NVM_OK / NVM_ERR_ERASE / NVM_ERR_WRITE / ... |
| `flash_erase_page(idx)` | 擦除指定页 | NVM_OK / NVM_ERR_ERASE |
| `flash_write(addr, buf, len)` | 写入数据（内部用） | NVM_OK / NVM_ERR_ALIGN / ... |
| `flash_read(addr, buf, len)` | 读取数据（内部用） | 无返回值 |
| `flash_crc32(data, len)` | 计算 CRC32 | CRC 值 |

### 错误码速查

| 错误码 | 宏名 | 含义 | 处理建议 |
|--------|------|------|---------|
| 0 | `NVM_OK` | 操作成功 | - |
| -1 | `NVM_ERR_PARAM` | 参数非法（NULL 指针） | 检查函数调用参数 |
| -2 | `NVM_ERR_ERASE` | 页擦除失败 | 检查 Flash 状态，重试 |
| -3 | `NVM_ERR_WRITE` | 数据写入失败 | 检查地址、重试 |
| -4 | `NVM_ERR_ALIGN` | 地址未对齐 | 确保地址 % 8 == 0 |
| -5 | `NVM_ERR_VERIFY` | 写后校验失败 | 另一页仍有备份，系统可恢复 |
| -6 | `NVM_ERR_INVALID` | 数据全部无效 | 使用默认值并保存 |
| -7 | `NVM_ERR_PAGE_IDX` | 页索引超范围 | 检查页号，应在 0-63 内 |

### 地址常量

```c
#define NVM_SAVE_ADDR_A     0x0800F800  /* Page A 起始地址 */
#define NVM_SAVE_ADDR_B     0x0800FC00  /* Page B 起始地址 */
#define NVM_SAVE_PAGE_A_INDEX  62       /* Page A 页号 */
#define NVM_SAVE_PAGE_B_INDEX  63       /* Page B 页号 */
```

### 调试输出控制

```c
/* 在 bsp_flash.h 中修改 */
#define NVM_DEBUG_ENABLE  1   /* 1=打印调试信息，0=关闭 */
```

---

## 附录：项目文件清单

```
Driver7_Flash/
├── BSP/
│   ├── inc/
│   │   └── bsp_flash.h          ← Flash 驱动头文件
│   └── src/
│       └── bsp_flash.c          ← Flash 驱动实现
├── App/
│   └── src/
│       └── main.c               ← 应用示例
├── RTT/
│   ├── SEGGER_RTT.h
│   ├── SEGGER_RTT.c
│   ├── SEGGER_RTT_Conf.h
│   └── SEGGER_RTT_printf.c      ← 用于 printf 重定向
├── nvm_flash.md                 ← 本文档
└── readme.txt

编译方式：
  - Keil µVision 5.x
  - 确保 Linker → Scatter File 中代码上限为 0x0800F7FF
```

---

## 总结

本模块通过 **A/B 双页 + seq + CRC32** 的设计，为 GD32C231G8 提供了 **掉电安全的参数保存机制**。

**核心优势：**
- ✓ 掉电保护：保存过程中断电不会导致数据完全丢失
- ✓ 自动切换：系统自动选择最新有效数据，无需人工干预
- ✓ 错误诊断：细粒度错误码便于故障排查
- ✓ 易于集成：接口简洁，参考本文档可快速上手

**快速开始：**
1. 在 `main.c` 的启动代码中调用 `flash_load_data()`
2. 如果返回 `NVM_ERR_INVALID`，初始化默认值并调用 `flash_save_data()`
3. 之后每次修改参数时，调用 `flash_save_data()` 即可

有任何问题，参考本文档的 **常见问题与排查** 章节。
