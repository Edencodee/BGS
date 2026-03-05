# BGS 光电传感器开关（GD32C231 + ISL700_2A）应用层框架与任务设计（V1 / 边沿索引分拣版）
日期：2026-02-03  
目标：非 RTOS（superloop + event flags），5ms 响应；PLS_SO 双边沿 EXTI；ADC 由定时器触发 + DMA 环形 raw buffer；软件按 PLS_SO 边沿记录的 DMA 位置（CNT）把 raw 分拣到 A_buffer/B_buffer，再计算 VA/VB/V 与 presence，最后按 Light-ON / Dark-ON 输出与指示灯。

---

## 0. 术语与关键量
- **PLS_SO**：A/B 相位标志（数字输入），双边沿触发 EXTI。
- **SH_OUT**：模拟输出（ADC 采样对象）。
- **raw ring buffer**：DMA 持续写入的环形 ADC 原始样本数组。
- **CNT**：DMA 剩余传输计数（`dma_transfer_number_get()`）。
- **toggle event**：PLS_SO 边沿发生时，记录“当前 DMA 写入位置 pos 与新的相位 phase”，用于在块处理时精确切分相位段。
- **A_buffer/B_buffer**：软件分拣后的 A/B 样本缓冲（用于调试/画波形/验证）。
- **VA/VB**：A/B 的统计值（平均或中值）。
- **V**：差分量 `V = VB - VA`（int32）。
- **V_SET**：阈值（可配置）。
- **presence**：物理检测结果 `presence = (V <= V_SET)`（不随 Light/Dark 变化）。
- **output_mode**：Light-ON / Dark-ON（按键切换、Flash 保存）。
- **output_on**：输出“ON 态”概念量（导通有效）。硬件映射：`output_on=1 => NPN 导通 => GPIO=0`。

---

## 1. 总体分层
### 1.1 Driver（你已完成）
- GPIO / EXTI / TIM / ADC+DMA
- GPIO BitBang 配置 IC
- Flash 读写

### 1.2 Service（建议新增，薄封装）
- `meas_srv`：采样服务（raw DMA + toggle 记录 + block 提供）
- `param_srv`：参数服务（Flash 读写、默认值、校验）
- `ic_cfg_srv`：IC 配置服务（BitBang）
- `timebase_srv`：SysTick 节拍 & 软定时

### 1.3 App Tasks（你要实现）
- `Task_Init`：上电流程状态机
- `Task_Key`：按键扫描 & output_mode 切换（保存请求）
- `Task_Meas`：处理 DMA block + toggle 切分 → A/B 分拣 → 生成 5ms 帧（VA/VB/V/presence_raw）
- `Task_Output`：presence 稳定化 + Light/Dark 映射 → output_on → 驱动 NPN GPIO
- `Task_LED`：指示灯（电源灯常亮；工作灯建议显示 output_on）
- `Task_Param`：延时写 Flash（写时建议短暂停测量）

---

## 2. 全局事件与调度（superloop）
### 2.1 事件位（示例）
- `EVT_ADC_HALF`：DMA half-transfer 完成（raw[0..HALF-1] 有新数据）
- `EVT_ADC_FULL`：DMA full-transfer 完成（raw[HALF..RAW-1] 有新数据）
- `EVT_SYSTICK_1MS`：1ms tick
- `EVT_KEY_TICK`：10ms tick（按键扫描）
- `EVT_LED_TICK`：10ms tick（LED 刷新）
- `EVT_PARAM_TICK`：20~50ms tick（参数后台）
- `EVT_FRAME_READY`：Task_Meas 产出新的测量帧（约每 5ms 一次）
- （可选）`EVT_OUTPUT_UPDATE`：按键切换后立即刷新输出

### 2.2 主循环调度顺序（建议）
1. `Task_Init()`（直到进入 RUN）
2. `Task_Key()`（按键优先，切换模式快速生效）
3. `Task_Meas()`（处理 raw block）
4. `Task_Output()`（以 FRAME_READY 或 1ms tick 驱动）
5. `Task_LED()`（以 10ms tick 驱动）
6. `Task_Param()`（后台保存）

---

