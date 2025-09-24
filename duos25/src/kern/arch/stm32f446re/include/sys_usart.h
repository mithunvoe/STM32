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
#ifndef __SYS_USART_H
#define __SYS_USART_H
#ifdef __cplusplus
extern "C" {
#endif
#include <sys_bus_matrix.h>
#include <stdint.h>
#include <types.h>

#define USART1_DEV  0x0001
#define USART2_DEV  0x0002
#define USART3_DEV  0x0004
#define USART4_DEV  0x0008
#define USART5_DEV  0x0010
#define USART6_DEV  0x0020

#define USE_UART_REGISTER_CALLBACKS 0
#define UART_IT_MASK                     0x0000FFFFU
#define UART_CR1_REG_INDEX               1U
#define UART_CR2_REG_INDEX               2U
#define UART_CR3_REG_INDEX               3U


#define UART_BUFFER_SIZE 512
#define UART2_BUFFER_SIZE 512
/**
 * USART/UART Buffer data structure -- ring buffer 
**/
typedef struct __ring_in_buffer_t
{
  unsigned char buffer[UART_BUFFER_SIZE];
  volatile unsigned int head;
  volatile unsigned int tail;
} ring_buffer;

typedef struct __ring_in_buffer2_t
{
  unsigned char buffer[UART2_BUFFER_SIZE];
  volatile unsigned int head;
  volatile unsigned int tail;
} ring_buffer2;

/**
* USART Initialize type for setup
**/
typedef struct
{
  uint32_t BaudRate;                  /*!< This member configures the UART communication baud rate.
                                           The baud rate is computed using the following formula:
                                           - IntegerDivider = ((PCLKx) / (8 * (OVR8+1) * (huart->Init.BaudRate)))
                                           - FractionalDivider = ((IntegerDivider - ((uint32_t) IntegerDivider)) * 8 * (OVR8+1)) + 0.5
                                           Where OVR8 is the "oversampling by 8 mode" configuration bit in the CR1 register. */

  uint32_t WordLength;                /*!< Specifies the number of data bits transmitted or received in a frame.
                                           This parameter can be a value of @ref UART_Word_Length */

  uint32_t StopBits;                  /*!< Specifies the number of stop bits transmitted.
                                           This parameter can be a value of @ref UART_Stop_Bits */

  uint32_t Parity;                    /*!< Specifies the parity mode.
                                           This parameter can be a value of @ref UART_Parity
                                           @note When parity is enabled, the computed parity is inserted
                                                 at the MSB position of the transmitted data (9th bit when
                                                 the word length is set to 9 data bits; 8th bit when the
                                                 word length is set to 8 data bits). */

  uint32_t Mode;                      /*!< Specifies whether the Receive or Transmit mode is enabled or disabled.
                                           This parameter can be a value of @ref UART_Mode */

  uint32_t HwFlowCtl;                 /*!< Specifies whether the hardware flow control mode is enabled or disabled.
                                           This parameter can be a value of @ref UART_Hardware_Flow_Control */

  uint32_t OverSampling;              /*!< Specifies whether the Over sampling 8 is enabled or disabled, to achieve higher speed (up to fPCLK/8).
                                           This parameter can be a value of @ref UART_Over_Sampling */
} UART_InitTypeDef;

/**
* USART status type definition
**/

typedef enum
{
  UART_STATE_RESET             = 0x00U,    /*!< Peripheral is not yet Initialized
                                                   Value is allowed for gState and RxState */
  UART_STATE_READY             = 0x20U,    /*!< Peripheral Initialized and ready for use
                                                  Value is allowed for gState and RxState */
  UART_STATE_BUSY              = 0x24U,    /*!< an internal process is ongoing
                                                   Value is allowed for gState only */
  UART_STATE_BUSY_TX           = 0x21U,    /*!< Data Transmission process is ongoing
                                                   Value is allowed for gState only */
  UART_STATE_BUSY_RX           = 0x22U,    /*!< Data Reception process is ongoing
                                                   Value is allowed for RxState only */
  UART_STATE_BUSY_TX_RX        = 0x23U,    /*!< Data Transmission and Reception process is ongoing
                                                   Not to be used for neither gState nor RxState.
                                                   Value is result of combination (Or) between gState and RxState values */
  UART_STATE_TIMEOUT           = 0xA0U,    /*!< Timeout state
                                                   Value is allowed for gState only */
  UART_STATE_ERROR             = 0xE0U     /*!< Error
                                                   Value is allowed for gState only */
} UART_StateTypeDef;


/**
* USART Handler type definition
**/
typedef struct __UART_HandleTypeDef
{
  USART_TypeDef                 *Instance;        /*!< UART registers base address        */

  UART_InitTypeDef              Init;             /*!< UART communication parameters      */

  ring_buffer                   *pTxBuffPtr;      /*!< Pointer to UART Tx transfer Buffer */

  uint16_t                      TxXferSize;       /*!< UART Tx Transfer size              */

  volatile uint16_t             TxXferCount;      /*!< UART Tx Transfer Counter           */

  ring_buffer                   *pRxBuffPtr;      /*!< Pointer to UART Rx transfer Buffer */

  uint16_t                      RxXferSize;       /*!< UART Rx Transfer size              */

  volatile uint16_t             RxXferCount;      /*!< UART Rx Transfer Counter           */

  //DMA_HandleTypeDef             *hdmatx;          /*!< UART Tx DMA Handle parameters      */

  //DMA_HandleTypeDef             *hdmarx;          /*!< UART Rx DMA Handle parameters      */

  LockTypeDef               Lock;             /*!< Locking object                     */

  volatile UART_StateTypeDef    gState;           /*!< UART state information related to global Handle management
                                                       and also related to Tx operations.
                                                       This parameter can be a value of @ref HAL_UART_StateTypeDef */

  volatile UART_StateTypeDef    RxState;          /*!< UART state information related to Rx operations.
                                                       This parameter can be a value of @ref HAL_UART_StateTypeDef */

  volatile uint32_t                 ErrorCode;        /*!< UART Error code                    */
	
#if (USE_UART_REGISTER_CALLBACKS == 1)
  void (* TxHalfCpltCallback)(struct __UART_HandleTypeDef *huart);        /*!< UART Tx Half Complete Callback        */
  void (* TxCpltCallback)(struct __UART_HandleTypeDef *huart);            /*!< UART Tx Complete Callback             */
  void (* RxHalfCpltCallback)(struct __UART_HandleTypeDef *huart);        /*!< UART Rx Half Complete Callback        */
  void (* RxCpltCallback)(struct __UART_HandleTypeDef *huart);            /*!< UART Rx Complete Callback             */
  void (* ErrorCallback)(struct __UART_HandleTypeDef *huart);             /*!< UART Error Callback                   */
  void (* AbortCpltCallback)(struct __UART_HandleTypeDef *huart);         /*!< UART Abort Complete Callback          */
  void (* AbortTransmitCpltCallback)(struct __UART_HandleTypeDef *huart); /*!< UART Abort Transmit Complete Callback */
  void (* AbortReceiveCpltCallback)(struct __UART_HandleTypeDef *huart);  /*!< UART Abort Receive Complete Callback  */
  void (* WakeupCallback)(struct __UART_HandleTypeDef *huart);            /*!< UART Wakeup Callback                  */

  void (* MspInitCallback)(struct __UART_HandleTypeDef *huart);           /*!< UART Msp Init callback                */
  void (* MspDeInitCallback)(struct __UART_HandleTypeDef *huart);         /*!< UART Msp DeInit callback              */
#endif  /* USE_UART_REGISTER_CALLBACKS */

} UART_HandleTypeDef ;


#define UART_IT_PE                       ((uint32_t)(UART_CR1_REG_INDEX << 28U | USART_CR1_PEIE))
#define UART_IT_TXE                      ((uint32_t)(UART_CR1_REG_INDEX << 28U | USART_CR1_TXEIE))
#define UART_IT_TC                       ((uint32_t)(UART_CR1_REG_INDEX << 28U | USART_CR1_TCIE))
#define UART_IT_RXNE                     ((uint32_t)(UART_CR1_REG_INDEX << 28U | USART_CR1_RXNEIE))
#define UART_IT_IDLE                     ((uint32_t)(UART_CR1_REG_INDEX << 28U | USART_CR1_IDLEIE))

#define UART_IT_LBD                      ((uint32_t)(UART_CR2_REG_INDEX << 28U | USART_CR2_LBDIE))

#define UART_IT_CTS                      ((uint32_t)(UART_CR3_REG_INDEX << 28U | USART_CR3_CTSIE))
#define UART_IT_ERR                      ((uint32_t)(UART_CR3_REG_INDEX << 28U | USART_CR3_EIE))

#define UART_PARITY_NONE                    0x00000000U
#define UART_PARITY_EVEN                    ((uint32_t)USART_CR1_PCE)
#define UART_PARITY_ODD                     ((uint32_t)(USART_CR1_PCE | USART_CR1_PS))

#define UART_MODE_RX                        ((uint32_t)USART_CR1_RE)
#define UART_MODE_TX                        ((uint32_t)USART_CR1_TE)
#define UART_MODE_TX_RX                     ((uint32_t)(USART_CR1_TE | USART_CR1_RE))

#define UART_WORDLENGTH_8B                  0x00000000U
#define UART_WORDLENGTH_9B                  ((uint32_t)USART_CR1_M)

#define UART_STOPBITS_1                     0x00000000U
#define UART_STOPBITS_2                     ((uint32_t)USART_CR2_STOP_1)

#define UART_HWCONTROL_NONE                  0x00000000U
#define UART_HWCONTROL_RTS                   ((uint32_t)USART_CR3_RTSE)
#define UART_HWCONTROL_CTS                   ((uint32_t)USART_CR3_CTSE)
#define UART_HWCONTROL_RTS_CTS               ((uint32_t)(USART_CR3_RTSE | USART_CR3_CTSE))

#define UART_ERROR_NONE              0x00000000U   /*!< No error            */
#define UART_ERROR_PE                0x00000001U   /*!< Parity error        */
#define UART_ERROR_NE                0x00000002U   /*!< Noise error         */
#define UART_ERROR_FE                0x00000004U   /*!< Frame error         */
#define UART_ERROR_ORE               0x00000008U   /*!< Overrun error       */
#define UART_ERROR_DMA               0x00000010U   /*!< DMA transfer error  */

#define UART_OVERSAMPLING_16                    0x00000000U
#define UART_OVERSAMPLING_8                     ((uint32_t)USART_CR1_OVER8)

#if (USE_UART_REGISTER_CALLBACKS == 1)
#define  UART_ERROR_INVALID_CALLBACK 0x00000020U   /*!< Invalid Callback error  */
#endif /* USE_UART_REGISTER_CALLBACKS */
#define __UART_ENABLE(__HANDLE__)               ((__HANDLE__)->Instance->CR1 |=  USART_CR1_UE)
#define __UART_DISABLE(__HANDLE__)              ((__HANDLE__)->Instance->CR1 &=  ~USART_CR1_UE)

#define __UART_ENABLE_IT(__HANDLE__, __INTERRUPT__)   ((((__INTERRUPT__) >> 28U) == UART_CR1_REG_INDEX)? ((__HANDLE__)->Instance->CR1 |= ((__INTERRUPT__) & UART_IT_MASK)): \
                                                           (((__INTERRUPT__) >> 28U) == UART_CR2_REG_INDEX)? ((__HANDLE__)->Instance->CR2 |= ((__INTERRUPT__) & UART_IT_MASK)): \
                                                           ((__HANDLE__)->Instance->CR3 |= ((__INTERRUPT__) & UART_IT_MASK)))
