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
#include <sys_gpio.h>
#include <sys_usart.h>
#include <sys_clock.h>
#include <types.h>
#include <stm32_assert.h>
#include <stm32f446xx.h>
#include <cm4.h>
#include <float.h>
#include <sys_err.h>

static void UART_SetConfig(UART_HandleTypeDef*);
/*
uint8_t _USART_READ_STR(USART_TypeDef* usart,uint8_t *buff,uint16_t size)
{
	uint8_t n=0;
	for(uint8_t i=0;i<size;i++){
		buff[i]=UART_GetChar(usart);
		n=i;
		if(buff[i]=='\0' || buff[i] == '\n' || buff[i] == ' ')
		{ 	
			buff[i]='\0';
			break;
		}
	}
	return n;
}
*/

/*
void UART_GetString(USART_TypeDef *uart,uint16_t size,uint8_t* buff)
{
	uint16_t i=0;
	while(size--)
	{
		uint8_t x=UART_GetChar(uart);
		buff[i]=x;
		i++;
	}
	buff[i]='\0';
		
}
*/
 StatusTypeDef UART_Init(UART_HandleTypeDef* huart)
{
	if(huart == NULL){
		return SYS_ERROR;
	}
	if(huart->Init.HwFlowCtl != UART_HWCONTROL_NONE)
	{
		assert_param(IS_UART_HWFLOW_INSTANCE(huart->Instance));
    	assert_param(IS_UART_HARDWARE_FLOW_CONTROL(huart->Init.HwFlowCtl));
	}else{
		assert_param(IS_UART_INSTANCE(huart->Instance));
	}
	assert_param(IS_UART_WORD_LENGTH(huart->Init.WordLength));
  	assert_param(IS_UART_OVERSAMPLING(huart->Init.OverSampling));
	if (huart->gState == UART_STATE_RESET)
  	{
    /* Allocate lock resource and initialize it */
    huart->Lock = SYS_UNLOCKED;

	#if (USE_UART_REGISTER_CALLBACKS == 1)
    UART_InitCallbacksToDefault(huart);

    if (huart->MspInitCallback == NULL)
    {
      huart->MspInitCallback = UART_MspInit;
    }

    /* Init the low level hardware */
    huart->MspInitCallback(huart);
	#else
    /* Init the low level hardware : GPIO, CLOCK */
    UART_MspInit(huart);
	#endif /* (USE_SYS_UART_REGISTER_CALLBACKS) */
  }

  huart->gState = UART_STATE_BUSY;

  /* Disable the peripheral */
  __UART_DISABLE(huart);

  /* Set the UART Communication parameters */
  UART_SetConfig(huart);

  /* In asynchronous mode, the following bits must be kept cleared:
     - LINEN and CLKEN bits in the USART_CR2 register,
     - SCEN, HDSEL and IREN  bits in the USART_CR3 register.*/
  CLEAR_BIT(huart->Instance->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
  CLEAR_BIT(huart->Instance->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN));

  /* Enable the peripheral */
  __UART_ENABLE(huart);

  /* Initialize the UART state */
  huart->ErrorCode = UART_ERROR_NONE;
  huart->gState = UART_STATE_READY;
  huart->RxState = UART_STATE_READY;

  return SYS_OK;
}
void UART_DeInit(UART_HandleTypeDef* huart)
{
	if(huart->Instance==USART6)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __RCC_USART6_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    GPIO_DeInit(GPIOC, GPIO_PIN_6|GPIO_PIN_7);

    /* USART1 interrupt DeInit */
    NVIC_DisableIRQ(USART6_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(huart->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __RCC_USART2_CLK_DISABLE();
  
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

    /* USART2 interrupt DeInit */
    NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
}	


uint16_t UART_BRR_SAMPLING16(uint32_t plclk,uint32_t baudRate){
	float usartdiv=0.00;
	float rem=0.00;
	float man=0.00;
	uint16_t div_mantissa;
	uint16_t div_frac;
	usartdiv = (float)(plclk*1000000)/(16*baudRate);
	man=(uint16_t)usartdiv;
	rem=usartdiv - man;
	div_mantissa=((uint16_t)man*16);
	div_frac = (((uint16_t)(16*(rem))) & (0x0F));
	return (div_mantissa | div_frac);
}

static void UART_SetConfig(UART_HandleTypeDef *huart)
{
  uint32_t tmpreg;
	/* Check the parameters */
  //assert_param(IS_UART_BAUDRATE(huart->Init.BaudRate));
  assert_param(IS_UART_STOPBITS(huart->Init.StopBits));
  assert_param(IS_UART_PARITY(huart->Init.Parity));
  assert_param(IS_UART_MODE(huart->Init.Mode));
	
	tmpreg = (uint32_t)huart->Init.WordLength | huart->Init.Parity | huart->Init.Mode | huart->Init.OverSampling;
  
	MODIFY_REG(huart->Instance->CR1,(uint32_t)(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS | USART_CR1_TE | USART_CR1_RE | USART_CR1_OVER8),tmpreg);

  /*-------------------------- USART CR3 Configuration -----------------------*/
  /* Configure the UART HFC: Set CTSE and RTSE bits according to huart->Init.HwFlowCtl value */
  	MODIFY_REG(huart->Instance->CR3, (USART_CR3_RTSE | USART_CR3_CTSE), huart->Init.HwFlowCtl);
	/*if(huart->Instance == USART2){
		huart->Instance->BRR |= ((0x18<<4) | (7<<0));
	}*/
	if(huart->Instance == USART2 || huart->Instance == USART3 || huart->Instance == UART4 || huart->Instance == UART5){
		huart->Instance->BRR=UART_BRR_SAMPLING16(__APB1CLK_FREQ(),huart->Init.BaudRate);
	}else if(huart->Instance == USART1 || huart->Instance == USART6){
		huart->Instance->BRR=UART_BRR_SAMPLING16(__APB2CLK_FREQ(),huart->Init.BaudRate);
	}
}

void UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(huart->Instance == USART1)
	{
		RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
		GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
		GPIO_Init(GPIOA,&GPIO_InitStruct);
		NVIC_SetPriority(USART1_IRQn, 0);
    	NVIC_EnableIRQ(USART1_IRQn);
	}
	if(huart->Instance == USART2)
	{
		RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
		GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
		GPIO_Init(GPIOA,&GPIO_InitStruct);
		NVIC_SetPriority(USART2_IRQn, 0);
    	NVIC_EnableIRQ(USART2_IRQn);
	}
	if(huart->Instance == USART3)
	{
		RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
		GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
		GPIO_Init(GPIOA,&GPIO_InitStruct);
		NVIC_SetPriority(USART3_IRQn, 0);
    	NVIC_EnableIRQ(USART3_IRQn);
	}
	if(huart->Instance == UART4)
	{
		RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
		GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
		GPIO_Init(GPIOA,&GPIO_InitStruct);
		NVIC_SetPriority(UART4_IRQn, 0);
    	NVIC_EnableIRQ(UART4_IRQn);
	}
	if(huart->Instance == UART5)
	{
		RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // need modifcation
		GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_2;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF8_UART5;
		GPIO_Init(GPIOA,&GPIO_InitStruct);
		NVIC_SetPriority(UART5_IRQn, 0);
    	NVIC_EnableIRQ(UART5_IRQn);
	}
	if(huart->Instance == USART6)
	{
		RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
		GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
		GPIO_Init(GPIOC,&GPIO_InitStruct);
		NVIC_SetPriority(USART6_IRQn, 0);
    	NVIC_EnableIRQ(USART6_IRQn);
	}
}


