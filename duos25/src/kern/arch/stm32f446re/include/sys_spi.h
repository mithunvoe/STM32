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
 * 
 */

#ifndef __SYS_SPI_H
#define __SYS_SPI_H

#include <sys_bus_matrix.h>
#include <stdint.h>
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SS_Pin          GPIO_PIN_9
#define SS_GPIO_Port    GPIOA      

/* SPI configuration structure definition */
typedef struct 
{
	uint32_t Mode;	//SPI operating mode
	uint32_t Direction; 
	uint32_t DataSize;
	uint32_t CLKPolarity;
	uint32_t CLKPhase;
	uint32_t NSS;
	uint32_t BaudRatePrescaler;
	uint32_t FirstBit;
	uint32_t TIMode;	//Specify if TI mode is enable
	uint32_t CRCCalculation;
	uint32_t CRCPolynomial;
}SPI_InitTypeDef;

typedef enum
{
    SPI_STATE_RESET      = 0x00U,    /*!< Peripheral not Initialized                         */
    SPI_STATE_READY      = 0x01U,    /*!< Peripheral Initialized and ready for use           */
    SPI_STATE_BUSY       = 0x02U,    /*!< an internal process is ongoing                     */
    SPI_STATE_BUSY_TX    = 0x03U,    /*!< Data Transmission process is ongoing               */
    SPI_STATE_BUSY_RX    = 0x04U,    /*!< Data Reception process is ongoing                  */
    SPI_STATE_BUSY_TX_RX = 0x05U,    /*!< Data Transmission and Reception process is ongoing */
    SPI_STATE_ERROR      = 0x06U     /*!< SPI error state */
}SPI_StateTypeDef;

typedef struct __SPI_HandleTypeDef 
{
  SPI_TypeDef               *Instance;    /*!< SPI registers base address */
  SPI_InitTypeDef           Init;         /*!< SPI communication parameters */
  uint8_t 					        unused[2];
  uint8_t                   *pTxBuffPtr;  /*!< Pointer to SPI Tx transfer Buffer */
  //
  uint8_t 					        unused1[6];
  uint16_t                  TxXferSize;   /*!< SPI Tx Transfer size */
  volatile uint16_t         TxXferCount;  /*!< SPI Tx Transfer Counter */
  uint8_t                   *pRxBuffPtr;  /*!< Pointer to SPI Rx transfer Buffer */
  uint8_t 					        unused2[4];
  uint16_t                  RxXferSize;   /*!< SPI Rx Transfer size */
  volatile uint16_t         RxXferCount;  /*!< SPI Rx Transfer Counter */
  void                      (*RxISR)(struct __SPI_HandleTypeDef * hspi); /*!< function pointer on Rx ISR */
  void                      (*TxISR)(struct __SPI_HandleTypeDef * hspi); /*!< function pointer on Tx ISR */
  //DMA_HandleTypeDef          *hdmatx;      /*!< SPI Tx DMA Handle parameters   */
  //DMA_HandleTypeDef          *hdmarx;      /*!< SPI Rx DMA Handle parameters   */
  LockTypeDef               Lock;         /*!< Locking object                 */
  volatile SPI_StateTypeDef State;        /*!< SPI communication state */
  volatile uint32_t         ErrorCode;    /*!< SPI Error code */
}SPI_HandleTypeDef;

#define SPI_ERROR_NONE              0x00000000U   /*!< No error             */
#define SPI_ERROR_MODF              0x00000001U   /*!< MODF error           */
#define SPI_ERROR_CRC               0x00000002U   /*!< CRC error            */
#define SPI_ERROR_OVR               0x00000004U   /*!< OVR error            */
#define SPI_ERROR_FRE               0x00000008U   /*!< FRE error            */
#define SPI_ERROR_DMA               0x00000010U   /*!< DMA transfer error   */
#define SPI_ERROR_FLAG              0x00000020U   /*!< Flag: RXNE,TXE, BSY  */
#define MAX_DELAY      0xFFFFFFFFU

#define __SYS_LOCK(__HANDLE__)                                           \
                                do{                                        \
                                    if((__HANDLE__)->Lock == SYS_LOCKED)   \
                                    {                                      \
                                       return SYS_BUSY;                    \
                                    }                                      \
                                    else                                   \
                                    {                                      \
                                       (__HANDLE__)->Lock = SYS_LOCKED;    \
                                    }                                      \
                                  }while (0U)

#define __SYS_UNLOCK(__HANDLE__)                                          \
                                  do{                                       \
                                      (__HANDLE__)->Lock = SYS_UNLOCKED;    \
                                    }while (0U)


#define SPI_RESET_CRC(__HANDLE__) do{(__HANDLE__)->Instance->CR1 &= (uint16_t)(~SPI_CR1_CRCEN);\
                                     (__HANDLE__)->Instance->CR1 |= SPI_CR1_CRCEN;}while(0U)

