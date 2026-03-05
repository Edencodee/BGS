# 调试打印系统使用指南

## 📚 概述

本项目实现了一套灵活、可配置的调试打印系统，支持：
- ✅ **分级控制**：5个调试级别（NONE/ERROR/WARN/INFO/DEBUG/VERBOSE）
- ✅ **分模块开关**：独立控制ADC/ANALYZE/OUTPUT/STAT模块
- ✅ **限频打印**：避免日志刷屏（每N次打印一次）
- ✅ **统计功能**：自动累计关键指标并周期性汇总

---

## 🎛️ 配置方法

### 1. 调试级别配置（`debug_config.h`）

```c
/* 设置全局调试级别 */
#define DEBUG_LEVEL  DEBUG_LEVEL_INFO  /* 可选：NONE/ERROR/WARN/INFO/DEBUG/VERBOSE */
```

| 级别 | 值 | 输出内容 |
|-----|---|----------|
| `DEBUG_LEVEL_NONE` | 0 | 关闭所有调试输出 |
| `DEBUG_LEVEL_ERROR` | 1 | 仅错误信息 |
| `DEBUG_LEVEL_WARN` | 2 | 错误 + 警告 |
| `DEBUG_LEVEL_INFO` | 3 | 错误 + 警告 + 信息（**推荐**） |
| `DEBUG_LEVEL_DEBUG` | 4 | 所有常规调试信息 |
| `DEBUG_LEVEL_VERBOSE` | 5 | 包含详细ADC原始数据（数据量大） |

### 2. 模块开关配置

```c
/* 分模块独立控制（1=启用，0=禁用） */
#define DEBUG_MODULE_ADC        1U  /* ADC采样调试 */
#define DEBUG_MODULE_ANALYZE    1U  /* 数据分析调试 */
#define DEBUG_MODULE_OUTPUT     1U  /* 输出控制调试 */
#define DEBUG_MODULE_STAT       1U  /* 统计信息 */
```

### 3. 限频打印配置

```c
/* 默认每100次打印一次 */
#define DEBUG_PRINT_INTERVAL_DEFAULT   100U
```

---

## 📝 使用示例

### 基本调试宏

```c
/* 1. 按级别打印 */
DEBUG_ERROR("Critical error: code=%d\r\n", error_code);   // [ERROR] 前缀
DEBUG_WARN("Warning: V_OUT=%d\r\n", V_OUT);               // [WARN] 前缀
DEBUG_INFO("System initialized\r\n");                     // [INFO] 前缀
DEBUG_LOG("Processing sample #%d\r\n", count);            // [DEBUG] 前缀
DEBUG_VERBOSE("Raw ADC: %d\r\n", adc_val);                // [VERBOSE] 前缀

/* 2. 按模块打印 */
DEBUG_ADC("DMA complete, samples=%d\r\n", count);         // [DEBUG][ADC] 前缀
DEBUG_ANALYZE("V_OUT=%d mV\r\n", V_OUT);                  // [DEBUG][ANALYZE] 前缀
DEBUG_OUTPUT("State: OBJECT_PRESENT\r\n");                // [DEBUG][OUTPUT] 前缀

/* 3. 限频打印（避免刷屏） */
DEBUG_THROTTLE(100, "V_OUT=%d mV\r\n", V_OUT);  // 每100次调用打印一次
```

### 统计功能

```c
/* 在适当位置更新统计 */
Debug_UpdateStatistics(is_valid, is_ready);  // 在 AnalyzeTask 中调用

/* 周期性打印统计汇总 */
if (total_samples % 1000 == 0) {
    Debug_PrintStatistics();  // 每1000次输出一次统计
}

/* 重置统计数据 */
Debug_ResetStatistics();
```

### 实用函数

```c
/* 打印 ADC 缓冲区（VERBOSE模式下才输出） */
Debug_PrintADCBuffers(g_A_Buffer, g_B_Buffer, SAMPLE_POINT);

/* 打印电压信息（带有效性标识） */
Debug_PrintVoltages(V_A, V_B, V_OUT, g_isVoutValid);

/* 打印输出状态 */
Debug_PrintOutputState();
```

---

## 🔧 典型配置场景

### 场景1：开发调试（详细模式）
```c
#define DEBUG_LEVEL  DEBUG_LEVEL_DEBUG      // 显示所有常规调试信息
#define DEBUG_MODE_ENABLE   1U              // 启用固定次数捕获
#define DEBUG_CAPTURE_GROUPS   20U          // 捕获20组数据后暂停
```

**输出示例：**
```
=== Debug Group 1 ===
[DEBUG][ANALYZE] V_A=1200, V_B=1350, V_OUT=150 mV [AVERAGED]
[DEBUG][OUTPUT] State: NO_OBJECT -> OBJECT_PRESENT (V_OUT=420 >= v_on=400)
...
=== DEBUG COMPLETE: 20 groups captured ===
[INFO][STAT] ======== Statistics Summary ========
[INFO][STAT] Total Samples:   20
[INFO][STAT] Valid Samples:   18 (90.0%)
[INFO][STAT] Invalid Samples: 2 (10.0%)
[INFO][STAT] Out of Range:    0
[INFO][STAT] Object Detected: 5
[INFO][STAT] State Changes:   10
```

### 场景2：性能验证（INFO级别）
```c
#define DEBUG_LEVEL  DEBUG_LEVEL_INFO       // 仅重要信息
#define DEBUG_MODE_ENABLE   0U              // 关闭捕获模式
```

