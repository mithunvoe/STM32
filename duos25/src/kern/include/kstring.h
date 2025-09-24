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
 
#ifndef __STRING_H
#define __STRING_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <types.h>
int __str_to_num(uint8_t*,uint8_t);
void __reverse_str(uint8_t*);
uint32_t __strlen(uint8_t*);
uint8_t * convert(int,uint8_t);
uint8_t * convertu32(uint32_t,uint8_t);
uint8_t *float2str(float);
float str2float(uint8_t*);
void *kmemset(void*,uint8_t,size_t);
void StrCat(char*,char*);
void strcopy(uint8_t*,const uint8_t*);
void clear_str(uint8_t*,uint32_t);
void strncopy(uint8_t*,const uint8_t*,uint32_t,uint32_t);
void byte_to_hex(uint8_t*,uint8_t);
void show_byte_hex_str(uint8_t);
void show_byte_stream_hex(uint8_t *,uint32_t);
uint8_t strcomp(uint8_t*,uint8_t*);
void strn4mcopy(uint8_t*,uint8_t*,uint32_t, uint32_t,uint32_t);
void strncopy_cmd(uint8_t*,const uint8_t*,const uint8_t);
void uint16_to_str(uint8_t*,uint16_t);
uint32_t strCat_n_to_m(uint8_t* target,uint8_t *src,uint32_t tr_start, uint32_t start,size_t size);
#ifdef __cplusplus
}
#endif
#endif
