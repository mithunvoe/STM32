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
 
#include <stm32_startup.h>
#include <syscall.h>
const uint32_t STACK_START = (uint32_t)SRAM_END;
uint32_t NVIC_VECTOR[] __attribute__((section (".isr_vector")))={
	STACK_START,
	(uint32_t) &Reset_Handler,
	(uint32_t) &NMI_Handler,
	(uint32_t) &HardFault_Handler,
	(uint32_t) &MemManage_Handler,
	(uint32_t) &BusFault_Handler,
	(uint32_t) &UsageFault_Handler,
	0,
	0,
	0,
	0,
	(uint32_t) &SVCall_Handler,
	(uint32_t) &DebugMonitor_Handler,
	0,
	(uint32_t) &PendSV_Handler,
	(uint32_t) &SysTick_Handler,
	(uint32_t) &WWDG_Handler,
	(uint32_t) &PVD_Handler,
	(uint32_t) &TAMP_STAMP_Handler,
	(uint32_t) &RTC_WKUP_Handler,
	(uint32_t) &FLASH_Handler,
	(uint32_t) &RCC_Handler,
	(uint32_t) &EXTI0_Handler,
	(uint32_t) &EXTI1_Handler,
	(uint32_t) &EXTI2_Handler,
	(uint32_t) &EXTI3_Handler,
	(uint32_t) &EXTI4_Handler,
	(uint32_t) &DMA1_Stream0_Handler,
	(uint32_t) &DMA1_Stream1_Handler,
	(uint32_t) &DMA1_Stream2_Handler,
	(uint32_t) &DMA1_Stream3_Handler,
	(uint32_t) &DMA1_Stream4_Handler,
	(uint32_t) &DMA1_Stream5_Handler,
	(uint32_t) &DMA1_Stream6_Handler,
	(uint32_t) &ADC_Handler,
	(uint32_t) &CAN1_TX_Handler,
	(uint32_t) &CAN1_RX0_Handler,
	(uint32_t) &CAN1_RX1_Handler,
	(uint32_t) &CAN1_SCE_Handler,
	(uint32_t) &EXTI9_5_Handler,
	(uint32_t) &TIM1_BRK_TIM9_Handler,
	(uint32_t) &TIM1_UP_TIM10_Handler,
	(uint32_t) &TIM1_TRG_COM_TIM11_Handler,
	(uint32_t) &TIM1_CC_Handler,
	(uint32_t) &TIM2_Handler,
	(uint32_t) &TIM3_Handler,
	(uint32_t) &TIM4_Handler,
	(uint32_t) &I2C1_EV_Handler,
	(uint32_t) &I2C1_ER_Handler,
	(uint32_t) &I2C2_EV_Handler,
	(uint32_t) &I2C2_ER_Handler,
	(uint32_t) &SPI1_Handler,
	(uint32_t) &SPI2_Handler,
	(uint32_t) &USART1_Handler,
	(uint32_t) &USART2_Handler,
	(uint32_t) &USART3_Handler,
	(uint32_t) &EXTI15_10_Handler,
	(uint32_t) &RTC_Alarm_Handler,
	(uint32_t) &OTG_FS_WKUP_Handler,
	(uint32_t) &TIM8_BRK_TIM12_Handler,
	(uint32_t) &TIM8_UP_TIM13_Handler,
	(uint32_t) &TIM8_TRG_COM_TIM14_Handler,
	(uint32_t) &TIM8_CC_Handler,
	(uint32_t) &DMA1_Stream7_Handler,
	(uint32_t) &FMC_Handler,
	(uint32_t) &SDIO_Handler,
	(uint32_t) &TIM5_Handler,
	(uint32_t) &SPI3_Handler,
	(uint32_t) &UART4_Handler,
	(uint32_t) &UART5_Handler,
	(uint32_t) &TIM6_DAC_Handler,
	(uint32_t) &TIM7_Handler,
	(uint32_t) &DMA2_Stream0_Handler,
	(uint32_t) &DMA2_Stream1_Handler,
	(uint32_t) &DMA2_Stream2_Handler,
	(uint32_t) &DMA2_Stream3_Handler,
	(uint32_t) &DMA2_Stream4_Handler,
	0,
	0,
	(uint32_t) &CAN2_TX_Handler,
	(uint32_t) &CAN2_RX0_Handler,
	(uint32_t) &CAN2_RX1_Handler,
	(uint32_t) &CAN2_SCE_Handler,
	(uint32_t) &OTG_FS_Handler,
	(uint32_t) &DMA2_Stream5_Handler,
	(uint32_t) &DMA2_Stream6_Handler,
	(uint32_t) &DMA2_Stream7_Handler,
	(uint32_t) &USART6_Handler,
	(uint32_t) &I2C3_EV_Handler,
	(uint32_t) &I2C3_ER_Handler,
	(uint32_t) &OTG_HS_EP1_OUT_Handler,
	(uint32_t) &OTG_HS_EP1_IN_Handler,
	(uint32_t) &OTG_HS_WKUP_Handler,
	(uint32_t) &OTG_HS_Handler,
	(uint32_t) &DCMI_Handler,
	0,
	0,
	(uint32_t) &FPU_Handler,
	0,
	0,
	(uint32_t) &SPI4_Handler,
	0,
	0,
	(uint32_t) &SAI1_Handler,
	0,
	0,
	0,
	(uint32_t) &SAI2_Handler,
	(uint32_t) &QuadSPI_Handler,
	(uint32_t) &HDMI_CEC_Handler,
	(uint32_t) &SPDIF_Rx_Handler,
	(uint32_t) &FMPI2C1_Handler,
	(uint32_t) &FMPI2C1_ERR_Handler
};

