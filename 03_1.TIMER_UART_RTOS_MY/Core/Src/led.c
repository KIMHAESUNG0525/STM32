#include "led.h"

void led_all_on(void);
void led_all_off(void);
void led_main(void);
void led_up_on(void);
void led_down_on(void);
void led_flower_on(void);
void led_flower_off(void);
void led_keepon_up(void);
void led_keepon_down(void);
void ledbar0_toggle(void);
void led_on_down();

extern void delay_ms(unsigned int ms);

extern volatile int TIM10_1ms_counter;
extern volatile int TIM10_1ms_counter1;
extern volatile int TIM3_10ms_counter;

int func_index=0;
int led_bar_count = 0;


void(*fp1[])() =
{
	led_up_on,
	led_down_on,
	led_flower_on,
	led_flower_off,
	led_keepon_up
};

void ledbar0_toggle(void)
{
	HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
}

void led_all_on(void)
{
	HAL_GPIO_WritePin(GPIOB, 0xff, 1); // ON = 1 , GPIO_SET
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 |GPIO_PIN_3 | GPIO_PIN_4 |
//			GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, 1); // ON = 1 , GPIO_SET
}

void led_all_off(void)
{
	HAL_GPIO_WritePin(GPIOB, 0xff, 0);
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 |GPIO_PIN_3 | GPIO_PIN_4 |
//			GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, 0); // ON = 0 , GPIO_RESET
}

void led_on_down()
{
#if 1
	for (int i=0; i < 8; i++)
	{
		led_all_off();
		HAL_GPIO_WritePin(GPIOB, 0x80 >> i, GPIO_PIN_SET);
		delay_ms(200);
	}
#else
	static int i = 0;
	if(TIM3_10ms_counter >= 20)
	{
		led_all_off();
		TIM3_10ms_counter = 0;
		HAL_GPIO_WritePin(GPIOB, 0x80 >> i, GPIO_PIN_SET);
		i++;
		i %= 8;

	}
#endif
}

// 7 <-6 <-5 <-4 <-3 <-2 <-1 <-0
// 200ms period 동작(기존에 on 된 led는 off 처리)
void led_up_on(void)
{
#if 1
	static int i = 0;
	if(TIM10_1ms_counter1 >= 200)
	{
		TIM10_1ms_counter1 = 0;
		led_all_off();
		HAL_GPIO_WritePin(GPIOB, 0x01 << i,1); // << = left shift
		i++;
		if (i >= 8)
		{
			i=0;
			func_index=1; //상태 천이
		}
	}
#else //original code
	for(int i = 0 ; i < 8 ; i++)
	{
		led_all_off();
		HAL_GPIO_WritePin(GPIOB, 0x01 << i,1); // << = left shift
		HAL_Delay(50);
	}
	led_all_off();
	HAL_Delay(50);
#endif
}

// 7 ->6 ->5 ->4 ->3 ->2 ->1 ->0
void led_down_on(void)
{
#if 1
	static int i = 0;
	if(TIM10_1ms_counter1 >= 200)
	{
		TIM10_1ms_counter1 = 0;
		led_all_off();
		HAL_GPIO_WritePin(GPIOB, 0x80 >> i,1); // << = left shift
		i++;

		if (i > 8)
		{
			i=0;
			func_index=2; //상태천이
		}
	}
#else
	for(int i = 0 ; i < 8 ; i++)
		{
			led_all_off();
			HAL_GPIO_WritePin(GPIOB, 0x80 >> i,1); // >> = right shift
			HAL_Delay(50);
		}
		led_all_off();
		HAL_Delay(50);
#endif
}

