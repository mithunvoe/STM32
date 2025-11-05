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
 
#include <unistd.h>
#include <syscall_def.h>
#include <stdint.h>
#include <stddef.h>

/* SVC_CALL macro: Executes SVC instruction with syscall number and arguments */
#define SVC_CALL(num, a0, a1, a2, a3)                                          \
  ({                                                                           \
    register uint32_t r0 __asm("r0") = (uint32_t)(a0);   /* Arg 1 */         \
    register uint32_t r1 __asm("r1") = (uint32_t)(a1);   /* Arg 2 */         \
    register uint32_t r2 __asm("r2") = (uint32_t)(a2);   /* Arg 3 */         \
    register uint32_t r3 __asm("r3") = (uint32_t)(a3);   /* Arg 4 */         \
    __asm volatile("svc %[imm]"                           /* Execute SVC */   \
                   : "+r"(r0)                             /* Output: r0 */    \
                   : [imm] "I"(num), "r"(r1), "r"(r2), "r"(r3)  /* Input */  \
                   : "memory");                           /* Clobber */       \
    r0;                                                   /* Return r0 */     \
  })

/* Read from file descriptor */
ssize_t read(int fd, void *buf, size_t count) {
  return (ssize_t)SVC_CALL(SYS_read, fd, buf, count, 0U);
}

/* Write to file descriptor */
ssize_t write(int fd, const void *buf, size_t count) {
  return (ssize_t)SVC_CALL(SYS_write, fd, buf, count, 0U);
}

/* Exit process */
void _exit(int status) {
  (void)SVC_CALL(SYS__exit, (uint32_t)status, 0U, 0U, 0U);
  /* Should not return */
  while(1);
}

/* Get process ID */
pid_t getpid(void) {
  return (pid_t)SVC_CALL(SYS_getpid, 0U, 0U, 0U, 0U);
}

/* Get system time */
uint32_t getSysTickTime(void) {
  return SVC_CALL(SYS___time, 0U, 0U, 0U, 0U);
}

/* Reboot system */
void reboot(void) {
  (void)SVC_CALL(SYS_reboot, 0U, 0U, 0U, 0U);
  /* Should not return */
  while(1);
}

/* Yield CPU (voluntary context switch) */
void yield(void) {
  (void)SVC_CALL(SYS_yield, 0U, 0U, 0U, 0U);
}

