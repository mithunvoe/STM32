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
 
#include <kfloat.h>
#include <kstdio.h>

double __aeabi_f2d(float x)
{
   __single_pnum num;
   __double_number dnum;
   dnum.num=0;
   num.num=x;
   dnum.reg |= ((uint64_t)(num.reg>>31)<<63); //sign bit
   dnum.reg |= (uint64_t) (num.reg & ~((uint32_t)0x1FF<<23))<<29;
   dnum.reg |= (uint64_t)(((uint16_t) ((num.reg>>23) & ~(1<<8)))-127+1023)<<52;
   return dnum.num;
}
float __aeabi_d2f(double x)
{
    __double_number dnum;
    __single_pnum num;
    num.num=0;
    dnum.num=x;
    num.reg |= ((uint32_t)(dnum.reg>>63)<<31);
    num.reg |= (uint32_t)(((((dnum.reg & ~(((uint64_t)1<<63)))>>52)-1023)+127)<<23);
    num.reg |= ((uint32_t)(((dnum.reg &~((uint64_t)0xFFF<<52))<<12)>>(63-22)));
    return num.num;
}
int32_t __aeabi_d2iz(double x)
{
    __double_number dnum;
    dnum.num=x;
    int32_t num;
    uint32_t mantissa;
    uint16_t exponent = (uint16_t)(((dnum.reg & ~ ((uint64_t)0x1<<63))>>52)-1023); 
    int8_t sign = (dnum.reg>>63)? -1:1;
    mantissa = (uint32_t)(((dnum.reg & ~((uint64_t)0xFFF<<52)) )>>(52-exponent));
    num = mantissa * sign;
    return num;
}
int __aeabi_d2i(double x)
{
    __double_number dnum;
    dnum.num=x;
    int num;
    uint32_t mantissa;
    uint16_t exponent = (uint16_t)(((dnum.reg & ~ ((uint64_t)0x1<<63))>>52)-1023); 
    int8_t sign = (dnum.reg>>63)? -1:1;
    mantissa = (uint32_t)(((dnum.reg & ~((uint64_t)0xFFF<<52)))>>(52-exponent));
    num = mantissa * sign;
    return num;
}

uint32_t get_decimal_part(double x)
{
    __double_number dnum;
    dnum.num=x;
    int32_t num=0;
    uint64_t mantissa;
    uint16_t exponent = (uint16_t)(((dnum.reg & ~ ((uint64_t)0x1<<63))>>52)-1023); 
    mantissa = (uint64_t)((dnum.reg & ~((uint64_t)0xFFF<<52))) | (uint64_t)0b1<<52;
    mantissa = mantissa<<(12+exponent);
    for(uint32_t i=0;i<DOUBLE_FRAC_PRECISION;i++)
    {
        if(mantissa >> (63-i) & 0b1){
            num += (LARGEST_FRAC >> i);
        }
    } 
    return num;
}

double __aeabi_ui2d(uint32_t val)
{
	__double_number dnum;
    uint32_t value=val,no_digit=0;

    while(val>0)
    {
        value = (uint32_t)(value>>2);
        no_digit++; 
    }
    uint16_t exponent = (uint16_t)(no_digit+1023) ;
    uint64_t mantissa = 0;
    for(uint32_t i=DOUBLE_FRAC_PRECISION - 1;i>=0;i--)
    {
        if(val & ((uint32_t)(0b1<<i) != 1)) continue;
        mantissa |= (0b1<<(52-DOUBLE_FRAC_PRECISION+i)); 
    }
        
    dnum.reg = ((~(((uint64_t)0x1<<63)))|((uint64_t)(exponent & ~(0xF800))<<52)|(mantissa & ~(((uint64_t)0xFFF<<52)))); 
    return dnum.num;
}

// Be Careful For the following functions we may have issues in implementation

