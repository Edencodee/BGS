;/*!
;    \file    startup_gd32c31xx.s
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

; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000400

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000400

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit

                PRESERVE8
                THUMB

;               /* reset Vector Mapped to at Address 0 */
                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp                      ; Top of Stack
                DCD     Reset_Handler                     ; Reset Handler
                DCD     NMI_Handler                       ; NMI Handler
                DCD     HardFault_Handler                 ; Hard Fault Handler
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     SVC_Handler                       ; SVCall Handler
                DCD     0                                 ; Reserved
                DCD     0                                 ; Reserved
                DCD     PendSV_Handler                    ; PendSV Handler
                DCD     SysTick_Handler                   ; SysTick Handler

;               /* external interrupts handler */
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


__Vectors_End

__Vectors_Size  EQU     __Vectors_End - __Vectors

                AREA    |.text|, CODE, READONLY

;/* reset Handler */
Reset_Handler   PROC
                EXPORT  Reset_Handler                     [WEAK]

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

                IMPORT  SystemInit
                IMPORT  __main
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP

;/* dummy Exception Handlers */
NMI_Handler\
                PROC
                EXPORT  NMI_Handler                       [WEAK]
                B       .
                ENDP
HardFault_Handler\
                PROC
                EXPORT  HardFault_Handler                 [WEAK]
                B       .
                ENDP
SVC_Handler\
                PROC
                EXPORT  SVC_Handler                       [WEAK]
                B       .
                ENDP
PendSV_Handler\
                PROC
                EXPORT  PendSV_Handler                    [WEAK]
                B       .
                ENDP
SysTick_Handler\
                PROC
                EXPORT  SysTick_Handler                   [WEAK]
                B       .
                ENDP

Default_Handler PROC
;               /* external interrupts handler */
                EXPORT  WWDGT_IRQHandler                  [WEAK]
                EXPORT  TIMESTAMP_IRQHandler              [WEAK]
                EXPORT  FMC_IRQHandler                    [WEAK]
                EXPORT  RCU_IRQHandler                    [WEAK]
                EXPORT  EXTI0_IRQHandler                  [WEAK]
                EXPORT  EXTI1_IRQHandler                  [WEAK]
                EXPORT  EXTI2_IRQHandler                  [WEAK]
                EXPORT  EXTI3_IRQHandler                  [WEAK]
                EXPORT  EXTI4_IRQHandler                  [WEAK]
                EXPORT  DMA_Channel0_IRQHandler           [WEAK]
                EXPORT  DMA_Channel1_IRQHandler           [WEAK]
                EXPORT  DMA_Channel2_IRQHandler           [WEAK]
                EXPORT  ADC_IRQHandler                    [WEAK]
                EXPORT  USART0_IRQHandler                 [WEAK]
                EXPORT  USART1_IRQHandler                 [WEAK]
                EXPORT  USART2_IRQHandler                 [WEAK]
                EXPORT  I2C0_EV_IRQHandler                [WEAK]
                EXPORT  I2C0_ER_IRQHandler                [WEAK]
                EXPORT  I2C1_EV_IRQHandler                [WEAK]
                EXPORT  I2C1_ER_IRQHandler                [WEAK]
                EXPORT  SPI0_IRQHandler                   [WEAK]
                EXPORT  SPI1_IRQHandler                   [WEAK]
                EXPORT  RTC_Alarm_IRQHandler              [WEAK]
                EXPORT  EXTI5_9_IRQHandler                [WEAK]
                EXPORT  TIMER0_TRG_CMT_UP_BRK_IRQHandler  [WEAK]
                EXPORT  TIMER0_Channel_IRQHandler         [WEAK]
                EXPORT  TIMER2_IRQHandler                 [WEAK]
                EXPORT  TIMER13_IRQHandler                [WEAK]
                EXPORT  TIMER15_IRQHandler                [WEAK]
                EXPORT  TIMER16_IRQHandler                [WEAK]
                EXPORT  EXTI10_15_IRQHandler              [WEAK]
                EXPORT  DMAMUX_IRQHandler                 [WEAK]
                EXPORT  CMP0_IRQHandler                   [WEAK]
                EXPORT  CMP1_IRQHandler                   [WEAK]
                EXPORT  I2C0_WKUP_IRQHandler              [WEAK]
                EXPORT  I2C1_WKUP_IRQHandler              [WEAK]
                EXPORT  USART0_WKUP_IRQHandler            [WEAK]
                        
;/* external interrupts handler */
WWDGT_IRQHandler                
TIMESTAMP_IRQHandler
FMC_IRQHandler                  
RCU_IRQHandler              
EXTI0_IRQHandler                
EXTI1_IRQHandler                
EXTI2_IRQHandler                
EXTI3_IRQHandler                
EXTI4_IRQHandler                
DMA_Channel0_IRQHandler         
DMA_Channel1_IRQHandler         
DMA_Channel2_IRQHandler         
ADC_IRQHandler                  
USART0_IRQHandler               
USART1_IRQHandler               
USART2_IRQHandler               
I2C0_EV_IRQHandler              
I2C0_ER_IRQHandler              
I2C1_EV_IRQHandler              
I2C1_ER_IRQHandler              
SPI0_IRQHandler                 
SPI1_IRQHandler                 
RTC_Alarm_IRQHandler            
EXTI5_9_IRQHandler              
TIMER0_TRG_CMT_UP_BRK_IRQHandler
TIMER0_Channel_IRQHandler       
TIMER2_IRQHandler               
TIMER13_IRQHandler              
TIMER15_IRQHandler              
TIMER16_IRQHandler              
EXTI10_15_IRQHandler            
DMAMUX_IRQHandler               
CMP0_IRQHandler                 
CMP1_IRQHandler                 
I2C0_WKUP_IRQHandler            
I2C1_WKUP_IRQHandler            
USART0_WKUP_IRQHandler          

                B       .
                ENDP

                ALIGN

; user Initial Stack & Heap

                IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap

__user_initial_stackheap PROC
                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR
                ENDP

                ALIGN

                ENDIF

                END
