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

  This example is based on the GD32C231C-EVAL-V1.0 board, it shows how to use CRC to 
calculate the CRC value.

  On the GD32C231C-EVAL-V1.0 board, this demo demonstrates five types of CRC calculations.
    - The first 32-bit CRC value is calculated for single 32-bit data.

    - The second 32-bit CRC value is calculated for a 32-bit data array.

    - The third 16-bit CRC value is calculated for a 16-bit data array. The polynomial is modified in this
      calculation. At the same time, the input and output data reverse function are configured.

    - The fourth 8-bit CRC value is calculated for an 8-bit data array. The polynomial is modified in this
      calculation. At the same time, the input data reverse function is configured.

    - The fifth 7-bit CRC value is calculated for an 8-bit data array. The polynomial is modified in this
      calculation. At the same time, the input data reverse function is configured.

     If all results are correct, LED1 is turned on.