uint32_t __aeabi_d2uiz(double d)
{
  __double_number dnum;
  uint32_t num=0;
  dnum.num = d; //Signed number problem -- We will solve it later
  uint16_t exponent = (uint16_t)(((dnum.reg & ~ ((uint64_t)0x1<<63))>>52)-1023); 
  num= (uint32_t)(((uint64_t)((dnum.reg & ~((uint64_t)0xFFF<<52))))>>(52-exponent));  
  //num = ((mantissa & ~(((uint64_t)0xFFF<<52)))>>(52-exponent));
  return num;
}
double __aeabi_dsub(double d1,double d2)
{
    //Sign number issues

    __double_number d1num,d2num,dnum3;
    d1num.num = d1;
    d2num.num = d2; 
    dnum3.reg=0UL;
    uint64_t mantissa1, mantissa2,mantissa3;
    uint16_t ediff;
    uint16_t exp1= (uint16_t)(((d1num.reg & ~ ((uint64_t)0x1<<63))>>52)-1023);  
    uint16_t exp2= (uint16_t)(((d2num.reg & ~ ((uint64_t)0x1<<63))>>52)-1023); 
    uint16_t exp;
    if(exp1 < exp2)
    {
        ediff = exp2-exp1;
        mantissa1 = (uint64_t)((((d1num.reg & ~((uint64_t)0xFFF<<52))) | (uint64_t)0b1<<52)>>ediff); 
        mantissa2 = (uint64_t)(((d2num.reg & ~((uint64_t)0xFFF<<52)))| (uint64_t)0b1<<52); 
        exp=exp2;
        
    }else if (exp1 > exp2)
    {
        ediff = exp1-exp2;
        mantissa1 = (uint64_t)(((d1num.reg & ~((uint64_t)0xFFF<<52))) | (uint64_t)0b1<<52); 
        mantissa2 = (uint64_t)((((d2num.reg & ~((uint64_t)0xFFF<<52)))| (uint64_t)0b1<<52)>>ediff); 
        exp=exp1;
    } else 
    {
        exp=exp1;
        mantissa1 = (uint64_t)(((d1num.reg & ~((uint64_t)0xFFF<<52))) | (uint64_t)0b1<<52); 
        mantissa2 = (uint64_t)(((d2num.reg & ~((uint64_t)0xFFF<<52)))| (uint64_t)0b1<<52); 
        
    }
    mantissa3 = (mantissa1-mantissa2);
    while ((mantissa3 & (uint64_t)0xFFE<<52)>0)
    {
        mantissa3=(uint64_t)(mantissa3 >> 1UL);
        exp=(uint16_t)(exp+1);
    }
    dnum3.reg = ((d1num.reg & (uint64_t)0x1UL<<63) ^ ((d2num.reg & (uint64_t)0x1UL<<63))) | (uint64_t)((exp & ~((uint64_t)0xF100))<< 52) | (mantissa3);   
    return dnum3.num;
}
double __aeabi_dmul(double d1,double d2)
{
        
    __double_number x,y,z;
    x.num = d1;
    y.num = d2;
    uint16_t xexp =  (uint16_t)(((x.reg & ~ ((uint64_t)0x1<<63))>>52)-1023);  
    uint16_t yexp =  (uint16_t)(((x.reg & ~ ((uint64_t)0x1<<63))>>52)-1023);  
    uint16_t zexp = xexp+yexp+1023;
    uint64_t xman = (uint64_t)(x.reg & ~((uint64_t)0xFFF<<52)) | ((uint64_t)0b1<<52); 
    uint64_t yman = (uint64_t)(y.reg & ~((uint64_t)0xFFF<<52)) | ((uint64_t)0b1<<52);
    uint64_t zman = xman*yman;
    while((zman & (uint64_t)0xFFF<<52) > ((uint64_t)0b1<<52))
    {
        zman=zman >> 1;
        zexp++;
    }
    z.reg=((x.reg & (uint64_t)0b1<<63) ^ (y.reg & (uint64_t)0b1<<63)) | ((zexp & (uint64_t)0x07FF)<<52) | (zman & ~((uint64_t)0xFFF<<52));
    return z.num;
}