#define __UART_DISABLE_IT(__HANDLE__, __INTERRUPT__)  ((((__INTERRUPT__) >> 28U) == UART_CR1_REG_INDEX)? ((__HANDLE__)->Instance->CR1 &= ~((__INTERRUPT__) & UART_IT_MASK)): \
                                                           (((__INTERRUPT__) >> 28U) == UART_CR2_REG_INDEX)? ((__HANDLE__)->Instance->CR2 &= ~((__INTERRUPT__) & UART_IT_MASK)): \
                                                           ((__HANDLE__)->Instance->CR3 &= ~ ((__INTERRUPT__) & UART_IT_MASK)))

#define __RCC_USART2_CLK_DISABLE() (RCC->APB1ENR &= ~(RCC_APB1ENR_USART2EN))
#define __RCC_USART6_CLK_DISABLE() (RCC->APB2ENR &= ~(RCC_APB2ENR_USART6EN))


/******************** USART Instances : Synchronous mode **********************/
#define IS_USART_INSTANCE(INSTANCE) (((INSTANCE) == USART1) || \
                                     ((INSTANCE) == USART2) || \
                                     ((INSTANCE) == USART3) || \
                                     ((INSTANCE) == USART6))

/******************** UART Instances : Half-Duplex mode **********************/
#define IS_UART_HALFDUPLEX_INSTANCE(INSTANCE) (((INSTANCE) == USART1) || \
                                               ((INSTANCE) == USART2) || \
                                               ((INSTANCE) == USART3) || \
                                               ((INSTANCE) == UART4)  || \
                                               ((INSTANCE) == UART5)  || \
                                               ((INSTANCE) == USART6))

