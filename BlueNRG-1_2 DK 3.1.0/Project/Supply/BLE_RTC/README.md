The RTC Demo can  accurate  timing. 
              Features:
1.	It include RTC time and Unix timestamp(Number of seconds passed  since 1970-01-01 00:00:00 GMT).
a.	uint32_t get_unix_timestamp(void);
b.	sysTime_t get_date(void);
2.	It base on VTimer(internal time), so  sleep do not effect timing.
3.	It can sync system time by RTC time and by Unix timestamp.
a.	bool sync_time_by_sec(uint32_t timestamp);
b.	bool sync_time_by_rtc(uint16_t year,uint8_t month,uint8_t day,
                                                                                      uint8_t hour,uint8_t minute,uint8_t second);

Explain:
              internal time units is about 2.4114us (1000000/409600)
              409600 ticks is 1s(Relatively accurate)


Use caution:
              It must be called periodically in a period  of internal time tick (called period < internal time tick period [87 min])


Usage examples:
              while(1) {
    /* BlueNRG-1 stack tick */
    refresh_sys_time();// or get_unix_timestamp();
    BTLE_StackTick();
    BlueNRG_Sleep(SLEEPMODE_WAKETIMER, wakeup_source, wakeup_level);
}




HAL_VTimerGetCurrentTime_sysT32  获取的值是一个不断增加的数值(tick)，当增加到0XFFFFFFFF后又从0开始递增。
1个tick 大概是2.4414us.  409600个tick数是精确的1秒钟