## 3. 采样与分拣（V1：用 DMA CNT 精确记录边沿位置）
### 3.1 DMA 关键配置建议（100kHz 起步）
- 采样率：`fs = 100kHz`（TIM 触发 ADC 单次采样）
- raw buffer：`RAW_LEN = 200`（uint16_t），DMA circular
- half size：`HALF = 100`（1ms 一个 block）
- DMA 中断：HTF + FTF

> 后续升级 500k/1MHz：保持“每 block 0.5~1ms”的原则重选 RAW_LEN。

### 3.2 PLS_SO EXTI：双边沿
- `exti_init(..., EXTI_TRIG_BOTH)`
- ISR 中读取当前电平，映射为 phase（A/B）。

### 3.3 边沿位置 pos 的计算（核心）
在 EXTI ISR 中：
1. 读 `cnt = dma_transfer_number_get(adc_dma_ch)`  
2. `pos = (RAW_LEN - cnt) % RAW_LEN`  
   - `pos` 表示“DMA 当前写入位置的游标”（近似；差 1 个样本可被丢弃策略吸收）。
3. 读 `level = gpio_input_bit_get(PLS_SO_PORT, PLS_SO_PIN)` 得到新相位 `phase`
4. 将 `{pos, phase, tick}` 写入一个小型 `toggle_queue`（环形队列）

**注意：ISR 要极简，只做记录，不做搬运。**

### 3.4 Task_Meas 如何用 toggle_queue 切分 block
处理一个 block（例如 raw[0..99]）时，Task_Meas：
1. 确定该 block 的 [start_pos, end_pos) 在环形中的范围
2. 从 toggle_queue 中取出所有落在此范围内的 toggle 事件（可能有多个）
3. 将 block 切为若干段 segment，每段对应一个确定 phase
4. 对每段：
   - 丢弃首尾 `N_discard` 个点（按采样率换算，100kHz 时可先取 1）
   - 将剩余点拷贝到对应的 A_buffer 或 B_buffer
5. 累计到 5ms 窗口后生成 frame（VA/VB/V/presence_raw）

---

## 4. 5ms 响应：帧窗口与输出节拍建议
- **测量帧窗口**：5ms（即处理 5 个 1ms block）
- 每帧输出：`VA/VB` 用均值（或中值）计算，得到 `V` 和 `presence_raw`
- 输出稳定化：建议 2~3ms 防抖（仍满足 5ms 需求）

---

## 5. 任务设计（接口清单 + 步骤）
### 5.1 Task_Init（上电状态机）
**目的**：初始化参数、配置 IC、启动采样服务，进入 RUN。  
**输入**：无（由 main 每轮调用）  
**输出**：app 状态转为 RUN；启动测量。

建议接口：
- `void Task_Init(void);`
- `bool App_IsRunning(void);`

步骤：
1. `param_srv_load()`：读取 Flash（含 output_mode、V_SET）
2. `ic_cfg_srv_init()`：BitBang 下发 IC 配置（可重试）
3. `meas_srv_start()`：启动 TIM/ADC/DMA + 使能 EXTI
4. 进入 RUN

---

### 5.2 Task_Key（按键扫描、切换 Light/Dark、保存请求）
**触发**：`EVT_KEY_TICK`（10ms）  
**输入**：按键 GPIO 电平  
**输出**：更新 `output_mode`，置 `param_dirty=1`，可置 `EVT_OUTPUT_UPDATE`

接口建议：
- `void Task_Key(void);`
- `void KeySrv_Scan10ms(void);`
- `bool KeySrv_GetShortPress(void);`

逻辑：
- 短按：`output_mode = (LIGHT_ON <-> DARK_ON)`
- `param_dirty=1`，并启动“2s 延时保存计时器”

---

### 5.3 Task_Meas（采样 block 处理、分拣、生成 frame）
**触发**：`EVT_ADC_HALF` / `EVT_ADC_FULL`  
**输入**：raw block + toggle_queue  
**输出**：更新 `meas_state`；每 5ms 产出 `frame` 并置 `EVT_FRAME_READY`

接口建议：
- `void Task_Meas(void);`
- `bool MeasSrv_GetBlock(MeasBlock* blk);`  // blk: 指针、长度、start_pos
- `bool MeasSrv_PopToggle(ToggleEvent* ev);` // 从队列取边沿事件
- `void MeasSplitter_ProcessBlock(const MeasBlock* blk);`
- `bool MeasFrame_IsReady(void);`
- `void MeasFrame_Get(MeasFrame* frame);`

