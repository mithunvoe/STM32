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
#include <syscall.h>
#include <unistd.h>
#include <kunistd.h>
#include <UsartRingBuffer.h>

#include <timer.h>
#ifndef DEBUG
#define DEBUG 1
#endif
void kmain(void)
{
    __sys_init();
    
    /* Add delay to allow UART to initialize and flush */
    ms_delay(100);
    {
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
    
    /* Set task ID for testing (use global variable from syscall.c) */
    g_current_task_id = 1000;
    
    /* Test 1: kprintf (kernel mode, direct UART) */
    kprintf("Task ID set to: %d\r\n", g_current_task_id);
    ms_delay(10);
    
    /* Test 2: write syscall (user mode, via SVC) */
    char msg[] = "Hello from userland via write()\r\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1U);
    ms_delay(10);
    
    /* Test 3: getSysTickTime syscall */
    uint32_t t = getSysTickTime();
    kprintf("getSysTickTime() returned: %d ms\r\n", t);
    ms_delay(10);
    
    /* Test 4: getpid syscall */
    int pid = getpid();
    kprintf("getpid() returned: %d\r\n", pid);
    ms_delay(10);
    
    /* Test 5: Combined output */
    kprintf("Current time: %d ms, pid: %d\r\n", t, pid);
    ms_delay(10);
    
    /* Test 6: yield syscall (triggers PendSV - but PendSV is stub, so just return) */
    kprintf("Calling yield()...\r\n");
    ms_delay(10);
    yield();  /* Triggers PendSV but should return */
    ms_delay(10);
    kprintf("After yield()\r\n");
    
    /* Test 7: Quick RX interrupt test */
    extern volatile uint32_t usart2_isr_count;
    extern volatile uint32_t usart2_rxne_count;
    extern volatile uint32_t usart2_txe_count;
    extern volatile uint32_t usart2_error_count;

    kprintf("\r\n=== Quick RX Test ===\r\n");
    kprintf("Type something: ");
    ms_delay(50);

    /* Wait max 3 seconds for one character */
    uint32_t wait_start = __getTime();
    while (usart2_rxne_count == 0 && (__getTime() - wait_start) < 3000) {
        ms_delay(10);
    }

    ms_delay(50);
    kprintf("\r\nRXNE interrupts: %d\r\n", usart2_rxne_count);
    if (usart2_rxne_count > 0) {
        kprintf("SUCCESS - RX interrupts working!\r\n");
    } else {
        kprintf("FAILED - No RX interrupts received\r\n");
        kprintf("Check: Is PA3 (USART2_RX) connected?\r\n");
    }
    ms_delay(50);

    /* Test 8: read syscall with actual input */
    char input_buf[64];
    kprintf("\r\nType and press Enter:\r\n");
    ms_delay(100);

    /* Call read() - it will wait for input or timeout internally */
    ssize_t bytes_read = read(STDIN_FILENO, input_buf, sizeof(input_buf));

    ms_delay(50);
    kprintf("read() returned: %d bytes\r\n", bytes_read);
    ms_delay(10);

    if (bytes_read > 0) {
        kprintf("You typed: ");
        write(STDOUT_FILENO, input_buf, bytes_read);
        kprintf("\r\n");
    }
    ms_delay(10);

    kprintf("RX interrupts: %d\r\n", usart2_rxne_count);
    ms_delay(10);

    /* Don't call exit(0) as it triggers PendSV which causes infinite loop */
    /* Instead, just loop forever */
    kprintf("System ready. Entering main loop...\r\n");


    /* End of program */
    while (1) {
        ms_delay(1000);
    }
}
