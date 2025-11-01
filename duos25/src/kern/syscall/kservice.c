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

#include <kservice.h>
#include <cm4.h>
#include <types.h>
#include <kmain.h>
#include <stdint.h>
#include <syscall_def.h>

/* Mock task ID for getpid() */
static uint16_t mock_task_id = 1000;

/* Mock time counter */
static uint32_t mock_time = 0;

/**
 * @brief Exit the current task and mark it as terminated
 * Must change status to 'terminated' and call k_yield()
 * @param status Exit status
 */
void k_exit(int status)
{
	/* TODO: In a full implementation, this would:
	 * 1. Find the current task's TCB
	 * 2. Set task status to 'terminated'
	 * 3. Clean up task resources
	 * For now, this is a mock implementation
	 */
	(void)status;  /* Status is ignored in mock */
	
	/* Call k_yield() to trigger rescheduling */
	k_yield();
	
	/* Should not return - task is terminated */
	/* In real implementation, this would never return */
}

/**
 * @brief Get the current process ID (mock implementation)
 * @return Mock task ID
 */
uint16_t k_getpid(void)
{
	/* Mock implementation - returns a fixed task ID */
	return mock_task_id;
}

/**
 * @brief Read data from a file descriptor (mock implementation)
 * @param fd File descriptor
 * @param buf Buffer to store read data
 * @param count Number of bytes to read
 * @return Number of bytes read (mock)
 */
int k_read(int fd, void *buf, size_t count)
{
	/* Mock implementation */
	(void)fd;
	(void)buf;
	(void)count;
	
	/* Return 0 bytes read for mock */
	return 0;
}

/**
 * @brief Write data to a file descriptor (mock implementation)
 * @param fd File descriptor
 * @param buf Buffer containing data to write
 * @param count Number of bytes to write
 * @return Number of bytes written (mock)
 */
int k_write(int fd, const void *buf, size_t count)
{
	/* Mock implementation */
	/* In a real implementation, this would write to stdout/stderr via USART */
	(void)fd;
	(void)buf;
	(void)count;
	
	/* Return number of bytes written (mock: return count) */
	return (int)count;
}

/**
 * @brief Get current time (mock implementation)
 * @return Mock elapsed time
 */
uint32_t k_time(void)
{
	/* Mock implementation - returns a mock elapsed time */
	/* In a real implementation, this would return actual system time */
	return mock_time++;
}

/**
 * @brief Reboot the system (mock implementation)
 */
void k_reboot(void)
{
	/* Mock implementation */
	/* In a real implementation, this would trigger system reset via SCB */
	/* For now, this is a placeholder */
	
	/* In a real implementation:
	 * SCB->AIRCR = (0x5FAUL << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk;
	 */
}

/**
 * @brief Yield CPU to allow task rescheduling
 * Triggers PendSV for context switch
 */
void k_yield(void)
{
	/* Trigger PendSV exception for context switch */
	/* PendSV is used for context switching as it has lowest priority */
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
	
	/* Memory barrier to ensure write completes */
	__DSB();
	__ISB();
}

