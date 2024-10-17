#include "servo_motor.h"

extern volatile int TIM10_servo_motor_counter;
extern TIM_HandleTypeDef htim3;

extern int step;

void servo_motor_main(void);
void servo_motor_control(void);


/*
 * System clock = 84MHz(84,000,000)
 * TIM3 50KHz 입력 : 84000000 / 1680 ==> 50000Hz = 50 KHz
 * T = 1/f = 1/50000 = 0.00002 sec = 20usec
 * 20ms : 20us * 1000개
 * ---- pulse width
 * 2ms(180도) : 0.00002 * 100개
 * 1.5ms(90도) : 0.00002 * 75개
 * 1ms(0도) : 0.00002 * 50개
 */
void servo_motor_main(void)
{
	static uint8_t servo_state = 0;

	if(TIM10_servo_motor_counter >= 1000) // 1sec
	{
		TIM10_servo_motor_counter = 0;
		// 180도 -> 90도 -> 0도

		switch(servo_state)
		{
		case 0: // 180도
			//__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 190);
			TIM3->CCR2 = 190; //이것도 위와 같은 의미미며 CCR2는 채널 2를 의미
			break;
		case 1: // 90도
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 80);
			break;
		case 2: // 0도
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 40);
			break;
		}
		servo_state = (servo_state + 1) % 3;
		// servo_state++;
		// servo_state %= 3;
	}
}


//servo_motor_control은
//GPIO_PIN_1 : 발생 시 0 ~ 180도 (3초 유지) --> 다시 0도 복귀
void servo_motor_control(void)
{
    switch(step)
    {
    	TIM10_servo_motor_counter = 0;
    case 0:
    	if(TIM10_servo_motor_counter >= 3000) // 3sec
		{
			TIM3->CCR2 = 40;
			TIM10_servo_motor_counter = 0;
		}
    break;

    case 1:
    	TIM3->CCR2 = 190;
    	step = 0;
    break;
    }
}
