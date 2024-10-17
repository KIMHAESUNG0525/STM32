extern void lcd_command(uint8_t command);//10.10
extern void lcd_data(uint8_t data);//10.10
extern void i2c_lcd_init(void);//10.10
extern void lcd_string(uint8_t *str);//10.10
extern void move_cursor(uint8_t row, uint8_t column);//10.10


extern RTC_HandleTypeDef hrtc;
extern uint8_t pdatetime; //default print date & time //10.10
extern RTC_TimeTypeDef sTime; // 시각 정보
extern RTC_DateTypeDef sDate; // 날짜 정보



extern int get_button(GPIO_TypeDef* GPIO, uint16_t GPIO_PIN, int button_num);
extern volatile int TIM10_1ms_counter1;

extern unsigned char bcd2dec(unsigned char byte);
extern unsigned char dec2bcd(unsigned char byte);

extern void HAL_RTC_DST_ClearStoreOperation(RTC_HandleTypeDef *hrtc);
