#include "main.h"

//write mode //
#define ADDR_SECONDS 0x80
#define ADDR_MINUTES 0x82
#define ADDR_HOURS 0x84
#define ADDR_DATE 0x86
#define ADDR_MONTH 0x88
#define ADDR_DAY_OF_WEEK 0x8A
#define ADDR_YEAR 0x8C
#define ADDR_WRITEPROTECTED 0x8E

extern unsigned char dec2bcd(unsigned char byte);
extern unsigned char bcd2dec(unsigned char byte);
extern volatile int TIM10_1ms_counter;
