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
 
#include <serial_lin.h>
#include <sys_err.h>
#include <sys_usart.h>
#include <UsartRingBuffer.h>
#include <system_config.h>
#include <kstring.h>
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart6;
Data_TypeDef errObj={SYS_USART_t,0};
UART_HandleTypeDef huart2;
static ring_buffer uart2_ring_buffer_tx={ { 0 }, 0, 0};
static ring_buffer uart2_ring_buffer_rx={ { 0 }, 0, 0};

UART_HandleTypeDef huart6;
static ring_buffer uart6_ring_buffer_tx={ { 0 }, 0, 0};
static ring_buffer uart6_ring_buffer_rx={ { 0 }, 0, 0};
/*



*/

void SerialLin2_init(UART_HandleTypeDef *huart,uint32_t baudrate)
{
	huart->Instance=USART2;
	if(baudrate == 0)
	{
		huart->Init.BaudRate=115200;
	}else
	{
		huart->Init.BaudRate=baudrate;
	}
	huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart->Init.Parity = UART_PARITY_NONE;
	huart->Init.Mode = UART_MODE_TX_RX;
	huart->Init.OverSampling = UART_OVERSAMPLING_16;
	huart->Init.WordLength = UART_WORDLENGTH_8B;
	huart->Init.StopBits = UART_STOPBITS_1;
	//huart->pRxBuffPtr = &uart2_ring_buffer_rx;
	huart->pRxBuffPtr = &uart2_ring_buffer_rx;
	huart->pTxBuffPtr = &uart2_ring_buffer_tx;
	huart->RxXferSize = UART2_BUFFER_SIZE;
	huart->TxXferSize = UART2_BUFFER_SIZE;
	if(UART_Init(huart)!= SYS_OK){
		errObj.p_address_t = USART2;
		Error_Handler(&errObj);
	}
}
/*
void SerialLin3_init(UART_HandleTypeDef *huart, uint32_t baudrate)
{
	huart->Instance=USART3;
	if(baudrate == 0)
	{
		huart->Init.BaudRate=115200;
	}else
	{
		huart->Init.BaudRate=baudrate;
	}
	huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart->Init.Parity = UART_PARITY_NONE;
	huart->Init.Mode = UART_MODE_TX_RX;
	huart->Init.OverSampling = UART_OVERSAMPLING_16;
	huart->Init.WordLength = UART_WORDLENGTH_8B;
	huart->Init.StopBits = UART_STOPBITS_1;
	if(UART_Init(huart)!= SYS_OK){
		Error_Handler();
	}
}
void SerialLin4_init(UART_HandleTypeDef *huart,uint32_t baudrate)
{
	huart->Instance=UART4;
	if(baudrate == 0)
	{
		huart->Init.BaudRate=115200;
	}else
	{
		huart->Init.BaudRate=baudrate;
	}
	huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart->Init.Parity = UART_PARITY_NONE;
	huart->Init.Mode = UART_MODE_TX_RX;
	huart->Init.OverSampling = UART_OVERSAMPLING_16;
	huart->Init.WordLength = UART_WORDLENGTH_8B;
	huart->Init.StopBits = UART_STOPBITS_1;
	if(UART_Init(huart)!= SYS_OK){
		Error_Handler();
	}
}
void SerialLin5_init(UART_HandleTypeDef *huart,uint32_t baudrate)
{
	huart->Instance=USART5;
	if(baudrate == 0)
	{
		huart->Init.BaudRate=115200;
	}else
	{
		huart->Init.BaudRate=baudrate;
	}
	huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart->Init.Parity = UART_PARITY_NONE;
	huart->Init.Mode = UART_MODE_TX_RX;
	huart->Init.OverSampling = UART_OVERSAMPLING_16;
	huart->Init.WordLength = UART_WORDLENGTH_8B;
	huart->Init.StopBits = UART_STOPBITS_1;
	if(UART_Init(huart)!= SYS_OK){
		Error_Handler();
	}
}
*/
void SerialLin6_init(UART_HandleTypeDef *huart,uint32_t baudrate)
{
	huart->Instance=USART6;
	if(baudrate == 0)
	{
		huart->Init.BaudRate=115200;
	}else
	{
		huart->Init.BaudRate=baudrate;
	}
	huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart->Init.Parity = UART_PARITY_NONE;
	huart->Init.Mode = UART_MODE_TX_RX;
	huart->Init.OverSampling = UART_OVERSAMPLING_16;
	huart->Init.WordLength = UART_WORDLENGTH_8B;
	huart->Init.StopBits = UART_STOPBITS_1;
	huart->pRxBuffPtr = &uart6_ring_buffer_rx;
	huart->pTxBuffPtr = &uart6_ring_buffer_tx;
	huart->RxXferSize = UART_BUFFER_SIZE;
	huart->TxXferSize = UART_BUFFER_SIZE;
	if(UART_Init(huart)!= SYS_OK){
		errObj.p_address_t = USART6;
		Error_Handler(&errObj);
	}
	
}

/*
void USART1_IRQHandler(void)
{
	Uart_isr (&huart1);
}
*/

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_Handler(void)
{
  	Uart_isr (&huart2);
}

/*
void USART3_IRQHandler(void)
{
  Uart_isr (&huart3);
}*/

/**
  * @brief This function handles USART4 global interrupt.
  */
/*
void USART4_IRQHandler(void)
{
	Uart_isr (&huart4);
}
*/
/**
  * @brief This function handles USART5 global interrupt.
  */

/*
void USART5_IRQHandler(void)
{
  Uart_isr (&huart5);

}*/

/**
  * @brief This function handles USART6 global interrupt.
  */
void USART6_Handler(void)
{
	Uart_isr (&huart6);
}

void noIntWrite(UART_HandleTypeDef *huart,char ch)
{
	while(!(USART_SR_TXE & huart->Instance->SR));
	huart->Instance->DR=ch;
}
void noIntSendString(UART_HandleTypeDef *huart,char * s)
{
	while (*s != '\0')
	{
		noIntWrite(huart,(char)(*s));
		s++;
	}
}