# ADC + DMA 双缓冲采样数据错乱：问题排查与修复记录

> 项目：HP6_BS · GD32C231 + ISL700_2A 光电传感器  
> 版本：V1.0.2  
> 日期：2026-02-24  

---

## 一、系统背景

### 硬件拓扑简述

```
PLS_SO (PA6) ──EXTI双边沿──► ISR 锁存电平 → g_plsState
                                            → g_isStartSampling = true
SH_OUT (PA1) ──ADC_CH1──► ADC ──DMA──► g_A_Buffer[10] / g_B_Buffer[10]
TIMER2 (T2_TRGO) ──────────────────────────────────────────────────────► ADC 触发
```

### 测量原理

| 相位 | PLS_SO 电平 | DMA 写入目标 | 含义 |
|------|------------|-------------|------|
| A | LOW  | `g_A_Buffer` | 背景光期间采样 |
| B | HIGH | `g_B_Buffer` | 发射光期间采样 |

每次完整测量 = A路采样 + B路采样，差分计算：

$$V_{OUT} = V_A - V_B$$

### 关键全局标志

| 标志 | 类型 | 作用 |
|------|------|------|
| `g_isSampleDone` | `__IO bool` | DMA 空闲门控，防止 EXTI 重入 |
| `g_isStartSampling` | `__IO bool` | 主循环触发 MeasureTask 的信号 |
| `g_isA_Done` | `__IO bool` | A路DMA完成标志 |
| `g_isB_Done` | `__IO bool` | B路DMA完成标志 |
| `s_isFillingA` | `static volatile bool` | ISR 分路置位用，记录当前写哪路缓冲 |

---

## 二、问题现象与时间线

### 问题 0：程序只运行一次就"卡死"

**现象**：上电后完成一次 `AnalyzeTask`，之后主循环永远不再进入分析，看起来像死机。

**原因**：EXTI 配置为 `EXTI_TRIG_RISING`（只有上升沿），主循环执行极快，`MeasureTask` 执行时 PLS_SO 引脚仍为 HIGH，每次都只向 `g_B_Buffer` 采样，`g_isA_Done` 永远不被置位，分析条件 `g_isA_Done && g_isB_Done` 永远不成立。

**当时的"解法"**：加 `delay_1ms(5)`，让主循环晚执行几毫秒，此时 PLS 电平已跳低，碰巧读到 LOW 填 A Buffer，属于**时序依赖的巧合**，不可靠。

---

### 问题 1：两路数据整组错乱（A路数据出现在B Buffer中）

**触发条件**：注释了 `delay_1ms(5)` 后，这种整组错乱几乎消失；加上 `delay_1ms(20)` 时，整组错乱频繁复现。

**根因 A：EXTI 只监听上升沿，导致 A/B 路非交替采样**

`bsp_exit.c` 中：
```c
// 错误：仅上升沿
exti_init(PLS_SO_EXTI_LINE, EXTI_INTERRUPT, EXTI_TRIG_RISING);
```
PLS_SO 的下降沿（A路相位）根本不触发中断，只采 B 不采 A。

**根因 B：`g_plsState` 在主循环中延迟读取，PLS 电平已变化**

```c
// 错误：MeasureTask 内才读引脚，此时电平可能已经跳变
void MeasureTask(void) {
    g_plsState = gpio_input_bit_get(...);   // ← 读的是"现在"不是"中断时"
    ...
}
```

**根因 C：`delay_1ms(20)` 扩大了跨轮积累的时间窗**

```
① 上升沿EXTI → 填B → DMA完B → g_isB_Done=true
   ↓ delay 20ms 期间 ↓
   下降沿EXTI → g_plsState=LOW, g_isStartSampling=true

② 循环顶: MeasureTask 填A → DMA完A → g_isA_Done=true
   ↓ delay 20ms 期间 ↓
   上升沿EXTI → g_plsState=HIGH, g_isStartSampling=true

③ 循环顶: MeasureTask 填B（旧B正被覆盖！）
   → g_isStartSampling=false
   → 检查: g_isA_Done=true && g_isB_Done=true ← 两个flag均true！
            A = ②本轮新数据
            B = ①上轮旧数据，且DMA正在覆盖中！
   → AnalyzeTask读B的同时DMA写B → 数据竞争 + 非配对
```

**delay 越长 → 窗口越大 → 跨轮 flag 积累概率越高 → 整组错乱越频繁**。

---

