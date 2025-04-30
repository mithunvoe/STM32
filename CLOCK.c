#include "CLOCK.h"
#include "stm32f4xx.h"
/*
* Configure the clock
*/
void initClock(void)
{
 /**** RM0390 chapter 3 Reset and clock Control (RCC)
	* Steps to be followed to configure clock 
	* 1. Enable HSE (High SPeed External Clock) and wait for HSE to become ready 
	*			-- HSE (i) External Crystal/ceramic resonator (ii) HSE external user clock
	* 2. Set the power enable clock and volatge regulator
	* 3. Configure the FLASH PREFETCH and the LATENCY Related Settings 
	* 4. Configure the PRESCALERS HCLK, PCLK1, PCLK2
    * 5. Configugure the main PLL
	* 6. Enable PLL and wait for it to become ready
	* 7. Select clock source and wait for it to be set
	* Clock Set done
	***/
	#define PLL_M 4
	#define PLL_N 180
	#define PLL_P 0 //0 to the sixteenth position (PLL=2)
/* Turn HSE on and wait for to be ready RCC CR Register 16 th bit set*/
	/* Enable HSE clock See register RCC_CR (RCC clock control register -- 32-bit register
	* Rest value 0x0000 xx83
	* Bit 16 HSEON and bit-17 HSERDY (status)
	*/
/*
* Here we enable our system intended to use HSE clock 
*/	
RCC->CR |=RCC_CR_HSEON; // set CR bit 16 
/** Check if clock is ready RCC CR register 17th bit set*/ 
while(!(RCC->CR & RCC_CR_HSERDY)); //wait for the clock is enabled See RCC CR bit-17; HSE crystal is On
/* Set the POWER enable CLOCK and VOLTAGE REGULATOR */
RCC->APB1ENR |= RCC_APB1ENR_PWREN; //power enable for APB1
PWR->CR |= PWR_CR_VOS; //VOS always correspond to reset value 

/*3. Configure the FLASH PREFETCH and the LATENCY Related Settings */
FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_5WS; //ICEN -- instruction cache, DCEN -- Data Cache, PRFTEN -- prefetch and LAtency;

/* 4. Configure the PRESCALERS HCLK, PCLK1, PCLK2 */
//AHB prescaler 
RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
//APB1 prescaler
RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
//APB2 prescaler
RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;
//5. Configugure the main PLL
RCC->PLLCFGR = (PLL_M<<0) | (PLL_N<<6) | (PLL_P<<16) | (RCC_PLLCFGR_PLLSRC_HSE); 
//6. Enable PLL and wait for it to become ready
RCC->CR |= RCC_CR_PLLON;
//Check if PLL clock is ready
while(!(RCC->CR & RCC_CR_PLLRDY))
	; //wait for PLL ready
//7. Select clock source and wait for it to be set
	RCC->CFGR |= RCC_CFGR_SW_PLL;
while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}
