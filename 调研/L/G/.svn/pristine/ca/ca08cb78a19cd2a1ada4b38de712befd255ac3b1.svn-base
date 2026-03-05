/*!
    \file    gd32c2x1_it.c
    \brief   interrupt service routines

    \version 2025-05-30, V1.0.0, firmware for gd32c2x1
*/

/*
    Copyright (c) 2025, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "gd32c2x1_it.h"
#include "main.h"

#define SRAM_ECC_ERROR_HANDLE(s)    do{}while(1)

extern uint8_t spi1_send_array[ ];
extern uint8_t spi0_receive_array[ ];
extern volatile uint32_t send_n;
extern volatile uint32_t receive_n;

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
    \brief      this function handles SPI1 Handler exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SPI1_IRQHandler(void)
{
    if(spi_i2s_interrupt_flag_get(SPI1, SPI_I2S_INT_FLAG_TBE) != RESET) {
        /* send data */
        spi_i2s_data_transmit(SPI1, spi1_send_array[send_n++]);
        if(ARRAYSIZE == send_n) {
#if SPI_CRC_ENABLE
            /* send the CRC value */
            spi_crc_next(SPI1);
#endif /* enable CRC function */
            spi_i2s_interrupt_disable(SPI1, SPI_I2S_INT_TBE);
        }
    }
}

/*!
    \brief      this function handles SPI0 Handler exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SPI0_IRQHandler(void)
{
    /* receive data */
    if(spi_i2s_interrupt_flag_get(SPI0, SPI_I2S_INT_FLAG_RBNE) != RESET) {
        spi0_receive_array[receive_n++] = spi_i2s_data_receive(SPI0);
#if SPI_CRC_ENABLE
        if((ARRAYSIZE - 1) == receive_n) {
            /* receive the CRC value */
            spi_crc_next(SPI0);
        }
#endif /* enable CRC function */
    }
}