### 问题 2：每组数据 `[0]` 位被前一轮 RDATA 残留污染

**现象**：无论整组是否正确，`A[0]` 或 `B[0]` 经常出现一个与该路信号明显不符的异常值（等于另一路信号的量级）。

**调试截图数据分析（第1组）**：
```
A[0]: 7   ← B 路基线值，B 的 RDATA 残留
A[1..9]: 547~534  ← A 真实信号
B[0]: 533 ← A 路信号值，更早一轮 A 的 RDATA 残留
B[1..9]: 7~8  ← B 真实信号
```

**根因**：GD32 ADC 完成最后一次转换后，`RDATA` 寄存器保留旧值且 **EOC 标志仍然置位**（ADC DMA 请求由 EOC 驱动）。

原来 `ADC_SwitchBuffer` 末尾调用了 `dma_channel_enable`，此时 TIMER2 尚未启动，DMA Enable 后立即响应这个挂起的 EOC，将旧 `RDATA` 写入新缓冲区的 `[0]`，随后 `ADC_StartSampling` 再次 Disable/Enable DMA，TIMER2 启动，真实 10 次采样才写入 `[1..9]`。

```
旧 ADC_SwitchBuffer 末尾：
  dma_channel_enable()   ← DMA 立即响应挂起的 EOC → 写 [0] ← 污染！

旧 ADC_StartSampling：
  dma_channel_disable()
  dma_channel_enable()   ← 实际有效的 Enable
  timer_enable(TIMER2)   ← Timer 触发真实采样 → 写 [1..9]
```

---

## 三、修复措施

### Fix 1：EXTI 改为双边沿触发

**文件**：`BSP/src/bsp_exit.c`

```c
// 修改前
exti_init(PLS_SO_EXTI_LINE, EXTI_INTERRUPT, EXTI_TRIG_RISING);

// 修改后
exti_init(PLS_SO_EXTI_LINE, EXTI_INTERRUPT, EXTI_TRIG_BOTH);
```

---

### Fix 2：在 EXTI ISR 中立即锁存 PLS 电平

**文件**：`App/src/gd32c2x1_it.c`

```c
// 修改前：主循环中才读引脚，时机不确定
void EXTI5_9_IRQHandler(void) {
    ...
    g_isStartSampling = true;
}

// 修改后：中断触发时刻立即锁存
void EXTI5_9_IRQHandler(void) {
    ...
    g_plsState = (SET == gpio_input_bit_get(PLS_SO_PORT, PLS_SO_PIN)) ? kHigh : kLow;
    g_isStartSampling = true;
}
```

---

### Fix 3：`g_isSampleDone = false` 提前到函数入口，关闭重入窗口

**文件**：`App/src/task_meas.c`

```c
// 修改前：SwitchBuffer 期间 EXTI 仍可打断
void MeasureTask(void) {
    ADC_SwitchBuffer(...);
    g_isSampleDone = false;   // ← 太晚
    ADC_StartSampling();
}

// 修改后：最先关闭 EXTI 重入
void MeasureTask(void) {
    g_isSampleDone = false;   // ← 最先执行
    ADC_SwitchBuffer(...);
    ADC_StartSampling();
}
```

---

### Fix 4：启动新路采样前清除该路旧 Done 标志，杜绝跨轮配对

**文件**：`App/src/task_meas.c`

```c
// 修改前：旧 g_isB_Done 可能残留 true，与本轮新 g_isA_Done 误配对
if (g_plsState == kHigh) {
    ADC_SwitchBuffer(g_B_Buffer);
}

// 修改后：先清旧 flag 再开始采样
if (g_plsState == kHigh) {
    g_isB_Done = false;          // ← 旧 flag 立即作废
    ADC_SwitchBuffer(g_B_Buffer);
} else {
    g_isA_Done = false;
    ADC_SwitchBuffer(g_A_Buffer);
}
```

---

### Fix 5：`ADC_SwitchBuffer` 不再 Enable DMA，统一由 `ADC_StartSampling` 负责

**文件**：`BSP/src/bsp_adc.c`

```c
// 修改前：SwitchBuffer 末尾多余的 Enable 触发 [0] 污染
void ADC_SwitchBuffer(int16_t *buf) {
    dma_channel_disable(ADC_DMA_CHANNEL);
    dma_memory_address_config(...);
    dma_transfer_number_config(...);
    dma_channel_enable(ADC_DMA_CHANNEL);   // ← 删除
}

// 修改后：只配置地址，不 Enable
void ADC_SwitchBuffer(int16_t *buf) {
    dma_channel_disable(ADC_DMA_CHANNEL);
    dma_memory_address_config(...);
    dma_transfer_number_config(...);
    // Enable 交给 ADC_StartSampling
}
```

