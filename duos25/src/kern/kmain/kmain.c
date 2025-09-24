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

#include <sys_init.h>
#include <cm4.h>
#include <kmain.h>
#include <stdint.h>
#include <sys_usart.h>
#include <kstdio.h>
#include <sys_rtc.h>
#include <kstring.h>
#include <sys_gpio.h>
#include <system_config.h>
#include <timer.h>
#ifndef DEBUG
#define DEBUG 1
#endif
void kmain(void)
{
    __sys_init();
    /* Quick check: if SysTick isn't ticking, we'll fall back to TIM2-based delay */
    uint8_t use_tim2_delay = 1;
    {
        uint32_t t0 = __getTime();
        /* short wait using busy loop on CPU cycles */
        for (volatile uint32_t i = 0; i < 100000U; ++i)
        {
            __NOP();
        }
        uint32_t t1 = __getTime();
        if (t1 == t0)
        {
            use_tim2_delay = 1;
            kprintf("[WARN] SysTick not advancing; using TIM2 Delay() for blink.\r\n");
        }
    }
    /* Configure LED (e.g., Nucleo-F446RE LD2 on PA5) */
    {
        /* Enable GPIOA clock (AHB1ENR bit 0) */
        RCC->AHB1ENR |= (1U << 0);
        (void)RCC->AHB1ENR; /* dummy read to ensure clock is enabled */

        GPIO_InitTypeDef gi;
        gi.Pin = GPIO_PIN_5;
        gi.Mode = GPIO_MODE_OUTPUT_PP;
        gi.Pull = GPIO_NOPULL;
        gi.Speed = GPIO_SPEED_FREQ_LOW;
        gi.Alternate = 0U;
        GPIO_Init(GPIOA, &gi);
        /* Start LED OFF */
        GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
    }
    int i = 0;
    while (1)
    {
        /* Toggle LED every BLINK_PERIOD_MS */
        static uint8_t on = 0;
        on ^= 1U;
        GPIO_WritePin(GPIOA, GPIO_PIN_5, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
        if (use_tim2_delay)
        {
            Delay(BLINK_PERIOD_MS);
        }
        else
        {
            ms_delay(BLINK_PERIOD_MS);
        }
        kprintf("%d\n", ++i);
    }
}
