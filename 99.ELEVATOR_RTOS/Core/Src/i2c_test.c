#include "main.h"

#define I2C_LCD_ADDRESS (0x27 << 1)
unsigned char lcd_test[10] = { '1', 'F', 0};
extern I2C_HandleTypeDef hi2c1;

void i2c_test(void);

void i2c_test(void)
{
  while (1)
  {
    while(HAL_I2C_Master_Transmit(&hi2c1, I2C_LCD_ADDRESS,
          lcd_test, 2, 100) != HAL_OK)
    {
      //HAL_Delay(1);
    }
    HAL_Delay(1000);
  }  // 여기에 닫는 중괄호 추가
}
