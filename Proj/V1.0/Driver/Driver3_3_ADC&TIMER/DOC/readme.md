0. #### 环境

开发板：GD32C231K8T6   Start

PCB使用MCU：GD32C231G8T6

IDE：VS code \& keil5

Debug输出：RTT


1. #### 介绍

功能：对应ISL700\_2A的规格书，完成了:
        PLS_SO引脚的外部中断:PLS_SO -> PA6 -> EXTI5_9_IRQHandler
        ADC初始化，单次采样   PA1
        DMA初始化  中断停止采样
        TIMER初始化 100KHz采样速率

输出：电源指示灯（绿）常亮；外部中断调用开始采样函数，更新标志，主程序处理采样数据及标志位，并打印输出


#### 附录

对应引脚参考规格书：《ISL700\_2A 集成光电传感器芯片说明书\_2022\_V3》

