/*!
    \file    main.c
    \brief   main flash program, write protection

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

#define FLASH_PAGE_PROGRAM
#define WRITE_PROTECTION_ENABLE
//#define WRITE_PROTECTION_DISABLE

typedef enum {FAILED = 0, PASSED = !FAILED} test_state;

#define FMC_WRITE_START_ADDR    ((uint32_t)0x08004000U)
#define FMC_WRITE_END_ADDR      ((uint32_t)0x080047FFU)

uint32_t erase_counter = 0x0, address = 0x0;
uint64_t data = 0x1122334455667788;
uint32_t protect_stat, protected_start_addr, protected_end_addr = 0x0;
uint32_t page_number = 0U;
uint32_t page_size;
uint32_t page_start;

__IO fmc_state_enum fmc_state = FMC_READY;
__IO test_state program_state = PASSED;

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

    /* get page size and start page number */
    page_start = (FMC_WRITE_START_ADDR - MAIN_FLASH_BASE_ADDRESS) / MAIN_FLASH_PAGE_SIZE;
    page_number = (FMC_WRITE_END_ADDR - FMC_WRITE_START_ADDR + 1U) / MAIN_FLASH_PAGE_SIZE ;

    /* unlock the flash program/erase controller */
    fmc_unlock();
    ob_unlock();

    /* clear all pending flags */
    fmc_flag_clear(FMC_FLAG_ENDF | FMC_FLAG_PGERR | FMC_FLAG_WPERR | FMC_FLAG_OPRERR | FMC_FLAG_PGSERR | FMC_FLAG_PGMERR | FMC_FLAG_PGAERR);

    /* get page write protection status */
    protect_stat = ob_write_protection_area_get(WP_AREA_0, &protected_start_addr, &protected_end_addr);

#ifdef WRITE_PROTECTION_DISABLE
    if(VLD_RETURN_VALUE == protect_stat) {
        FMC_WP0 &= ~(FMC_WP0_WP0_SADDR | FMC_WP0_WP0_EADDR);
        FMC_WP0 |= FMC_WP0_WP0_SADDR;
        FMC_CTL |= FMC_CTL_OBSTART;
        /* reload option bytes and generate a system reset */
        ob_reload();
        ob_lock();
        fmc_lock();
    }
#elif defined WRITE_PROTECTION_ENABLE
    if(INVLD_RETURN_VALUE == protect_stat) {
        ob_write_protection_area_config(WP_AREA_0, page_start, (page_start + page_number));
        /* reload option bytes and generate a system reset */
        ob_reload();
        ob_lock();
        fmc_lock();
    }

#endif /* WRITE_PROTECTION_DISABLE */

#ifdef FLASH_PAGE_PROGRAM
    fmc_unlock();
    if(INVLD_RETURN_VALUE == protect_stat) {

        /* clear all pending flags */
        fmc_flag_clear(FMC_FLAG_ENDF | FMC_FLAG_PGERR | FMC_FLAG_WPERR | FMC_FLAG_OPRERR | FMC_FLAG_PGSERR | FMC_FLAG_PGMERR | FMC_FLAG_PGAERR);

        /* erase the flash pages */
        for(erase_counter = 0; (erase_counter < page_number) && (FMC_READY == fmc_state); erase_counter++) {
            fmc_state = fmc_page_erase( page_start + erase_counter);
        }

        /* flash double word program of data 0x1122334455667788 at addresses defined by FMC_WRITE_START_ADDR and FMC_WRITE_END_ADDR */
        address = FMC_WRITE_START_ADDR;

        while((address < FMC_WRITE_END_ADDR) && (FMC_READY == fmc_state)) {
            fmc_state = fmc_doubleword_program(address, data);
            address = address + 8U;
        }
        /* check the correctness of written data */
        address = FMC_WRITE_START_ADDR;
        while((address < FMC_WRITE_END_ADDR) && (PASSED == program_state)) {
            if(REG64(address) != data) {
                program_state = FAILED;
            }
            address += 8U;
            /* if all pages are checked correctly, the LED1 is on */
            if(address == (FMC_WRITE_END_ADDR + 1U)) {
                gd_eval_led_on(LED1);
            }
        }
    } else {
        /* error to erase the flash: the desired pages are write protected */
        for(erase_counter = 0; (erase_counter < page_number); erase_counter++) {
            fmc_state = fmc_page_erase(page_start + erase_counter);
            if(FMC_WPERR == fmc_state){
                gd_eval_led_on(LED2);
            }
        }
    }
    fmc_lock();
#endif /* FLASH_PAGE_PROGRAM */
    while(1) {
    }
}
