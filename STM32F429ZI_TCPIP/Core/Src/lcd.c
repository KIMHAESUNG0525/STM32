#include "lcd.h"

void delay_us(unsigned int us)
{
	// TCNT= 0; AVR의 방법
	__HAL_TIM_SET_COUNTER(&htim11, 0); // TIMER 2번의 COUNTER를 CLEAR 한다.
	// 사용자가 지정한 us 동안 머물러 있도록 한다.
	while(__HAL_TIM_GET_COUNTER(&htim11) < us ) // 사용자가 지정한 pluse를 만날 때 까지 wait
		; // ;의 의미는 NOP, 즉 ; 자체가 설정한 pulse가 지나갈때까지 대기?
}


//HW DIRECT ACCESS
//GPIOE GPIOD ==> AHB1 USED
//GPIOD : 0x4002 0C00
//GPIOx OUTPUT Data Register : 0x14
void gpio_lcd_Enable_Pulse(void) // Enable 펄스 발생 함수 // 주기 수정 필요할수도?
{
	*(unsigned int*) GPIO_PORTD_OUTPUT |= GPIO_ENABLE_SET;
	HAL_Delay(1);
	*(unsigned int*) GPIO_PORTD_OUTPUT &= ~(GPIO_ENABLE_SET);
	HAL_Delay(1);
}

void Enable(void)
{
	*(unsigned int*) GPIO_PORTD_OUTPUT &= ~(GPIO_ENABLE_SET);
	HAL_Delay(1);

	*(unsigned int*) GPIO_PORTD_OUTPUT |= GPIO_ENABLE_SET;
	HAL_Delay(1);
}

void Disable(void)
{
	*(unsigned int*) GPIO_PORTD_OUTPUT |= GPIO_ENABLE_SET;
	HAL_Delay(1);

	*(unsigned int*) GPIO_PORTD_OUTPUT &= ~(GPIO_ENABLE_SET);
	HAL_Delay(1);
}

void gpio_lcd_4bit_data(char data) //D7~D4 4bit Data Input
{

	Enable();

	if ((data >> 3) & 0x01)
		*(unsigned int*)GPIO_PORTD_OUTPUT |= LCD_D7_Pin;// SET
	else
		*(unsigned int*)GPIO_PORTD_OUTPUT &= ~(LCD_D7_Pin); // RESET

	if ((data >> 2) & 0x01)
		*(unsigned int*)GPIO_PORTD_OUTPUT |= LCD_D6_Pin;
	else
		*(unsigned int*)GPIO_PORTD_OUTPUT &= ~(LCD_D6_Pin);

	if ((data >> 1) & 0x01)
		*(unsigned int*)GPIO_PORTD_OUTPUT |= LCD_D5_Pin;
	else
		*(unsigned int*)GPIO_PORTD_OUTPUT &= ~(LCD_D5_Pin);

	if (data & 0x01)
		*(unsigned int*)GPIO_PORTD_OUTPUT |= LCD_D4_Pin;
	else
		*(unsigned int*)GPIO_PORTD_OUTPUT &= ~(LCD_D4_Pin);
	// D7의 경우 4번째 BIT를 3번 시프트 하여 0x01과 AND함으로서, 4번째 비트만 추출하는 원리
	// D6의 경우 3번째 BIT를 2번 시프트 하여 0x01과 AND함으로서, 3번째 비트만 추출하는 원리
	// D5의 경우 2번째 BIT를 1번 시프트 하여 0x01과 AND함으로서, 2번째 비트만 추출하는 원리
	// D4의 경우 1번째 BIT를 0번 시프트 하여 0x01과 AND함으로서, 1번째 비트만 추출하는 원리
	//gpio_lcd_Enable_Pulse(); //Enable 신호에 맞춰 데이터 전송
	Disable();
}

void gpio_lcd_8bit_data(char data) // 상위 4bit 하위 4bit 생성
{
	gpio_lcd_4bit_data((data >> 4) & 0x0f); //상위 4bit
	gpio_lcd_4bit_data(data & 0x0f); //하위 4bit
}

void gpio_lcd_init(void) //Text LCD 초기화
{
	*(unsigned int*) GPIO_PORTD_OUTPUT &= ~(GPIO_RS_SET); //RS : RESET
	HAL_Delay(15); // >15ms Data Sheet

	gpio_lcd_4bit_data(MODE_4BIT_SETUP);
	HAL_Delay(5); // >4.1ms Data Sheet

	gpio_lcd_4bit_data(MODE_4BIT_SETUP);
	delay_us(110); // >100us Data Sheet

	gpio_lcd_4bit_data(MODE_4BIT_SETUP);
	HAL_Delay(1);

	gpio_lcd_4bit_data(MODE_4BIT);
	HAL_Delay(1);

	gpio_lcd_8bit_data(FUNCTION_SET); //Function Set // DL(1) : 8bit, DL(0) : 4bit
	delay_us(40); // >40us Data Sheet

	gpio_lcd_8bit_data(DISPLAY_OFF); //Display off(switch) // display cursor blink
	delay_us(40); // >40us Data Sheet

	gpio_lcd_8bit_data(DISPLAY_CLEAR); //Display Clear
	HAL_Delay(2); // >1.64ms Data Sheet

	gpio_lcd_8bit_data(INPUT_SET); //Input Set //1/D : cursor input 위치 : 1--> 오른쪽 입력
	HAL_Delay(2); // >1.64ms Data Sheet

	gpio_lcd_8bit_data(DISPLAY_ON); //Display on(switch) // display cursor blink
	delay_us(40); // >40us Data Sheet

}

void gpio_lcd_clear(void) //Display Clear Function
{
	*(unsigned int*) GPIO_PORTD_OUTPUT &= ~(GPIO_RS_SET); //RS : RESET
	gpio_lcd_8bit_data(DISPLAY_CLEAR); //Display Clear
	HAL_Delay(2); // >1.64ms Data Sheet
}

void gpio_lcd_cursor(int row , int col)
{
	*(unsigned int*) GPIO_PORTD_OUTPUT &= ~(GPIO_RS_SET); //RS : RESET

	switch(row) //row 0 or 1 switch case
	{
		case 0:
			col |= 0x80; // if row = 0 col = 0--> col address 0x80;
		break;
			//첫 번째 라인의 주소 : 0x00 두 번째 라인 주소 : 0x40
			//address = (0x40 * row) + col
			//command = 0x80 + address
		case 1:
			col |= 0xC0; // if row = 1 col = 0--> col address 0xC0;
		break;

	}
	gpio_lcd_8bit_data(col); //LCD DDRAM : Display Data RAM,
							 //화면에 출력할 데이터를 저장하는 공간에 대한 주소
}

void gpio_lcd_write_string(char *string)
{
	*(unsigned int*) GPIO_PORTD_OUTPUT |= GPIO_RS_SET; // RS : SET

	while (*string)
	{
		gpio_lcd_8bit_data(*string++);
	}
}
