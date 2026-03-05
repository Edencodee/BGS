#include "gd32c2x1_it.h"
#include "main.h"
#include "systick.h"

#define SRAM_ECC_ERROR_HANDLE(s)    do{}while(1)

/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
    if(SET == syscfg_interrupt_flag_get(SYSCFG_FLAG_ECCME)) {
        SRAM_ECC_ERROR_HANDLE("SRAM two bits non-correction check error\r\n"); 
    } else if(SET == syscfg_interrupt_flag_get(SYSCFG_FLAG_ECCSE)) {
        SRAM_ECC_ERROR_HANDLE("RAM single bit correction check error\r\n"); 
    } else { 
        /* if NMI exception occurs, go to infinite loop */
        /* HXTAL clock monitor NMI error or NMI pin error */
        while(1) {
        }
    }
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
    /* if SVC exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
    /* if PendSV exception occurs, go to infinite loop */
    while(1) {
    }
}

/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
    delay_decrement();
}

/*!
    \brief      this function handles EXTI5_9 exception (PLS_SO on PA6)
    \param[in]  none
    \param[out] none
    \retval     none
*/
void EXTI5_9_IRQHandler(void)
{
    extern uint16_t adc_value_buf[ADC_BUF_LEN];
    extern uint8_t adc_buf_index;
    
    if(RESET != exti_interrupt_flag_get(EXTI_6)) {
        /* clear EXTI flag */
        exti_interrupt_flag_clear(EXTI_6);
        
        /* trigger ADC software start conversion */
        adc_software_trigger_enable(ADC_ROUTINE_CHANNEL);
        
        /* get ADC conversion result and store in buffer */
        adc_value_buf[adc_buf_index] = adc_routine_data_read();
        printf("ADC=%u\r\n", adc_value_buf[adc_buf_index]);
        adc_buf_index++;
        
        /* circular buffer */
        if(adc_buf_index >= ADC_BUF_LEN) {
            adc_buf_index = 0;
        }
    }
}
