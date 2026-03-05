#include "main.h"


/*
 * ISL700_2A 配置程序
 * 功能: ISL700_2A 锁相环集成电路的GPIO Bit Bang驱动
 * 通信方式: 通过GPIO引脚模拟时钟和数据线进行串行通信
 * 数据格式: LSB first (最低位优先)
 */

static inline void IC_Delay1us(void)
{
	// Bit-bang timing delay (blocking, ~1us)
    //for (volatile int i = 0; i < 20; i++);  
	delay_1us(1);  
}

/**
 * @brief 通过GPIO发送一个数据字段
 * @param bits   发送的比特数
 * @param value  要发送的数据值，采用LSB first方式
 * 
 * @details 按LSB first方式逐bit发送数据
 */
static void IC_SendField(uint8_t bits, uint32_t value)
{
    for (uint8_t i = 0; i < bits; i++)
    {
        // 检查数据的最低位，设置数据线电平
        if (value & 0x01)
            CAL_IN_HIGH;  
        else
            CAL_IN_LOW;   

        IC_Delay1us();      // 延迟确保数据稳定
        CAL_CLK_HIGH;     // 时钟上升沿，从器件读取数据
        IC_Delay1us();        
        CAL_CLK_LOW;      

        value >>= 1;      // LSB first: 右移准备发送下一bit
    }
}

/**
 * @brief 计算ISL700_2A寄存器配置数据的CRC校验值
 * @param r  指向 ISL700 配置参数的指针
 * @return 计算得到的8位CRC值
 * 
 * @details
 * 该函数用于计算配置数据的CRC-8校验码。硬件采用LSB-first串行发送方式，
 * CRC-8查表算法按字节（MSB→LSB）方式计算，需要进行转换:
 * - 将LSB-first的bit流重新组织成MSB-first的byte流
 * - CRC_PUSH宏完成转换过程
 * - CRC (8bit):CRC校验码 (在此函数中计算)
 */
static uint8_t IC_CalcCRC(const volatile isl700_config_t *r)
{
    uint8_t buf[8] = {0};           // 存储重新组织后的字节数据
    uint8_t bit_cnt = 0;            // 当前字节内的bit计数器
    uint8_t byte_idx = 0;           // 字节缓冲区的索引
    uint8_t datareg = 0;            // 临时数据寄存器，用于bit到字节的转换

	/* CRC_PUSH:LSB-first bit to MSB-first byte */
    #define CRC_PUSH(bits, val)                     \
        do {                                        \
            uint32_t v = (val);                     \
            for (uint8_t i = 0; i < (bits); i++) {  \
                if (v & 0x01) datareg |= 0x80;  	\
                datareg >>= 1;                   	\
                v >>= 1;                         	\
                if (++bit_cnt == 8) {           	\
                    buf[byte_idx++] = datareg;   	\
                    datareg = 0;                	\
                    bit_cnt = 0;                	\
                }                                   \
            }                                       \
        } while (0)

    /* 按照寄存器表格式依次压入各个字段的bit */
    CRC_PUSH(4, r->fuse_check_low);     // SR[0:3]: Fuse check low (固定值9)
    CRC_PUSH(8, r->pls);                // SR[4:11]: 发射脉冲幅值
    CRC_PUSH(2, r->no_used);            // SR[12:13]: 保留未使用 (固定值1)
    CRC_PUSH(10, r->fine_gain);         // SR[14:23]: 细调增益
    CRC_PUSH(1, r->gain_select);        // SR[24]: 增益选择 (0=x1, 1=x1.5)
    CRC_PUSH(4, r->fuse_check_middle);  // SR[25:28]: Fuse check middle (固定值10)
    CRC_PUSH(4, r->coarse_gain1);       // SR[29:32]: 粗调增益1
    CRC_PUSH(4, r->coarse_gain2);       // SR[33:36]: 粗调增益2
    CRC_PUSH(4, r->sctrl_res_a);        // SR[37:40]: 前级增益a
    CRC_PUSH(4, r->sctrl_res_b);        // SR[41:44]: 前级增益b
    CRC_PUSH(1, r->sctrl_internal);     // SR[45]: 电阻选择 (0=外部, 1=内部)
    CRC_PUSH(4, r->fuse_check_high);    // SR[46:49]: Fuse check high (固定值6)
    CRC_PUSH(8, r->fuse_check_floating);// SR[50:57]: Fuse check floating (固定值0)

    // 调用CRC-8查表算法计算最终的CRC值
    return crc8_calc(buf, byte_idx);
}


