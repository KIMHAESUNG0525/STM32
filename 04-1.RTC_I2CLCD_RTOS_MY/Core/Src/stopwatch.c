#include "stopwatch.h"
#include "extern.h"

void button_check2(void);
void get_rtc_date_time2(void);
void stopwatch(void);
void led_on(void);

int seconds = 0;
int minutes = 0;
int msec = 0;

int reset_seconds = 0;
int pause_seconds = 0;
int pause_minutes = 0;
int mode = 1;
int stopwatch_flag = 0;


void(*fp4[])() =
{
	get_rtc_date_time2,
	stopwatch
};

void button_check2(void)
{

	get_rtc_date_time2();
    static int state = 0;



    if (state == 0 && get_button(GPIOC, GPIO_PIN_1, BUTTON1) == BUTTON_PRESS)
    {
        reset_seconds = 1;

        pause_seconds = bcd2dec(sTime.Seconds);  // 현재 초를 저장
        pause_minutes = bcd2dec(sTime.Minutes);  // 현재 분을 저장

        mode = !mode;
        led_on();


    }
    if(state == 1 && get_button(GPIOC, GPIO_PIN_1, BUTTON1) == BUTTON_PRESS) //stop watch 조건
    {
    	stopwatch_flag = !stopwatch_flag;
    	move_cursor(1,8);
    	lcd_string("SW STO");
    }

    if (state == 0 && reset_seconds == 1 && get_button(GPIOC, GPIO_PIN_2, BUTTON2) == BUTTON_PRESS)
    {


        if (mode == 0)  // 초 변경 모드
        {
            pause_seconds++;
            if (pause_seconds >= 60)
                pause_seconds = 0;
            sTime.Seconds = dec2bcd(pause_seconds);
            move_cursor(1,8);
            lcd_string("CL SEC");
        }
        else if (mode == 1)  // 분 변경 모드
        {
            pause_minutes++;
            if (pause_minutes >= 60)
                pause_minutes = 0;
            sTime.Minutes = dec2bcd(pause_minutes);
            move_cursor(1,8);
            lcd_string("CL MIN");
        }

        HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
        get_rtc_date_time2();



    }

    if (reset_seconds == 1 && get_button(GPIOC, GPIO_PIN_0, BUTTON0) == BUTTON_PRESS)
    {
        reset_seconds = 0;
        HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
        move_cursor(1,8);
        lcd_string("CL RUN");
    }

    // 버튼 0을 눌렀을 때 모드 변경
    if (reset_seconds == 0 && get_button(GPIOC, GPIO_PIN_0, BUTTON0) == BUTTON_PRESS)
    {
        state++;
        if (state > 1)
            state = 0;

    }


    // 초 또는 분 변경 모드가 아닐 때 RTC 동작
    if (reset_seconds == 0)
        fp4[state]();  // 상태에 따라 get_rtc_date_time2 또는 stopwatch 실행
}


void get_rtc_date_time2(void)
{
	//static RTC_TimeTypeDef oldTime = {0};  // 시각 정보 저장 변수 (쓰레기값 방지)
	unsigned char lcd_buff[40];            // LCD 출력 버퍼
	if(reset_seconds == 0)
	{
		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);

	}
	if (TIM10_1ms_counter1 >= 1000)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_2);
		TIM10_1ms_counter1 = 0;

	}
	// 시각 정보를 출력 (1초에 1번씩 업데이트)
	sprintf((char*)lcd_buff, "%02d-%02d %02d:%02d:%02d", bcd2dec(sDate.Month), bcd2dec(sDate.Date),
			bcd2dec(sTime.Hours), bcd2dec(sTime.Minutes), bcd2dec(sTime.Seconds));
	move_cursor(0, 0);
	lcd_string((unsigned char*)lcd_buff);

}

void stopwatch(void)
{
	unsigned char lcd_stop_buff[50];  // LCD 출력 버퍼
	if(stopwatch_flag ==0)
	{
		if (TIM10_1ms_counter1 >= 10)
		{
			msec++;
			TIM10_1ms_counter1 = 0;

		}
		if(msec >= 100)
		{
			seconds++;
			msec = 0;
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
		}

		if (seconds == 60)
		{
			seconds = 0;
			minutes++;
		}
		if (minutes == 10)
			minutes = 0;

		move_cursor(1,8);
		lcd_string("SW RUN");

	}
	else
	{
		if (get_button(GPIOC, GPIO_PIN_2, BUTTON2) == BUTTON_PRESS)
			{
				seconds = 0;
				minutes = 0;
				msec = 0;
				move_cursor(1,8);
				lcd_string("SW RST");

			}
	}


	sprintf(lcd_stop_buff, "%d:%02d:%2d", minutes, seconds, msec);
	move_cursor(1, 0);
	lcd_string(lcd_stop_buff);

}

void led_on(void)
{
	HAL_GPIO_WritePin(GPIOB, 0x01, 1);
}
