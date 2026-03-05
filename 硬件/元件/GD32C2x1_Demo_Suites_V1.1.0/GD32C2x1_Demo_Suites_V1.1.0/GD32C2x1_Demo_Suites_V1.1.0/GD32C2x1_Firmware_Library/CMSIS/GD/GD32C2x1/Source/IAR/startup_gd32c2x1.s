;/*!
;    \file    startup_gd32c231xx.s
;    \brief   start up file
;
;    \version 2025-05-30, V1.0.0, firmware for gd32c2x1
;*/
;
;/*
;    Copyright (c) 2025, GigaDevice Semiconductor Inc.
;
;    Redistribution and use in source and binary forms, with or without modification, 
;are permitted provided that the following conditions are met:
;
;    1. Redistributions of source code must retain the above copyright notice, this 
;       list of conditions and the following disclaimer.
;    2. Redistributions in binary form must reproduce the above copyright notice, 
;       this list of conditions and the following disclaimer in the documentation 
;       and/or other materials provided with the distribution.
;    3. Neither the name of the copyright holder nor the names of its contributors 
;       may be used to endorse or promote products derived from this software without 
;       specific prior written permission.
;
;    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
;AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
;WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
;IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
;INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
;NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
;PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
;WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
;ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
;OF SUCH DAMAGE.
;*/

        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(2)

        EXTERN  __iar_program_start
        EXTERN  SystemInit
        PUBLIC  __vector_table

        DATA
__vector_table
        DCD     sfe(CSTACK)                         ; top of stack
        DCD     Reset_Handler                       ; Vector Number 1,Reset Handler

        DCD     NMI_Handler                         ; Vector Number 2,NMI Handler
        DCD     HardFault_Handler                   ; Vector Number 3,Hard Fault Handler
        DCD     0                                   ; Reserved
        DCD     0                                   ; Reserved
        DCD     0                                   ; Reserved
        DCD     0                                   ; Reserved
        DCD     0                                   ; Reserved
        DCD     0                                   ; Reserved
        DCD     0                                   ; Reserved
        DCD     SVC_Handler                         ; Vector Number 11,SVCall Handler
        DCD     0                                   ; Reserved
        DCD     0                                   ; Reserved
        DCD     PendSV_Handler                      ; Vector Number 14,PendSV Handler
        DCD     SysTick_Handler                     ; Vector Number 15,SysTick Handler

;       /* external interrupts handler */
        DCD     WWDGT_IRQHandler                  ; 16:Window Watchdog Timer
        DCD     TIMESTAMP_IRQHandler              ; 17:RTC TimeStamp through EXTI Line detect
        DCD     0                                 ; 18:Reserved
        DCD     FMC_IRQHandler                    ; 19:FMC global interrupt
        DCD     RCU_IRQHandler                    ; 20:RCU global interrupt
        DCD     EXTI0_IRQHandler                  ; 21:EXTI Line 0
        DCD     EXTI1_IRQHandler                  ; 22:EXTI Line 1
        DCD     EXTI2_IRQHandler                  ; 23:EXTI Line 2
        DCD     EXTI3_IRQHandler                  ; 24:EXTI Line 3
        DCD     EXTI4_IRQHandler                  ; 25:EXTI Line 4
        DCD     DMA_Channel0_IRQHandler           ; 26:DMA Channel 0 
        DCD     DMA_Channel1_IRQHandler           ; 27:DMA Channel 1 
        DCD     DMA_Channel2_IRQHandler           ; 28:DMA Channel 2 
        DCD     ADC_IRQHandler                    ; 29:ADC interrupt
        DCD     USART0_IRQHandler                 ; 30:USART0
        DCD     USART1_IRQHandler                 ; 31:USART1    
        DCD     USART2_IRQHandler                 ; 32:USART2
        DCD     I2C0_EV_IRQHandler                ; 33:I2C0 Event
        DCD     I2C0_ER_IRQHandler                ; 34:I2C0 Error
        DCD     I2C1_EV_IRQHandler                ; 35:I2C1 Event
        DCD     I2C1_ER_IRQHandler                ; 36:I2C1 Error
        DCD     SPI0_IRQHandler                   ; 37:SPI0
        DCD     SPI1_IRQHandler                   ; 38:SPI1
        DCD     RTC_Alarm_IRQHandler              ; 39:RTC Alarm through EXTI Line detect
        DCD     EXTI5_9_IRQHandler                ; 40:EXTI5 to EXTI9
        DCD     TIMER0_TRG_CMT_UP_BRK_IRQHandler  ; 41:TIMER0 Trigger, Communication, Update and Break
        DCD     TIMER0_Channel_IRQHandler         ; 42:TIMER0 Channel Capture Compare
        DCD     TIMER2_IRQHandler                 ; 43:TIMER2
        DCD     TIMER13_IRQHandler                ; 44:TIMER13
        DCD     TIMER15_IRQHandler                ; 45:TIMER15
        DCD     TIMER16_IRQHandler                ; 46:TIMER16
        DCD     EXTI10_15_IRQHandler              ; 47:EXTI10 to EXTI15
        DCD     0                                 ; 48:Reserved
        DCD     DMAMUX_IRQHandler                 ; 49:DMAMUX
        DCD     CMP0_IRQHandler                   ; 50:Comparator 0 interrupt through EXTI Line detect
        DCD     CMP1_IRQHandler                   ; 51:Comparator 1 interrupt through EXTI Line detect
        DCD     I2C0_WKUP_IRQHandler              ; 52:I2C0 Wakeup interrupt through EXTI Line detect
        DCD     I2C1_WKUP_IRQHandler              ; 53:I2C1 Wakeup interrupt through EXTI Line detect
        DCD     USART0_WKUP_IRQHandler            ; 54:USART0 Wakeup interrupt through EXTI Line detect

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        THUMB

        PUBWEAK Reset_Handler
        SECTION .text:CODE:NOROOT:REORDER(2)