**输出示例：**
```
[INFO] App Version: 1.0.2
[INFO] Debug Level: 3 (0=NONE, 1=ERROR, 2=WARN, 3=INFO, 4=DEBUG, 5=VERBOSE)
[WARN] OUT_OF_RANGE: invalid_count=5
[INFO] Recovered from OUT_OF_RANGE -> PRESENT (V_OUT=450)
... （每1000次采样打印一次统计）
[INFO][STAT] ======== Statistics Summary ========
[INFO][STAT] Total Samples:   1000
[INFO][STAT] Valid Samples:   950 (95.0%)
```

### 场景3：生产运行（关闭调试）
```c
#define DEBUG_LEVEL  DEBUG_LEVEL_ERROR      // 仅错误信息
// 或者
#define DEBUG_LEVEL  DEBUG_LEVEL_NONE       // 完全关闭
```

**输出示例：**
```
App Version: 1.0.2
（无其他输出，除非发生错误）
```

### 场景4：查看原始ADC数据（VERBOSE模式）
```c
#define DEBUG_LEVEL  DEBUG_LEVEL_VERBOSE    // 最详细模式
```

**输出示例：**
```
[VERBOSE][ADC] avgA=1234, avgB=1456
[VERBOSE][ADC] ADC Buffers (N=10):
[VERBOSE][ADC]   [0] A=1230, B=1450
[VERBOSE][ADC]   [1] A=1235, B=1455
...
[DEBUG][ANALYZE] Accumulating... (raw=222 mV)
[DEBUG][ANALYZE] V_A=1234, V_B=1456, V_OUT=222 mV [AVERAGED]
```

---

## 📊 统计数据说明

| 字段 | 说明 |
|-----|-----|
| `total_samples` | 总采样完成次数（平均完成的次数） |
| `valid_samples` | 有效样本数（V_B >= V_A） |
| `invalid_samples` | 无效样本数（V_B < V_A） |
| `out_of_range_count` | 进入超量程状态次数 |
| `object_present_count` | 检测到物体次数（NO_OBJECT→OBJECT_PRESENT转换次数） |
| `state_transitions` | 总状态切换次数 |

---

## 🎯 调试技巧

### 1. 快速定位问题

```c
/* 临时启用某个模块的详细输出 */
#define DEBUG_MODULE_OUTPUT     1U  // 仅看输出状态机
#define DEBUG_LEVEL  DEBUG_LEVEL_DEBUG

/* 其他模块关闭 */
#define DEBUG_MODULE_ADC        0U
#define DEBUG_MODULE_ANALYZE    0U
```

### 2. 限频避免刷屏

```c
/* 每100次打印一次，适用于高频事件 */
DEBUG_THROTTLE(100, "V_OUT=%d\r\n", V_OUT);

/* 每1000次打印一次，适用于统计信息 */
DEBUG_THROTTLE(1000, "Total: %lu\r\n", total_count);
```

### 3. 状态切换追踪

```c
/* task_output.c 自动记录状态切换 */
// 当 g_outputState 改变时，会自动打印：
// [DEBUG][OUTPUT] State: NO_OBJECT -> OBJECT_PRESENT (V_OUT=420 >= v_on=400)
```

### 4. 性能影响最小化

- 生产环境：设置 `DEBUG_LEVEL_NONE` 或 `DEBUG_LEVEL_ERROR`
- 调试时：使用限频宏 `DEBUG_THROTTLE`
- RTT输出：比UART快得多，影响较小

---

## ⚠️ 注意事项

1. **VERBOSE模式数据量大**
   - 会打印所有ADC原始数据（每组10个采样点×2路）
   - 建议仅在需要分析ADC波形时启用

2. **限频宏的使用**
   - 每个 `DEBUG_THROTTLE` 有独立的计数器（static变量）
   - 适用于循环内的高频打印

3. **统计数据自动维护**
   - `g_debug_stats` 在 `AnalyzeTask` 和 `OutputTask` 中自动更新
   - 无需手动调用（除非需要重置）

4. **编译优化**
   - 当 `DEBUG_LEVEL=NONE` 时，所有调试宏编译为空操作（零开销）

---

## 🚀 快速开始

### 第一次使用：

1. **打开详细调试**
   ```c
   #define DEBUG_LEVEL  DEBUG_LEVEL_DEBUG
   #define DEBUG_MODE_ENABLE   1U
   #define DEBUG_CAPTURE_GROUPS   10U
   ```

2. **编译并运行**，观察RTT输出

3. **根据需要调整**：
   - 数据太多 → 降低 `DEBUG_LEVEL` 或增加限频间隔
   - 需要看ADC波形 → 设置 `DEBUG_LEVEL_VERBOSE`
   - 只看状态切换 → 关闭其他模块开关

4. **生产部署时**：
   ```c
   #define DEBUG_LEVEL  DEBUG_LEVEL_NONE
   #define DEBUG_MODE_ENABLE   0U
   ```

---

## 📖 文件列表

| 文件 | 说明 |
|-----|-----|
| `App/inc/debug_config.h` | 调试系统配置和宏定义 |
| `App/src/debug_config.c` | 调试函数实现（统计、打印等） |
| `App/src/task_analyze.c` | 已集成调试打印和统计 |
| `App/src/task_output.c` | 已集成状态切换追踪 |
| `App/src/main.c` | 调试系统初始化和周期性统计 |

---

**提示**：建议在开发阶段保持 `DEBUG_LEVEL_INFO` 或 `DEBUG_LEVEL_DEBUG`，生产部署时改为 `DEBUG_LEVEL_ERROR` 或 `NONE`。
