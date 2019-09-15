/*****************************************************************************
File Name:    sys_time.c
Description:   system time manage
Note: The refresh_sys_time must be called periodically in a system time tick period
History:	
Date                Author                   Description
2017-03-13         Lucien                     Create
****************************************************************************/
#include "sys_time.h"
#include "bluenrg1_stack.h"

/* Each tick equals us */
#define SLICE_TIME    			(2.4414f) 
/* Two-level tick timing is used SECONDARY_TICKS_UNIT ticks is about 1S */ 
#define SECONDARY_TICKS_UNIT 	(409600)

/* get current system ticks */
#define GET_TICK	HAL_VTimerGetCurrentTime_sysT32




static TickType_t start_ticks;
static sys_tick_t sys_tick;
static const sysTime_t CONST_TIME = {1970,1,1,0,0,0};


static uint32_t abs_uint32_t(uint32_t start, uint32_t end)
{
	return abs(end-start);
}

/* refresh system time  form virtual time . The virtual timer can be work when sleep*/
/* This fuction must be called periodically in a system time tick period(called period < system time tick period [87 min])*/
void refresh_sys_time(void)
{
	uint32_t cur_ticks = GET_TICK();
	sys_tick.ticks += abs_uint32_t(cur_ticks, start_ticks);
	sys_tick.secondary_ticks += sys_tick.ticks / SECONDARY_TICKS_UNIT;
	sys_tick.ticks = sys_tick.ticks % SECONDARY_TICKS_UNIT;
	start_ticks = cur_ticks;
}

/* since 1970 01.01 00:00:00 */
uint32_t get_unix_timestamp(void)
{
	 uint32_t ret = 0;
	 refresh_sys_time();
	 ret = sys_tick.secondary_ticks;
	 return ret;
}

/* since 1970 01.01 00:00:00 */
sys_tick_t get_sys_timestamp_us(void)
{
	sys_tick_t ret;
	refresh_sys_time();
	ret = sys_tick;
	return ret;
}



static bool CheckIsRunYear(const uint16_t year)
{
		bool bRet = false;
		if( ((0 == year%4) && (year%100 != 0)) || (0 == year%400))
			bRet = true;

		return bRet;
}

/*  Convert unix time stamp to date */ 
sysTime_t sec_to_date(uint32_t sec)
{
    sysTime_t date = CONST_TIME;
	const uint32_t DaySum = 3600 * 24;

	uint32_t HaveDay = sec / DaySum;
	uint32_t leftSec = sec - HaveDay*DaySum;

	bool runFlag = CheckIsRunYear(date.year);

	while(HaveDay >= (365 + runFlag)){
		date.year += 1;
		HaveDay -= (365 + runFlag);
		runFlag = CheckIsRunYear(date.year);
	}

	// Processing days
	uint8_t monthMax[12]={31,28,31,30,31,30,31,31,30,31,30,31};

	// Determine if the last year is a leap years
	if( CheckIsRunYear(date.year) )
		monthMax[1]   = 29;

	uint8_t compDay = monthMax[0];
	uint8_t mon = CONST_TIME.month;
	
	while( (mon<=12) && (HaveDay>=compDay) )
	{
		HaveDay -= monthMax[mon-1];
		date.month = mon+1;
		mon++;
		if(mon>=12)
			break;
		else
			compDay = monthMax[mon-1];
	}

	date.day += HaveDay;

	date.hour 	= leftSec / 3600;
	date.min 	= (leftSec - date.hour*3600)/60;
	date.sec 	= leftSec - date.hour*3600 - date.min*60;
	return date;
}

sysTime_t get_date(void)
{
	return sec_to_date(get_unix_timestamp());
}

/*  Convert date to unix time stamp */ 
uint32_t date_to_sec(sysTime_t date)
{
	uint32_t retSec = 0;
	uint32_t RunYearNum = 0;
	if(date.year < CONST_TIME.year){
		return 0;
	}

	uint16_t year = date.year - CONST_TIME.year;
	// 
	for(uint32_t i=CONST_TIME.year;i<date.year;i++){
		if( CheckIsRunYear(i) ){
			RunYearNum++;
		}
	}
	// 
	uint8_t monthMax[12]={31,28,31,30,31,30,31,31,30,31,30,31};

	// if the date is run year
	if( CheckIsRunYear(date.year) )
		monthMax[1]   = 29;
	uint32_t mon = date.month - CONST_TIME.month;
	uint16_t sunMontDay = 0;
	for(int i=0;i<mon;i++){
		sunMontDay += monthMax[i];
	}
	
	uint32_t day    = date.day - CONST_TIME.day;
	uint32_t SumDay = year*365 + RunYearNum + sunMontDay + day;
	uint16_t hour   = date.hour - CONST_TIME.hour;
	uint16_t min    = date.min - CONST_TIME.min;
	uint16_t sec    = date.sec - CONST_TIME.sec;

	retSec = (SumDay*3600*24 + hour*3600 + min*60 + sec);
	return retSec;
}

/* validity time */
static bool check_time_legal(uint16_t year,uint8_t month,uint8_t day,uint8_t hour,uint8_t minute,uint8_t second)
{
		bool ret = true;
		uint8_t monthMax[12]={31,28,31,30,31,30,31,31,30,31,30,31};
		
		ret &= (year>=CONST_TIME.year)&&(month>0&&month<=12)&&(minute<60)&&(second<60);
		if(ret){
			// if the year is run year
			if( CheckIsRunYear(year) ){
				monthMax[1]   = 29;
				ret &= (day>0 && day<=monthMax[day-1]);
			}
		}
		return ret;
}

bool sync_time_by_sec(uint32_t timestamp)
{
	refresh_sys_time();
	sys_tick.secondary_ticks = timestamp;
	return true;
}

bool sync_time_by_rtc(uint16_t year,uint8_t month,uint8_t day,
						uint8_t hour,uint8_t minute,uint8_t second)
{
	bool bRet = true;

	if(check_time_legal(year,month,day,hour,minute,second)){
		static sysTime_t current;
		current.year 	= year;
		current.month	= month;
		current.day     = day;
		current.hour    = hour;
		current.min     = minute;
		current.sec     = second;

		uint32_t sum_sec = date_to_sec(current);
		sync_time_by_sec(sum_sec);
	}
	else
			bRet = false;
	return bRet;
}









