/*!
    \file    readme.txt
    \brief   USART synchronous
    
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

  This example is based on the GD32C231C-EVAL-V1.0, it provides 
a basic communication between USART (Synchronous mode) and SPI0. First, USART transmits 
a series of data to SPI0, then SPI0 transmits a series of data to USART. Compare the 
transmitted data and the received data. If the data transmitted from USART and received by 
SPI0 are the same, LED1 will be on, otherwise off. If the data transmitted from SPI0 and 
received by USART are the same, LED2 will be on, otherwise off.
 
  on the GD32C231C-EVAL-V1.0:
    Connect SPI0 SCK  pin(PA1) TO USART0_CK  pin(PB12)
    Connect SPI0 MISO pin(PA6) TO USART0_RX  pin(PA3)
    Connect SPI0 MOSI pin(PA7) TO USART0_TX  pin(PA2)
  

