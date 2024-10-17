#include "gpio_74hc595.h"

extern void delay_us(unsigned int us);

void shift_clock(void);
void latch_clock(void);
//void write_ds_74hc595(unsigned char data);
void write_ds_74hc595(unsigned char data);
void gpio_74hc595_main(void);

//led 값 0x55 = 0b01010101
// 0xAA = 0b10101010 ->memory test 시 read write 검증 사용

void gpio_74hc595_main(void)
{
	static int past_time = 0;
	static int led_index = 0;
	static int shift_order = 0;
	uint8_t pattern = 0;

	while(1)
	{
		uint32_t now_time = HAL_GetTick(); //1ms가 읽힌다.
		// 1.처음 past_time = 0, now_time = 500
		if(now_time - past_time >= 500)
		{
			past_time = now_time;
#if 0
			if(!shift_order)

				pattern = 1 << led_index;
			else
				pattern = 0x80 >> led_index;

			write_ds_74hc595(pattern);
			led_index = (led_index + 1) % 8;

			if(!led_index)
				shift_order = !shift_order;



			switch(led_index)
			{
			case 0: // 0x55 = 0b01010101
				write_ds_74hc595(0b01010101);
				break;
			case 1: // 0xAA = 0b10101010
				write_ds_74hc595(0xAA);
				break;
			}
			led_index = !led_index;
#endif
		}
	}
}



void write_ds_74hc595(unsigned char data)
{
	unsigned char temp = data;


	for(int i = 0 ; i < 8 ; i++)
	{
		if(temp & 0b10000000)
			HAL_GPIO_WritePin(GPIOC, DS_74HC595_Pin, 1);
		else
			HAL_GPIO_WritePin(GPIOC, DS_74HC595_Pin, 0);

		shift_clock();
		temp = temp << 1;
	}

	latch_clock();

	//76543210 i original
	//10101010 0 msb -> 1:high, 0: low
	//01010100 1
	//10101000 2
	//01010000 3
	//10100000 4
	//01000000 5
	//10000000 6
	//00000000 7
}

void shift_clock(void)
{
	HAL_GPIO_WritePin(CLK_74HC595_GPIO_Port, CLK_74HC595_Pin, 1);
	//delay_us(10);
	HAL_GPIO_WritePin(CLK_74HC595_GPIO_Port, CLK_74HC595_Pin, 0);
	//delay_us(10);
}

void latch_clock(void)
{
	HAL_GPIO_WritePin(LATCH_74HC595_GPIO_Port, LATCH_74HC595_Pin, 1);
	//delay_us(10);
	HAL_GPIO_WritePin(LATCH_74HC595_GPIO_Port, LATCH_74HC595_Pin, 0);
	//delay_us(10);
}