---

### Fix 6：Enable DMA 前清除 ADC EOC 标志，消除 RDATA 残留

**文件**：`BSP/src/bsp_adc.c`

```c
// 修改前：DMA Enable 时 EOC 挂起 → 立即写旧 RDATA 到 [0]
void ADC_StartSampling(void) {
    dma_channel_disable(ADC_DMA_CHANNEL);
    dma_transfer_number_config(...);
    dma_channel_enable(ADC_DMA_CHANNEL);   // ← [0] 被旧 RDATA 污染
    timer_enable(TIMER2);
}

// 修改后：先清 EOC 再 Enable
void ADC_StartSampling(void) {
    dma_channel_disable(ADC_DMA_CHANNEL);
    dma_transfer_number_config(...);
    adc_flag_clear(ADC_FLAG_EOC);          // ← 撤销挂起的 DMA 请求
    dma_channel_enable(ADC_DMA_CHANNEL);
    timer_enable(TIMER2);
}
```

---

## 四、修复前后对比

| 问题 | 修复前 | 修复后 |
|------|--------|--------|
| 程序运行一次卡死 | EXTI 仅上升沿，A路永不采样 | EXTI 双边沿，A/B 均可触发 |
| g_plsState 读时机错误 | MeasureTask 中延迟读引脚 | EXTI ISR 中断时刻立即锁存 |
| 整组数据跨轮配对 | delay 扩大窗口，旧 flag 残留触发误配 | 启动采样前先清该路旧 Done 标志 |
| DMA 传输中途换缓冲 | g_isSampleDone=false 置位太晚 | 函数入口最先置 false |
| [0] 位被旧 RDATA 污染 | SwitchBuffer 早于 Timer 多次 Enable DMA | 只在 StartSampling 中统一 Enable，且先清 EOC |

---

## 五、延伸：ADC + DMA 嵌入式采样知识点总结

### 5.1 ADC EOC 标志与 DMA 挂起请求

- ADC 每完成一次转换，EOC（End Of Conversion）标志置位，同时向 DMA 发出请求。
- 如果此时 DMA 未 Enable，这个请求**会被硬件保持（pending）**，直到 DMA 被 Enable 后立即响应。
- **结论**：Enable DMA 前必须用 `adc_flag_clear(ADC_FLAG_EOC)` 清除 EOC，否则必然触发一次无效 DMA 传输写入 `[0]`。

### 5.2 Timer 触发 ADC 采样的时序要求

```
正确时序：
  配置 DMA 地址 → 清 EOC → Enable DMA → Enable Timer
                                         ↑ Timer 产生 TRGO → ADC 转换 → 写 [0]
错误时序：
  配置 DMA 地址 → Enable DMA → Enable Timer
                 ↑ 残留 EOC 驱动 DMA 立即写 [0]（旧值）→ Timer 触发写 [1..9]
```

- **规则**：DMA enable 和 Timer enable 紧邻，中间不能插入可以产生 ADC 触发的路径。

### 5.3 多缓冲区切换的并发安全原则

1. **标志清零必须先于 DMA 启动**：启动新路采样前，先清该路旧的 Done 标志，防止主循环检查时用到上一轮的残留 flag 造成误配对。
2. **禁止在 DMA 传输期间重配目标地址**：必须先 `dma_channel_disable` 再修改 `memory_addr`，否则 DMA 内部指针与配置寄存器不一致。
3. **"忙"标志置位时机要最早**：`g_isSampleDone = false` 应在任何共享资源操作（SwitchBuffer）之前，才能关闭中断重入窗口。

### 5.4 EXTI 触发沿选择

| 场景 | 建议 |
|------|------|
| 只关心信号到来（单向变化） | `EXTI_TRIG_RISING` 或 `FALLING` |
| A/B 双相位切换（如 PLS_SO） | **必须** `EXTI_TRIG_BOTH`，否则一路相位永远不被触发 |
| 防抖动要求高 | 配合软件防抖或硬件滤波器 |

### 5.5 ISR 中应立即锁存引脚电平，不要延迟到主循环读取

