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
#include <syscall_def.h>
#include <syscall.h>

#include <timer.h>
#ifndef DEBUG
#define DEBUG 1
#endif

/* Helper macros for making SVC calls */
/* SVC instruction format: SVC #imm, where imm is the syscall number */
#define __SVC_0(n) __asm volatile("SVC %0" : : "i" (n))
#define __SVC_1(n, a1) \
    do { \
        register uint32_t r0 __asm("r0") = (uint32_t)(a1); \
        __asm volatile("SVC %0" : "+r" (r0) : "i" (n) : "r1", "r2", "r3", "r12", "lr"); \
    } while(0)
#define __SVC_2(n, a1, a2) \
    do { \
        register uint32_t r0 __asm("r0") = (uint32_t)(a1); \
        register uint32_t r1 __asm("r1") = (uint32_t)(a2); \
        __asm volatile("SVC %0" : "+r" (r0) : "r" (r1), "i" (n) : "r2", "r3", "r12", "lr"); \
    } while(0)
#define __SVC_3(n, a1, a2, a3) \
    do { \
        register uint32_t r0 __asm("r0") = (uint32_t)(a1); \
        register uint32_t r1 __asm("r1") = (uint32_t)(a2); \
        register uint32_t r2 __asm("r2") = (uint32_t)(a3); \
        __asm volatile("SVC %0" : "+r" (r0) : "r" (r1), "r" (r2), "i" (n) : "r3", "r12", "lr"); \
    } while(0)

/* Wrapper functions for syscalls */
/* These use inline assembly to make SVC calls in Thumb mode
 * After SVC returns, hardware restores R0 from stack with return value
 */
static inline uint32_t sys_getpid(void) {
    uint32_t retval;
    __asm volatile(
        "mov r0, #0\n"
        "svc %1\n"
        "mov %0, r0"
        : "=r" (retval)
        : "I" (SYS_getpid)
        : "memory"
    );
    return retval;
}

static inline uint32_t sys_time(void) {
    uint32_t retval;
    __asm volatile(
        "mov r0, #0\n"
        "svc %1\n"
        "mov %0, r0"
        : "=r" (retval)
        : "I" (SYS_time)
        : "memory"
    );
    return retval;
}

static inline int sys_write(int fd, const void *buf, size_t count) {
    int retval;
    __asm volatile(
        "mov r0, %1\n"
        "mov r1, %2\n"
        "mov r2, %3\n"
        "svc %4\n"
        "mov %0, r0"
        : "=r" (retval)
        : "r" (fd), "r" (buf), "r" (count), "I" (SYS_write)
        : "memory"
    );
    return retval;
}

static inline int sys_read(int fd, void *buf, size_t count) {
    int retval;
    __asm volatile(
        "mov r0, %1\n"
        "mov r1, %2\n"
        "mov r2, %3\n"
        "svc %4\n"
        "mov %0, r0"
        : "=r" (retval)
        : "r" (fd), "r" (buf), "r" (count), "I" (SYS_read)
        : "memory"
    );
    return retval;
}

static inline void sys_yield(void) {
    __asm volatile(
        "svc %0"
        :
        : "i" (SYS_yield)
        : "memory"
    );
}

static inline void sys_exit(int status) {
    __asm volatile(
        "mov r0, %0\n"
        "svc %1"
        :
        : "r" (status), "i" (SYS_exit)
        : "memory"
    );
}
void kmain(void)
{
    __sys_init();
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
    /* SVC/Syscall Testing */
    kprintf("\n=== SVC Handler Testing ===\n");
    
    /* Test 1: SYS_getpid */
    kprintf("Test 1: SYS_getpid\n");
    uint32_t pid = sys_getpid();
    kprintf("  PID returned: %u\n", (unsigned int)pid);
    
    /* Test 2: SYS_time */
    kprintf("Test 2: SYS_time\n");
    uint32_t time1 = sys_time();
    kprintf("  Time 1: %u\n", (unsigned int)time1);
    ms_delay(100);
    uint32_t time2 = sys_time();
    kprintf("  Time 2: %u\n", (unsigned int)time2);
    kprintf("  Time difference: %u\n", (unsigned int)(time2 - time1));
    
    /* Test 3: SYS_write */
    kprintf("Test 3: SYS_write\n");
    const char *test_msg = "Hello from SYS_write!\n";
    int bytes_written = sys_write(STDOUT_FILENO, test_msg, __strlen((uint8_t*)test_msg));
    kprintf("  Bytes written: %d\n", bytes_written);
    
    /* Test 4: SYS_read (mock returns 0) */
    kprintf("Test 4: SYS_read\n");
    char read_buf[64];
    int bytes_read = sys_read(STDIN_FILENO, read_buf, sizeof(read_buf));
    kprintf("  Bytes read: %d\n", bytes_read);
    
    /* Test 5: SYS_yield (should trigger PendSV) */
    kprintf("Test 5: SYS_yield\n");
    kprintf("  Calling yield...\n");
    sys_yield();
    kprintf("  Yield returned\n");
    
    /* Test 6: Invalid syscall (should return -ENOSYS) */
    kprintf("Test 6: Invalid syscall\n");
    int invalid_result;
    __asm volatile(
        "mov r0, #0\n"
        "svc %1\n"
        "mov %0, r0"
        : "=r" (invalid_result)
        : "i" (255)
        : "memory"
    );
    kprintf("  Invalid syscall result: %d (should be -ENOSYS = -1)\n", invalid_result);
    
    kprintf("=== SVC Testing Complete ===\n\n");
    
    int i = 0;
    while (1)
    {
        /* Toggle LED every BLINK_PERIOD_MS */
        static uint8_t on = 0;
        on ^= 1U;
        kprintf("Main loop: %d (PID: %u, Time: %u)\n", 
                i++, (unsigned int)sys_getpid(), (unsigned int)sys_time());
        ms_delay(1000);
        // GPIO_WritePin(GPIOA, GPIO_PIN_5, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
        // ms_delay(10);
    }
}
