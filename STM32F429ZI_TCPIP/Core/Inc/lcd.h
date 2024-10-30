#include "main.h"

#define DISPLAY_CLEAR 0b00000001
#define DISPLAY_OFF 0b00001000
#define DISPLAY_ON 0b00001100
#define FUNCTION_SET 0b00101000
#define INPUT_SET 0b00000110
#define MODE_4BIT_SETUP 0b0011
#define MODE_4BIT 0b0010

#define GPIO_PORTD_OUTPUT 0x40020C14
#define GPIO_ENABLE_SET 0x4000
#define GPIO_RS_SET 0x1000

extern TIM_HandleTypeDef htim11;

void delay_us(unsigned int us);
void gpio_lcd_init(void);
void gpio_lcd_Enable_Pulse(void);
void gpio_lcd_4bit_data(char data);
void gpio_lcd_8bit_data(char data);
void gpio_lcd_clear(void);
void gpio_lcd_cursor(int row , int col);
void gpio_lcd_write_string(char *string);
void Enable(void);
void Disable(void);
