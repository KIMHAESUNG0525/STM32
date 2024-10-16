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

// 7 <-6 <-5 <-4 <-3 <-2 <-1 <-0
// 200ms period 동작(기존에 on 된 led는 off 처리)
void led_up_on(void)
{
	for(int i = 0 ; i < 8 ; i++)
	{
		led_all_off();
		HAL_GPIO_WritePin(GPIOB, 0x01 << i,1); // << = left shift
		HAL_Delay(200);
	}
	led_all_off();
	HAL_Delay(200);
}

// 7 ->6 ->5 ->4 ->3 ->2 ->1 ->0
void led_down_on(void)
{
	for(int i = 0 ; i < 8 ; i++)
		{
			led_all_off();
			HAL_GPIO_WritePin(GPIOB, 0x80 >> i,1); // >> = right shift
			HAL_Delay(200);
		}
		led_all_off();
		HAL_Delay(200);
}

void led_flower_on(void)
{
//	for(int i = 0 ; i < 8 ; i++)
//		{
//			HAL_GPIO_WritePin(GPIOB, 0x10 << i ,1); // << = left shift
//			HAL_GPIO_WritePin(GPIOB, 0x08 >> i ,1);
//			HAL_Delay(200);
//		}
	for(int i = 0 ; i < 4 ; i++)
		{
			HAL_GPIO_WritePin(GPIOB, 0x10 << i |  0x08 >> i  ,1); // << = left shift
			HAL_Delay(200);
		}
		//HAL_Delay(200);
}

void led_flower_off(void)
{
//	for(int i = 0 ; i < 8 ; i++)
//		{
//			HAL_GPIO_WritePin(GPIOB, 0x80 >> i ,0); // << = left shift
//			HAL_GPIO_WritePin(GPIOB, 0x01 << i ,0);
//			HAL_Delay(200);
//		}
	for(int i = 0 ; i < 4 ; i++)
		{
			HAL_GPIO_WritePin(GPIOB, 0x80 >> i | 0x01 << i ,0); // << = left shift
			HAL_Delay(200);
		}
		//HAL_Delay(200);
}

void led_keepon_up(void)
{
	for(int i = 0 ; i < 8 ; i++)
	{
		//led_all_off();
		HAL_GPIO_WritePin(GPIOB, 0x01 << i,1); // << = left shift
		HAL_Delay(200);
	}
	led_all_off();
	HAL_Delay(200);

}

void led_keepon_down(void)
{
	for(int i = 0 ; i < 8 ; i++)
	{
		//led_all_off();
		HAL_GPIO_WritePin(GPIOB, 0x80 >> i,1); // >> = right shift
		HAL_Delay(200);
	}
	led_all_off();
	HAL_Delay(200);
}

void led_main(void)
{
	while(1)
	{
		led_all_on();
		HAL_Delay(500);
		led_all_off();
		HAL_Delay(500);
		led_up_on();
		led_down_on();
		led_flower_on();
		led_flower_off();
		led_keepon_up();
		led_keepon_down();
	}
}
