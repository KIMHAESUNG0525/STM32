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

extern void set_rtc(char *date_time); //10.08
extern int dotmatrix_main_func(void); //10.14
extern void dotmatrix_off(); //10.14

extern UART_HandleTypeDef huart2;
extern uint8_t rx_data;

uint8_t pdht11 = 1; //default print dht 11, 10.10
uint8_t pdatetime = 1; //default print date & time, 10.10
uint8_t dot_on = 0;

char help_cmd[][50] =//10.10, const -> flash memory에 저장하는 경우 read only와 같은
{
		"pdht11 : print dht11 debug message",
		"pdatetime : print datetime debug message"
};

void pc_command_processing(void);
void print_status(void); //10.10
void print_help(void); //10.10

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
		else if (strncmp(rx_buff[front], "setrtc",strlen("setrtc")) == 0 )
			set_rtc(&rx_buff[front][6]); //setrtc241008154500
		else if (strncmp(rx_buff[front], "pdatetime",strlen("pdatetime")) == 0 ) //10.10
			pdatetime = !pdatetime;
		else if (strncmp(rx_buff[front], "pdht11",strlen("pdht11")) == 0 ) //10.10
			pdht11 = !pdht11;
		else if (strncmp(rx_buff[front], "help",strlen("help")) == 0 ) //10.10
			print_help();
		else if (strncmp(rx_buff[front], "status",strlen("status")) == 0 ) //10.10
			print_status();
		else if (strncmp(rx_buff[front], "DOTDISON",strlen("DOTDISON")) == 0 ) //10.14
			dot_on = !dot_on;
		else if (strncmp(rx_buff[front], "DOTDISOFF",strlen("DOTDISOFF")) == 0 ) //10.14
			dot_on = 0;
		front++;
		front %= COMMAND_NUMBER;
		// !!!!! queue full check 하는 logic이 들어 가야 함!!!!!
	}

	//fp3[state]();
}

void print_help(void) //10.10
{
	for(int i = 0 ; i < HELP_NUMBER; i++)
	{
		printf("%s\n", help_cmd[i]); //help_cmd[i] ==> &help_cmd[i][0]

	}
}

void print_status(void) //10.10
{
	if(pdht11 == 1)
		printf("pdht11 : on\n");
	else
		printf("pdht11 : off\n");

	if(pdatetime == 1)
		printf("pdatetime : on\n");
	else
		printf("pdatetime : off\n");
}
