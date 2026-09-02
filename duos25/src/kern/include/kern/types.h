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
#ifndef __TYPES_H
#define __TYPES_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#undef NULL
#define NULL 0


#define size_t uint64_t

#define SYS_GPIO_t    0x001
#define SYS_USART_t   0x002
#define SYS_SPI_t     0x003
#define SYS_I2C_t     0x004
#define SYS_CAN_t     0x005
#define SYS_RCC_t     0x006
#define SYS_RTC_t     0x007
#define SYS_ADC_t     0x008
#define SYS_DAC_t     0x009

typedef struct peripheral_type_t
{
    uint32_t p_type_t;
    void *p_address_t;
}Data_TypeDef;

typedef enum 
{
  SYS_OK       = 0x00U,
  SYS_ERROR    = 0x01U,
  SYS_BUSY     = 0x02U,
  SYS_TIMEOUT  = 0x03U,
  SYS_FRAME_ERROR = 0x04U,
  SYS_BUFFER_EMPTY = 0x05,
  SYS_NO_MESSAGE=0x06
} StatusTypeDef;

typedef enum
{
  SYS_UNLOCKED = 0x00U,
  SYS_LOCKED   = 0x01U
} LockTypeDef;

typedef enum 
{
  RESET = 0U, 
  SET = !RESET
} FlagStatus, ITStatus;

typedef enum
{
  DISABLE = 0U,
  ENABLE = !DISABLE
} FunctionalState;

#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

typedef enum
{
  SUCCESS = 0U,
  ERROR = !SUCCESS
} ErrorStatus;


typedef struct task_tcb{
	uint32_t magic_number; //here it is 0xFECABAA0
	uint16_t task_id; //a unsigned 16 bit integer starting from 1000 
	void *psp; //task stack pointer or stackframe address
	uint16_t status; //task status: running, waiting, ready, killed, or terminated
	uint32_t execution_time; //total execution time (in ms)
	uint32_t waiting_time; //total waiting time (in ms)
	uint32_t digital_sinature; //current value is 0x00000001
} TCB_TypeDef;

#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) /* ARM Compiler V6 */
  #ifndef __weak
    #define __weak  __attribute__((weak))
  #endif
  #ifndef __packed
    #define __packed  __attribute__((packed))
  #endif
#elif defined ( __GNUC__ ) && !defined (__CC_ARM) /* GNU Compiler */
  #ifndef __weak
    #define __weak   __attribute__((weak))
  #endif /* __weak */
  #ifndef __packed
    #define __packed __attribute__((__packed__))
  #endif /* __packed */
#endif /* __GNUC__ */

#if !defined(UNUSED)
#define UNUSED(X) (void)X      /* To avoid gcc/g++ warnings */
#endif /* UNUSED */

#ifndef   __INLINE
  #define __INLINE                               inline
#endif
#ifndef   __STATIC_INLINE
  #define __STATIC_INLINE                        static inline
#endif
#ifndef   __STATIC_FORCEINLINE
  #define __STATIC_FORCEINLINE                   __STATIC_INLINE
#endif

#ifndef __builtin_arm_rbit
  #define  __builtin_arm_rbit                   __rbit
#endif

#ifndef __CLZ
  #define __CLZ             __clz
#endif



__attribute__((always_inline)) __STATIC_INLINE uint32_t __RBIT(uint32_t value)
{
  uint32_t result;
  uint32_t s = (4U /*sizeof(v)*/ * 8U) - 1U; /* extra shift needed at end */

  result = value;                      /* r will be reversed bits of v; first get LSB of v */
  for (value >>= 1U; value != 0U; value >>= 1U)
  {
    result <<= 1U;
    result |= value & 1U;
    s--;
  }
  result <<= s;                        /* shift when v's highest bits are zero */
  return result;
}



#ifdef __cplusplus
}
#endif
#endif

