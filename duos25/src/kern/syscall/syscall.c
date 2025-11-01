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
#include <kservice.h>
#include <stdint.h>

/**
 * Stack frame layout on exception entry (Cortex-M):
 * stack_frame[0] = R0
 * stack_frame[1] = R1
 * stack_frame[2] = R2
 * stack_frame[3] = R3
 * stack_frame[4] = R12
 * stack_frame[5] = LR (EXC_RETURN value)
 * stack_frame[6] = PC (points to instruction after SVC)
 * stack_frame[7] = xPSR
 */

/**
 * @brief C dispatcher for SVC handler
 * Extracts syscall ID from SVC instruction and dispatches to kernel service
 * @param stack_frame Pointer to the exception stack frame
 */
void SVC_Handler_C(uint32_t *stack_frame)
{
	uint8_t svc_id;
	uint16_t *svc_instr;
	
	/* Extract syscall ID from SVC instruction
	 * PC (stack_frame[6]) points to instruction after SVC
	 * In Thumb mode, instructions are 2 bytes, so subtract 2
	 * SVC instruction format: SVC #imm, where imm is in bits [7:0]
	 */
		svc_instr = (uint16_t *)(stack_frame[6] - 2);
	svc_id = (uint8_t)(*svc_instr & 0xFF);
	
	/* Extract arguments from stack frame */
	uint32_t arg0 = stack_frame[0];  /* R0 - syscall number or first arg */
	uint32_t arg1 = stack_frame[1];  /* R1 - second argument */
	uint32_t arg2 = stack_frame[2];  /* R2 - third argument */
	uint32_t arg3 = stack_frame[3];  /* R3 - fourth argument */
	(void)arg3;  /* Reserved for future syscalls */
	
	/* Dispatch based on syscall ID */
	switch (svc_id)
	{
		case SYS_exit:
			k_exit((int)arg0);  /* arg0 is exit status */
			stack_frame[0] = 0;  /* Return value */
			break;
			
		case SYS_getpid:
			stack_frame[0] = (uint32_t)k_getpid();
			break;
			
		case SYS_read:
			stack_frame[0] = (uint32_t)k_read((int)arg0, (void *)arg1, (size_t)arg2);
			break;
			
		case SYS_write:
			stack_frame[0] = (uint32_t)k_write((int)arg0, (const void *)arg1, (size_t)arg2);
			break;
			
		case SYS_time:
			stack_frame[0] = (uint32_t)k_time();
			break;
			
		case SYS_reboot:
			k_reboot();
			stack_frame[0] = 0;
			break;
			
		case SYS_yield:
			k_yield();
			stack_frame[0] = 0;
			break;
			
		default:
			/* Unknown syscall - return error */
			stack_frame[0] = (uint32_t)(-ENOSYS);
			break;
	}
	
	/* Return to user mode - hardware will automatically restore context */
	/* The EXC_RETURN value in LR ensures return to Thread mode with PSP */
}

/* Legacy syscall function - kept for compatibility */
void syscall(uint16_t callno)
{
	/* This function is deprecated in favor of SVC_Handler_C */
	/* It's kept for backward compatibility but should not be used */
	(void)callno;
}

