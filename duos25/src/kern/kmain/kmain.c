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

#include <timer.h>
#ifndef DEBUG
#define DEBUG 1
#endif
void kmain(void)
{
    __sys_init();
    
    /* Add delay to allow UART to initialize and flush */
    ms_delay(100);
    
    /* Set task ID for testing */
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
    
    /* Don't call exit(0) as it triggers PendSV which causes infinite loop */
    /* Instead, just loop forever */
    kprintf("System ready. Entering main loop...\r\n");
    
    while (1) {
        /* Main loop - don't call exit() */
        ms_delay(1000);
    }
}
