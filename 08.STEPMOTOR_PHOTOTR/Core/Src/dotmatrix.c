#include "dotmatrix.h"
#include "button.h"

extern SPI_HandleTypeDef hspi2;
extern uint8_t dot_on;

void dotmatrix_main_test();
int dotmatrix_main(void);
int dotmatrix_main_func(void);
void init_dotmatrix(void);
void dotmatrix_off();
void button0_check(void);
void up_arrow_display(void);
void down_arrow_display(void);
void init_dotmatrix_arrow(void);


void(*fp_dot[])() =
{
	//dotmatrix_off,
	up_arrow_display,
	down_arrow_display
};

uint8_t allon[] = {			// allon 문자 정의
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111
};


uint8_t smile[] = {			// 스마일 문자 정의
	0b00111100,
	0b01000010,
	0b10010101,
	0b10100001,
	0b10100001,
	0b10010101,
	0b01000010,
	0b00111100 };

uint8_t hart[] = {		// hart
	0b00000000,    // hart
	0b01100110,
	0b11111111,
	0b11111111,
	0b11111111,
	0b01111110,
	0b00111100,
	0b00011000
};

uint8_t one[] =
{0b00011000,
0b00111000,
0b00011000,
0b00011000,
0b00011000,
0b00011000,
0b01111110,
0b01111110};

uint8_t my_name[] =
{0B11111010,
0B00001010,
0B00011010,
0B00110010,
0B01100010,
0B00111110,
0B00100010,
0B00111110};

uint8_t off[] =
{0B00000000,
0B00000000,
0B00000000,
0B00000000,
0B00000000,
0B00000000,
0B00000000,
0B00000000};

uint8_t up[] =
{0B00011000, //UP_ARROW
0B00111100,
0B01111110,
0B00011000,
0B00011000,
0B00011000,
0B00011000,
0B00000000};

uint8_t down[] =
{0B00000000, //DOWN_ARROW
0B00011000,
0B00011000,
0B00011000,
0B01111110,
0B00111100,
0B00011000,
0B00000000};

uint8_t col[4]={0,0,0,0};

void dotmatrix_main_test()
{
  //dotmatrix_main();

  while (1)
  {
        for (int i=0; i < 8; i++)
        {
			col[0] = ~(1 << i);  // 00000001  --> 11111110
			col[1] = my_name[i];
			HAL_SPI_Transmit(&hspi2, col, 2, 1); //1번째 출력하고자 하는 column, 2번째는 출력하고자 하는 데이터, 2byte write
			GPIOB->ODR &= ~GPIO_PIN_15;   // latch핀을 pull-down
			GPIOB->ODR |= GPIO_PIN_15;   // latch핀을 pull-up //latch를 low -> high로 하면 연결된 시프트 레지스터에 값이 출력되고 동작으로 이어짐
			HAL_Delay(1);
        }
  }
}

uint8_t number_data[20][10] =
{
	{
		0b01110000,	//0
		0b10001000,
		0b10011000,
		0b10101000,
		0b11001000,
		0b10001000,
		0b01110000,
	    0b00000000
	},
	{
		0b01000000,	//1
		0b11000000,
		0b01000000,
		0b01000000,
		0b01000000,
		0b01000000,
		0b11100000,
	    6   // 점 0b00000110
	},
	{
		0b01110000,	//2
		0b10001000,
		0b00001000,
		0b00010000,
		0b00100000,
		0b01000000,
		0b11111000,
	    6
	},
	{
		0b11111000,	//3
	    0b00010000,
		0b00100000,
		0b00010000,
		0b00001000,
		0b10001000,
		0b01110000,
	    6
	},
	{
		0b00010000,	//4
		0b00110000,
		0b01010000,
		0b10010000,
		0b11111000,
		0b00010000,
		0b00010000,
	    6
	},
	{
		0b11111000,	//5
		0b10000000,
		0b11110000,
		0b00001000,
		0b00001000,
		0b10001000,
		0b01110000,
	    6
	},
	{
		0b00110000,	//6
		0b01000000,
		0b10000000,
		0b11110000,
		0b10001000,
		0b10001000,
		0b01110000,
	    6
	},
	{
		0b11111000,	//7
		0b10001000,
		0b00001000,
		0b00010000,
		0b00100000,
		0b00100000,
		0b00100000,
	    6
	},
	{
		0b01110000,	//8
		0b10001000,
		0b10001000,
		0b01110000,
		0b10001000,
		0b10001000,
		0b01110000,
	    6
	},
	{
		0b01110000,	//9
		0b10001000,
		0b10001000,
		0b01111000,
		0b00001000,
		0b00010000,
		0b01100000,
	    6
	},
	{
		0b00000000,    // hart
		0b01100110,
		0b11111111,
		0b11111111,
		0b11111111,
		0b01111110,
		0b00111100,
		0b00011000
	}
};