/* Legacy defines */
#define IS_UART_INSTANCE          IS_UART_HALFDUPLEX_INSTANCE

/****************** UART Instances : Hardware Flow control ********************/
#define IS_UART_HWFLOW_INSTANCE(INSTANCE) (((INSTANCE) == USART1) || \
                                           ((INSTANCE) == USART2) || \
                                           ((INSTANCE) == USART3) || \
                                           ((INSTANCE) == UART4)  || \
                                           ((INSTANCE) == UART5)  || \
                                           ((INSTANCE) == USART6))
/******************** UART Instances : LIN mode **********************/
#define IS_UART_LIN_INSTANCE          IS_UART_HALFDUPLEX_INSTANCE

#define IS_UART_HARDWARE_FLOW_CONTROL(FLOW_CTRL) (((FLOW_CTRL) == UART_HWCONTROL_NONE) || \
                                                  ((FLOW_CTRL) == UART_HWCONTROL_CTS) || \
                                                  ((FLOW_CTRL) == UART_HWCONTROL_RTS) || \
                                                  ((FLOW_CTRL) == UART_HWCONTROL_RTS_CTS))

#define IS_UART_WORD_LENGTH(WORD_LENGTH) (((WORD_LENGTH) == UART_WORDLENGTH_8B) || \
                                          ((WORD_LENGTH) == UART_WORDLENGTH_9B))

