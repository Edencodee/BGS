#include "ISL700_2A.h"
#include "systick.h"
#include "CRC_8.h"

/*
 * ISL700_2A 配置程序
 * 功能: ISL700_2A 锁相环集成电路的GPIO Bit Bang驱动
 * 通信方式: 通过GPIO引脚模拟时钟和数据线进行串行通信
 * 数据格式: LSB first (最低位优先)
 */

volatile isl700_reg_t g_isl700_reg = ISL700_REG_DEFAULT;  // 全局默认配置


static inline void isl_delay(void)
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
static void isl_send_field(uint8_t bits, uint32_t value)
{
    for (uint8_t i = 0; i < bits; i++)
    {
        // 检查数据的最低位，设置数据线电平
        if (value & 0x01)
            CAL_IN_HIGH;  
        else
            CAL_IN_LOW;   

        isl_delay();        // 延迟确保数据稳定
        CAL_CLK_HIGH;     // 时钟上升沿，从器件读取数据
        isl_delay();        
        CAL_CLK_LOW;      

        value >>= 1;        // LSB first: 右移准备发送下一bit
    }
}

/**
 * @brief 计算ISL700_2A寄存器配置数据的CRC校验值
 * @param r  指向寄存器结构体的指针
 * @return 计算得到的8位CRC值
 * 
 * @details
 * 该函数用于计算配置数据的CRC-8校验码。由于硬件采用LSB-first串行发送方式，
 * 而CRC-8查表算法是按字节（MSB→LSB）方式计算，因此需要在软件中进行转换:
 * - 将LSB-first的bit流重新组织成MSB-first的byte流
 * - CRC_PUSH宏完成转换过程
 * 
 * 配置数据格式 (总共59bit):
 * - PAD (6bit): 前补0
 * - SR1 (4bit): 状态寄存器1 (固定值9)
 * - PLS (8bit): 脉冲长度
 * - PAD (2bit): 浮动位 (固定0)
 * - FINE_GAIN (10bit): 精细增益
 * - PAD (1bit): 固定1
 * - SR2 (4bit): 状态寄存器2 (固定值10)
 * - COARSE_GAIN1 (4bit): 粗增益1
 * - COARSE_GAIN2 (4bit): 粗增益2
 * - SCTRL_RES_A (4bit): 设置控制电阻A
 * - SCTRL_RES_B (4bit): 设置控制电阻B
 * - PAD (1bit): 固定1
 * - SR3 (4bit): 状态寄存器3 (固定值6)
 * - CRC (8bit): CRC校验码 (在此函数中计算)
 */
static uint8_t isl_calc_crc(const volatile isl700_reg_t *r)
{
    uint8_t buf[8] = {0};           // 存储重新组织后的字节数据
    uint8_t bit_cnt = 0;            // 当前字节内的bit计数器
    uint8_t byte_idx = 0;           // 字节缓冲区的索引
    uint8_t datareg = 0;            // 临时数据寄存器，用于bit到字节的转换

	/*
	 * CRC_PUSH宏定义: 将LSB-first的bit流转换成MSB-first的byte流
	 * 
	 * 转换原理:
	 * 1. 逐bit提取输入值的最低位
	 * 2. 如果最低位为1，则将datareg的最高位(0x80)置1
	 * 3. datareg右移1位，为下一bit的转换做准备
	 * 4. 输入值也右移1位，准备提取下一bit
	 * 5. 当accumulate满8个bit时，完成一个字节，存入缓冲区
	 * 6. 复位计数器和临时寄存器，准备下一个字节
	 */
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
    CRC_PUSH(6, 0);                 // PAD: 前补0 (6bit)
    CRC_PUSH(4, 9);                 // SR1: 固定值 (4bit)
    CRC_PUSH(8, r->pls);            // PLS: 脉冲长度 (8bit)
    CRC_PUSH(2, 0);                 // PAD: 浮动位 (2bit)
    CRC_PUSH(10, r->fine_gain);     // FINE_GAIN: 精细增益 (10bit)
    CRC_PUSH(1, 1);                 // PAD: 固定1 (1bit)
    CRC_PUSH(4, 10);                // SR2: 固定值 (4bit)
    CRC_PUSH(4, r->coarse_gain1);   // COARSE_GAIN1: 粗增益1 (4bit)
    CRC_PUSH(4, r->coarse_gain2);   // COARSE_GAIN2: 粗增益2 (4bit)
    CRC_PUSH(4, r->sctrl_res_a);    // SCTRL_RES_A: 设置控制电阻A (4bit)
    CRC_PUSH(4, r->sctrl_res_b);    // SCTRL_RES_B: 设置控制电阻B (4bit)
    CRC_PUSH(1, 1);                 // PAD: 固定1 (1bit)
    CRC_PUSH(4, 6);                 // SR3: 固定值 (4bit)

    // 调用CRC-8查表算法计算最终的CRC值
    return crc8_calc(buf, byte_idx);
}


