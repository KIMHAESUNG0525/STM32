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
	*(unsigned int *) 0x40020414 |= 0xffff;

	//HAL_GPIO_WritePin(GPIOB, 0xff, 1); // ON = 1 , GPIO_SET
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 |GPIO_PIN_3 | GPIO_PIN_4 |
//			GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, 1); // ON = 1 , GPIO_SET
}

void led_all_off(void)
{
	*(unsigned int *) 0x40020414 &= 0xff00;
	//HAL_GPIO_WritePin(GPIOB, 0xff, 0);
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
		*(unsigned int *) 0x40020414 |= 0x01 << i;
		//HAL_GPIO_WritePin(GPIOB, 0x01 << i,1); // << = left shift
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
			*(unsigned int *) 0x40020414 |= 0x80 >> i;
			//HAL_GPIO_WritePin(GPIOB, 0x80 >> i,1); // >> = right shift
			HAL_Delay(200);
		}
		led_all_off();
		HAL_Delay(200);
}

void led_flower_on(void)
{
#if 1
	for(int i = 0 ; i < 4 ; i++)
	{
		//*(unsigned char *) 0x40020417 = 0x10 << i | 0x08 >> i;
		*(unsigned int *) 0x40020414 |= 0x10 << i | 0x08 >> i;
		HAL_Delay(200);
	}
	//*(unsigned char *) 0x40020417 |= 0x000f;
	*(unsigned int *) 0x40020414 &= 0xff00;
	HAL_Delay(200);

#else //original code
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
#endif
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
			*(unsigned int *) 0x40020414 |= 0x80 >> i | 0x01 << i;
			//HAL_GPIO_WritePin(GPIOB, 0x80 >> i | 0x01 << i ,0); // << = left shift
			HAL_Delay(200);
		}
		*(unsigned int *) 0x40020414 &= 0xff00;
		//HAL_Delay(200);
}

void led_keepon_up(void)
{
	for(int i = 0 ; i < 8 ; i++)
	{
		//led_all_off();
		*(unsigned int *) 0x40020414 |= 0x01 << i;
		//HAL_GPIO_WritePin(GPIOB, 0x01 << i,1); // << = left shift
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
		*(unsigned int *) 0x40020414 |= 0x80 >> i;
		//HAL_GPIO_WritePin(GPIOB, 0x80 >> i,1); // >> = right shift
		HAL_Delay(200);
	}
	led_all_off();
	HAL_Delay(200);
}

void led_main(void)
{
	printf("GPIO->ODR: %0x\n", &GPIOB->ODR);
	while(1)
	{
//		led_all_on();
//		HAL_Delay(500);
//		led_all_off();
//		HAL_Delay(500);
//		led_up_on();
//		led_down_on();
//		led_flower_on();
//		led_flower_off();
//		led_keepon_up();
//		led_keepon_down();
// ====     Direct GPIO Access======
		//500ms마다 전체 toggle
		//GPIOB->ODR ^= GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
		//GPIOB->ODR ^= 0xff;

		//*(unsigned int *) 0x40020414 ^= GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
		//		GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
				//0x40020414 포인터가 주소이며 unsigned int 타입의 주소이다.
				//왜 unsigned int? -> int가 32bit이기 때문에 사용, 부호 비트 사용 없이 있는 그대로 표현하기 위해
				// *(unsigned int *) 앞의 *은 해당 주소에 값을 업데이트 한다는 의미
		led_all_on();
		HAL_Delay(500);
		led_all_off();
		HAL_Delay(500);
		led_up_on();
		HAL_Delay(500);
		led_down_on();
		HAL_Delay(500);
		led_flower_on();
		HAL_Delay(500);
		led_flower_off();
		HAL_Delay(500);
		led_keepon_up();
		HAL_Delay(500);
		led_keepon_down();
		HAL_Delay(500);
#if 0
		GPIOB->ODR |= GPIO_PIN_0; //LED0 ON
		HAL_Delay(500);
		GPIOB->ODR &= ~GPIO_PIN_0; //LED0 OFF
		GPIOB->ODR ^= GPIO_PIN_1; //LED1 Toggle
		HAL_Delay(500);
#endif
	}
}
