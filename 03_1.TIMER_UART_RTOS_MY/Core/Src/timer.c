#include "timer.h" //현재 디렉토리에 있는 파일을 포함, <> : 스탠다드 라이브러리에 있는 헤더를 포함하는 문

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

//extern volatile unsigned int timer_ms;
extern volatile int TIM3_10ms_counter;


//delay_us(10);
void delay_us(unsigned int us);
void delay_ms(unsigned int ms);


// 1M로 분주 주파수가 TIM2으로 입력 된다.
// T = 1/f = 1/1000000Hz = 0.000001sec(1us) : 1개의 pulse 소요 시간
// ex) 1000us --> 1ms
void delay_us(unsigned int us)
{
	// TCNT= 0; AVR의 방법
	__HAL_TIM_SET_COUNTER(&htim2, 0); // TIMER 2번의 COUNTER를 CLEAR 한다.
	// 사용자가 지정한 us 동안 머물러 있도록 한다.
	while(__HAL_TIM_GET_COUNTER(&htim2) < us ) // 사용자가 지정한 pluse를 만날 때 까지 wait
		; // ;의 의미는 NOP, 즉 ; 자체가 설정한 pulse가 지나갈때까지 대기?
}

void delay_ms(unsigned int ms)
{
	//{
	//	// TCNT= 0; AVR의 방법
	//	//__HAL_TIM_SET_COUNTER(&htim3, 0); // TIMER 3번의 COUNTER를 CLEAR 한다.
	//	// 사용자가 지정한 us 동안 머물러 있도록 한다.
	//	//__HAL_TIM_SET_AUTORELOAD(&htim3, ms);
	//	//HAL_TIM_Base_Start(&htim3);
	//	//while(__HAL_TIM_GET_COUNTER(&htim3) < ms ) // 사용자가 지정한 pluse를 만날 때 까지 wait
	//		; // ;의 의미는 NOP, 즉 ; 자체가 설정한 pulse가 지나갈때까지 대기?
	//		 // 타이머 3번의 카운터를 0으로 초기화
	//	unsigned int start_time = timer_ms;
	//	while ((timer_ms - start_time) < ms)
	//		;
    unsigned int initial_time = TIM3_10ms_counter;
    unsigned int start_counter = __HAL_TIM_GET_COUNTER(&htim3);

    while (((TIM3_10ms_counter - initial_time) * 10) +
           ((__HAL_TIM_GET_COUNTER(&htim3) - start_counter + 10) % 10) < ms)
    		;
}
