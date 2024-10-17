#include "uart2.h"
#include <string.h>

extern void led_all_on(void);
extern void led_all_off(void);
extern void led_up_on(void);
extern void led_down_on(void);
extern void led_flower_on(void);
extern void led_flower_off(void);
extern void led_keepon_up(void);
extern void led_keepon_down(void);


extern UART_HandleTypeDef huart2;
extern uint8_t rx_data;

void pc_command_processing(void);

void(*fp3[])() =
{
	led_all_on, // state 0
	led_all_off, // 1
	led_up_on,
	led_down_on,
	led_flower_on,
	led_flower_off,
	led_keepon_up,
	led_keepon_down
};

// move Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c to here
// ex) led_all_on\n 컴포트마스터에서 아스키코드를 체크 하는 이유 = \n 동작을 위해 체크 필수
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	volatile static int i = 0; //지역변수지만 전역변수처럼 유지가 되기 위해 static 사용

	if(huart == &huart2)
	{
		uint8_t data;
		data = rx_data;

		if(data == '\r' || data == '\n')
			{
				rx_buff[rear][i] = '\0'; //\n --> \0(문장의 끝을 알리는 indicator), 문장 끝을 가리키는 NULL
				i = 0; //다음 string을 저장 하기 위해 index값을 0으로 만든다.
				rear++;
				rear %= COMMAND_NUMBER; // 0 ~ 9 if(rear >= 9) rear = 0;
				// !!!! 이곳에 queue full(rx_buff) 상태를 check하는 로직이 들어가야함 !!!!
			}
			else
			{
				// !!!! 이곳에 COMMAND_LENGTH를 check 하는 로직 추가 !!!!
				rx_buff[rear][i++] = data;
			}
		//HAL_UART_Transmit(&huart2, &data, 1, 0xFFFF);
		HAL_UART_Receive_IT(&huart2, &rx_data, 1); // 반드시 작성해야한다.
	}
}

void pc_command_processing(void)
{
	static int state = 1;

	if(front != rear) //data가 rx_buff에 존재 한다.
	{
		printf("%s\n",rx_buff[front]); //&rx_buff[front][0]
		if(strncmp(rx_buff[front], "led_all_on",strlen("led_all_on")) == 0 )
			state = 0;
		else if (strncmp(rx_buff[front], "led_all_off",strlen("led_all_off")) == 0 )
			state = 1;
		else if (strncmp(rx_buff[front], "led_up_on",strlen("led_up_on")) == 0 )
			state = 2;
		else if (strncmp(rx_buff[front], "led_down_on",strlen("led_down_on")) == 0 )
			state = 3;
		else if (strncmp(rx_buff[front], "led_flower_on",strlen("led_flower_on")) == 0 )
			state = 4;
		else if (strncmp(rx_buff[front], "led_flower_off",strlen("led_up_off")) == 0 )
			state = 5;
		else if (strncmp(rx_buff[front], "led_keepon_up",strlen("led_keepon_up")) == 0 )
			state = 6;
		else if (strncmp(rx_buff[front], "led_keepon_down",strlen("led_keepon_down")) == 0 )
			state = 7;
		front++;
		front %= COMMAND_NUMBER;
		// !!!!! queue full check 하는 logic이 들어 가야 함!!!!!
	}

	fp3[state]();
}
