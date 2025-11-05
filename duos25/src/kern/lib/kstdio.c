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
 #include <stdarg.h>
#include <kstdio.h>
#include <sys_usart.h>
#include <UsartRingBuffer.h>
#include <kstring.h>
#include <float.h>
#include <system_config.h>

/**
* first argument define the type of string to kprintf and kscanf, 
* %c for charater
* %s for string, 
* %d for integer
* %x hexadecimal
* %o octal number
* %f for floating point number
*/
// Simplified version of printf
void kprintf(char *format,...)
{
//write your code here
	char *tr;
	uint32_t i;
	uint8_t *str;
	va_list list;
	double dval;
	//uint32_t *intval;
	va_start(list,format);
	for(tr = format;*tr != '\0';tr++)
	{
		while(*tr != '%' && *tr!='\0')
		{
			Uart_write(*tr,__CONSOLE);
			tr++;
		}
		if(*tr == '\0') break;
		tr++;
		switch (*tr)
		{
		case 'c': i = va_arg(list,int);
			Uart_write(i,__CONSOLE);
			break;
		case 'd': i = va_arg(list,int);
			if(i<0)
			{
				Uart_write('-',__CONSOLE);
				i=-i;				
			}
			Uart_sendstring((char*)convert(i,10),__CONSOLE);
			break;
		case 'o': i = va_arg(list,int);
			if(i<0)
			{
				Uart_write('-',__CONSOLE);
				i=-i;				
			}
			Uart_sendstring((char*)convert(i,8),__CONSOLE);
			break;
		case 'x': i = va_arg(list,int);
			/*if(i<0)
			{
				Uart_write('-',__CONSOLE);
				i=-i;				
			}*/
			Uart_sendstring((char*)convertu32(i,16),__CONSOLE);
			break;
		case 'u':	
		case 's': str = va_arg(list,uint8_t*);
			Uart_sendstring((char*)str,__CONSOLE);
			break;
		case 'f': 
			dval = va_arg(list,double);
			Uart_sendstring((char*)float2str(dval),__CONSOLE);
			break;	
		default:
			break;
		}
	}
	va_end(list);
}

void putstr(const uint8_t *str,size_t size)
{
	for(uint32_t i=0;i<size;i++)
	{
		Uart_write(str[i],__CONSOLE);
	}
}

/* Helper function to read a string from UART until whitespace or newline */
static void read_string_from_uart(uint8_t *buff, uint32_t max_len)
{
	uint32_t idx = 0U;
	int c;
	
	/* Wait for data to be available */
	while (IsDataAvailable(__CONSOLE) == 0) {
		/* Busy wait */
	}
	
	/* Read characters until whitespace, newline, or buffer full */
	while (idx < (max_len - 1U)) {
		/* Wait for data */
		while (IsDataAvailable(__CONSOLE) == 0) {
			/* Busy wait */
		}
		
		c = Uart_read(__CONSOLE);
		if (c < 0) {
			break;
		}
		
		/* Stop on whitespace or newline */
		if ((uint8_t)c == '\n' || (uint8_t)c == '\r' || (uint8_t)c == ' ' || (uint8_t)c == '\t') {
			/* Echo the newline/whitespace for user feedback */
			if ((uint8_t)c == '\n' || (uint8_t)c == '\r') {
				Uart_write('\r', __CONSOLE);
				Uart_write('\n', __CONSOLE);
			}
			break;
		}
		
		/* Echo character back for user feedback */
		Uart_write((uint8_t)c, __CONSOLE);
		
		/* Store character */
		buff[idx++] = (uint8_t)c;
	}
	
	/* Null terminate */
	buff[idx] = '\0';
}

// Simplified version of scanf
void kscanf(char *format,...)
{
	va_list list;
	char *ptr;
	uint8_t buff[50];
	uint8_t *str;
	int len;
	int c;
	
	ptr = format;
	va_start(list, format);
	
	while (*ptr)
	{
		if(*ptr == '%') //looking for format of an input
		{
			ptr++;
			switch (*ptr)
			{
			case 'c': //character
				/* Wait for data */
				while (IsDataAvailable(__CONSOLE) == 0) {
					/* Busy wait */
				}
				c = Uart_read(__CONSOLE);
				if (c >= 0) {
					/* Echo character back */
					Uart_write((uint8_t)c, __CONSOLE);
					*(uint8_t*)va_arg(list, uint8_t*) = (uint8_t)c;
				}
				break;
				
			case 'd': //integer number (decimal)
				read_string_from_uart(buff, sizeof(buff));
				*(uint32_t*)va_arg(list, uint32_t*) = __str_to_num(buff, 10);
				break;
				
			case 's': //string without spaces
				str = va_arg(list, uint8_t*);
				read_string_from_uart(buff, sizeof(buff));
				len = __strlen(buff);
				/* Copy string including null terminator */
				for(int u = 0; u <= len; u++) {
					str[u] = buff[u];
				}
				break;
				
			case 'x': //hexadecimal number
				read_string_from_uart(buff, sizeof(buff));
				*(uint32_t*)va_arg(list, uint32_t*) = __str_to_num(buff, 16);
				break;
				
			case 'o': //octal number
				read_string_from_uart(buff, sizeof(buff));
				*(uint32_t*)va_arg(list, uint32_t*) = __str_to_num(buff, 8);
				break;
				
			case 'f': //floating point number
				read_string_from_uart(buff, sizeof(buff));
				*(float*)va_arg(list, float*) = str2float(buff);
				break;
				
			default: //rest not recognized
				break;
			}
		}
		ptr++;
	}
	va_end(list);
}
