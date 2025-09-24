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
#ifndef __KFLOAT_H
#define __KFLOAT_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#define __DBL_INFINITY ((uint64_t)0x7FF<<52)
#define __DBL_NAN   (((uint64_t)0x7FF<<52)|((uint64_t)0b1<<51))
#define DOUBLE_FRAC_PRECISION 32
#define LARGEST_FRAC (uint32_t)0x1DCD6500
typedef union dpnum
{
    uint64_t reg;
    double num;
}__double_number;

typedef union spnum
{
    uint32_t reg;
    float num;
}__single_pnum;

double __aeabi_f2d(float);

float __aeabi_d2f(double);

int32_t __aeabi_d2iz(double);

int __aeabi_d2i(double);

uint32_t get_decimal_part(double);

double __aeabi_dadd(double, double);

float __aeabi_fadd(float,float);

double __aeabi_ui2d(uint32_t);

uint32_t __aeabi_d2uiz(double);

double __aeabi_dsub(double,double);

double __aeabi_dmul(double,double);

uint64_t __aeabi_d2ulz(double);

double __aeabi_ddiv(double,double);

#ifdef __cplusplus
}
#endif
#endif