#define __SPI_CLEAR_OVRFLAG(__HANDLE__)        \
do{                                                \
    volatile uint32_t tmpreg_ovr = 0x00U;              \
    tmpreg_ovr = (__HANDLE__)->Instance->DR;       \
    tmpreg_ovr = (__HANDLE__)->Instance->SR;       \
    UNUSED(tmpreg_ovr);                            \
  } while(0U)
//#define SET_BIT(__VAL__,__BIT__) (__VAL__|= __BIT__)
#define __SPI_GET_FLAG(__HANDLE__, __FLAG__) ((((__HANDLE__)->Instance->SR) & (__FLAG__)) == (__FLAG__))
#define __SPI_DISABLE_IT(__HANDLE__, __INTERRUPT__)  ((__HANDLE__)->Instance->CR2 &= (~(__INTERRUPT__)))
#define __SPI_DISABLE(__HANDLE__) ((__HANDLE__)->Instance->CR1 &= (~SPI_CR1_SPE))
#define __SPI_ENABLE(__HANDLE__) ((__HANDLE__)->Instance->CR1 |=  SPI_CR1_SPE)
#define UNUSED(X) (void)X
#define IS_SPI_DIRECTION(MODE) (((MODE) == SPI_DIRECTION_2LINES)        || \
                                ((MODE) == SPI_DIRECTION_2LINES_RXONLY) || \
                                ((MODE) == SPI_DIRECTION_1LINE))
#define IS_SPI_MODE(MODE) (((MODE) == SPI_MODE_SLAVE) || \
                           ((MODE) == SPI_MODE_MASTER))
#define USE_SPI_CRC 0U

#define IS_SPI_DIRECTION_2LINES(MODE) ((MODE) == SPI_DIRECTION_2LINES)

#define IS_SPI_DIRECTION_2LINES_OR_1LINE(MODE) (((MODE) == SPI_DIRECTION_2LINES)  || \
                                                ((MODE) == SPI_DIRECTION_1LINE))

#define IS_SPI_DATASIZE(DATASIZE) (((DATASIZE) == SPI_DATASIZE_16BIT) || \
                                   ((DATASIZE) == SPI_DATASIZE_8BIT))

#define IS_SPI_CPOL(CPOL) (((CPOL) == SPI_POLARITY_LOW) || \
                           ((CPOL) == SPI_POLARITY_HIGH))

#define IS_SPI_CPHA(CPHA) (((CPHA) == SPI_PHASE_1EDGE) || \
                           ((CPHA) == SPI_PHASE_2EDGE))

#define IS_SPI_NSS(NSS) (((NSS) == SPI_NSS_SOFT)       || \
                         ((NSS) == SPI_NSS_HARD_INPUT) || \
                         ((NSS) == SPI_NSS_HARD_OUTPUT))


#define IS_SPI_ALL_INATANCE(INSTANCE) (((INSTANCE) == SPI1) || \
                                       ((INSTANCE) == SPI2) || \
                                       ((INSTANCE) == SPI3) || \
                                       ((INSTANCE) == SPI4))

#define IS_SPI_BAUDRATE_PRESCALER(PRESCALER) (((PRESCALER) == SPI_BAUDRATEPRESCALER_2) || \
                                              ((PRESCALER) == SPI_BAUDRATEPRESCALER_4) || \
                                              ((PRESCALER) == SPI_BAUDRATEPRESCALER_8) || \
                                              ((PRESCALER) == SPI_BAUDRATEPRESCALER_16) || \
                                              ((PRESCALER) == SPI_BAUDRATEPRESCALER_32) || \
                                              ((PRESCALER) == SPI_BAUDRATEPRESCALER_64) || \
                                              ((PRESCALER) == SPI_BAUDRATEPRESCALER_128) || \
                                              ((PRESCALER) == SPI_BAUDRATEPRESCALER_256))

#define IS_SPI_FIRST_BIT(BIT) (((BIT) == SPI_FIRSTBIT_MSB) || \
                               ((BIT) == SPI_FIRSTBIT_LSB))

#define IS_SPI_CRC_CALCULATION(CALCULATION) (((CALCULATION) == SPI_CRCCALCULATION_DISABLE) || \
                                             ((CALCULATION) == SPI_CRCCALCULATION_ENABLE))

#define IS_SPI_CRC_POLYNOMIAL(POLYNOMIAL) (((POLYNOMIAL) >= 0x01U) && ((POLYNOMIAL) <= 0xFFFFU))



void SPI_Init(void);
void spi_rw(uint8_t* data, uint8_t size);
uint8_t reverse_bit(uint8_t num);

StatusTypeDef Init_SPI1(SPI_HandleTypeDef *);
StatusTypeDef SPI_TransmitReceive(uint8_t*,uint8_t*,uint8_t,uint32_t);
StatusTypeDef SPI_Error(StatusTypeDef);													


#ifdef __cplusplus
}
#endif
#endif
