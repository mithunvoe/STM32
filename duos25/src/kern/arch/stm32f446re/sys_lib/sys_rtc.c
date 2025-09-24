#include<sys_rtc.h>
#include <sys_bus_matrix.h>
#include <stm32_assert.h>


void SYS_RTC_init(RTCDate_Typedef *dt,RTCTime_Typedef *tm)
{
    dt->year=dt->year-yr100;
    assert_param(IS_VALID_DATE(dt->date));
    assert_param(IS_VALID_YEAR(dt->year));
    assert_param(IS_VALID_MONTH(dt->month));
    assert_param(IS_VALID_WEEK_DAY(dt->weekday));
    assert_param(IS_VALID_HOUR(tm->hour));
    assert_param(IS_VALID_MINUTE(tm->min));
    assert_param(IS_VALID_SECOND(tm->sec));

    PWR->CR |= PWR_CR_DBP;
    RCC->CFGR |= RCC_CFGR_RTCPRE_3;
    RCC->BDCR |= RCC_BDCR_RTCEN|RCC_BDCR_RTCSEL;
    RTC->WPR |= 0xCAU<<RTC_WPR_KEY_Pos; 
    RTC->WPR |= 0x53U<<RTC_WPR_KEY_Pos;
    RTC->ISR |= RTC_ISR_INIT;
    while(!(RTC->ISR & RTC_ISR_INITF));
    RTC->PRER |= (0x7C<<RTC_PRER_PREDIV_A_Pos);
    RTC->PRER |= (0x1F3F<<RTC_PRER_PREDIV_S_Pos);
//Update Date  registers
    RTC->DR |= ((dt->year%10)<<RTC_DR_YU_Pos)|(((dt->year/10)%10)<<RTC_DR_YT_Pos); //year unit 
    RTC->DR |= ((dt->weekday%10)<<RTC_DR_WDU_Pos);
    RTC->DR |= ((dt->month%10)<<RTC_DR_MU_Pos) | (((dt->month/10)%10)<<RTC_DR_MT_Pos);
    RTC->DR |= ((dt->date%10)<<RTC_DR_DU_Pos) | (((dt->date/10)%10)<<RTC_DR_DT_Pos);
//Update Time registers
    RTC->TR |= ((tm->hour%10)<<RTC_TR_HU_Pos) | (((tm->hour/10)%10)<<RTC_TR_HT_Pos);
    RTC->TR |= ((tm->min%10)<<RTC_TR_MNU_Pos) | (((tm->min/10)%10)<<RTC_TR_MNT_Pos);
    RTC->TR |= ((tm->sec%10)<<RTC_TR_SU_Pos) | (((tm->sec/10)%10)<<RTC_TR_ST_Pos);
    RTC->CR |= RTC_CR_BYPSHAD;
    RTC->ISR &= ~RTC_ISR_INIT;
    PWR->CR &= ~PWR_CR_DBP;
}

void get_timeofDay(RTCTime_Typedef *sTime)
{
    sTime->sec = (((RTC->TR & 0x7f) >> 4)*10)+(RTC->TR & 0xf);
    sTime->min = ((RTC->TR & 0x7f00) >> 8);
    sTime->min = (((sTime->min & 0x7f)>>4)*10)+(sTime->min & 0xf);
    sTime->hour = ((RTC->TR & 0x7f0000) >> 16);
    sTime->hour = (((sTime->hour & 0x7f)>>4)*10)+(sTime->hour & 0xf);
}