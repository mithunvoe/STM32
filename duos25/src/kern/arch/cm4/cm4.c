/*
 * Copyright (c) 2022 
 * Computer Science and Engineering, University of Dhaka
 * Credit: CSE Batch 25 (starter) and Prof. Mosaddek Tushar
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE UNIVERSITY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
 
#include <cm4.h>
#include <sys_clock.h>
#include <syscall.h>

/* Millisecond tick counter incremented by SysTick_Handler */
static volatile uint32_t s_msTick = 0U;

/************************************************************************************
* __SysTick_init(uint32_t reload) 
* Function initialize the SysTick clock. The function with a weak attribute enables 
* redefining the function to change its characteristics whenever necessary.
**************************************************************************************/

void __SysTick_init(uint32_t reload)
{
    
    /* 'reload' is expected as tick frequency in Hz (e.g., 1000 for 1 ms tick) */
    uint32_t ahb_clk = __AHB_CLK();
    uint32_t reload_val = 0U;


    reload_val = ahb_clk / reload; /* number of core cycles per tick */

    SYSTICK->LOAD = (reload_val - 1U) & SysTick_LOAD_RELOAD_Msk;
    SYSTICK->VAL = 0U; /* reset current counter */
    /* Processor clock as source, enable interrupt and counter */
    SYSTICK->CTRL = (SysTick_CTRL_CLKSOURCE_Msk |
                     SysTick_CTRL_TICKINT_Msk   |
                     SysTick_CTRL_ENABLE_Msk);
    s_msTick = 0U;
}
void SysTickIntDisable(void)
{
    SYSTICK->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
}

void SysTickIntEnable(void)
{
    SYSTICK->CTRL |= SysTick_CTRL_TICKINT_Msk;
}
/************************************************************************************
* __sysTick_enable(void) 
* The function enables the SysTick clock if already not enabled. 
* redefining the function to change its characteristics whenever necessary.
**************************************************************************************/
void __SysTick_enable(void)
{
    SYSTICK->CTRL |= SysTick_CTRL_ENABLE_Msk;
}
void __sysTick_disable(void)
{
    SYSTICK->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}
uint32_t __getSysTickCount(void)
{
    return s_msTick;
}
/************************************************************************************
* __updateSysTick(uint32_t count) 
* Function reinitialize the SysTick clock. The function with a weak attribute enables 
* redefining the function to change its characteristics whenever necessary.
**************************************************************************************/

void __updateSysTick(uint32_t count)
{
    /* Reconfigure SysTick with a new tick frequency (Hz) */
    uint32_t ahb_clk = __AHB_CLK();
    uint32_t reload_val;

    if (count == 0U) {
        count = 1000U;
    }
    reload_val = ahb_clk / count;
    if (reload_val == 0U) {
        reload_val = 1U;
    }
    if (reload_val > (SysTick_LOAD_RELOAD_Msk + 1U)) {
        reload_val = (SysTick_LOAD_RELOAD_Msk + 1U);
    }

    /* Keep running but update reload and reset current value */
    SYSTICK->LOAD = (reload_val - 1U) & SysTick_LOAD_RELOAD_Msk;
    SYSTICK->VAL = 0U;
}

/************************************************************************************
* __getTime(void) 
* Function return the SysTick elapsed time from the begining or reinitialing. The function with a weak attribute enables 
* redefining the function to change its characteristics whenever necessary.
**************************************************************************************/

uint32_t __getTime(void)
{
    return s_msTick; /* elapsed time in ms since __SysTick_init */
}

uint32_t __get__Second(void){
    return (s_msTick / 1000U);
}
uint32_t __get__Minute(void){
    return (s_msTick / (1000U * 60U));
}
uint32_t __get__Hour(void){
    return (s_msTick / (1000U * 60U * 60U));
}
void SysTick_Handler(void)
{
    /* Increment millisecond tick counter */
    s_msTick++;
}

void __enable_fpu()
{
    SCB->CPACR |= ((0xFUL<<20));
}

uint8_t ms_delay(uint32_t delay)
{
    uint32_t start = s_msTick;
    while ((uint32_t)(s_msTick - start) < delay) {
        /* busy wait */
    }
    return 0U;
}

uint32_t getmsTick(void)
{
    return s_msTick;
}

uint32_t wait_until(uint32_t delay)
{
    uint32_t start = s_msTick;
    while ((uint32_t)(s_msTick - start) < delay) {
        /* busy wait */
    }
    return (uint32_t)(s_msTick - start);
}

void SYS_SLEEP_WFI(void)
{
    __WFI();
}
