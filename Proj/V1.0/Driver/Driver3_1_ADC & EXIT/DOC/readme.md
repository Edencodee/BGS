0. #### 环境

开发板：GD32C231K8T6   Start

PCB使用MCU：GD32C231G8T6

IDE：VS code \& keil5

Debug输出：RTT


1. #### 介绍

功能：对应ISL700\_2A的规格书，完成了:
        PLS_SO引脚的外部中断:PLS_SO -> PA6 -> EXTI5_9_IRQHandler
        ADC初始化，单次采样   PA1

输出：电源指示灯（绿）常亮；外部中断通过主程序间隔1s触发ADC单次采样，并打印ADC采样数据


#### 附录

对应引脚参考规格书：《ISL700\_2A 集成光电传感器芯片说明书\_2022\_V3》