void Reset_Handler(void){
	uint32_t size = (uint32_t)&_edata - (uint32_t)&_sdata;
	uint8_t *pDst = (uint8_t*)&_sdata;
	uint8_t *pSrc = (uint8_t*)&_la_data;
	for(uint32_t i=0;i<size;i++){
		*pDst++ = *pSrc++;
	}
	size = (uint32_t)&_ebss - (uint32_t)&_sbss;
	pDst = (uint8_t*)&_sbss;
	for(uint32_t i=0;i<size;i++){
		*pDst++ = 0;
	}
	_text_size = (uint32_t)&_etext - (uint32_t)&_stext;
	_data_size = (uint32_t)&_edata - (uint32_t)&_sdata;
	_bss_size = (uint32_t)&_ebss - (uint32_t)&_sbss;
	kmain();
}
void Default_Handler(void){
	while(1);
}
//2. implement the fault handlers
void HardFault_Handler(void)
{
//	printf("Exception : Hardfault\n");
	while(1);
}


void MemManage_Handler(void)
{
//	printf("Exception : MemManage\n");
	while(1);
}

void BusFault_Handler(void)
{
//	printf("Exception : BusFault\n");
	while(1);
}

/* Forward declaration */
void SVC_Handler_C(uint32_t *stack);

/* Naked assembly handler to determine stack pointer */
__attribute__((naked)) void SVCall_Handler(void) {
  __asm volatile(
    "tst lr, #4\n"        /* Test bit 2 of LR (EXC_RETURN) */
    "ite eq\n"            /* If-Then-Else */
    "mrseq r0, msp\n"     /* If EQ: r0 = MSP (Main Stack Pointer) */
    "mrsne r0, psp\n"     /* If NE: r0 = PSP (Process Stack Pointer) */
    "b SVC_Handler_C\n"   /* Branch to C handler */
  );
}

/* C handler to extract SVC number and call dispatcher */
void SVC_Handler_C(uint32_t *stack) {
  /* Step 1: Get return address (PC) from stack frame */
  uint32_t pc = stack[6];
  
  /* Step 2: Extract SVC number from instruction (PC-2 points to svc instruction) */
  uint8_t svc_no = ((const uint8_t *)(pc - 2U))[0];
  
  /* Step 3: Extract arguments from stack */
  uint32_t a0 = stack[0];  /* r0 - Argument 1 / Return value */
  uint32_t a1 = stack[1];  /* r1 - Argument 2 */
  uint32_t a2 = stack[2];  /* r2 - Argument 3 */
  uint32_t a3 = stack[3];  /* r3 - Argument 4 */
  
  /* Step 4: Call kernel dispatcher */
  uint32_t rc = syscall_dispatch((uint16_t)svc_no, a0, a1, a2, a3);
  
  /* Step 5: Store return value back to r0 in stack */
  stack[0] = rc;
}

/* PendSV handler stub - just returns to allow yield() and exit() to work */
/* This is a minimal implementation that will be replaced with full context switching later */
void PendSV_Handler(void) {
  /* PendSV stub - currently just returns immediately */
  /* In a full implementation, this would:
   *   1. Save current task context
   *   2. Select next task (scheduler)
   *   3. Restore next task context
   *   4. Return
   */
  /* For now, just return to prevent infinite loop in Default_Handler */
  return;
}


