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
 
#include <kstring.h>
#include <kmath.h>
#include <kstdio.h>
#include <kfloat.h>

static uint8_t __outbuf[50];


int __str_to_num(uint8_t* buff,uint8_t base)
{
	uint8_t sign=0;
	int decimal=0;
    uint32_t val;
	if(buff[0] == '-') 
	{
		sign=1;
		buff[0]='0';
	}
	__reverse_str(buff);
	uint32_t i=0;
	while(buff[i]){
		if(buff[i]>='0' && buff[i]<='9'){
			val=(uint32_t)(buff[i]-'0');
		}else if (buff[i]>='a' && buff[i]<='f')
		{
			val=(uint32_t)(buff[i]-97+10);
		}else if (buff[i]>='A' && buff[i]<='F')
        {
            val=(uint32_t)(buff[i]-65+10);
        }
        decimal+=val*__pow(base,i);
		i++;
	}
	if(sign) decimal = decimal*-1;
	return decimal;
}
void __reverse_str(uint8_t* buff)
{
	uint32_t n=__strlen(buff);
	uint8_t tmp;
	for(uint32_t i=0,j=n-1;i<j;i++,j--)
	{
		tmp = buff[i];
		buff[i]=buff[j];
		buff[j]=tmp;
		//kprintf("%c %c %c\n", tmp,buff[i],buff[j]);
	}
}
uint32_t __strlen(uint8_t* buff)
{
	uint32_t i=0;
	while(*buff++)i++;
	return i;
}
uint8_t * convert(int x,uint8_t base)
{
	uint8_t sign=0;
	if(x<0){
		sign=1;
		x = x*-1;
	}
	static uint8_t baseval [] = "0123456789ABCDEF";
	uint8_t *ptr;
	ptr = &__outbuf[49];
	*ptr = '\0';
	do
	{
		*--ptr = baseval[x%base];
		//kprintf("%c",baseval[x%base]);
		x /= base;
	} while (x != 0);
	if(sign){*--ptr = '-';}

	return (ptr);
}
uint8_t * convertu32(uint32_t x,uint8_t base)
{
	static uint8_t baseval [] = "0123456789ABCDEF";
	uint8_t *ptr;
	ptr = &__outbuf[49];
	*ptr = '\0';
	do
	{
		*--ptr = baseval[x%base];
		//kprintf("%c",baseval[x%base]);
		x /= base;
	} while (x != 0);

	return (ptr);
}
uint8_t *float2str(float f)
{
	uint8_t *ptr;
	static uint8_t baseval [] = "0123456789ABCDEF";
	ptr = &__outbuf[49];
	*ptr = '\0';
	int32_t d = f;
	uint32_t frac=get_decimal_part(f);
	do
	{
		*--ptr=baseval[frac%10];;
		frac/=10;
	}while(frac!=0);
	*--ptr='.';
	do
	{
		*--ptr=baseval[d%10];;
		d=d/10;
	}while (d!=0);
	return (ptr);	
}
float str2float(uint8_t* buff)
{
	float f=0;
	//first add the fractional part
	uint32_t i=0;
	uint8_t j=0;
	while(buff[i]!='.' && buff[i]!='\0') i++;
	if(buff[i]=='.'){
		buff[i]='\0';
		i++;
		j=1;
		while(buff[i]!='\0')
		{	
			f = f + (float)((float)(buff[i]-'0')/((float)__pow(10,j)));
			j++;
			i++;
		}
	}
	__reverse_str(buff);	// Reverse so that the unit's place can be evaluated first
	i=0;
	while(buff[i]!='\0')
	{	
		f=f+((buff[i]-'0')*__pow(10,i));
		i++;
	}
	return f;
}

void *kmemset(void* to_ptr,uint8_t ch,size_t size)
{
	void *tmp;
	tmp = to_ptr;
	uint32_t i=0;
	while(i<size)
	{
		(*(uint8_t *)tmp) = ch;
		tmp++;
		i++;
	}
	return to_ptr;
}

