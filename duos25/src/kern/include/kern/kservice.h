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

#ifndef __KSERVICE_H
#define __KSERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <syscall_def.h>
#include <types.h>

/* Kernel service function declarations */

/**
 * @brief Exit the current task and mark it as terminated
 * @param status Exit status
 */
void k_exit(int status);

/**
 * @brief Get the current process ID (mock implementation)
 * @return Mock task ID
 */
uint16_t k_getpid(void);

/**
 * @brief Read data from a file descriptor (mock implementation)
 * @param fd File descriptor
 * @param buf Buffer to store read data
 * @param count Number of bytes to read
 * @return Number of bytes read (mock)
 */
int k_read(int fd, void *buf, size_t count);

/**
 * @brief Write data to a file descriptor (mock implementation)
 * @param fd File descriptor
 * @param buf Buffer containing data to write
 * @param count Number of bytes to write
 * @return Number of bytes written (mock)
 */
int k_write(int fd, const void *buf, size_t count);

/**
 * @brief Get current time (mock implementation)
 * @return Mock elapsed time
 */
uint32_t k_time(void);

/**
 * @brief Reboot the system (mock implementation)
 */
void k_reboot(void);

/**
 * @brief Yield CPU to allow task rescheduling
 * Triggers PendSV for context switch
 */
void k_yield(void);

#ifdef __cplusplus
}
#endif

#endif /* __KSERVICE_H */

