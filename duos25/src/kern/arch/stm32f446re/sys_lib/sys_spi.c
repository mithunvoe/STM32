#include <sys_spi.h>
#include <sys_err.h>
#include <stm32_assert.h>
#include <stm32f446xx.h>
#include <sys_gpio.h>
#include <cm4.h>

#define _SPI_HARDWARE_LSB
#define _SPI_TIMEOUT  10
//#define USE_SPI_CRC 0
#define SPI_DEFAULT_TIMEOUT 100U
#define SET 1U
#define RESET 0U

static StatusTypeDef SPI_WaitFlagStateUntilTimeout(uint32_t Flag, uint32_t State, uint32_t Timeout, uint32_t Tickstart);
static StatusTypeDef SPI_CheckFlag_BSY(uint32_t Timeout, uint32_t Tickstart);
static SPI_HandleTypeDef hSPI;
Data_TypeDef errObj={SYS_SPI_t,0};
void SPI_Init(void)
{
	hSPI.Instance=SPI1;
	hSPI.Init.Mode=SPI_MODE_MASTER;
	hSPI.Init.Direction = SPI_DIRECTION_2LINES;
	hSPI.Init.DataSize=SPI_DATASIZE_8BIT;
	hSPI.Init.CLKPolarity=SPI_POLARITY_LOW; //clock polarity low
	hSPI.Init.CLKPhase=SPI_PHASE_1EDGE;
	hSPI.Init.NSS = SPI_NSS_SOFT;
	hSPI.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_128;
	hSPI.Init.FirstBit = SPI_FIRSTBIT_LSB;
	hSPI.Init.TIMode = SPI_TIMODE_DISABLE;
	hSPI.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hSPI.Init.CRCPolynomial = 10;
	if(Init_SPI1(&hSPI) != SYS_OK)
	{ 
    errObj.p_address_t = SPI1;
		Error_Handler(&errObj);
	}
}


uint8_t reverse_bit(uint8_t num) {
    uint8_t result = 0;
    for (uint8_t i = 0; i < 8; i++) {
        result <<= 1;
        result += (num & 1);
        num >>= 1;
    }
    return result;
}

