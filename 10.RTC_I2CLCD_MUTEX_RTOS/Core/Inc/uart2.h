#include "main.h" //for GPIO & HAL func를 call하기 위함

#define COMMAND_NUMBER 20
#define COMMAND_LENGTH 40
#define HELP_NUMBER 2

volatile uint8_t rx_buff[COMMAND_NUMBER][COMMAND_LENGTH]; //uart0로부터 들어온 문자 저장 버퍼, 메세지 20개 최대 길이 80
volatile int rear = 0; //input index : USART0_RX_vect에 집어넣는 index
volatile int front = 0; //output index