unsigned char display_data[8];  // 최종 8x8 출력할 데이터
unsigned char scroll_buffer[50][8] = {0};  // 스코롤할 데이타가 들어있는 버퍼
int number_of_character = 11;  // 출력할 문자 갯수

// 초기화 작업
// 1. display_data에 number_data[0]에 있는 내용 복사
// 2. number_data를 scroll_buffer에 복사
// 3. dotmatrix의 led를 off
void init_dotmatrix(void)
{
	for (int i=0; i < 8; i++)
	{
		display_data[i] = number_data[i];
	}
	for (int i=1; i < number_of_character+1; i++)
	{
		for (int j=0; j < 8; j++) // scroll_buffer[0] = blank
		{
			scroll_buffer[i][j] = number_data[i-1][j];
		}
	}
}




// scroll 문자 출력 프로그램
int dotmatrix_main(void)
{
	static int count=0;  // 컬럼 count
	static int index=0;  // scroll_buffer의 2차원 index값
	static uint32_t past_time=0;  // 이전 tick값 저장

	init_dotmatrix();

	while(1)
	{
		uint32_t now = HAL_GetTick();  // 1ms = 1tick
		// 1.처음시작시 past_time=0; now: 500 --> past_time=500
		if (now - past_time >= 500) // 500ms scroll
		{
			past_time = now; //현재 now를 past에 저장
			for (int i=0; i < 8; i++)
			{

				display_data[i] = (scroll_buffer[index][i] >> count) |
						(scroll_buffer[index+1][i] << 8 - count);
			}
			if (++count == 8) // 8칼람을 다 처리 했으면 다음 scroll_buffer로 이동
			{
				count =0;
				index++;  // 다음 scroll_buffer로 이동
				if (index == number_of_character+1) index=0;
				// 11개의 문자를 다 처리 했으면 0번 scroll_buffer를 처리 하기위해 이동
			}
		}
		for (int i=0; i < 8; i++)
		{
			// 공통 양극 방식
			// column에는 0을 ROW에는 1을 출력해야 해당 LED가 on된다.
			col[0] = ~(1 << i);  // 00000001  --> 11111110
			col[1] = display_data[i];
			HAL_SPI_Transmit(&hspi2, col, 2, 1);
			GPIOB->ODR &= ~GPIO_PIN_15;   // latch핀을 pull-down
			GPIOB->ODR |= GPIO_PIN_15;   // latch핀을 pull-up
			HAL_Delay(1);
		}
	}
	return 0;
}


//실습 4
int dotmatrix_main_func(void)
{
	static int count=0;  // 컬럼 count
	static int index=0;  // scroll_buffer의 2차원 index값
	static uint32_t past_time=0;  // 이전 tick값 저장

	//init_dotmatrix();

	//while(1)
	//{
	if(dot_on == 1)
	{
		uint32_t now = HAL_GetTick();  // 1ms = 1tick
		// 1.처음시작시 past_time=0; now: 500 --> past_time=500
		if (now - past_time >= 500) // 500ms scroll
		{
			past_time = now; //현재 now를 past에 저장
			for (int i=0; i < 8; i++)
			{

				display_data[i] = (scroll_buffer[index][i] >> count) |
						(scroll_buffer[index+1][i] << 8 - count);
			}
			if (++count == 8) // 8칼람을 다 처리 했으면 다음 scroll_buffer로 이동
			{
				count =0;
				index++;  // 다음 scroll_buffer로 이동
				if (index == number_of_character+1) index=0;
				// 11개의 문자를 다 처리 했으면 0번 scroll_buffer를 처리 하기위해 이동
			}
		}
		for (int i=0; i < 8; i++)
		{
			// 공통 양극 방식
			// column에는 0을 ROW에는 1을 출력해야 해당 LED가 on된다.
			col[0] = ~(1 << i);  // 00000001  --> 11111110
			col[1] = display_data[i];
			HAL_SPI_Transmit(&hspi2, col, 2, 1);
			GPIOB->ODR &= ~GPIO_PIN_15;   // latch핀을 pull-down
			GPIOB->ODR |= GPIO_PIN_15;   // latch핀을 pull-up
			HAL_Delay(1);
		}

	//}
	}
	else if(dot_on == 0)
		dotmatrix_off();
	return 0;
}

