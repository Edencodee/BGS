# 响应时间测量 - 快速入门

> **5分钟快速开始测量传感器响应时间**

---

## 📋 前置准备

- [x] GD32C231开发板 + ISL700传感器
- [x] J-Link调试器
- [x] USB转串口模块
- [x] 串口终端软件（PuTTY/SecureCRT等）

---

## 🚀 快速开始（5步）

### Step 1: 配置参数（30秒）

编辑 `App/inc/task_debug.h`：

```c
#define DEBUG_MODE_ENABLE         1U    // 启用调试
#define DEBUG_CAPTURE_GROUPS      10U   // 测量10次
#define ENABLE_TIMING_MEASUREMENT 1U    // 启用时间测量
```

### Step 2: 编译烧录（1分钟）

```
1. 打开 Keil工程: App/Keil_project/App.uvprojx
2. 编译: 按 F7
3. 烧录: 按 F8
```

### Step 3: 连接串口（30秒）

```
串口设置:
- 波特率: 115200
- 数据位: 8
- 停止位: 1
- 校验:   None
```

### Step 4: 启动测试（10秒）

```
1. 按下板子RESET按钮复位
2. 观察启动信息: "App Version: 1.0.2"
3. 等待系统就绪
```

### Step 5: 触发测量（2分钟）

```
缓慢移动金属目标靠近传感器（10-50mm/s）
重复10次，观察串口输出测量数据
```

---

## 📊 输出示例

```
=== Debug Group 1 ===
OBJECT PRESENT
========== Timing Statistics ==========
Sampling Time  :   0.049 ms (49 us)
Analyze Time   :   0.140 ms (140 us)
Output Time    :   0.227 ms (227 us)
---------------------------------------
Total Time     :   0.416 ms (416 us)    ← 总响应时间
Sum Check      :   0.416 ms (416 us)    ← 一致性验证
=======================================
```

---

## ✅ 正常数值参考

| 项目 | 典型值 | 正常范围 |
|-----|-------|---------|
| Sampling Time | 40-50 μs | 30-100 μs |
| Analyze Time | 100-200 μs | 50-300 μs |
| Output Time | 200-250 μs | 150-400 μs |
| **Total Time** | **400-500 μs** | **300-800 μs** |

---

## ❓ 常见问题速查

### Q1: 串口无输出？
```
✓ 检查TX/RX是否接反
✓ 确认波特率是115200
✓ 确认 DEBUG_MODE_ENABLE = 1
```

### Q2: 只显示一次就停止？
```c
// 修改捕获次数
#define DEBUG_CAPTURE_GROUPS 10U  // 改为10或更多
```

### Q3: Sum Check 和 Total Time 不相等？
```
重新编译并烧录程序
检查是否使用了正确的编译优化级别
```

### Q4: 数据波动很大（±50%以上）？
```
可能原因：
- 中断优先级配置不当
- 目标移动速度不稳定
- 系统负载过高

建议：多次测量取平均值（≥50次）
```

---

## 📈 数据分析建议

### 方法1: 手动计算（Excel）

```
1. 复制所有 Total Time 数值到Excel
2. 使用公式:
   - 平均值: =AVERAGE(A1:A10)
   - 最大值: =MAX(A1:A10)
   - 最小值: =MIN(A1:A10)
   - 标准差: =STDEV(A1:A10)
```

### 方法2: Python分析

```python
import numpy as np

# 从串口日志提取的数据
times = [416, 435, 442, 451, 433, 449, 437, 441, 450, 514]

print(f"平均响应时间: {np.mean(times):.1f} μs")
print(f"最大值: {np.max(times)} μs")
print(f"最小值: {np.min(times)} μs")
print(f"标准差: {np.std(times):.1f} μs")
```

---

## 🎯 测试完成检查清单

- [ ] 测量次数 ≥ 10次
- [ ] Sum Check = Total Time（所有次数）
- [ ] Total Time 在正常范围内（300-800 μs）
- [ ] 数据波动合理（标准差 < 20%平均值）
- [ ] 已保存完整测试日志

---

## 📚 详细文档

需要更多信息？查看完整文档：
- [详细操作指南](response_time_measurement_guide.md)
- [测量方法详解](sensor_response_time_measurement.md)

---

## 💡 提示

**生产环境部署前：**
```c
// 关闭调试功能以节省性能
#define DEBUG_MODE_ENABLE         0U
#define ENABLE_TIMING_MEASUREMENT 0U
```

**保存测试数据：**
```
串口工具 → File → Log Session
文件名: timing_test_YYYYMMDD.log
```

---

**祝测试顺利！** 🎉