void StrCat(char *dest,char *src)
{
	while(*dest){dest++;}
	while(*src){
		*dest=*src;
		src++;
		dest++;
	}
	*dest='\0';
}

void strcopy(uint8_t* dst,const uint8_t* src)
{
	while (*src)
	{
		*dst=*src;
		dst++;
		src++;
	}
	*dst='\0';
}
/*
* Target, Size, 
*/
void clear_str(uint8_t* target,uint32_t size)
{
	uint32_t i=0;
	while(i<size) // need to check length of the array
	{
		target[i]='\0';
		i++;
	}
	target[i]='\0';
}

/*
*  strncopy ( target, src, target_start,target_end)
*/
void strncopy(uint8_t* target,const uint8_t* src,uint32_t start,uint32_t end)
{
	uint32_t j=0;
	for(uint32_t i=start;i<end;i++)
	{
		target[j]=src[i];
		j++;
	}
	target[j] = '\0';
}
void strn4mcopy(uint8_t *target,uint8_t *src,uint32_t from, uint32_t start,uint32_t end)
{
	uint32_t j=0;
	j=from;
	for(uint32_t i=start;i<end;i++)
	{
		target[j]=src[i];
		j++;
	}
}
void byte_to_hex(uint8_t *dst,uint8_t byte)
{
	static uint8_t baseval [] = "0123456789ABCDEF";
	if(byte == 0x00U){
		dst[0]='0';
		dst[1]='0';
	}else if(byte < 0x10U)
	{
		dst[0]='0';
		dst[1] = baseval[byte%16];
	}else{
		dst[1] = baseval[byte%16];
		byte = byte/16;
		dst[0] = baseval[byte%16];
	}	
}

void show_byte_hex_str(uint8_t byte)
{
	uint8_t dst[3]={0,0,0};
	byte_to_hex(dst,byte);
	kprintf("%s",dst);
}
void show_byte_stream_hex(uint8_t *byte_stream,uint32_t size)
{
	for(uint32_t i=0;i<size;i++)
	{
		show_byte_hex_str(byte_stream[i]);
	}
	kprintf("\n");
}
uint8_t strcomp(uint8_t *s1,uint8_t *s2)
{
	
	while(*s1 && *s2)
	{
		if(*s1!=*s2) 
		{
			return (*s1-*s2);
		}
		s1++;
		s2++;
	}
	return 0;
}

void strncopy_cmd(uint8_t* target,const uint8_t* src,const uint8_t delim)
{
	uint32_t i=0;
	while(src[i]!=delim)
	{
		target[i]=src[i];
		i++;
	}
	target[i]=0x7E;
}

void uint16_to_str(uint8_t *dst,uint16_t x)
{
	static uint8_t baseval [] = "0123456789ABCDEF";
	if(x==0x0000U)
	{
		dst[0]='0';
		dst[1]='0';
		dst[2]='0';
		dst[3]='0';

	}else if((x & 0xF000U)>0)
	{
		dst[3]=baseval[x%16];
		x=x/16;
		dst[2]=baseval[x%16];
		x=x/16;
		dst[1]=baseval[x%16];
		x=x/16;
		dst[0]=baseval[x%16];
	}else if((x & 0x0F00U)>0)
	{
		dst[0]='0';
		dst[3]=baseval[x%16];
		x=x/16;
		dst[2]=baseval[x%16];
		x=x/16;
		dst[1]=baseval[x%16];
	}else if((x & 0x00F0U)>0)
	{
		dst[0]='0';
		dst[1]='0';
		dst[3]=baseval[x%16];
		x=x/16;
		dst[2]=baseval[x%16];
	}else
	{
		dst[0]='0';
		dst[1]='0';
		dst[2]='0';
		dst[3]=baseval[x%16];
	}
	
}

uint32_t strCat_n_to_m(uint8_t* target,uint8_t *src,uint32_t tr_start, uint32_t start,size_t size)
{
	for(uint32_t i=start;i<(start+size);i++)
	{
		target[tr_start] = src[i];
		tr_start++;
	}
	target[tr_start]=0;
	return tr_start;
}