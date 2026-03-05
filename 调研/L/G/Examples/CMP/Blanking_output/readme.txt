/*!
    \file    readme.txt
    \brief   Blanking_output

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

  This example is based on the GD32C231C-EVAL-V1.0 board, it shows how to configure the
comparator blank function.

  CMP0 is configured as following:
  - Inverting input is internally connected to VREFINT = 1.2V.
  - Non inverting input is connected to PA1.
  - Output is internally connected to port output(PA6).
  - TIMER0_CH1 is selected as blanking source.
  - PA1 is connected to TIMER0_CH3(PA3).
  TIMER0 is configured as following:
  - TIMER clock frequency is set to systemcoreclock.
  - Prescaler is set to 47 make the counter clock is 1MHz.
  - Configure TIMER0_CH1 as the blank source.
  - Counter autoreload value is 9999 and CH1/CH3 output compare value is 999/4999.
  - TIMER1_CH1/TIMER1_CH3 outputs PWM with 10/50% dutycycle.

  Connect PA3 and PA1 as CMP non inverting input. use oscilloscope to observe CMP
input pin(PA1) and output pin(PA6) at the same time, the output signal is low level at the
beginning of the dutycycle.
