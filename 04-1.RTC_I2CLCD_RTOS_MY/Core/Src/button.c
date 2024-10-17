#include "button.h"


unsigned char button_status[BUTTON_NUMBER] = {
		BUTTON_RELEASE, BUTTON_RELEASE, BUTTON_RELEASE, BUTTON_RELEASE, BUTTON_RELEASE //초기 상태
};

extern void led_all_on(void);
extern void led_all_off(void);
extern void led_up_on(void);
extern void led_down_on(void);
extern void led_flower_on(void);
extern void led_flower_off(void);
extern void led_keepon_up(void);
extern void led_keepon_down(void);

static int state = 0;
int get_button(GPIO_TypeDef* GPIO, uint16_t GPIO_PIN, int button_num);

void(*fp[])() =
{
	led_all_on,
	led_all_off,
	led_up_on,
	led_down_on,
	led_keepon_up,
	led_keepon_down,
	led_flower_on,
	led_flower_off
};

void button_check(void)
{
	// 데모보드 버튼을 한 번 눌렀다 뗏는지 확인하는 함수
	{
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	}
	if(get_button(GPIOC, GPIO_PIN_0, BUTTON0) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
	}
	if(get_button(GPIOC, GPIO_PIN_1, BUTTON1) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);
	}
	if(get_button(GPIOC, GPIO_PIN_2, BUTTON2) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_2);
	}
	if(get_button(GPIOC, GPIO_PIN_3, BUTTON3) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);
	}
}

//버튼을 한 번 눌렀다 뗀 상태이면 1을 리턴
int get_button(GPIO_TypeDef* GPIO, uint16_t GPIO_PIN, int button_num) // 예) GPIOC
{
	int state;

	state = HAL_GPIO_ReadPin(GPIO, GPIO_PIN); //active : 0 , release : 1
	if(state == BUTTON_PRESS && button_status[button_num] == BUTTON_RELEASE) // 처음 눌러진 상태
	{
		HAL_Delay(60); // 노이즈가 지나가길 기다리는 딜레이
		button_status[button_num] = BUTTON_PRESS; // 처음 눌러진 상태가 아님
		return BUTTON_RELEASE; //아직은 완전히 눌려진 상태가 아니다.
	}
	else if(button_status[button_num] == BUTTON_PRESS && state == BUTTON_RELEASE) // 버튼이 이전에는 눌러진 상태이나 지금은 떼어진 상태
	{
		button_status[button_num] = BUTTON_RELEASE; //다음 버튼 상태를 체크 하기 위해서 초기화
		HAL_Delay(60); // 노이즈가 지나가길 기다리는 딜레이
		return BUTTON_PRESS; // 완전히 눌렸다 떼어진 상태이다.
	}
	return BUTTON_RELEASE;
}

void button_up(void)
{
	static int i = 0;

	// 데모보드 버튼을 한 번 눌렀다 뗏는지 확인하는 함수
	if(i<8 && get_button(GPIOC, GPIO_PIN_13, BUTTON0) == BUTTON_PRESS)
	{

		HAL_GPIO_WritePin(GPIOB, 0x01 << i,1);
		i++;
	}
	else if(i >= 8 && get_button(GPIOC, GPIO_PIN_13, BUTTON0) == BUTTON_PRESS)
	{
		HAL_GPIO_WritePin(GPIOB, 0x01 << 15-i,0);
		i++;
	}
	i %= 16;
}

void button0_test(void)
{
	if(get_button(GPIOC, GPIO_PIN_0, BUTTON0) == BUTTON_PRESS)
	{
		state++;
		if(state >=4)
		{
			state = 0;
		}

	}

	fp[state]();
}

void button1_test(void)
{
	if(get_button(GPIOC, GPIO_PIN_1, BUTTON1) == BUTTON_PRESS)
	{
		if(state != 4)
		{
			state = 4;
		}
		else if(state == 4)
		{
			state = 5;
		}
	}
}

void button2_test(void)
{
	if(get_button(GPIOC, GPIO_PIN_2, BUTTON2) == BUTTON_PRESS)
	{
		if(state != 6)
		{
			state = 6;
		}
		else if(state == 6)
		{
			state = 7;
		}
	}
}