Reset_Handler

                LDR     r0, =0x1FFF0BE0
                LDR     r2, [r0]
                LDR     r0, = 0xFFFF0000
                ANDS    r2, r2, r0
                LSRS    r2, r2, #16
                LSLS    r2, r2, #10
                LDR     r1, =0x20000000
                MOV     r0, #0x00
SRAM_INIT       STM     r1!, {r0}
                SUBS    r2, r2, #4
                CMP     r2, #0x00
                BNE     SRAM_INIT

        LDR     R0, =SystemInit
        BLX     R0
        LDR     R0, =__iar_program_start
        BX      R0

        PUBWEAK NMI_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
NMI_Handler
        B NMI_Handler

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
HardFault_Handler
        B HardFault_Handler      

        PUBWEAK SVC_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
SVC_Handler
        B SVC_Handler

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
PendSV_Handler
        B PendSV_Handler

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:NOROOT:REORDER(1)
SysTick_Handler
        B SysTick_Handler

        PUBWEAK WWDGT_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
WWDGT_IRQHandler
        B WWDGT_IRQHandler

        PUBWEAK TIMESTAMP_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIMESTAMP_IRQHandler
        B TIMESTAMP_IRQHandler

        PUBWEAK FMC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
FMC_IRQHandler
        B FMC_IRQHandler

        PUBWEAK RCU_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
RCU_IRQHandler
        B RCU_IRQHandler

        PUBWEAK EXTI0_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EXTI0_IRQHandler
        B EXTI0_IRQHandler

        PUBWEAK EXTI1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EXTI1_IRQHandler
        B EXTI1_IRQHandler

        PUBWEAK EXTI2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EXTI2_IRQHandler
        B EXTI2_IRQHandler

        PUBWEAK EXTI3_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EXTI3_IRQHandler
        B EXTI3_IRQHandler 
 
        PUBWEAK EXTI4_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EXTI4_IRQHandler 
        B EXTI4_IRQHandler 

        PUBWEAK DMA_Channel0_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
DMA_Channel0_IRQHandler
        B DMA_Channel0_IRQHandler

        PUBWEAK DMA_Channel1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
DMA_Channel1_IRQHandler
        B DMA_Channel1_IRQHandler

        PUBWEAK DMA_Channel2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
DMA_Channel2_IRQHandler
        B DMA_Channel2_IRQHandler

        PUBWEAK ADC_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
ADC_IRQHandler
        B ADC_IRQHandler

        PUBWEAK USART0_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
USART0_IRQHandler
        B USART0_IRQHandler

        PUBWEAK USART1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
USART1_IRQHandler
        B USART1_IRQHandler

        PUBWEAK USART2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
USART2_IRQHandler
        B USART2_IRQHandler

        PUBWEAK I2C0_EV_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
I2C0_EV_IRQHandler
        B I2C0_EV_IRQHandler

        PUBWEAK I2C0_ER_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
I2C0_ER_IRQHandler
        B I2C0_ER_IRQHandler

        PUBWEAK I2C1_EV_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
I2C1_EV_IRQHandler
        B I2C1_EV_IRQHandler

        PUBWEAK I2C1_ER_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
I2C1_ER_IRQHandler
        B I2C1_ER_IRQHandler

        PUBWEAK SPI0_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
SPI0_IRQHandler
        B SPI0_IRQHandler

        PUBWEAK SPI1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
SPI1_IRQHandler
        B SPI1_IRQHandler

        PUBWEAK RTC_Alarm_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
RTC_Alarm_IRQHandler
        B RTC_Alarm_IRQHandler

        PUBWEAK EXTI5_9_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EXTI5_9_IRQHandler
        B EXTI5_9_IRQHandler

        PUBWEAK TIMER0_TRG_CMT_UP_BRK_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIMER0_TRG_CMT_UP_BRK_IRQHandler
        B TIMER0_TRG_CMT_UP_BRK_IRQHandler

        PUBWEAK TIMER0_Channel_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIMER0_Channel_IRQHandler
        B TIMER0_Channel_IRQHandler

        PUBWEAK TIMER2_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIMER2_IRQHandler
        B TIMER2_IRQHandler

        PUBWEAK TIMER13_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIMER13_IRQHandler
        B TIMER13_IRQHandler

        PUBWEAK TIMER15_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIMER15_IRQHandler
        B TIMER15_IRQHandler

        PUBWEAK TIMER16_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
TIMER16_IRQHandler
        B TIMER16_IRQHandler

        PUBWEAK EXTI10_15_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
EXTI10_15_IRQHandler
        B EXTI10_15_IRQHandler

        PUBWEAK DMAMUX_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
DMAMUX_IRQHandler
        B DMAMUX_IRQHandler

        PUBWEAK CMP0_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
CMP0_IRQHandler
        B CMP0_IRQHandler

        PUBWEAK CMP1_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
CMP1_IRQHandler
        B CMP1_IRQHandler

        PUBWEAK I2C0_WKUP_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
I2C0_WKUP_IRQHandler
        B I2C0_WKUP_IRQHandler

        PUBWEAK I2C1_WKUP_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
I2C1_WKUP_IRQHandler
        B I2C1_WKUP_IRQHandler

        PUBWEAK USART0_WKUP_IRQHandler
        SECTION .text:CODE:NOROOT:REORDER(1)
USART0_WKUP_IRQHandler
        B USART0_WKUP_IRQHandler

        END