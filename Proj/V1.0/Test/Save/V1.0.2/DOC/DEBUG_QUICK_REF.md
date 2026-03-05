# 调试系统速查表

## 🎛️ 快速配置（debug_config.h）

```c
/* 调试级别：0=NONE, 1=ERROR, 2=WARN, 3=INFO, 4=DEBUG, 5=VERBOSE */
#define DEBUG_LEVEL  DEBUG_LEVEL_INFO

/* 模块开关：1=启用，0=禁用 */
#define DEBUG_MODULE_ADC        1U
#define DEBUG_MODULE_ANALYZE    1U
#define DEBUG_MODULE_OUTPUT     1U
#define DEBUG_MODULE_STAT       1U

/* 限频间隔（每N次打印一次） */
#define DEBUG_PRINT_INTERVAL_DEFAULT   100U
```

## 📝 常用宏

```c
/* 基础调试宏 */
DEBUG_ERROR("Error: %d\r\n", code);           // [ERROR] 错误
DEBUG_WARN("Warning: %s\r\n", msg);           // [WARN] 警告
DEBUG_INFO("Info: %s\r\n", info);             // [INFO] 信息
DEBUG_LOG("Debug: %d\r\n", val);              // [DEBUG] 调试
DEBUG_VERBOSE("Verbose: %d\r\n", detail);     // [VERBOSE] 详细

/* 模块调试宏 */
DEBUG_ADC("ADC: %d\r\n", val);                // [DEBUG][ADC]
DEBUG_ANALYZE("Analyze: %d\r\n", val);        // [DEBUG][ANALYZE]
DEBUG_OUTPUT("Output: %s\r\n", state);        // [DEBUG][OUTPUT]
DEBUG_STAT("Stat: %lu\r\n", count);           // [INFO][STAT]

/* 限频打印（每N次打印一次） */
DEBUG_THROTTLE(100, "Frequent: %d\r\n", val); // 每100次打印
```

## 🔧 实用函数

```c
/* 统计功能 */
Debug_ResetStatistics();                      // 重置统计
Debug_PrintStatistics();                      // 打印统计汇总
Debug_UpdateStatistics(is_valid, is_ready);   // 更新统计（自动调用）

/* 打印函数 */
Debug_PrintADCBuffers(buf_a, buf_b, len);     // 打印ADC数据
Debug_PrintVoltages(v_a, v_b, v_out, valid);  // 打印电压
Debug_PrintOutputState();                     // 打印输出状态
```

## 📊 统计字段

```c
g_debug_stats.total_samples        // 总样本数
g_debug_stats.valid_samples        // 有效样本数
g_debug_stats.invalid_samples      // 无效样本数
g_debug_stats.out_of_range_count   // 超量程次数
g_debug_stats.object_present_count // 检测物体次数
g_debug_stats.state_transitions    // 状态切换次数
```

## 🎯 典型场景

### 开发调试
```c
#define DEBUG_LEVEL  DEBUG_LEVEL_DEBUG
#define DEBUG_MODE_ENABLE   1U
#define DEBUG_CAPTURE_GROUPS   20U
```

### 性能测试
```c
#define DEBUG_LEVEL  DEBUG_LEVEL_INFO
#define DEBUG_MODE_ENABLE   0U
```

### 生产部署
```c
#define DEBUG_LEVEL  DEBUG_LEVEL_NONE  // 或 ERROR
#define DEBUG_MODE_ENABLE   0U
```

### 查看ADC波形
```c
#define DEBUG_LEVEL  DEBUG_LEVEL_VERBOSE
Debug_PrintADCBuffers(g_A_Buffer, g_B_Buffer, SAMPLE_POINT);
```

## ⚡ 性能提示

- `DEBUG_LEVEL_NONE` → 零开销（编译为空）
- 使用 `DEBUG_THROTTLE` 避免刷屏
- 生产环境建议 `<=ERROR` 级别
- RTT输出比UART快10-100倍

## 📍 集成位置

| 文件 | 已集成功能 |
|-----|----------|
| `task_analyze.c` | 电压打印、有效性检测、限频无效样本警告 |
| `task_output.c` | 状态切换追踪、超量程检测、限频输出打印 |
| `main.c` | 统计初始化、周期性汇总 |