/**
 * @brief 向ISL700_2A写入寄存器配置
 * @param cfg  指向 ISL700 配置参数的指针
 * @return true  写入成功
 * @return false 写入失败（3次重试均未成功）
 * 
 * @details
 * 写入流程:
 * 1. 复位CRC
 * 2. 计算CRC校验值（仅使用 ISL700 配置参数）
 * 3. 按照寄存器表顺序依次发送各个字段
 * 4. 发送CRC校验码
 * 5. 读取状态位判断写入是否成功
 * 6. 如果失败，最多重试3次
 * 
 * @note 该函数使用GPIO Bit Bang方式进行通信
 *       仅接收 isl700_config_t，不使用应用层参数
 */
bool ISL700_Write_Reg(const volatile isl700_config_t *cfg)
{
    uint8_t retry = 0;  // 重试计数器

    // 最多尝试3次写入操作
    while (retry++ < 3)
    {
        /* ========== Step1: Reset CRC PIN ========== */
        CRC_RST_LOW;      
        IC_Delay1us();   
        CRC_RST_HIGH;     

        /* ========== Step2: Calculate CRC ========== */
        uint8_t crc = IC_CalcCRC(cfg);  // ISL700_Cfg_t

        /* ========== Step3: Send config data in register order ========== */
        // 发送的顺序必须与IC_CalcCRC()函数中CRC_PUSH的顺序完全一致
        // 注意：只发送 ISL700 配置参数
        IC_SendField(4, cfg->fuse_check_low);      // SR[0:3]: Fuse check low (固定值9)
        IC_SendField(8, cfg->pls);                 // SR[4:11]: 发射脉冲幅值
        IC_SendField(2, cfg->no_used);             // SR[12:13]: 保留未使用 (固定值1)
        IC_SendField(10, cfg->fine_gain);          // SR[14:23]: 细调增益
        IC_SendField(1, cfg->gain_select);         // SR[24]: 增益选择 (0=x1, 1=x1.5)
        IC_SendField(4, cfg->fuse_check_middle);   // SR[25:28]: Fuse check middle (固定值10)
        IC_SendField(4, cfg->coarse_gain1);        // SR[29:32]: 粗调增益1
        IC_SendField(4, cfg->coarse_gain2);        // SR[33:36]: 粗调增益2
        IC_SendField(4, cfg->sctrl_res_a);         // SR[37:40]: 前级增益a
        IC_SendField(4, cfg->sctrl_res_b);         // SR[41:44]: 前级增益b
        IC_SendField(1, cfg->sctrl_internal);      // SR[45]: 电阻选择 (0=外部, 1=内部)
        IC_SendField(4, cfg->fuse_check_high);     // SR[46:49]: Fuse check high (固定值6)
        IC_SendField(8, cfg->fuse_check_floating); // SR[50:57]: Fuse check floating (固定值0)
        
        /* ========== Step4: Send CRC ========== */
        IC_SendField(8, crc);             // CRC: 8bit校验码 (由IC_CalcCRC()计算)

        IC_Delay1us();                     

        /* ========== Step5: Check write status ========== */
        //Check STS pin
        if (READ_STS){
            printf("ISL700 Reg Write Success\r\n");
            return true; 
        }else{
            printf("ISL700 Reg Write Failed, Retry %d\r\n", retry);
        }
               
    }
    // 3次重试均失败，返回false
    return false;
}