StatusTypeDef Init_SPI1(SPI_HandleTypeDef *hSPI)
{
    assert_param(IS_SPI_ALL_INATANCE(hSPI->Instance));
    assert_param(IS_SPI_MODE(hSPI->Init.Mode));
    assert_param(IS_SPI_DIRECTION(hSPI->Init.Direction));
    assert_param(IS_SPI_DATASIZE(hSPI->Init.DataSize));
    assert_param(IS_SPI_CPOL(hSPI->Init.CLKPolarity));
    assert_param(IS_SPI_CPHA(hSPI->Init.CLKPhase));
    assert_param(IS_SPI_NSS(hSPI->Init.NSS));
    assert_param(IS_SPI_BAUDRATE_PRESCALER(hSPI->Init.BaudRatePrescaler));
    assert_param(IS_SPI_FIRST_BIT(hSPI->Init.FirstBit));
	#if (USE_SPI_CRC != 0U)
	assert_param(IS_SPI_CRC_CALCULATION(hSPI->>Init.CRCCalculation));
  if(hSPI->>Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
  {
    assert_param(IS_SPI_CRC_POLYNOMIAL(hSPI->Init.CRCPolynomial));
  }
#else
  hSPI->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
#endif /* USE_SPI_CRC */
	 if(hSPI->State == SPI_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hSPI->Lock = SYS_UNLOCKED;

    /* Init the low level hardware : GPIO, CLOCK, NVIC... */
    SPI_GPIOInit(hSPI);
  }
	hSPI->State = SPI_STATE_BUSY;
	__SPI_DISABLE(hSPI);
	WRITE_REG(hSPI->Instance->CR1,(hSPI->Init.Mode | hSPI->Init.Direction | hSPI->Init.DataSize| hSPI->Init.CLKPolarity | hSPI->Init.CLKPhase |(hSPI->Init.NSS & SPI_CR1_SSM)|hSPI->Init.BaudRatePrescaler|hSPI->Init.FirstBit|hSPI->Init.CRCCalculation));
	WRITE_REG(hSPI->Instance->CR2,(((hSPI->Init.NSS >> 16U) & SPI_CR2_SSOE) | hSPI->Init.TIMode));
  
	#if (USE_SPI_CRC != 0U)
  /*---------------------------- SPIx CRCPOLY Configuration ------------------*/
  /* Configure : CRC Polynomial */
  if(hSPI->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
  {
    WRITE_REG(hSPI->Instance->CRCPR, hSPI->Init.CRCPolynomial);
  }
#endif /* USE_SPI_CRC */
	#if defined(SPI_I2SCFGR_I2SMOD)
  /* Activate the SPI mode (Make sure that I2SMOD bit in I2SCFGR register is reset) */
  CLEAR_BIT(hSPI->Instance->I2SCFGR, SPI_I2SCFGR_I2SMOD);
#endif /* SPI_I2SCFGR_I2SMOD */

  hSPI->ErrorCode = SPI_ERROR_NONE;
  hSPI->State = SPI_STATE_READY;
	return SYS_OK;
}

void spi_rw(uint8_t* data, uint8_t size)
	{
		GPIO_WritePin(SS_GPIO_Port,SS_Pin,GPIO_PIN_RESET);
		ms_delay(1);
		#ifndef _SPI_HARDWARE_LSB
		SPI_TransmitReceive(SPI1,data,data,size,_SPI_TIMEOUT);
		for (uint8_t i = 0; i < size; i++) {
        data[i] = reverse_bit(data[i]);
    }
    //kprintf("We are inside\n"); 
		#else
    //kprintf("We are inside here before: %x\n",data);
		SPI_TransmitReceive(data,data,size,_SPI_TIMEOUT);
    //kprintf("We are inside here: %d %x\n",status,data); 
		#endif
		ms_delay(1);
		GPIO_WritePin(SS_GPIO_Port,SS_Pin,GPIO_PIN_SET);
	}
StatusTypeDef SPI_TransmitReceive(uint8_t* pTxData,uint8_t* pRxData,uint8_t size,uint32_t timeout)
	{
		uint32_t tmp = 0U, tmp1 = 0U;
		uint32_t tickstart;
		#if (USE_SPI_CRC != 0U)
		volatile uint16_t tmpreg1 = 0U;
		#endif /* USE_SPI_CRC */
		uint32_t txallowed = 1U;
		StatusTypeDef errorcode = SYS_OK;
		//assert_param(IS_SPI_DIRECTION_2LINES(hSPI));
		__SYS_LOCK(&hSPI);
		tickstart=getmsTick();
		tmp = hSPI.State;
		tmp1 = hSPI.Init.Mode;
		if(!((tmp == SPI_STATE_READY) || ((tmp1 == SPI_MODE_MASTER) && (hSPI.Init.Direction == SPI_DIRECTION_2LINES) && (tmp == SPI_STATE_BUSY_RX))))
		{
			errorcode = SYS_BUSY;
			return SPI_Error(errorcode);
		}
		if((pTxData == NULL ) || (pRxData == NULL ) || (size == 0U))
		{
			errorcode = SYS_ERROR;
			return SPI_Error(errorcode);
		}
		/* Don't overwrite in case of SYS_SPI_STATE_BUSY_RX */
  if(hSPI.State == SPI_STATE_READY)
		{
			hSPI.State = SPI_STATE_BUSY_TX_RX;
		}
		/* Set the transaction information */
		hSPI.ErrorCode   = SPI_ERROR_NONE;
		hSPI.pTxBuffPtr  = (uint8_t*)pTxData;
		hSPI.TxXferSize  = size;
		hSPI.TxXferCount = size;
		hSPI.pRxBuffPtr  = (uint8_t*)pRxData;
		hSPI.RxXferSize  = size;
		hSPI.RxXferCount = size;
    //kprintf("Data sending %x\r\n",hSPI.pTxBuffPtr);
		/* Init field not used in handle to zero */
		hSPI.RxISR       = NULL;
		hSPI.TxISR       = NULL;
		
#if (USE_SPI_CRC != 0U)
  /* Reset CRC Calculation */
  if(hSPI->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
  {
    SPI_RESET_CRC(hspi);
  }
#endif /* USE_SPI_CRC */
	/* Check if the SPI is already enabled */
  if((hSPI.Instance->CR1 &SPI_CR1_SPE) != SPI_CR1_SPE)
  {
    /* Enable SPI peripheral */
    __SPI_ENABLE(&hSPI);
  }
	/* Transmit and Receive data in 16 Bit mode */
  if(hSPI.Init.DataSize == SPI_DATASIZE_16BIT)
  {
    if((hSPI.Init.Mode == SPI_MODE_SLAVE) || (hSPI.TxXferCount == 0x01U))
    {
      hSPI.Instance->DR = (*((uint16_t *)pTxData));
      pTxData += sizeof(uint16_t);
      hSPI.TxXferCount--;
    }
    while ((hSPI.TxXferCount > 0U) || (hSPI.RxXferCount > 0U))
    {
      /* Check TXE flag */
      if(txallowed && (hSPI.TxXferCount > 0U) && (__SPI_GET_FLAG(&hSPI, SPI_FLAG_TXE)))
      {
        hSPI.Instance->DR = *((uint16_t*)pTxData);
        pTxData += sizeof(uint16_t);
        hSPI.TxXferCount--;
        /* Next Data is a reception (Rx). Tx not allowed */ 
        txallowed = 0U;

#if (USE_SPI_CRC != 0U)
        /* Enable CRC Transmission */
        if((hspi->TxXferCount == 0U) && (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE))
        {
          SET_BIT(hspi->Instance->CR1, SPI_CR1_CRCNEXT);
        }
#endif /* USE_SPI_CRC */
      }

      /* Check RXNE flag */
      if((hSPI.RxXferCount > 0U) && (__SPI_GET_FLAG(&hSPI, SPI_FLAG_RXNE)))
      {
        *(uint16_t *)pRxData = (uint16_t)(hSPI.Instance->DR);
        pRxData += sizeof(uint16_t);
        hSPI.RxXferCount--;
        /* Next Data is a Transmission (Tx). Tx is allowed */ 
        txallowed = 1U;
      }
      if((timeout != MAX_DELAY) && ((getmsTick()-tickstart) >=  timeout))
      {
        errorcode = SYS_TIMEOUT;
        return SPI_Error(errorcode);
      }
    }
  }
	else
  {
    if((hSPI.Init.Mode == SPI_MODE_SLAVE) || (hSPI.TxXferCount == 0x01U))
    {
      *((volatile uint8_t*)&hSPI.Instance->DR) = (*pTxData);
      pTxData += sizeof(uint8_t);
      hSPI.TxXferCount--;
    }
    while((hSPI.TxXferCount > 0U) || (hSPI.RxXferCount > 0U))
    {
      /* check TXE flag */
      if(txallowed && (hSPI.TxXferCount > 0U) && (__SPI_GET_FLAG(&hSPI, SPI_FLAG_TXE)))
      {
        *(volatile uint8_t *)&hSPI.Instance->DR = (*pTxData++);
        hSPI.TxXferCount--;
        /* Next Data is a reception (Rx). Tx not allowed */ 
        txallowed = 0U;

#if (USE_SPI_CRC != 0U)
        /* Enable CRC Transmission */
        if((hspi->TxXferCount == 0U) && (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE))
        {
          SET_BIT(hspi->Instance->CR1, SPI_CR1_CRCNEXT);
        }
#endif /* USE_SPI_CRC */
      }

      /* Wait until RXNE flag is reset */
      if((hSPI.RxXferCount > 0U) && (__SPI_GET_FLAG(&hSPI, SPI_FLAG_RXNE)))
      {
        (*(uint8_t *)pRxData++) = (uint8_t)(hSPI.Instance->DR);
        hSPI.RxXferCount--;
        /* Next Data is a Transmission (Tx). Tx is allowed */ 
        txallowed = 1U;
      }
      if((timeout != MAX_DELAY) && ((getmsTick()-tickstart) >=  timeout))
      {
        errorcode = SYS_TIMEOUT;
        return SPI_Error(errorcode);
      }
    }
  }
	#if (USE_SPI_CRC != 0U)
  /* Read CRC from DR to close CRC calculation process */
  if(hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
  {
    /* Wait until TXE flag */
    if(SPI_WaitFlagStateUntilTimeout(hspi, SPI_FLAG_RXNE, SET, Timeout, tickstart) != SYS_OK)
    {
      /* Error on the CRC reception */
      SET_BIT(hspi->ErrorCode, SPI_ERROR_CRC);
      errorcode = SYS_TIMEOUT;
      return SPI_Error(hSPI,errorcode);
    }
    /* Read CRC */
    tmpreg1 = hspi->Instance->DR;
    /* To avoid GCC warning */
    UNUSED(tmpreg1);
  }

  /* Check if CRC error occurred */
  if(__SPI_GET_FLAG(hspi, SPI_FLAG_CRCERR) != RESET)
  {
    /* Check if CRC error is valid or not (workaround to be applied or not) */
    if (SPI_ISCRCErrorValid(hspi) == SPI_VALID_CRC_ERROR)
    {
      SET_BIT(hspi->ErrorCode, SPI_ERROR_CRC);

      /* Reset CRC Calculation */
      SPI_RESET_CRC(hspi);

   	  errorcode = SYS_ERROR;
    }
    else
    {
      __SPI_CLEAR_CRCERRFLAG(hspi);
    }
  }
#endif /* USE_SPI_CRC */
/* Wait until TXE flag */
  if(SPI_WaitFlagStateUntilTimeout(SPI_FLAG_TXE, SET, timeout, tickstart) != SYS_OK)
  {
    errorcode = SYS_TIMEOUT;
    return SPI_Error(errorcode);
  }
  
  /* Check Busy flag */
  if(SPI_CheckFlag_BSY(timeout,tickstart) != SYS_OK)
  {
    errorcode = SYS_ERROR;
    hSPI.ErrorCode = SPI_ERROR_FLAG;
    return SPI_Error(errorcode);
  }

  /* Clear overrun flag in 2 Lines communication mode because received is not read */
  if(hSPI.Init.Direction == SPI_DIRECTION_2LINES)
  {
    __SPI_CLEAR_OVRFLAG(&hSPI);
  }
	hSPI.State = SPI_STATE_READY;
	__SYS_UNLOCK(&hSPI);
	return SPI_Error(errorcode);
	}

/* Transmission/Receive Mode 10*/	
StatusTypeDef SPI_TransmitReceiveMod(uint8_t* pTxData,uint8_t* pRxData,uint8_t size,uint32_t timeout)
  {
    
    return SYS_OK;
  }

StatusTypeDef SPI_Error(StatusTypeDef errcd)
	{
  /* Process Unlocked */
  __SYS_UNLOCK(&hSPI);
  return errcd;
}

static StatusTypeDef SPI_WaitFlagStateUntilTimeout(uint32_t Flag, uint32_t State, uint32_t Timeout, uint32_t Tickstart)
{
  while((((hSPI.Instance->SR & Flag) == (Flag)) ? SET : RESET) != State)
  {
    if(Timeout != MAX_DELAY)
    {
      if((Timeout == 0U) || ((getmsTick()-Tickstart) >= Timeout))
      {
        /* Disable the SPI and reset the CRC: the CRC value should be cleared
        on both master and slave sides in order to resynchronize the master
        and slave for their respective CRC calculation */

        /* Disable TXE, RXNE and ERR interrupts for the interrupt process */
        __SPI_DISABLE_IT(&hSPI, (SPI_IT_TXE | SPI_IT_RXNE | SPI_IT_ERR));

        if((hSPI.Init.Mode == SPI_MODE_MASTER)&&((hSPI.Init.Direction == SPI_DIRECTION_1LINE)||(hSPI.Init.Direction == SPI_DIRECTION_2LINES_RXONLY)))
        {
          /* Disable SPI peripheral */
          __SPI_DISABLE(&hSPI);
        }

        /* Reset CRC Calculation */
        if(hSPI.Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
        {
          SPI_RESET_CRC(&hSPI);
        }

        hSPI.State= SPI_STATE_READY;

        /* Process Unlocked */
        __SYS_UNLOCK(&hSPI);

        return SYS_TIMEOUT;
      }
    }
  }

  return SYS_OK;
}

static StatusTypeDef SPI_CheckFlag_BSY(uint32_t Timeout, uint32_t Tickstart)
{
  /* Control the BSY flag */
  if(SPI_WaitFlagStateUntilTimeout(SPI_FLAG_BSY, RESET, Timeout, Tickstart) != SYS_OK)
  {
    SET_BIT(hSPI.ErrorCode, SPI_ERROR_FLAG);
    return SYS_TIMEOUT;
  }
  return SYS_OK;
}
