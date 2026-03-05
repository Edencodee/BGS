0. #### 环境

开发板：GD32C231K8T6   Start

PCB使用MCU：GD32C231G8T6

IDE：VS code \& keil5

Debug输出：RTT


1. #### 介绍

功能：对应ISL700\_2A的规格书，完成了:
        IC寄存器配置参数结构体，仅定义可修改的参数
        GPIO_BitBang通信，配置IC寄存器
        IC引脚配置，如clk_mode、CRC_rst、Close_pls、freq-c等

输出：打印输出配置信息


#### 附录

对应引脚参考规格书：《ISL700\_2A 集成光电传感器芯片说明书\_2022\_V3》