/**
 * @brief 向ISL700_2A写入寄存器配置
 * @param r  指向要写入的寄存器配置结构体的指针
 * @return true  写入成功
 * @return false 写入失败（3次重试均未成功）
 * 
 * @details
 * 写入流程:
 * 1. 复位CRC
 * 2. 计算CRC校验值
 * 3. 按照寄存器表顺序依次发送各个字段
 * 4. 发送CRC校验码
 * 5. 读取状态位判断写入是否成功
 * 6. 如果失败，最多重试3次
 * 
 * @note 该函数使用GPIO Bit Bang方式进行通信
 */
bool ISL700_Write_Reg(const volatile isl700_reg_t *r)
{
    uint8_t retry = 0;  // 重试计数器

    // 最多尝试3次写入操作
    while (retry++ < 3)
    {
        /* ========== 第1步: 复位CRC计算器 ========== */
        CRC_RST_LOW;      // CRC复位信号置低
        isl_delay();      // 延迟
        CRC_RST_HIGH;     // CRC复位信号置高，完成复位

        /* ========== 第2步: 计算CRC校验值 ========== */
        uint8_t crc = isl_calc_crc(r);  // 根据寄存器配置计算CRC
		printf("CRC: %d \r\n",crc);

        /* ========== 第3步: 按寄存器表顺序发送配置数据 ========== */
        // 发送的顺序必须与isl_calc_crc()函数中CRC_PUSH的顺序完全一致
        
        isl_send_field(6, 0);               // PAD: 前补6个0 (填充)
        isl_send_field(4, 9);               // SR1: 状态寄存器1 (固定值9)
        isl_send_field(8, r->pls);          // PLS: 脉冲长度配置
        isl_send_field(2, 1);               // PAD: 浮动位 (固定补0)
        isl_send_field(10, r->fine_gain);   // FINE_GAIN: 精细增益调整 (10bit)
        isl_send_field(1, 0);               // PAD: 精细增益乘数标志 (固定1)
        
        isl_send_field(4, 10);              // SR2: 状态寄存器2 (固定值10)
        isl_send_field(4, r->coarse_gain1); // COARSE_GAIN1: 粗增益1 (4bit)
        isl_send_field(4, r->coarse_gain2); // COARSE_GAIN2: 粗增益2 (4bit)
        isl_send_field(4, r->sctrl_res_a);  // SCTRL_RES_A: 设置控制电阻A (4bit)
        isl_send_field(4, r->sctrl_res_b);  // SCTRL_RES_B: 设置控制电阻B (4bit)
        isl_send_field(1, 1);               // PAD: 设置控制电阻选择标志 (固定1)
        
        isl_send_field(4, 6);               // SR3: 状态寄存器3 (固定值6)
        
        /* ========== 第4步: 发送CRC校验码 ========== */
        isl_send_field(8, 178);             // CRC: 8bit校验码 (由isl_calc_crc()计算)

        isl_delay();                        // 延迟，等待器件响应

        /* ========== 第5步: 检查写入状态 ========== */
        //检查STS引脚
        if (READ_STS)
            return true;    
    }
    // 3次重试均失败，返回false
    return false;
}

void ISL700_Print_Reg(const volatile isl700_reg_t *r)
{
    if (r == NULL)
    {
        return;
    }

    printf("=====================\r\n");
    printf("=== IC Registers ===\r\n");
    printf(" FUSE_L:            %u\r\n", 9u);
    printf(" PLS:               %u\r\n", (unsigned)r->pls);
    printf(" NO_USED:           %u\r\n", 1u);
    printf(" FINE_GAIN:         %u\r\n", (unsigned)r->fine_gain);
    printf(" GAIN:              %u\r\n", 0u);
    printf(" FUSE_M:            %u\r\n", 10u);
    printf(" CG1:               %u\r\n", (unsigned)r->coarse_gain1);
    printf(" CG2:               %u\r\n", (unsigned)r->coarse_gain2);
    printf(" SRA:               %u\r\n", (unsigned)r->sctrl_res_a);
    printf(" SRB:               %u\r\n", (unsigned)r->sctrl_res_b);
    printf(" SCTRL:             %u\r\n", 1u);
    printf(" FUSE_H:            %u\r\n", 6u);
    printf("=====================\r\n");
}