void dotmatrix_off()
{
  //dotmatrix_main();

	for (int i=0; i < 8; i++)
	{
		col[0] = ~(1 << i);  // 00000001  --> 11111110
		col[1] = off[i];
		HAL_SPI_Transmit(&hspi2, col, 2, 1); //1번째 출력하고자 하는 column, 2번째는 출력하고자 하는 데이터, 2byte write
		GPIOB->ODR &= ~GPIO_PIN_15;   // latch핀을 pull-down
		GPIOB->ODR |= GPIO_PIN_15;   // latch핀을 pull-up //latch를 low -> high로 하면 연결된 시프트 레지스터에 값이 출력되고 동작으로 이어짐
		HAL_Delay(1);
	}

}




void up_arrow_display(void)
{
	static int count=0;  // 컬럼 count
	static int index=0;  // scroll_buffer의 2차원 index값
	static uint32_t past_time=0;  // 이전 tick값 저장


		uint32_t now = HAL_GetTick();  // 1ms = 1tick
		// 1.처음시작시 past_time=0; now: 500 --> past_time=500
		if (now - past_time >= 500) // 500ms scroll
		{
			past_time = now; //현재 now를 past에 저장

			uint8_t first_row;
			first_row = display_data[0];
			for (int i=0; i < 7; i++)
			{
				display_data[i] = display_data[i+1];

			}
			display_data[7] = first_row;

		}
		for (int i=0; i < 8; i++)
		{
			// 공통 양극 방식
			// column에는 0을 ROW에는 1을 출력해야 해당 LED가 on된다.
			col[0] = ~(1 << i);  // 00000001  --> 11111110
			col[1] = display_data[i];
			HAL_SPI_Transmit(&hspi2, col, 2, 1);
			GPIOB->ODR &= ~GPIO_PIN_15;   // latch핀을 pull-down
			GPIOB->ODR |= GPIO_PIN_15;   // latch핀을 pull-up
			HAL_Delay(1);
		}

}

void down_arrow_display(void)
{
	static int count=0;  // 컬럼 count
	static int index=0;  // scroll_buffer의 2차원 index값
	static uint32_t past_time=0;  // 이전 tick값 저장

	//init_dotmatrix_arrow();

	uint32_t now = HAL_GetTick();  // 1ms = 1tick
	// 1.처음시작시 past_time=0; now: 500 --> past_time=500
	if (now - past_time >= 500) // 500ms scroll
	{
		past_time = now; //현재 now를 past에 저장

		uint8_t last_row;
		last_row = display_data[7];
		for (int i=7; i > -1; i--)
		{
			display_data[i] = display_data[i-1];

		}
		display_data[0] = last_row;
	}
	for (int i=0; i < 8; i++)
	{
		// 공통 양극 방식
		// column에는 0을 ROW에는 1을 출력해야 해당 LED가 on된다.
		col[0] = ~(1 << i);  // 00000001  --> 11111110
		col[1] = display_data[i];
		HAL_SPI_Transmit(&hspi2, col, 2, 1);
		GPIOB->ODR &= ~GPIO_PIN_15;   // latch핀을 pull-down
		GPIOB->ODR |= GPIO_PIN_15;   // latch핀을 pull-up
		HAL_Delay(1);
	}

}

//{
////    static int state_mode = 2;
////    static int last_state_mode = -1;
////
////    if (get_button(GPIOC, GPIO_PIN_0, BUTTON0) == BUTTON_PRESS)
////    {
////        if (state_mode >= 1)
////        {
////            state_mode = 0;
////            for (int i=0; i < 8; i++)
////        	{
////        		display_data[i] = up[i];
////        	}
////        }
////        else
////        {
////            state_mode++;
////        	for (int i=0; i < 8; i++)
////        	{
////        		display_data[i] = down[i];
////        	}
////        }
////
////        last_state_mode = state_mode;
////
////    }
////
////    if (last_state_mode != -1)
////    {
////        fp_dot[last_state_mode]();
////    }
//}

void button0_check(void)
{
	static int state_mode = UP_ARROW;
	if(get_button(GPIOC, GPIO_PIN_0, BUTTON0) == BUTTON_PRESS)
	{

		switch(state_mode)
		{
		case UP_ARROW:
		state_mode = DOWN_ARROW;
		for (int i=0; i < 8; i++)
		{
			display_data[i] = down[i];
		}
		break;

		case DOWN_ARROW:
		state_mode = UP_ARROW;
		for (int i=0; i < 8; i++)
		{
		display_data[i] = up[i];
		}
		break;
		}

	}
	fp_dot[state_mode]();
}