void led_flower_on(void)
{

//	for(int i = 0 ; i < 8 ; i++)
//		{
//			HAL_GPIO_WritePin(GPIOB, 0x10 << i ,1); // << = left shift
//			HAL_GPIO_WritePin(GPIOB, 0x08 >> i ,1);
//			HAL_Delay(200);
//		}
#if 1
	static int i = 6;

	if (i >= 5)
	{
		led_all_off();
		i=0;

		//func_index=3; //상태천이

	}

	if(TIM10_1ms_counter1 >= 200)
	{

		TIM10_1ms_counter1 = 0;

		HAL_GPIO_WritePin(GPIOB, 0x10 << i |  0x08 >> i  ,1); // << = left shift
		i++;
	}
}
#else
	for(int i = 0 ; i < 4 ; i++)
		{
			HAL_GPIO_WritePin(GPIOB, 0x10 << i |  0x08 >> i  ,1); // << = left shift
			HAL_Delay(50);
		}
	//led_all_off();
	if (i >= 3)
	{
		i=0;
		func_index=3; //상태 천이
	}
		//HAL_Delay(200);
}
#endif

void led_flower_off(void)
{
#if 1

	static int i = 6;

	if (i >= 5)
	{
		led_all_on();
		i=0;

		//func_index=0; //상태천이

	}
	if(TIM10_1ms_counter1 >= 200)
	{
		TIM10_1ms_counter1 = 0;
		//led_all_on();
		HAL_GPIO_WritePin(GPIOB, 0x80 >> i | 0x01 << i ,0); // << = left shift
		i++;

	}
}
//	for(int i = 0 ; i < 8 ; i++)
//		{
//			HAL_GPIO_WritePin(GPIOB, 0x80 >> i ,0); // << = left shift
//			HAL_GPIO_WritePin(GPIOB, 0x01 << i ,0);
//			HAL_Delay(200);
//		}
#else
	for(int i = 0 ; i < 4 ; i++)
		{
			HAL_GPIO_WritePin(GPIOB, 0x80 >> i | 0x01 << i ,0); // << = left shift
			HAL_Delay(50);
		}
	//led_all_on();
	if (i >= 3)
	{
		i=0;
		func_index=0; //상태 천이
	}
		//HAL_Delay(200);
}
#endif
void led_keepon_up(void)
{
#if 1
	static int i = 10;
	if(i>=9)
		{
			led_all_off();
			i = 0;


		}
		if(TIM10_1ms_counter1 >= 200)
		{
			TIM10_1ms_counter1 = 0;
			HAL_GPIO_WritePin(GPIOB, 0x01 << i,1); // << = left shift
			i++;
		}


}
#else
	for(int i = 0 ; i < 8 ; i++)
	{
		//led_all_off();
		HAL_GPIO_WritePin(GPIOB, 0x01 << i,1); // << = left shift
		HAL_Delay(50);
	}
	led_all_off();
	HAL_Delay(50);

}
#endif
void led_keepon_down(void)
{
#if 1
	static int i = 10;
	if(i>=9)
	{
		led_all_off();
		i = 0;
	}
	if(TIM10_1ms_counter1 >= 200)
	{
		TIM10_1ms_counter1 = 0;
		HAL_GPIO_WritePin(GPIOB, 0x80 >> i,1); // << = left shift
		i++;
	}

}
#else
	for(int i = 0 ; i < 8 ; i++)
	{
		//led_all_off();
		HAL_GPIO_WritePin(GPIOB, 0x80 >> i,1); // >> = right shift
		HAL_Delay(50);
	}
	led_all_off();
	HAL_Delay(50);
}
#endif
void led_main(void)
{
	//led_up_on을 다 실행하고 led_down_on을 실행
	//함수 포인터 배열을 이용하여 동작 되도록 구현 하시오
	fp1[func_index]();

//	//static int i = 0;
//	if(TIM10_1ms_counter >= 50)
//	{
//	//	i++;
//		TIM10_1ms_counter = 0;
//		switch(led_bar_count)
//		{
//		case 0:
//			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
//			break;
//		case 1:
//			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
//			break;
//		case 2:
//			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_2);
//			break;
//		case 3:
//			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);
//			break;
//		case 4:
//			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_4);
//			break;
//		case 5:
//			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_5);
//			break;
//		case 6:
//			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_6);
//			break;
//		case 7:
//			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
//			break;
//		}
//		led_bar_count++;
//		led_bar_count %= 8;
//	//	led_all_off();
//
//	}
	//i%=8;
	//HAL_GPIO_WritePin(GPIOB, 0x01 << i,1); // << = left shift
}
