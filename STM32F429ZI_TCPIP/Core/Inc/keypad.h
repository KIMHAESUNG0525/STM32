#ifndef DEVICE_KEYPAD_H_
#define DEVICE_KEYPAD_H_

#include "stm32f4xx_hal.h"
#define PUSHED		1
#define RELEASED	0

void keypadInit();
int new_input();
uint8_t getKeypadState(uint8_t col, uint8_t row);
uint8_t keypadScan();

#endif /* DEVICE_KEYPAD_H_ */
