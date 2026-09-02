/*
 * Copyright (c) 2022 
 * 			Computer Science and Engineering, University of Dhaka
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
 * ARE DISCLAIMED. IN NO EVENT SHALL THE UNIVERSITY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include <sys_clock.h>

const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
const uint8_t APBPrescTable[8]  = {0, 0, 0, 0, 1, 2, 3, 4};

void __init_sys_clock(void)
{
RCC->CR |= RCC_CR_HSEON; // set CR bit 16 
/** Check if clock is ready RCC CR register 17th bit set*/ 
while(!(RCC->CR & RCC_CR_HSERDY)); //wait for the clock is enabled See RCC CR bit-17; HSE crystal is On
/* Set the POWER enable CLOCK and VOLTAGE REGULATOR */
RCC->APB1ENR |= RCC_APB1ENR_PWREN; //power enable for APB1
PWR->CR |=  PWR_CR_VOS; // PWR_CR_VOS; //VOS always correspond to reset value 

/*3. Configure the FLASH PREFETCH and the LATENCY Related Settings */
FLASH->ACR |= FLASH_ACR_DCEN | FLASH_ACR_ICEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_5WS; // 1<<9 | 1<<10 | 1<<8 | 5<<0; -- ICEN -- instruction cache, DCEN -- Data Cache, PRFTEN -- prefetch and LAtency;

/* 4. Configure the PRESCALERS HCLK, PCLK1, PCLK2 */
//AHB prescaler 
RCC->CFGR &= ~RCC_CFGR_HPRE;
//APB1 prescaler
RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;// 5<<10;
//APB2 prescaler
RCC->CFGR |= RCC_CFGR_PPRE2_DIV2; // 4<<13;
//5. Configugure the main PLL
RCC->PLLCFGR = RCC_PLLCFGR_PLLM_2 | PLL_N<<RCC_PLLCFGR_PLLN_Pos | RCC_PLLCFGR_PLLSRC_HSE; // (PLL_N<<6) | (PLL_P<<16) | (1<<22); 
RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP_Msk;
//6. Enable PLL and wait for it to become ready
RCC->CR |= RCC_CR_PLLON; //1<<24;
//Check if PLL clock is ready

while(!(RCC->CR & RCC_CR_PLLRDY))
	; //wait for PLL ready
//7. Select clock source and wait for it to be set
RCC->CFGR |= RCC_CFGR_SW_PLL;
while(!(RCC->CFGR &  RCC_CFGR_SWS_PLL));

}

uint32_t __APB1CLK_FREQ(void)
{
	uint32_t __AHBCLK=__AHB_CLK();
	uint32_t __APB1PRE = ((RCC->CFGR & RCC_CFGR_PPRE1)>>RCC_CFGR_PPRE1_Pos);
	uint32_t __APB1PRE_VALUE=1;
	switch(APBPrescTable[__APB1PRE]){
		case 0:__APB1PRE_VALUE=1;break; 
		case 1:__APB1PRE_VALUE=2;break;
		case 2:__APB1PRE_VALUE=4;break;
		case 3:__APB1PRE_VALUE=8;break;
		case 4:__APB1PRE_VALUE=16;break;
	}
	return (__AHBCLK/__APB1PRE_VALUE);
}
uint32_t __APB2CLK_FREQ(void)
{
	uint32_t __AHBCLK=__AHB_CLK();
	uint32_t __APB2PRE = ((RCC->CFGR & RCC_CFGR_PPRE2)>>RCC_CFGR_PPRE2_Pos);
	uint32_t __APB2PRE_VALUE=1;
	switch(APBPrescTable[__APB2PRE]){
		case 0:__APB2PRE_VALUE=1;break; 
		case 1:__APB2PRE_VALUE=2;break;
		case 2:__APB2PRE_VALUE=4;break;
		case 3:__APB2PRE_VALUE=8;break;
		case 4:__APB2PRE_VALUE=16;break;
	}
	return (__AHBCLK/__APB2PRE_VALUE);
}
uint32_t __PLL_SYS_CLK(void)
{
	//uint32_t __CFGR = RCC->CFGR;
	//if(!(__CFGR & (RCC_CFGR_SW_PLL|RCC_CFGR_SWS_1))) 
	uint32_t __PLLCFGR = RCC->PLLCFGR;
	uint32_t __PLLM;
	uint32_t __PLLN;
	uint32_t __PLLP;
	uint32_t __PLLP_MUL=2;
	//if(!(__RCC_CR & (RCC_CR_HSEON | RCC_CR_HSERDY)))__HSE_RC_USE=0;else __HSE_RC_USE=1;
	//if(!(__CFGR & (RCC_CFGR_SW_HSE|RCC_CFGR_SWS_1)))__PLL_USE=0;else __PLL_USE=1;
	__PLLM = (__PLLCFGR & ((uint32_t)0x1FUL));
	__PLLN = ((__PLLCFGR & ((uint32_t)0x1FFUL<<RCC_PLLCFGR_PLLN_Pos))>>RCC_PLLCFGR_PLLN_Pos); //clock speed multiplier
	__PLLP = ((__PLLCFGR & ((uint32_t)0x2UL<<RCC_PLLCFGR_PLLP_Pos))>>RCC_PLLCFGR_PLLP_Pos);
	//check the clock source 
	uint8_t __PLLSRC = ((__PLLCFGR & ((uint32_t)0x1UL<<RCC_PLLCFGR_PLLSRC_Pos)) != 0 ? ((uint8_t)0x1U):((uint8_t)0U));
	
	if(__PLLP == 0) {__PLLP_MUL=2;}else if(__PLLP == 1){__PLLP_MUL=4;}else if(__PLLP == 2){__PLLP_MUL=6;}else __PLLP_MUL = 8;
	
	if(__PLLSRC == 1)
	{
		return ((uint32_t)(CRYSTAL_CLK*__PLLN)/(__PLLM*__PLLP_MUL));
	}else
	{ 		
		return ((uint32_t)(HSIRC_CLK*__PLLN)/(__PLLM*__PLLP_MUL));
	}
	
}
uint32_t __AHB_CLK(void)
{
	uint32_t __PLL_CLK = __PLL_SYS_CLK();
	// determine AHB prescaler
	uint32_t __HPRE= ((RCC->CFGR & RCC_CFGR_HPRE)>>RCC_CFGR_HPRE_Pos);
	uint32_t __AHB_DIV=1;
	switch(AHBPrescTable[__HPRE]){
		case 0: __AHB_DIV=1; break;
		case 1: __AHB_DIV=2; break;
		case 2: __AHB_DIV=4; break;
		case 3: __AHB_DIV=8; break;
		case 4: __AHB_DIV=16; break;
		case 6: __AHB_DIV=64; break;
		case 7: __AHB_DIV=128; break;
		case 8: __AHB_DIV=256; break;
		case 9: __AHB_DIV=512; break;
	}
	return (uint32_t)(__PLL_CLK/__AHB_DIV);
}
