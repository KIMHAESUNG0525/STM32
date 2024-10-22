#include "stepmotor.h"
#include "button.h"

extern void delay_us(unsigned int us);
extern void up_arrow_display(void);
extern void down_arrow_display(void);
extern void dotmatrix_off();


//extern unsigned char display_data1[8];
//extern unsigned char display_data2[8];
//extern uint8_t up[];
//extern uint8_t down[];
//
//display_data1 = up


extern unsigned char display_data[8];
extern uint8_t up[];
extern uint8_t down[];

void set_rpm(int rpm);
void stepmotor_main(void);
void stepmotor_driver(int step);
void forward_step(void);
void backward_step(void);
void stop_motor(void);
void dot_dis_main(void);

void(*fp_step[])() =
{
	stop_motor,
	forward_step,
	backward_step
};

void(*fp_dot2[])() =
{
	dotmatrix_off,
	up_arrow_display,
	down_arrow_display
};

/*
 * RPM (Revolutions Per Minutes) : 분당 회전 수
 * 1분 = 60sec : 1,00,000us(1초) * 60 = 60,000,000us
 * 1초 : 1000msec --> 1ms(1000us) * 1000ms(1초) => 1,000,000us
 * 1바퀴 회전 4096스텝 필요
 *
 * 4096 / 8 --> 512sequence 필요 : 360도 회전
 * 1sequence(8step) : 0.70312도
 * 0.70312도 * 512sequence = 360도
 *
 * ------RPM 조절 함수 ------
 * 60,000,000us / 4096 / rpm(1~13)
 * 13회전: 60,000,000us / 4096 / 13 ==> step과 step간의 간격 time : 1126us
 * 1126 us * 4096(1회전 필요 스텝) = 4,615,384us = 4615ms = 4.6sec
 *
 * 60초 / 4.6(1회전 소요 시간) = 13회전
 */

void set_rpm(int rpm) //rpm : 1~13
{
	delay_us(60000000 / 4096 / rpm - 1000);
	//최대 speed 기준 : delay_us(1126); -> step과 step 간의 딜레이 -> 1126으로 하면 최대 스피드
}

int step = STOP;

void stepmotor_main(void)
{

	static int dot = DOT_OFF;
	if(get_button(GPIOC, GPIO_PIN_1, BUTTON1) == BUTTON_PRESS)
	{
		switch(step)
		{
			case STOP:
			step = FORWARD;
			break;

			case FORWARD:
			step = BACKWARD;
			break;

			case BACKWARD:
			step = FORWARD;
			break;
		}
		switch(dot)
		{
			case DOT_OFF:
			dot = UP_ARROW1;
			for (int i=0; i < 8; i++)
			{
				display_data[i] = up[i];
			}
			break;

			case UP_ARROW1:   // UP_ARROW
			dot = DOWN_ARROW1;   // DOWN_ARROW
			for (int i=0; i < 8; i++)
			{
				display_data[i] = down[i];
			}
			break;

			case DOWN_ARROW1:  // DOWN_ARROW
			dot = UP_ARROW1;  // DOWN_ARROW
			for (int i=0; i < 8; i++)
			{
				display_data[i] = up[i];
			}
			break;
		}



	}

	if(get_button(GPIOC, GPIO_PIN_0, BUTTON0) == BUTTON_PRESS)
	{
		switch(step)
		{
			case FORWARD:
			step = STOP;
			break;

			case BACKWARD:
			step = STOP;
			break;
		}
		switch(dot)
		{
			case UP_ARROW1:
			dot = DOT_OFF;
			break;

			case DOWN_ARROW1:
			dot = DOT_OFF;
			break;
		}
	}

	fp_step[step]();
	fp_dot2[dot]();
}



void stop_motor(void)
{
    HAL_GPIO_WritePin(GPIOC, IN1_Pin, 0);
    HAL_GPIO_WritePin(GPIOC, IN2_Pin, 0);
    HAL_GPIO_WritePin(GPIOC, IN3_Pin, 0);
    HAL_GPIO_WritePin(GPIOC, IN4_Pin, 0);
}

void forward_step(void)
{
	static uint8_t forward_step = 0;

	stepmotor_driver(forward_step);
	forward_step++;
	set_rpm(13); // 1126us 동안 wait

	forward_step = (forward_step + 1) % 8;
}

void backward_step(void)
{
	static int8_t backward_step = 7;
	static int back_complete = 1;

	if (back_complete)
	{
		backward_step = 7;
		back_complete = 0;
	}

	stepmotor_driver(backward_step);
	backward_step--;
	set_rpm(13); // 1126us 동안 wait

	if (backward_step > 0) {
		backward_step--;
	} else {
		back_complete = 1;
	}
}
#if 0
void stepmotor_main(void) //for demo
{
	for(int i=0; i < 512 ; i++) //시계방향으로 1회전
	{
		for(int j=0 ; j < 8; j++)
		{
			stepmotor_driver(j);
			set_rpm(13); // 1126us 동안 wait
		}
	}

	for(int i=0; i < 512 ; i++) //반시계방향으로 1회전
	{
		for(int j=7 ; j >= 0 ; j--)
		{
			stepmotor_driver(j);
			set_rpm(13); // 1126us 동안 wait
		}
	}
}
#endif

