#ifndef __SERIAL_LIN_H_
#define __SERIAL_LIN_H_
#ifdef __cplusplus
extern "C" {
#endif
#include <sys_usart.h>



void SerialLin1_init(UART_HandleTypeDef*,uint32_t);
void SerialLin2_init(UART_HandleTypeDef*,uint32_t);
void SerialLin3_init(UART_HandleTypeDef*,uint32_t);
void SerialLin4_init(UART_HandleTypeDef*,uint32_t);
void SerialLin5_init(UART_HandleTypeDef*,uint32_t);
void SerialLin6_init(UART_HandleTypeDef*,uint32_t);

void USART2_Handler(void);

void noIntWrite(UART_HandleTypeDef*,char);
void noIntSendString(UART_HandleTypeDef*,char*);
#ifdef __cplusplus
}
#endif
#endif
