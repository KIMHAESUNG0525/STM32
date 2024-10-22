#include "extint.h"
#include "stepmotor.h"

extern int step;
extern volatile int TIM10_servo_motor_counter;

//extern void(*fp_step[])();

//move from Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c to here
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch(GPIO_Pin)
	{
	case GPIO_PIN_1: //PA1
		step = FORWARD;
		TIM10_servo_motor_counter = 0;
		//printf("GPIO_PIN_1\n"); //1char = 1ms 이므로 현재 10ms, 10ms 이내에 인터럽트가 뜨면 안됨, printf는 확인 후 주석처리 해야함
		break;
	case GPIO_PIN_4: //PA4
		step = BACKWARD;
		//printf("GPIO_PIN_4\n");
		break;
	}
}
