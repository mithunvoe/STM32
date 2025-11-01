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
#include <sys_clock.h>
#include <sys_usart.h>
#include <serial_lin.h>
#include <sys_gpio.h>
#include <kstdio.h>
#include <debug.h>
#include <timer.h>
#include <UsartRingBuffer.h>
#include <system_config.h>
#include <mcu_info.h>
#include <sys_rtc.h>
#ifndef DEBUG
#define DEBUG 1
#endif
extern UART_HandleTypeDef huart6;

/* Forward declaration to avoid implicit declaration warning */
void display_group_info(void);

void __sys_init(void)
{
	__init_sys_clock(); //configure system clock 180 MHz
	__ISB();	
	__enable_fpu(); //enable FPU single precision floating point unit
	__ISB();
	/* Ensure vector table base points to Flash */
	SCB->VTOR = 0x08000000U;
	__DSB();
	__ISB();
	NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	__SysTick_init(1000);	//enable systick for 1ms
	//SYS_RTC_init();
	SerialLin2_init(__CONSOLE,0);
	SerialLin6_init(&huart6,0);
	Ringbuf_init(__CONSOLE);
	Ringbuf_init(&huart6);
	ConfigTimer2ForSystem();
	__ISB();
	#ifdef DEBUG
	kprintf("\n************************************\r\n");
	kprintf("Booting Machine Intelligence System 1.0 .....\r\n");
	kprintf("Copyright (c) 2024, Prof. Mosaddek Tushar, CSE, DU\r\n");
	kprintf("CPUID %x\n", SCB->CPUID);
	kprintf("OS Version: 2024.1.0.0\n");
	kprintf("Time Elapse %d ms\n",__getTime());
	kprintf("*************************************\r\n");
	kprintf("# ");
	show_system_info();
	display_group_info();
	#endif
}

/*
* Do not remove it is for debug purpose
*/

void SYS_ROUTINE(void)
{
	__debugRamUsage();
}

/*
* Display your Full Name, Registration Number and Class Roll
* Each line displays a student or group member information
*/
void display_group_info(void)
{
	kprintf("Group Members: Kabya Mithun Saha - Muhaiminul Islam Ninad\n");
	

}
