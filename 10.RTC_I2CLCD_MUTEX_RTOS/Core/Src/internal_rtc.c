#include "main.h" //HAL GPIO INCLUDE
#include <string.h> //표준 라이브러리, strcmp, strcpy, strlen ...
#include <stdlib.h> // atoi(ascii -> int), itoa function include

#include "extern.h"

//선언부
void get_rtc_date_time(void);
void set_rtc(char *date_time);
unsigned char bcd2dec(unsigned char byte);
unsigned char dec2bcd(unsigned char byte);

void get_rtc_date(void); //10.18
void get_rtc_time(void); //10.18

RTC_TimeTypeDef sTime = {0}; // 시각 정보
RTC_DateTypeDef sDate = {0}; // 날짜 정보



//uint8_t Year
//ex) 24년의 Year에 저장된 data format
// 7654 3210
// 0010 0100
//   2   4
// ==> 24 return
unsigned char bcd2dec(unsigned char byte)
{
	unsigned char high, low; //high nibble과 low nibbleㅇ을 담는 변수

	low = byte & 0x0f;
	high = (byte >> 4) * 10; //4bit shift 이후 * 10을 하면 0000 0010 * 10

	return (high + low); //unsigned char
}

//10진수를 BCD로 변환
//ex) 24(0001 1000) --> 0010 0100

unsigned char dec2bcd(unsigned char byte)
{
	unsigned char high, low; //high nibble과 low nibbleㅇ을 담는 변수

	high = (byte / 10) << 4; //4bit shift 이후 * 10을 하면 0000 0010 * 10
	low = byte % 10;

	return (high + low); //unsigned char
}

//STM32의 RTC에서 날짜와 시각 정보를 읽어오는 함수를 작성.
void get_rtc_date_time(void)
{
	static RTC_TimeTypeDef oldTime = {0}; // 시각 정보 //쓰레기값 방지를 위해 0으로 초기화 , 지역변수지만 전역변수처럼 사용

	char lcd_buff[40]; //10.10

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);

	if(oldTime.Seconds != sTime.Seconds)
	{
		//update 된 시각 정보를 출력(1초에 1번씩 출력)
		//YYYY-MM-DD HH:mm:ss
		//if(pdatetime) // if(pdatetime>=1) 조건과 동일
		//{
//			printf("%4d-%2d-%2d %2d:%2d:%2d\n",
//				bcd2dec(sDate.Year)+2000, bcd2dec(sDate.Month), bcd2dec(sDate.Date),
//				bcd2dec(sTime.Hours), bcd2dec(sTime.Minutes), bcd2dec(sTime.Seconds));
#if 1 // 1번째 라인 : 날짜, 2번째 라인 : 시각
			sprintf(lcd_buff,"%02d-%02d-%02d",bcd2dec(sDate.Year)+2000, bcd2dec(sDate.Month), bcd2dec(sDate.Date)); //10.10
				move_cursor(0,0);
				lcd_string(lcd_buff);
			sprintf(lcd_buff,"%02d:%02d:%02d",bcd2dec(sTime.Hours), bcd2dec(sTime.Minutes), bcd2dec(sTime.Seconds)); //10.10
				move_cursor(1,0);
				lcd_string(lcd_buff);
		//}
#else //1라인에 날짜와 시각을 둘 다 출력
			sprintf(lcd_buff,"%02d-%02d-%02d%02d:%02d:%02d",
					bcd2dec(sDate.Year), bcd2dec(sDate.Month), bcd2dec(sDate.Date),
					bcd2dec(sTime.Hours), bcd2dec(sTime.Minutes), bcd2dec(sTime.Seconds)); //10.10
			move_cursor(0,0);
			lcd_string(lcd_buff);
		}
#endif
		oldTime.Seconds = sTime.Seconds;
	}
}


//setrtc241008154500
//      YYMMDDHHmmss
// date_time에는 241008154500의 주소
void set_rtc(char *date_time)
{
	char yy[4], mm[4], dd[4]; //date 정보
	char hh[4], min[4], ss[4]; //time 정보 //ascii를 int로 변환하기 위해 작성 atoi를 활용하기 위함

	strncpy(yy, date_time, 2);
	strncpy(mm, date_time+2, 2);
	strncpy(dd, date_time+4, 2);

	strncpy(hh, date_time+6, 2);
	strncpy(min, date_time+8, 2);
	strncpy(ss, date_time+10, 2);

	//1. ascii --> int --> bcd --> RTC에 적용
	//        atoi(stdlib)
	sDate.Year = dec2bcd(atoi(yy));
	sDate.Month = dec2bcd(atoi(mm));
	sDate.Date = dec2bcd(atoi(dd));

	sTime.Hours = dec2bcd(atoi(hh));
	sTime.Minutes = dec2bcd(atoi(min));
	sTime.Seconds = dec2bcd(atoi(ss));

	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
	HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
}



//10.18 mutex ---------------------------------------
void get_rtc_date(void) //date function
{
	static RTC_TimeTypeDef oldTime = {0}; // 시각 정보 //쓰레기값 방지를 위해 0으로 초기화 , 지역변수지만 전역변수처럼 사용

	char lcd_buff[40]; //10.10

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);

	if(oldTime.Seconds != sTime.Seconds)
	{
		//update 된 시각 정보를 출력(1초에 1번씩 출력)
		//YYYY-MM-DD HH:mm:ss

//		printf("%4d-%2d-%2d %2d:%2d:%2d\n",
//		bcd2dec(sDate.Year)+2000, bcd2dec(sDate.Month), bcd2dec(sDate.Date),
//		bcd2dec(sTime.Hours), bcd2dec(sTime.Minutes), bcd2dec(sTime.Seconds));
    // 1번째 라인 : 날짜
		sprintf(lcd_buff,"%02d-%02d-%02d",bcd2dec(sDate.Year)+2000, bcd2dec(sDate.Month), bcd2dec(sDate.Date)); //10.10
		move_cursor(0,0);
		lcd_string(lcd_buff);

		oldTime.Seconds = sTime.Seconds;
	}
}
//10.18 mutex ---------------------------------------
void get_rtc_time(void) //time function
{
	static RTC_TimeTypeDef oldTime = {0}; // 시각 정보 //쓰레기값 방지를 위해 0으로 초기화 , 지역변수지만 전역변수처럼 사용

	char lcd_buff[40]; //10.10

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);

	if(oldTime.Seconds != sTime.Seconds)
	{
		//update 된 시각 정보를 출력(1초에 1번씩 출력)
		//YYYY-MM-DD HH:mm:ss

//		printf("%4d-%2d-%2d %2d:%2d:%2d\n",
//		bcd2dec(sDate.Year)+2000, bcd2dec(sDate.Month), bcd2dec(sDate.Date),
//		bcd2dec(sTime.Hours), bcd2dec(sTime.Minutes), bcd2dec(sTime.Seconds));
 //  2번째 라인 : 시각

		sprintf(lcd_buff,"%02d:%02d:%02d",bcd2dec(sTime.Hours), bcd2dec(sTime.Minutes), bcd2dec(sTime.Seconds)); //10.10
		move_cursor(1,0);
		lcd_string(lcd_buff);


		oldTime.Seconds = sTime.Seconds;
	}
}
