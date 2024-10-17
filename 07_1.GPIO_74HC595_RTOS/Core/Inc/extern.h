extern void lcd_command(uint8_t command);//10.10
extern void lcd_data(uint8_t data);//10.10
extern void i2c_lcd_init(void);//10.10
extern void lcd_string(uint8_t *str);//10.10
extern void move_cursor(uint8_t row, uint8_t column);//10.10

extern RTC_HandleTypeDef hrtc;
extern uint8_t pdatetime; //default print date & time //10.10
