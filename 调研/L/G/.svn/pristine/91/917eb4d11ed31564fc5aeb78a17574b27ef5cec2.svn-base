/*!
    \file    main.c
    \brief   main flash program, erase

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

#include "gd32c2x1.h"
#include "gd32c231c_eval.h"
#include <stdio.h>

#define FMC_WRITE_START_ADDR    ((uint32_t)0x08004000U)
#define FMC_WRITE_END_ADDR      ((uint32_t)0x08005000U)

uint64_t *ptrd = NULL;
uint64_t data = 0x01234567AABBCCDDU;

led_typedef_enum led_num = LED3;

/* calculate the count of doubleword to be programmed/erased */
uint32_t dword_cnt = ((FMC_WRITE_END_ADDR - FMC_WRITE_START_ADDR) >> 3U);

void fmc_erase_page(void);
void fmc_program(void);
void fmc_erase_pages_check(void);
void fmc_program_check(void);

/*!
    \brief      erase fmc pages from FMC_WRITE_START_ADDR to FMC_WRITE_END_ADDR
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_erase_pages(void)
{
    uint32_t erase_count;
    uint32_t erase_page_num_start;
    uint32_t page_cnt;
    uint32_t address = FMC_WRITE_START_ADDR;
    /* calculate the count of page to be programmed/erased */
    page_cnt = (FMC_WRITE_END_ADDR - FMC_WRITE_START_ADDR) / MAIN_FLASH_PAGE_SIZE + 1;
    /* calculate the start page number to be programmed/erased */
    erase_page_num_start = (address - MAIN_FLASH_BASE_ADDRESS) / MAIN_FLASH_PAGE_SIZE;
    /* unlock register FMC_CTL */
    fmc_unlock();

    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_ENDF | FMC_FLAG_PGERR | FMC_FLAG_WPERR | FMC_FLAG_OPRERR | FMC_FLAG_PGSERR | FMC_FLAG_PGMERR | FMC_FLAG_PGAERR);
   
    /* erase the flash pages */
    for(erase_count = 0U; erase_count < page_cnt; erase_count++){
        fmc_page_erase( erase_page_num_start );
        fmc_flag_clear(FMC_FLAG_ENDF | FMC_FLAG_PGERR | FMC_FLAG_WPERR | FMC_FLAG_OPRERR | FMC_FLAG_PGSERR | FMC_FLAG_PGMERR | FMC_FLAG_PGAERR);
        erase_page_num_start++;
    }

    /* lock register FMC_CTL after the erase operation */
    fmc_lock();
}

/*!
    \brief      program fmc word by word from FMC_WRITE_START_ADDR to FMC_WRITE_END_ADDR
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_program(void)
{
    /* unlock register FMC_CTL */
    fmc_unlock();

    uint32_t address  = FMC_WRITE_START_ADDR;

    /* program flash */
    while(address < FMC_WRITE_END_ADDR){
        fmc_doubleword_program(address, data);
        address += sizeof(uint64_t);
        fmc_flag_clear(FMC_FLAG_ENDF | FMC_FLAG_PGERR | FMC_FLAG_WPERR | FMC_FLAG_OPRERR | FMC_FLAG_PGSERR | FMC_FLAG_PGMERR | FMC_FLAG_PGAERR);
    }

    /* lock register FMC_CTL after the program operation */
    fmc_lock();
}

/*!
    \brief      check fmc erase result
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_erase_pages_check(void)
{
    uint32_t i;

    ptrd = (uint64_t *)FMC_WRITE_START_ADDR;

    /* check flash whether has been erased */
    for(i = 0U; i < dword_cnt; i++){
        if(0xFFFFFFFFFFFFFFFFU != (*ptrd)){
            led_num = LED1;
            gd_eval_led_on(led_num);
            break;
        }else{
            ptrd++;
        }
    }
}

/*!
    \brief      check fmc program result
    \param[in]  none
    \param[out] none
    \retval     none
*/
void fmc_program_check(void)
{
    uint32_t i;

    ptrd = (uint64_t *)FMC_WRITE_START_ADDR;

    /* check flash whether has been programmed */
    for(i = 0U; i < dword_cnt; i++){
        if((*ptrd) != data){
            led_num = LED2;
            gd_eval_led_on(led_num);
            break;
        }else{
            ptrd++;
        }
    }
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* initialize led on the board */
    gd_eval_led_init(LED1);    
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    
    /* step1: erase pages and check if it is successful. If not, light the LED1. */
    fmc_erase_pages();
    fmc_erase_pages_check();

    /* step2: program and check if it is successful. If not, light the LED2. */
    fmc_program();
    fmc_program_check();
    
    /* if all the operations are successful, light the LED3. */
    if(LED3 == led_num) {
        gd_eval_led_on(LED3);
    }
    while(1){
    }
}
