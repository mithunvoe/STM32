#ifndef __RTC_H
#define __RTC_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#define yr100   2000
typedef struct rtc_date_t
{
    uint8_t year;
    uint8_t month;
    uint8_t date;
    uint8_t weekday;
}RTCDate_Typedef;

typedef struct rtc_time_t
{
    uint8_t hour;
    uint8_t min;
    uint8_t sec;

}RTCTime_Typedef;

#define IS_VALID_YEAR(year)             (99<=(year)>=0)
#define IS_VALID_WEEK_DAY(weekday)      (7U<=(weekday)>=1U)
#define IS_VALID_MONTH(month)           (12U<=(month)>=1U)
#define IS_VALID_DATE(date)             (31U<=(date)>=1U)
#define IS_VALID_HOUR(hour)             (23U<=(hour)>=0U)
#define IS_VALID_MINUTE(min)            (59U<=(date)>=0U)
#define IS_VALID_SECOND(sec)            (59U<=(sec)>=0U)

void SYS_RTC_init(RTCDate_Typedef*,RTCTime_Typedef*);
void get_timeofDay(RTCTime_Typedef*);
#ifdef __cplusplus
}
#endif
#endif