```c
// 错误：主循环读时 PLS 可能已跳变（尤其高频信号 + 长主循环周期）
void EXTI_Handler(void) {
    g_isStartSampling = true;           // 只设 flag
}
void MeasureTask(void) {
    state = gpio_input_bit_get(...);    // ← 此时电平已变！
}

// 正确：在 ISR 最短路径内立即锁存
void EXTI_Handler(void) {
    g_plsState = gpio_input_bit_get(...);   // ← 中断触发瞬间的电平才准确
    g_isStartSampling = true;
}
```

- **规则**：所有与"触发边沿相关的状态"必须在 ISR 中立即记录，ISR 执行期间引脚电平最接近触发瞬间。

### 5.6 delay 在主循环中的危害

裸机 superloop 中，主循环的 `delay_1ms(N)` 会：
- **扩大中断积累窗口**：delay 期间多个 EXTI 可以连续触发并更新 `g_plsState`，但 `g_isStartSampling` 只保留最后一次，中间的采样请求丢失。
- **允许 Done 标志跨轮积累**：`g_isA_Done` 来自本轮，`g_isB_Done` 来自上轮，误配对触发分析。
- **造成数据竞争**：分析任务读缓冲区时，新的 DMA 可能已经在写同一块缓冲区。

> **原则**：裸机主循环中不要使用阻塞 delay，改用非阻塞时间片（SysTick 计数）或状态机节拍。

### 5.7 ADC 采样值的滤波建议

| 方法 | 适用场景 | 代码复杂度 |
|------|---------|-----------|
| 算术平均 | 噪声均匀，无明显毛刺 | 低 |
| 截尾均值（Trimmed Mean） | 有偶发脉冲毛刺（本项目采用） | 低 |
| 中位数 | 毛刺较多，样本量小 | 低 |
| 最密集簇均值（聚类窗口） | 样本分散但有稳定核心（本项目二级滤波） | 中 |
| IIR 低通 | 实时流式采样，延迟可接受 | 低 |
| Kalman 滤波 | 高精度，噪声模型已知 | 高 |

本项目两级滤波：
```
ADC 原始 10 点 → 截尾均值（去掉最大最小各1个）→ V_OUT 计算
→ 累积 20 组 V_OUT → 聚类窗口8（最密集8个）均值 → 最终输出
```

### 5.8 双缓冲设计最佳实践（裸机）

```
推荐结构：
  缓冲 A ──────► [分析任务读]
  缓冲 B ──DMA──► [ADC写入中]

  A 满 → 通知分析任务 → 分析读 A → 同时 DMA 可写 B
  B 满 → 通知分析任务 → 分析读 B → 同时 DMA 可写 A

关键约束：
  1. 分析任务读某缓冲时，DMA 绝对不能写同一缓冲
  2. 切换缓冲前必须确认 DMA 已停止（dma_channel_disable 后再改地址）
  3. 每次切换前清除目标缓冲的"完成"标志
```

---

## 六、最终时序（修复后）

```
PLS上升沿
  → EXTI ISR: g_plsState=HIGH, g_isStartSampling=true
  → 主循环: MeasureTask()
      g_isSampleDone=false          // 关闭 EXTI 重入窗口（最先执行）
      g_isB_Done=false              // 清除该路旧 flag
      ADC_SwitchBuffer(g_B_Buffer)  // 配置 DMA 地址（不 Enable）
      adc_flag_clear(ADC_FLAG_EOC)  // 清除挂起的旧 EOC
      dma_channel_enable()          // Enable DMA（此时无挂起请求）
      timer_enable(TIMER2)          // Timer 触发真实采样
  → DMA 完成中断: g_isB_Done=true, g_isSampleDone=true

PLS下降沿
  → EXTI ISR: g_plsState=LOW, g_isStartSampling=true
  → 主循环: MeasureTask()
      g_isSampleDone=false
      g_isA_Done=false              // 清除该路旧 flag
      ADC_SwitchBuffer(g_A_Buffer)
      adc_flag_clear(ADC_FLAG_EOC)
      dma_channel_enable()
      timer_enable(TIMER2)
  → DMA 完成中断: g_isA_Done=true, g_isSampleDone=true

主循环检查:
  g_isA_Done && g_isB_Done → true（同一轮 A+B 均完成）
  → g_isA_Done=false, g_isB_Done=false
  → AnalyzeTask()  // 读缓冲区时 DMA 已停止，无竞争
  → OutputTask()
  → LedTask()
```

---

*文档版本：V1.0.2 · 2026-02-24*