#define IS_UART_OVERSAMPLING(OVERSAMPLE) (((OVERSAMPLE) == UART_OVERSAMPLING_8) || \
                                          ((OVERSAMPLE) == UART_OVERSAMPLING_16))

//#define IS_UART_BAUDRATE(BAUD_RATE) (((BAUD_RATE) == IS_UART_BAUDRATE))

#define IS_UART_STOPBITS(STOP_BIT)  (((STOP_BIT) == UART_STOPBITS_1) || \
                                     ((STOP_BIT) == UART_STOPBITS_2))

#define IS_UART_PARITY(PARITY)      (((PARITY) == UART_PARITY_EVEN) ||  \
                                     ((PARITY) == UART_PARITY_ODD)  ||  \
                                     ((PARITY) == UART_PARITY_NONE))

#define IS_UART_MODE(MODE)          (((MODE) == UART_MODE_RX)  || \
                                     ((MODE) == UART_MODE_TX) || \
                                     ((MODE) == UART_MODE_TX_RX))
                                     
void UART_MspInit(UART_HandleTypeDef*);

uint16_t UART_BRR_SAMPLING16(uint32_t,uint32_t);

StatusTypeDef UART_Init(UART_HandleTypeDef* huart);

/* Using UARTs */

#ifdef __cplusplus
}
#endif

#endif
