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

#include <syscall.h>
#include <syscall_def.h>
#include <errno.h>
#include <errmsg.h>
#include <kunistd.h>
#include <UsartRingBuffer.h>
#include <system_config.h>
#include <cm4.h>

/* Global task ID variable */
volatile uint16_t g_current_task_id = 0;

/* The SVC_Handler_C calls this function to evaluate and execute the actual function */
uint32_t syscall_dispatch(uint16_t callno, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3)
{
	switch(callno)
	{
		case SYS_read: {
			/* SYS_read: Read from UART (stdin) */
			uint32_t fd = a0;
			uint8_t *buf = (uint8_t *)a1;
			uint32_t len = a2;
			
			/* Validate parameters */
			if (fd != STDIN_FILENO || buf == 0 || len == 0U) {
				return 0U;
			}
			
			/* Limit buffer size */
			if (len > 256U) {
				len = 256U;
			}
			
			/* Read from UART */
			uint32_t count = 0U;
			while (count < len) {
				while (IsDataAvailable(__CONSOLE) == 0) {
					/* Busy wait for data */
				}
				int c = Uart_read(__CONSOLE);
				if (c < 0) {
					break;
				}
				buf[count++] = (uint8_t)c;
				if ((uint8_t)c == '\n') {
					break;  /* Stop on newline */
				}
			}
			
			/* Null terminate if we didn't fill the buffer */
			if (count < len) {
				buf[count] = '\0';
			}
			
			return count;  /* Return bytes read */
		}
		
		case SYS_write: {
			/* SYS_write: Write to UART (stdout) */
			uint32_t fd = a0;
			const uint8_t *buf = (const uint8_t *)a1;
			uint32_t len = a2;
			
			/* Validate parameters */
			if (fd != STDOUT_FILENO || buf == 0 || len == 0U) {
				return 0U;
			}
			
			/* Write to UART */
			for (uint32_t i = 0; i < len; i++) {
				Uart_write(buf[i], __CONSOLE);
			}
			
			return len;  /* Return bytes written */
		}
		
		case SYS_reboot: {
			/* SYS_reboot: System reset */
			__NVIC_SystemReset();
			return 0U;
		}
		
		case SYS__exit: {
			/* SYS__exit: Terminate process (trigger PendSV) */
			SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
			return 0U;
		}
		
		case SYS_getpid: {
			/* SYS_getpid: Get current task ID */
			return (uint32_t)g_current_task_id;
		}
		
		case SYS___time: {
			/* SYS___time: Get system time in milliseconds */
			return __getTime();
		}
		
		case SYS_yield: {
			/* SYS_yield: Voluntary context switch (trigger PendSV) */
			SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
			return 0U;
		}
		
		default:
			/* Return error code for unimplemented syscalls */
			return (uint32_t)ENOSYS;
	}
}

