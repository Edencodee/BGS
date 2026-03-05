#include "main.h"

#define APP_VERSION		"1.0.0"
#define APPLICATION_ADDRESS 0x8007000

void BSP_Init(void);

int main(void)
{
#if BOOT_EN
	SCB->VTOR = APPLICATION_ADDRESS;
#endif

	BSP_Init();
	while(1) {
		MeasureTask();
		OutputTask();
	}
}

void BSP_Init(void)
{
	__enable_irq();
	systick_config();
	
//	/* RTT init */
//	SEGGER_RTT_ConfigUpBuffer(0, "RTTUP", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_TRIM);	// rtt init
//	rtt_log("Run WD-01 App Version %s", APP_VERSION);

	GPIO_UserInit();
	FLASH_RegLoadChk();
	TIM_UserInit();	
	I2C_UserInit();
	
#if DEBUG	
	UART_UserInit(115200u);	
#endif	
	
	SetFlag(kFlagInit);
	
	IWDG_Init();
	VL53L1CB_Init();
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE* f)
{
    USART_SendData(USER_COM, (uint8_t)ch);
    while (USART_GetFlagStatus(USER_COM, USART_FLAG_TXDE) == RESET)
        ;

    return (ch);
}

#ifdef USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param file pointer to the source file name
 * @param line assert_param error line source number
 */
void assert_failed(const uint8_t* expr, const uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}

#endif
