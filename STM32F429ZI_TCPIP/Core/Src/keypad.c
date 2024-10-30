#include "keypad.h"

#if 1
GPIO_TypeDef* keypadRowPort[4] = {GPIOE, GPIOE, GPIOE, GPIOE}; //R1~R4
GPIO_TypeDef* keypadColPort[4] = {GPIOF, GPIOF, GPIOF, GPIOG}; //C1~C4
uint16_t keypadRowPin[4] = {GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_3}; //R1~R4 GPIO Input & Pull-up으로 설정을 해야 한다.
uint16_t keypadColPin[4] = {GPIO_PIN_8, GPIO_PIN_7, GPIO_PIN_9, GPIO_PIN_1}; //C1~C4  GPIO Output으로만 설정 한다.
#else  // orginal 
GPIO_TypeDef* keypadRowPort[4] = {GPIOF, GPIOF, GPIOF, GPIOG}; //R1~R4
GPIO_TypeDef* keypadColPort[4] = {GPIOE, GPIOE, GPIOE, GPIOE}; //C1~C4
uint16_t keypadRowPin[4] = {GPIO_PIN_8, GPIO_PIN_7, GPIO_PIN_9, GPIO_PIN_1}; //R1~R4
uint16_t keypadColPin[4] = {GPIO_PIN_3, GPIO_PIN_6, GPIO_PIN_5, GPIO_PIN_4}; //C1~C4
#endif 

uint8_t keypadScan();
void keypadInit();

void keypadInit()
{
	for(uint8_t col = 0; col < 4; col++)
	{
		HAL_GPIO_WritePin(keypadColPort[col], keypadColPin[col], SET); //초기 값 1로 셋팅 //스위치를 누르더라도 전위차가 없기에 전류가 흐르지 않음
	}
}

uint8_t getKeypadState(uint8_t col, uint8_t row)
{
#if 1
	uint8_t keypadChar[4][4] = {//KeyPad mapping Table
			{'/', '3', '2', '1'},
			{'*', '6', '5', '4'},
			{'-', '9', '8', '7'},
			{'+', '=', '0', ' '},
	};

#else
	uint8_t keypadChar[4][4] = {
			{'1', '4', '7', '*'},
			{'2', '5', '8', '0'},
			{'3', '6', '9', '#'},
			{'A', 'B', 'C', 'D'},
	};
#endif
	static uint8_t prevState[4][4] = {//버튼이 눌리지 않는 초기 상태//스태틱 활용은 변수가 함수 내에서 값을 유지함
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0},
			{0, 0, 0, 0},
	};
	uint8_t curState = 1;

	HAL_GPIO_WritePin(keypadColPort[col], keypadColPin[col], SET);
	curState = HAL_GPIO_ReadPin(keypadRowPort[row], keypadRowPin[row]);

	HAL_GPIO_WritePin(keypadColPort[col], keypadColPin[col], RESET);

	if(curState == PUSHED && prevState[col][row] == RELEASED)
	{
		prevState[col][row] = curState;
		return 0; //누른 상태를 체크
	}
	else if (curState == RELEASED && prevState[col][row] == PUSHED)
	{
		prevState[col][row] = curState;
		return keypadChar[col][row]; //뗀 상태를 체크
	}
	return 0; //이것도 저것도 아니면 return 0
	//polling 방식이 아닌 scanning 방식이기에 디바운스현상이 일어나지 않고, HAL_Delay가 없어도 동작한다.
}

uint8_t keypadScan()//타이머쪽에서 키패드스캔 함수를 불러서 계속 상태를 체크 하는 것임 //만약 10ms면 해당 되는 key에 대해 반복 //현재 65ms
{
	uint8_t data;

	for(uint8_t col=0; col<4; col++)
	{
		for(uint8_t row=0; row<4; row++)
		{
			data = getKeypadState(col, row);
			if(data != 0)
			{
				return data;
			}
		}
	}
	return 0;
}

int new_input()
{
    uint8_t key = keypadScan();
    if (key != 0)
    {
        return 1;
    }
    return 0;
}