#if 1
void stepmotor_driver(int direction)
{

	switch(direction)
	{
		case 0:
			HAL_GPIO_WritePin(GPIOC, IN1_Pin, 1);
			HAL_GPIO_WritePin(GPIOC, IN2_Pin, 0);
			HAL_GPIO_WritePin(GPIOC, IN3_Pin, 0);
			HAL_GPIO_WritePin(GPIOC, IN4_Pin, 0);
			break;
		case 1:
			HAL_GPIO_WritePin(GPIOC, IN1_Pin, 1);
			HAL_GPIO_WritePin(GPIOC, IN2_Pin, 1);
			HAL_GPIO_WritePin(GPIOC, IN3_Pin, 0);
			HAL_GPIO_WritePin(GPIOC, IN4_Pin, 0);
			break;
		case 2:
			HAL_GPIO_WritePin(GPIOC, IN1_Pin, 0);
			HAL_GPIO_WritePin(GPIOC, IN2_Pin, 1);
			HAL_GPIO_WritePin(GPIOC, IN3_Pin, 0);
			HAL_GPIO_WritePin(GPIOC, IN4_Pin, 0);
			break;
		case 3:
			HAL_GPIO_WritePin(GPIOC, IN1_Pin, 0);
			HAL_GPIO_WritePin(GPIOC, IN2_Pin, 1);
			HAL_GPIO_WritePin(GPIOC, IN3_Pin, 1);
			HAL_GPIO_WritePin(GPIOC, IN4_Pin, 0);
			break;
		case 4:
			HAL_GPIO_WritePin(GPIOC, IN1_Pin, 0);
			HAL_GPIO_WritePin(GPIOC, IN2_Pin, 0);
			HAL_GPIO_WritePin(GPIOC, IN3_Pin, 1);
			HAL_GPIO_WritePin(GPIOC, IN4_Pin, 0);
			break;
		case 5:
			HAL_GPIO_WritePin(GPIOC, IN1_Pin, 0);
			HAL_GPIO_WritePin(GPIOC, IN2_Pin, 0);
			HAL_GPIO_WritePin(GPIOC, IN3_Pin, 1);
			HAL_GPIO_WritePin(GPIOC, IN4_Pin, 1);
			break;
		case 6:
			HAL_GPIO_WritePin(GPIOC, IN1_Pin, 0);
			HAL_GPIO_WritePin(GPIOC, IN2_Pin, 0);
			HAL_GPIO_WritePin(GPIOC, IN3_Pin, 0);
			HAL_GPIO_WritePin(GPIOC, IN4_Pin, 1);
			break;
		case 7:
			HAL_GPIO_WritePin(GPIOC, IN1_Pin, 1);
			HAL_GPIO_WritePin(GPIOC, IN2_Pin, 0);
			HAL_GPIO_WritePin(GPIOC, IN3_Pin, 0);
			HAL_GPIO_WritePin(GPIOC, IN4_Pin, 1);
			break;
		}


}
#else
void stepmotor_driver(int step)
{
	switch(step)
	{
	case 0:
		HAL_GPIO_WritePin(GPIOC, IN1_Pin, 1);
		HAL_GPIO_WritePin(GPIOC, IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOC, IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOC, IN4_Pin, 0);
		break;
	case 1:
		HAL_GPIO_WritePin(GPIOC, IN1_Pin, 1);
		HAL_GPIO_WritePin(GPIOC, IN2_Pin, 1);
		HAL_GPIO_WritePin(GPIOC, IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOC, IN4_Pin, 0);
		break;
	case 2:
		HAL_GPIO_WritePin(GPIOC, IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOC, IN2_Pin, 1);
		HAL_GPIO_WritePin(GPIOC, IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOC, IN4_Pin, 0);
		break;
	case 3:
		HAL_GPIO_WritePin(GPIOC, IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOC, IN2_Pin, 1);
		HAL_GPIO_WritePin(GPIOC, IN3_Pin, 1);
		HAL_GPIO_WritePin(GPIOC, IN4_Pin, 0);
		break;
	case 4:
		HAL_GPIO_WritePin(GPIOC, IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOC, IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOC, IN3_Pin, 1);
		HAL_GPIO_WritePin(GPIOC, IN4_Pin, 0);
		break;
	case 5:
		HAL_GPIO_WritePin(GPIOC, IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOC, IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOC, IN3_Pin, 1);
		HAL_GPIO_WritePin(GPIOC, IN4_Pin, 1);
		break;
	case 6:
		HAL_GPIO_WritePin(GPIOC, IN1_Pin, 0);
		HAL_GPIO_WritePin(GPIOC, IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOC, IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOC, IN4_Pin, 1);
		break;
	case 7:
		HAL_GPIO_WritePin(GPIOC, IN1_Pin, 1);
		HAL_GPIO_WritePin(GPIOC, IN2_Pin, 0);
		HAL_GPIO_WritePin(GPIOC, IN3_Pin, 0);
		HAL_GPIO_WritePin(GPIOC, IN4_Pin, 1);
		break;
	}

}
#endif
