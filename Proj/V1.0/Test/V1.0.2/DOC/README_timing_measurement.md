# 响应时间测量系统 - 文档索引

## 📖 文档导航

### 🚀 快速入门
新手推荐从这里开始：
- **[5分钟快速入门](QUICK_START_timing_measurement.md)** - 最快上手指南

### 📘 操作指南
完整的操作手册：
- **[响应时间测量操作指南](response_time_measurement_guide.md)** - 详细操作步骤

### 📚 技术文档
深入了解测量原理：
- **[传感器响应时间测量方法详解](sensor_response_time_measurement.md)** - 理论与实践

### 🏗️ 架构文档
代码结构参考：
- **[应用架构文档](app_architecture_tasks.md)** - 系统架构说明

---

## 🔍 按需求查找

### 我想...

| 需求 | 推荐文档 | 章节 |
|-----|---------|------|
| **快速开始测量** | [快速入门](QUICK_START_timing_measurement.md) | 完整文档 |
| **了解测量原理** | [测量方法详解](sensor_response_time_measurement.md) | 第2章 |
| **配置测量参数** | [操作指南](response_time_measurement_guide.md) | 第2章 |
| **解决问题** | [操作指南](response_time_measurement_guide.md) | 第7章 |
| **分析数据** | [操作指南](response_time_measurement_guide.md) | 第6章 |
| **修改代码** | [操作指南](response_time_measurement_guide.md) | 第8章 |
| **了解代码结构** | [测量方法详解](sensor_response_time_measurement.md) | 第3章 |

---

## 📂 当前测量系统文件结构

```
V1.0.2/
├── App/
│   ├── inc/
│   │   ├── task_debug.h          ← 调试配置（修改测量参数）
│   │   └── ...
│   ├── src/
│   │   ├── main.c                ← 主程序（测量流程）
│   │   ├── task_debug.c          ← 调试实现（时间测量）
│   │   └── ...
│   └── Keil_project/
│       └── App.uvprojx           ← Keil工程文件
│
├── BSP/
│   └── src/
│       └── systick.c             ← 系统时钟（时间戳获取）
│
└── DOC/
    ├── QUICK_START_timing_measurement.md       ← 快速入门 ⭐
    ├── response_time_measurement_guide.md      ← 完整操作指南 ⭐
    ├── sensor_response_time_measurement.md     ← 理论详解
    └── README_timing_measurement.md            ← 当前文档
```

---

## ⚙️ 关键配置文件

### task_debug.h（调试配置）
```c
位置: App/inc/task_debug.h

关键配置项:
#define DEBUG_MODE_ENABLE         1U    // 调试模式开关
#define DEBUG_CAPTURE_GROUPS      10U   // 测量次数
#define ENABLE_TIMING_MEASUREMENT 1U    // 时间测量开关
```

### main.c（主程序）
```c
位置: App/src/main.c

测量流程:
1. Debug_RecordTimestamp_Start()        // T0: 开始采样
2. MeasureTask()                        // 执行采样
3. Debug_RecordTimestamp_SampleDone()   // T1: 采样完成
4. AnalyzeTask()                        // 数据分析
5. Debug_RecordTimestamp_AnalyzeDone()  // T2: 分析完成
6. OutputTask() + LedTask()             // 输出控制
7. Debug_RecordTimestamp_OutputDone()   // T3: 输出完成
8. Debug_CalculateAndPrintTiming()      // 计算并打印结果
```

---

## 🎯 测量数据说明

### 时间点定义
```
T0: 开始采样      - EXTI中断触发（物体靠近）
T1: 采样完成      - ADC DMA完成
T2: 分析完成      - AnalyzeTask执行完毕
T3: 输出完成      - OutputTask + LedTask完成
```

### 阶段耗时计算
```
Sampling Time = T1 - T0    (ADC采样耗时)
Analyze Time  = T2 - T1    (数据分析耗时)
Output Time   = T3 - T2    (输出控制耗时)
Total Time    = T3 - T0    (总响应时间)

一致性验证: Total Time = Sampling + Analyze + Output
```

---

## 📊 正常性能指标

| 阶段 | 典型值 | 正常范围 |
|-----|-------|---------|
| Sampling Time | 40-50 μs | 30-100 μs |
| Analyze Time | 100-200 μs | 50-300 μs |
| Output Time | 200-250 μs | 150-400 μs |
| **Total Time** | **400-500 μs** | **300-800 μs** |

---

## 🔧 常用操作速查

### 启用时间测量
```c
// 编辑 App/inc/task_debug.h
#define ENABLE_TIMING_MEASUREMENT 1U
```

### 修改测量次数
```c
// 编辑 App/inc/task_debug.h
#define DEBUG_CAPTURE_GROUPS 100U  // 改为需要的次数
```

### 禁用调试（正式版本）
```c
// 编辑 App/inc/task_debug.h
#define DEBUG_MODE_ENABLE 0U
#define ENABLE_TIMING_MEASUREMENT 0U
```

### 编译并烧录
```
Keil MDK:
1. 编译: F7
2. 烧录: F8
```

### 查看串口输出
```
串口设置: 115200, 8, N, 1
工具: PuTTY / SecureCRT / Tera Term
```

---

## 🐛 问题排查

| 问题 | 快速解决 |
|-----|---------|
| 串口无输出 | 检查TX/RX接线、波特率、调试开关 |
| 只测量一次 | 增加 DEBUG_CAPTURE_GROUPS 值 |
| 数据不一致 | 重新编译烧录 |
| 波动很大 | 多次测量取统计值 |

详细排查步骤请参考[操作指南第7章](response_time_measurement_guide.md#7-常见问题)。

---

## 📞 技术支持

遇到问题请提供：
1. ✅ 完整串口输出日志
2. ✅ 配置参数截图
3. ✅ 硬件连接说明
4. ✅ 编译输出信息

---

## 📝 版本信息

- **系统版本**: V1.0.2
- **文档版本**: 1.0
- **更新日期**: 2026年3月2日

---

## 🔗 相关链接

- [项目根目录](../)
- [应用代码](../App/)
- [其他文档](./README.md)

---

**开始测量？** → [点击这里查看快速入门](QUICK_START_timing_measurement.md) 🚀
