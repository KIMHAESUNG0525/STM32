#include "main.h" // For GPIO Handling & HAL func

#define BUTTON0 0 // PC8
#define BUTTON1 1 // PC9
#define BUTTON2 2 // PC10
#define BUTTON3 3 // PC11
#define BUTTON4 4 // PC13 DEMO BUTTON

#define BUTTON_PRESS 0 // Active Low
#define BUTTON_RELEASE 1 // 떼면 High

#define BUTTON_NUMBER 5 // Button 개수


int get_button(GPIO_TypeDef* GPIO, uint16_t GPIO_PIN, int button_num);
void button_check(void);