内部步骤（每个 block）：
1. 获取 block（half/full）
2. 收集该 block 覆盖范围内的 toggle 事件（按 pos 排序）
3. segment 切分并分拣到 A_buffer/B_buffer
4. 当累计窗口到 5ms：
   - 计算 `VA = mean(A_buffer)`
   - 计算 `VB = mean(B_buffer)`
   - `V = VB - VA`
   - `presence_raw = (V <= V_SET)`
   - 输出 frame，清空 A/B 缓冲计数

---

### 5.4 Task_Output（稳定化 + Light/Dark 映射 + NPN 驱动）
**触发**：`EVT_FRAME_READY`（推荐）或 `EVT_SYSTICK_1MS`（可选）  
**输入**：presence_raw、output_mode  
**输出**：presence、output_on、GPIO

接口建议：
- `void Task_Output(void);`
- `void OutputDrv_SetOn(bool on);` // on=1 => GPIO=0
- `void OutputLogic_Update(const MeasFrame* frame);`

逻辑：
1. 防抖（例如 2~3 次 frame 一致才切换，或用 ms 计时）
2. `presence = stable(presence_raw)`
3. `output_on = (output_mode==LIGHT_ON) ? presence : !presence`
4. `OutputDrv_SetOn(output_on)`

---

### 5.5 Task_LED（指示灯）
**触发**：`EVT_LED_TICK`（10ms）  
**输入**：output_on  
**输出**：电源灯、工作灯 GPIO

接口建议：
- `void Task_LED(void);`
- `void LedDrv_SetPwr(bool on);`
- `void LedDrv_SetWork(bool on);`

建议规则：
- 电源灯：常亮
- 工作灯：显示 output_on（语义与 Light/Dark 一致）

---

### 5.6 Task_Param（Flash 延时保存 output_mode / V_SET）
**触发**：`EVT_PARAM_TICK`（20~50ms）  
**输入**：param_dirty、保存截止时间  
**输出**：Flash 写入

接口建议：
- `void Task_Param(void);`
- `void ParamSrv_RequestSave(void);`
- `bool ParamSrv_SaveNow(void);`

策略（推荐安全版）：
- `param_dirty=1` 后启动 2s deadline
- deadline 到期执行保存
- 保存时建议：
  1) `meas_srv_stop()`（停 TIM/ADC/DMA + 禁用 EXTI）
  2) Flash 写（关中断）
  3) `meas_srv_start()` 恢复采样

---

## 6. 数据结构建议（最小集）
- `AppParam`：{magic, ver, V_SET, output_mode, crc}
- `ToggleEvent`：{pos(uint16), phase(uint8), tick(uint32)}
- `MeasBlock`：{ptr(uint16*), len(uint16), start_pos(uint16), which_half}
- `MeasFrame`：{VA, VB, V, presence_raw, a_cnt, b_cnt, timestamp}

---

## 7. 测试与验证建议（实现顺序）
1) 先实现 `meas_srv`：DMA raw + half/full 事件 + CNT 读取
2) 实现 PLS_SO EXTI 双边沿 + toggle_queue（先只计数/打印）
3) 实现 `Task_Meas` 分段分拣到 A_buffer/B_buffer，并能输出 A/B 波形（串口/调试器观察）
4) 实现 `MeasFrame`（5ms 统计）并验证 V 曲线随距离变化
5) 接入 `Task_Output`（先不防抖，再加 2~3ms 防抖）
6) 接入 `Task_Key` 切换 Light/Dark，验证 output_on 与工作灯
7) 接入 `Task_Param` 保存 output_mode（保存时停采样，确保不乱）

---

## 8. 关键参数建议（100kHz 初版）
- `RAW_LEN=200`, `HALF=100`
- `N_discard=1`（段首尾各丢 1 点）
- `FRAME_WINDOW_MS=5`
- `KEY_TICK_MS=10`, `LED_TICK_MS=10`
- `PARAM_SAVE_DELAY_MS=2000`
- `OUTPUT_DEBOUNCE_MS≈2~3`（或等价帧数）