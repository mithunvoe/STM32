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
 
#ifndef __STM32_STARTUP_H
#define __STM32_STARTUP_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <sys_bus_matrix.h>



int kmain(void);

extern uint32_t _stext;
extern uint32_t _etext;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _la_data;

volatile uint32_t _bss_size=0;
volatile uint32_t _data_size=0;
volatile uint32_t _text_size=0;

void Reset_Handler(void) __attribute__((weak));
void NMI_Handler(void) __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void) __attribute__((weak));
void MemManage_Handler(void) __attribute__((weak));
void BusFault_Handler(void)__attribute__((weak));
void UsageFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVCall_Handler(void) __attribute__((weak));
void DebugMonitor_Handler(void) __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void) __attribute__((weak));
void WWDG_Handler(void) __attribute__((weak, alias("Default_Handler")));
void PVD_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TAMP_STAMP_Handler(void) __attribute__((weak, alias("Default_Handler")));
void RTC_WKUP_Handler(void) __attribute__((weak, alias("Default_Handler")));
void FLASH_Handler(void) __attribute__((weak, alias("Default_Handler")));
void RCC_Handler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI0_Handler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI1_Handler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI2_Handler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI3_Handler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI4_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream0_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream1_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream2_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream3_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream4_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream5_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream6_Handler(void) __attribute__((weak, alias("Default_Handler")));
void ADC_Handler(void) __attribute__((weak, alias("Default_Handler")));
void CAN1_TX_Handler(void) __attribute__((weak, alias("Default_Handler")));
void CAN1_RX0_Handler(void) __attribute__((weak, alias("Default_Handler")));
void CAN1_RX1_Handler(void) __attribute__((weak, alias("Default_Handler")));
void CAN1_SCE_Handler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI9_5_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIM1_BRK_TIM9_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIM1_UP_TIM10_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIM1_TRG_COM_TIM11_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIM1_CC_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIM2_Handler(void) __attribute__((weak));
void TIM3_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIM4_Handler(void) __attribute__((weak, alias("Default_Handler")));
void I2C1_EV_Handler(void) __attribute__((weak, alias("Default_Handler")));
void I2C1_ER_Handler(void) __attribute__((weak, alias("Default_Handler")));
void I2C2_EV_Handler(void) __attribute__((weak, alias("Default_Handler")));
void I2C2_ER_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SPI1_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SPI2_Handler(void) __attribute__((weak, alias("Default_Handler")));
void USART1_Handler(void) __attribute__((weak, alias("Default_Handler")));
void USART2_Handler(void)  __attribute__((weak));
void USART3_Handler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI15_10_Handler(void) __attribute__((weak, alias("Default_Handler")));
void RTC_Alarm_Handler(void) __attribute__((weak, alias("Default_Handler")));
void OTG_FS_WKUP_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIM8_BRK_TIM12_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIM8_UP_TIM13_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIM8_TRG_COM_TIM14_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIM8_CC_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream7_Handler(void) __attribute__((weak, alias("Default_Handler")));
void FMC_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SDIO_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIM5_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SPI3_Handler(void) __attribute__((weak, alias("Default_Handler")));
void UART4_Handler(void) __attribute__((weak, alias("Default_Handler")));
void UART5_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIM6_DAC_Handler(void) __attribute__((weak, alias("Default_Handler")));
void TIM7_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream0_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream1_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream2_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream3_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream4_Handler(void) __attribute__((weak, alias("Default_Handler")));
void CAN2_TX_Handler(void) __attribute__((weak, alias("Default_Handler")));
void CAN2_RX0_Handler(void) __attribute__((weak, alias("Default_Handler")));
void CAN2_RX1_Handler(void) __attribute__((weak, alias("Default_Handler")));
void CAN2_SCE_Handler(void) __attribute__((weak, alias("Default_Handler")));
void OTG_FS_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream5_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream6_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream7_Handler(void) __attribute__((weak, alias("Default_Handler")));
void USART6_Handler(void) __attribute__((weak, alias("Default_Handler")));
void I2C3_EV_Handler(void) __attribute__((weak, alias("Default_Handler")));
void I2C3_ER_Handler(void) __attribute__((weak, alias("Default_Handler")));
void OTG_HS_EP1_OUT_Handler(void) __attribute__((weak, alias("Default_Handler")));
void OTG_HS_EP1_IN_Handler(void) __attribute__((weak, alias("Default_Handler")));
void OTG_HS_WKUP_Handler(void) __attribute__((weak, alias("Default_Handler")));
void OTG_HS_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DCMI_Handler(void) __attribute__((weak, alias("Default_Handler")));
void FPU_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SPI4_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SAI1_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SAI2_Handler(void) __attribute__((weak, alias("Default_Handler")));
void QuadSPI_Handler(void) __attribute__((weak, alias("Default_Handler")));
void HDMI_CEC_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SPDIF_Rx_Handler(void) __attribute__((weak, alias("Default_Handler")));
void FMPI2C1_Handler(void) __attribute__((weak, alias("Default_Handler")));
void FMPI2C1_ERR_Handler(void) __attribute__((weak, alias("Default_Handler")));


void update_global_tick_count(void);


#ifdef __cplusplus
}
#endif
#endif


