#include "extint.h"
#include "ele.h"

int current_floor;
extern volatile int TIM10_servo_motor_counter;
extern int servo_step;
extern int floor_state;


//move from Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c to here
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch(GPIO_Pin)
	{
	case GPIO_PIN_1: //PA1 //1층

		current_floor = floor_1;
		if(floor_state == 1)
		{
			servo_step = 1;
			TIM10_servo_motor_counter = 0;
		}
		//printf("GPIO_PIN_1\n"); //1char = 1ms 이므로 현재 10ms, 10ms 이내에 인터럽트가 뜨면 안됨, printf는 확인 후 주석처리 해야함
		break;
	case GPIO_PIN_4: //PA4 // 4층
		current_floor = floor_4;
		if(floor_state == 4)
		{
			servo_step = 4;
			TIM10_servo_motor_counter = 0;
		}
		//printf("GPIO_PIN_4\n");
		break;
	case GPIO_PIN_12: //PB12 //2층
		current_floor = floor_2;

		if(floor_state == 2)
		{
			servo_step = 12;
			TIM10_servo_motor_counter = 0;
		}
		//printf("GPIO_PIN_12\n");
		break;
	case GPIO_PIN_9: //PA9 //3층
		current_floor = floor_3;
		if(floor_state == 3)
		{
			servo_step = 9;
			TIM10_servo_motor_counter = 0;
		}

		//printf("GPIO_PIN_13\n");
		break;
	}
}