double __aeabi_dadd(double d1,double d2)
{
    __double_number dnum1,dnum2,dnum3;
    dnum1.num = d1;
    dnum2.num = d2;
    dnum3.reg=0UL;
    uint16_t exp1 = (uint16_t)(((dnum1.reg & ~ ((uint64_t)0x1<<63))>>52)+1023); 
    uint16_t exp2 = (uint16_t)(((dnum2.reg & ~ ((uint64_t)0x1<<63))>>52)+1023);
    uint16_t exp;
    uint16_t ediff=0;
    uint64_t mantissa1,mantissa2,mantissa3;
    if(exp1 < exp2)
    {
        ediff = exp2-exp1;
        mantissa1 = (uint64_t)((((dnum1.reg & ~((uint64_t)0xFFF<<52))) | (uint64_t)0b1<<52)>>ediff); 
        mantissa2 = (uint64_t)(((dnum2.reg & ~((uint64_t)0xFFF<<52)))| (uint64_t)0b1<<52); 
        exp=exp2;
        
    }else if (exp1 > exp2)
    {
        ediff = exp1-exp2;
        mantissa1 = (uint64_t)(((dnum1.reg & ~((uint64_t)0xFFF<<52))) | (uint64_t)0b1<<52); 
        mantissa2 = (uint64_t)((((dnum2.reg & ~((uint64_t)0xFFF<<52)))| (uint64_t)0b1<<52)>>ediff); 
        exp=exp1;
    } else 
    {
        exp=exp1;
        mantissa1 = (uint64_t)(((dnum1.reg & ~((uint64_t)0xFFF<<52))) | (uint64_t)0b1<<52); 
        mantissa2 = (uint64_t)(((dnum2.reg & ~((uint64_t)0xFFF<<52)))| (uint64_t)0b1<<52); 
        
    }
    mantissa3 = (mantissa1 + mantissa2);
    while ((mantissa3 & (uint64_t)0xFFE<<52)>0)
    {
        mantissa3=(uint64_t)(mantissa3 >> 1UL);
        exp=(uint16_t)(exp+1);
    }
    dnum3.reg |= ((dnum1.reg & (uint64_t)0x1UL<<63) ^ (dnum2.reg & (uint64_t)0x1UL<<63)) | (exp & ~((uint64_t)0xF100<< 52)) | (mantissa3);   
    return dnum3.num; 
}

uint64_t __aeabi_d2ulz(double d)
{
  __double_number dnum;
  uint64_t num;
  dnum.num = d;
  uint16_t exponent = (uint16_t)(((dnum.reg & ~ ((uint64_t)0x1<<63))>>52)+1023); 
  uint64_t mantissa = (uint64_t)((dnum.reg & ~((uint64_t)0xFFF<<52)));  
  num = (uint64_t)((mantissa)>>(52-exponent));
  return num;
}

double __aeabi_ddiv(double d1,double d2)
{
     __double_number x,y,z;
    x.num = d1;
    y.num = d2;
    if(d2==0) {z.reg = (__DBL_INFINITY| ((x.reg & (uint64_t)0x1UL<<63) ^ (y.reg & (uint64_t)0x1UL<<63))); return z.num;}
    if(d1==0) {z.reg = ((uint64_t)__DBL_MIN__ | ((x.reg & (uint64_t)0x1UL<<63) ^ (y.reg & (uint64_t)0x1UL<<63))); return z.num;}
    if(d1==0 && d2==0) {z.reg = __DBL_NAN; return z.num;}
    uint16_t xexp = (uint16_t)(((x.reg & ~ ((uint64_t)0x1<<63))>>52)-1023);  
    uint16_t yexp = (uint16_t)(((x.reg & ~ ((uint64_t)0x1<<63))>>52)-1023);  
    uint16_t zexp = xexp-yexp+1023;
    uint64_t xman = (x.reg & ~((uint64_t)0xFFF<<52)) | ((uint64_t)0b1<<52); 
    uint64_t yman = (y.reg & ~((uint64_t)0xFFF<<52)) | ((uint64_t)0b1<<52);
    uint64_t zman=0;
    uint64_t a=xman,s=0;
    if(xman < yman){zman=0;}
    else
    {
      while(a > yman)
      {
        a=a-yman;
        s++;
      }
      zman = s; //this wrong 
    }
    
    z.reg = ((x.reg & (uint64_t)0x1UL<<63) ^ (y.reg & (uint64_t)0x1UL<<63)) | ((zexp & ~((uint64_t)0xF100)) << 52) | (zman & ((uint64_t)0xFFF<<52));
    return z.num;
}

uint32_t __aeabi_dcmpeq(double x,double y)
{
    __double_number a,b;
    a.num=x;
    b.num=y;
    if(a.reg == b.reg) return 1;
    return 0;
}
