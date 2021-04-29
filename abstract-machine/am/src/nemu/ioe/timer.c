#include <am.h>
#include <nemu.h>

AM_TIMER_UPTIME_T basetime;

void __am_timer_init() {
    basetime.us=inl(RTC_ADDR) + inl(RTC_ADDR + 4) * 1000000;
//    outl(RTC_ADDR, 0);
//    outl(RTC_ADDR + 4, 0);
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
    uptime->us = inl(RTC_ADDR) + inl(RTC_ADDR + 4) * 1000000-basetime.us;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
    rtc->second = 0;
    rtc->minute = 0;
    rtc->hour = 0;
    rtc->day = 0;
    rtc->month = 0;
    rtc->year = 1900;
}
