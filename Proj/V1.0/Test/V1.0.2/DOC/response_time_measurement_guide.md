# 响应时间测量操作指南

**版本：** V1.0.2  
**日期：** 2026年3月2日  
**适用项目：** HP6_BS 接近传感器测量系统

---

## 目录
- [1. 测量功能概述](#1-测量功能概述)
- [2. 配置步骤](#2-配置步骤)
- [3. 编译与烧录](#3-编译与烧录)
- [4. 硬件连接](#4-硬件连接)
- [5. 运行测试](#5-运行测试)
- [6. 数据解读](#6-数据解读)
- [7. 常见问题](#7-常见问题)

---

## 1. 测量功能概述

### 1.1 测量内容

本系统可测量传感器响应过程中的四个关键时间点和三个阶段耗时：

| 时间点 | 说明 | 触发事件 |
|-------|------|---------|
| **T0** | 开始采样 | EXTI中断触发（目标靠近） |
| **T1** | 采样完成 | ADC DMA完成（A+B通道） |
| **T2** | 分析完成 | AnalyzeTask执行完毕 |
| **T3** | 输出完成 | OutputTask + LedTask完成 |

| 阶段 | 计算公式 | 说明 |
|-----|---------|------|
| **Sampling Time** | T1 - T0 | ADC采样耗时 |
| **Analyze Time**  | T2 - T1 | 数据分析耗时 |
| **Output Time**   | T3 - T2 | 输出控制耗时 |
| **Total Time**    | T3 - T0 | 总响应时间 |

### 1.2 测量精度

- **时间分辨率**：微秒级（μs）
- **理论精度**：±1-10μs（受中断延迟影响）
- **测量范围**：0 ~ 4,294,967 ms（32位计数器）

---

## 2. 配置步骤

### 2.1 配置测量参数

编辑 `App/inc/task_debug.h` 文件，配置测量参数：

```c
/* ==================== Debug Configuration ==================== */
#define DEBUG_MODE_ENABLE         1U    // 1=启用调试模式，0=禁用
#define DEBUG_CAPTURE_GROUPS      10U   // 捕获次数（1-1000）
#define DEBUG_USE_DEFAULT_CONFIG  0U    // 1=使用默认配置，0=从Flash加载

/* ==================== Timing Measurement ==================== */
#define ENABLE_TIMING_MEASUREMENT 1U    // 1=启用时间测量，0=禁用
```

**参数说明：**

| 参数 | 推荐值 | 说明 |
|-----|-------|------|
| `DEBUG_MODE_ENABLE` | 1 | 测试时设为1，正式版本设为0 |
| `DEBUG_CAPTURE_GROUPS` | 10-100 | 捕获次数，建议≥10次进行统计分析 |
| `DEBUG_USE_DEFAULT_CONFIG` | 0 | 使用Flash存储的参数配置 |
| `ENABLE_TIMING_MEASUREMENT` | 1 | 测试时设为1，正式版本可设为0节省性能 |

### 2.2 配置串口输出

确保串口正确配置：

```c
// 在 BSP/src/bsp.c 中确认串口配置
// 波特率：115200
// 数据位：8
// 停止位：1
// 校验：None
```

---

## 3. 编译与烧录

### 3.1 使用Keil MDK编译

**步骤：**

1. 打开Keil项目文件：
   ```
   App/Keil_project/App.uvprojx
   ```

2. 选择编译配置：
   - Target：`App_GD32C231`
   - Mode：`Debug` 或 `Release`

3. 编译项目：
   - 快捷键：`F7`
   - 或菜单：`Project` → `Build Target`

4. 检查编译结果：
   ```
   Build Output:
   compiling...
   linking...
   Program Size: Code=XXX RO-data=XXX RW-data=XXX ZI-data=XXX
   0 Error(s), 0 Warning(s).
   ```

### 3.2 烧录程序

**使用J-Link烧录：**

1. 连接J-Link调试器到目标板

2. 在Keil中下载程序：
   - 快捷键：`F8`
   - 或菜单：`Flash` → `Download`

3. 确认烧录成功：
   ```
   Programming Done.
   Verify OK.
   ```

**使用命令行烧录：**

```batch
cd App/Keil_project
JLink -device GD32C231 -if SWD -speed 4000
> loadfile output/Project.hex
> r
> g
> exit
```

---

## 4. 硬件连接

### 4.1 基本连接

```
┌─────────────────┐
│   GD32C231开发板 │
├─────────────────┤
│ VCC  ──────────┼─── 3.3V电源
│ GND  ──────────┼─── GND
│ PA9  ──────────┼─── USB转串口 RX（接收板子发送的数据）
│ PA10 ──────────┼─── USB转串口 TX（发送数据到板子）
│ SWDIO ─────────┼─── J-Link SWDIO
│ SWCLK ─────────┼─── J-Link SWCLK
└─────────────────┘
```

### 4.2 传感器连接

```
ISL700_2A_38VC 接近传感器：
- 电源：24V DC（根据传感器规格）
- 输出：连接到MCU的GPIO/ADC输入
- 检测距离：根据参数配置（通常0-38mm）
```

### 4.3 串口终端设置

使用串口调试助手（如PuTTY、SecureCRT、Tera Term）：

| 参数 | 设置值 |
|-----|-------|
| 端口 | COMX（根据实际COM口） |
| 波特率 | 115200 |
| 数据位 | 8 |
| 停止位 | 1 |
| 校验 | None |
| 流控 | None |

---

## 5. 运行测试

### 5.1 启动测试

**步骤：**

1. **连接串口终端**
   ```
   打开串口工具，选择正确的COM口，波特率115200
   ```

2. **复位开发板**
   ```
   按下开发板的RESET按钮，或重新上电
   ```

3. **观察启动信息**
   ```
   App Version: 1.0.2
   ISL700 Configuration:
   - Detection Distance: 38mm
   - Sensitivity: High
   - Output Mode: NPN
   ...
   ```

4. **等待测量开始**
   ```
   系统会自动等待物体靠近传感器
   ```

### 5.2 触发测量

**方法1：物理目标触发**
```
缓慢移动标准测试目标（如金属板）靠近传感器
- 建议速度：10-50 mm/s
- 接近角度：垂直于传感器感应面
- 目标材质：金属（铁、铝等）
```

**方法2：自动连续测量**
```
如果系统配置为连续模式，会自动触发测量
每次检测到物体进入/离开会自动记录一次
```

### 5.3 测量进度

观察串口输出，会显示测量进度：

```
=== Debug Group 1 ===
OBJECT PRESENT
========== Timing Statistics ==========
Sampling Time  :   0.049 ms (49 us)
Analyze Time   :   0.140 ms (140 us)
Output Time    :   0.227 ms (227 us)
---------------------------------------
Total Time     :   0.416 ms (416 us)
Sum Check      :   0.416 ms (416 us)
=======================================

=== Debug Group 2 ===
OBJECT PRESENT
========== Timing Statistics ==========
Sampling Time  :   0.049 ms (49 us)
Analyze Time   :   0.159 ms (159 us)
Output Time    :   0.227 ms (227 us)
---------------------------------------
Total Time     :   0.435 ms (435 us)
Sum Check      :   0.435 ms (435 us)
=======================================

...（继续直到设定次数）

=== DEBUG COMPLETE: 10 groups captured ===
```

### 5.4 结束测试

测试完成后，系统会：
1. 显示 `DEBUG COMPLETE: XX groups captured`
2. 主循环暂停（`g_pauseMainLoop = true`）
3. 保持最后的输出状态

---

## 6. 数据解读

### 6.1 输出格式说明

```
========== Timing Statistics ==========
Sampling Time  :   0.049 ms (49 us)      ← ADC采样耗时
Analyze Time   :   0.140 ms (140 us)     ← 数据分析耗时
Output Time    :   0.227 ms (227 us)     ← 输出控制耗时
---------------------------------------
Total Time     :   0.416 ms (416 us)     ← 总响应时间
Sum Check      :   0.416 ms (416 us)     ← 一致性校验
=======================================
```

### 6.2 性能基准

**典型值（参考）：**

| 阶段 | 典型值 | 正常范围 | 说明 |
|-----|-------|---------|------|
| Sampling Time | 40-50 μs | 30-100 μs | ADC + DMA采样时间 |
| Analyze Time | 100-200 μs | 50-300 μs | 数据处理和算法 |
| Output Time | 200-250 μs | 150-400 μs | 输出状态控制 |
| **Total Time** | **400-500 μs** | **300-800 μs** | **总响应时间** |

### 6.3 异常数据识别

**⚠️ 异常情况：**

```
Analyze Time   :   1.153 ms (1153 us)    ← 异常！比正常值大10倍
```

**可能原因：**
- ✅ **中断抢占**：SysTick或其他高优先级中断插入
- ✅ **Flash等待**：CPU从Flash读取指令的等待周期
- ✅ **DMA冲突**：DMA与CPU争抢总线
- ✅ **printf开销**：串口发送中断（如果在测量期间调用）

**验证方法：**
```
查看 Sum Check 是否等于 Total Time：
- 相等 ✓ → 测量准确，异常是真实的执行时间波动
- 不等 ✗ → 测量错误，需要检查代码
```

### 6.4 数据统计分析

**手动统计（Excel/Python）：**

1. 将串口数据保存到文本文件
2. 提取所有 Total Time 数值
3. 计算统计指标：

```python
# Python示例
import numpy as np

times = [416, 435, 442, 451, 433, ...]  # 从测试数据提取

print(f"平均值: {np.mean(times):.2f} μs")
print(f"最小值: {np.min(times)} μs")
print(f"最大值: {np.max(times)} μs")
print(f"标准差: {np.std(times):.2f} μs")
print(f"P95值: {np.percentile(times, 95):.2f} μs")
print(f"P99值: {np.percentile(times, 99):.2f} μs")
```

**典型输出：**
```
平均值: 442.50 μs
最小值: 416 μs
最大值: 514 μs
标准差: 15.32 μs
P95值: 480.00 μs (95%的测量值小于此值)
P99值: 500.00 μs (99%的测量值小于此值)
```

---

## 7. 常见问题

### 7.1 串口无输出

**问题现象：** 串口终端没有任何输出

**排查步骤：**

1. ✅ 检查串口连接
   ```
   - TX/RX是否接反？
   - 波特率是否设置为115200？
   - COM口是否正确？
   ```

2. ✅ 检查代码配置
   ```c
   // 确认 DEBUG_MODE_ENABLE 已启用
   #define DEBUG_MODE_ENABLE 1U
   
   // 确认 ENABLE_TIMING_MEASUREMENT 已启用
   #define ENABLE_TIMING_MEASUREMENT 1U
   ```

3. ✅ 检查程序是否运行
   ```
   - LED是否闪烁？
   - 使用调试器单步执行检查
   ```

### 7.2 只打印一次就停止

**问题现象：** 只显示一组数据后不再更新

**原因：**
```c
// DEBUG_CAPTURE_GROUPS 设置为 1
#define DEBUG_CAPTURE_GROUPS 1U
```

**解决方法：**
```c
// 修改为更大的值
#define DEBUG_CAPTURE_GROUPS 10U  // 或更多
```

### 7.3 时间数据Sum Check不一致

**问题现象：**
```
Total Time     :   0.416 ms (416 us)
Sum Check      :   0.435 ms (435 us)  ← 不一致！
```

**原因：** 时间戳记录点错误或被优化

**解决方法：**
1. 检查编译优化级别（建议使用-O1或-O2）
2. 确认所有时间戳变量使用了volatile修饰
3. 重新编译并烧录程序

### 7.4 测量值波动很大

**问题现象：** 同一条件下，响应时间波动超过±50%

**可能原因：**
- 中断优先级配置不合理
- 系统负载过高
- DMA配置有问题
- 目标物体移动速度不稳定

**解决方法：**
1. 提高关键中断优先级：
   ```c
   NVIC_SetPriority(EXTI4_15_IRQn, 0);  // 最高优先级
   NVIC_SetPriority(DMA_Channel0_IRQn, 1);
   ```

2. 减少非关键任务：
   ```c
   // 禁用不必要的printf调用
   // 将调试打印移到测量完成后
   ```

3. 使用统计方法过滤异常值：
   ```
   取100次测量的中位数或去除最大最小10%后的平均值
   ```

### 7.5 程序不触发测量

**问题现象：** 物体靠近传感器，但不触发测量

**排查步骤：**

1. ✅ 检查传感器连接和供电
2. ✅ 使用万用表/示波器验证传感器输出
3. ✅ 检查EXTI配置是否正确
4. ✅ 在EXTI中断函数中添加LED指示验证中断触发

### 7.6 如何导出数据进行分析

**方法1：串口工具自动保存**
```
大部分串口工具都有"保存日志"功能
- SecureCRT: Session → Log Session
- Tera Term: File → Log
```

**方法2：重定向到文件（Windows）**
```powershell
# 使用mode命令配置串口，然后重定向输出
mode COM3 BAUD=115200 PARITY=N DATA=8 STOP=1
type COM3 > output.txt
```

**方法3：修改代码输出CSV格式**
```c
// 在 task_debug.c 中添加CSV输出函数
void Debug_PrintTimingCSV(void) {
    printf("%u,%u,%u,%u\n",
           g_timing_stats.sampling_time,
           g_timing_stats.analyze_time,
           g_timing_stats.output_time,
           g_timing_stats.total_time);
}
```

---

## 8. 高级操作

### 8.1 修改输出格式

如需自定义输出格式，编辑 `App/src/task_debug.c`：

```c
void Debug_PrintTimingStats(void)
{
    // 简化版输出
    printf("T_total=%u us (S=%u A=%u O=%u)\n",
           g_timing_stats.total_time,
           g_timing_stats.sampling_time,
           g_timing_stats.analyze_time,
           g_timing_stats.output_time);
}
```

### 8.2 禁用时间测量（正式版本）

```c
// 在 task_debug.h 中修改
#define ENABLE_TIMING_MEASUREMENT 0U    // 禁用
#define DEBUG_MODE_ENABLE         0U    // 禁用调试模式
```

编译后所有测量代码会被优化掉，不占用程序空间和运行时间。

### 8.3 添加采样数据打印

```c
// 在main.c中取消注释
if (debug_group_count < DEBUG_CAPTURE_GROUPS) {
    debug_group_count++;
    printf("=== Debug Group %d ===\r\n", debug_group_count);
    Debug_Print();  // ← 取消注释这行
}
```

会额外输出ADC采样的原始数据。

---

## 9. 快速参考

### 9.1 操作流程图

```
┌─────────────────────────────────────┐
│ 1. 配置 task_debug.h                │
│    - DEBUG_CAPTURE_GROUPS = 10      │
│    - ENABLE_TIMING_MEASUREMENT = 1  │
└──────────────┬──────────────────────┘
               │
┌──────────────▼──────────────────────┐
│ 2. 编译烧录程序 (Keil F7 → F8)      │
└──────────────┬──────────────────────┘
               │
┌──────────────▼──────────────────────┐
│ 3. 连接串口 (115200,8,N,1)         │
└──────────────┬──────────────────────┘
               │
┌──────────────▼──────────────────────┐
│ 4. 复位板子，观察启动信息            │
└──────────────┬──────────────────────┘
               │
┌──────────────▼──────────────────────┐
│ 5. 触发测量（移动目标靠近传感器）     │
└──────────────┬──────────────────────┘
               │
┌──────────────▼──────────────────────┐
│ 6. 观察串口输出，记录数据            │
└──────────────┬──────────────────────┘
               │
┌──────────────▼──────────────────────┐
│ 7. 测试完成，分析统计数据            │
└─────────────────────────────────────┘
```

### 9.2 关键文件清单

| 文件 | 路径 | 功能 |
|-----|------|------|
| **task_debug.h** | `App/inc/` | 调试配置和时间测量接口 |
| **task_debug.c** | `App/src/` | 时间测量和打印实现 |
| **main.c** | `App/src/` | 主程序，包含测量流程 |
| **App.uvprojx** | `App/Keil_project/` | Keil工程文件 |

### 9.3 快速配置表

| 使用场景 | DEBUG_MODE | TIMING_MEASURE | CAPTURE_GROUPS |
|---------|-----------|----------------|----------------|
| 开发调试 | 1 | 1 | 10-100 |
| 性能测试 | 1 | 1 | 100-1000 |
| 功能验证 | 1 | 0 | 10 |
| 正式发布 | 0 | 0 | N/A |

---

## 附录

### A. 相关文档

- [传感器响应时间测量方法详解](sensor_response_time_measurement.md)
- [代码架构文档](app_architecture_tasks.md)
- [Flash参数配置指南](nvm_param_add_param_guide.md)

### B. 技术支持

如遇到问题，请提供以下信息：
1. 完整的串口输出日志
2. 硬件连接照片
3. 配置参数截图
4. 编译输出信息

### C. 版本历史

| 版本 | 日期 | 修改内容 |
|-----|------|---------|
| 1.0 | 2026-03-02 | 初始版本，支持基本时间测量 |

---

**文档结束**
