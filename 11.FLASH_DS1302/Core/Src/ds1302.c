#include "ds1302.h"
#include <stdio.h>


void ds1302_gpio_init(void);
void init_ds1302();
void ds1302_init_time_date(void);
void ds1302_write(uint8_t addr, uint8_t data);
void ds1302_clk(void);
void ds1302_tx(uint8_t tx);
void ds1302_main(void);
void ds1302_DataLine_Input(void);
void ds1302_DataLine_Output(void);
void ds1302_rx(uint8_t *data8);
uint8_t ds1302_read(uint8_t addr);
void ds1302_read_time(void);
void ds1302_read_date(void);

t_ds1302 ds_time;



void ds1302_gpio_init(void)
{
	HAL_GPIO_WritePin(CLK_DS1302_GPIO_Port, CLK_DS1302_Pin, 0);
	HAL_GPIO_WritePin(IO_DS1302_GPIO_Port, IO_DS1302_Pin, 0);
	HAL_GPIO_WritePin(CE_DS1302_GPIO_Port, CE_DS1302_Pin, 0);
}

void init_ds1302()
{
	ds_time.year = 24;
	ds_time.month = 10;
	ds_time.date = 24;
	ds_time.dayofweek = 5;
	ds_time.hour = 10;
	ds_time.minutes = 50;
	ds_time.seconds = 00;
}

void ds1302_init_time_date(void)
{
	ds1302_write(ADDR_SECONDS, ds_time.seconds);
	ds1302_write(ADDR_MINUTES, ds_time.minutes);
	ds1302_write(ADDR_HOURS, ds_time.hour);
	ds1302_write(ADDR_DATE, ds_time.date);
	ds1302_write(ADDR_MONTH, ds_time.month);
	ds1302_write(ADDR_DAY_OF_WEEK, ds_time.dayofweek);
	ds1302_write(ADDR_YEAR, ds_time.year);
}

void ds1302_write(uint8_t addr, uint8_t data)
{
	//1. CE HIGH
	HAL_GPIO_WritePin(CE_DS1302_GPIO_Port, CE_DS1302_Pin, 1);
	//2. addr 전송
	ds1302_tx(addr);
	//3. data 전송
	ds1302_tx(dec2bcd(data));
	//4. CE LOW
	HAL_GPIO_WritePin(CE_DS1302_GPIO_Port, CE_DS1302_Pin, 0);
}



void ds1302_clk(void)
{
	HAL_GPIO_WritePin(CLK_DS1302_GPIO_Port, CLK_DS1302_Pin, 1);
	HAL_GPIO_WritePin(CLK_DS1302_GPIO_Port, CLK_DS1302_Pin, 0);
}

void ds1302_tx(uint8_t tx)
{
	ds1302_DataLine_Output();
	// 초를 write 80h -> memory : 1000 0000
	// 하지만 ds1302는 LSB부터 데이터 전송이 이루어져야함 0000 0001
	//  1000 0000
	//& 0000 0001 0번 bit가 0인지 1인지 체크한다고 하자
	//============
	//  0000 0000

	//  1000 0000
	//& 1000 0000 7번 bit가 0인지 1인지 체크한다고 하자
	//============
	//  1000 0000

	for(int i = 0 ; i < 8 ; i++)
	{
		if(tx & (1 << i)) //1이상이면
		{
			// 1: bit set
			HAL_GPIO_WritePin(IO_DS1302_GPIO_Port, IO_DS1302_Pin, 1);

		}
		else
		{
			// bit reset
			HAL_GPIO_WritePin(IO_DS1302_GPIO_Port, IO_DS1302_Pin, 0);
		}
		ds1302_clk();
	}
}

//data8 = 0
//76543210 : LSB부터 DS1302에서 BIT가 넘어온다.
//HAL_GPIO_ReadPin 해당 BIT가 0이면 data 변수의 해당 bit를 0
//							1이면 data 변수의 해당 bit를 1
void ds1302_rx(uint8_t *data8)
{

	uint8_t temp = 0;

	ds1302_DataLine_Input(); //입력 모드로 전환

	//LSB부터 DS1302에서 넘어 온다.
	for(int i = 0 ; i < 8 ; i++)
	{
		//1. bit를 읽고
		if(HAL_GPIO_ReadPin(IO_DS1302_GPIO_Port, IO_DS1302_Pin))
		{
			//2.
			temp |= 1 << i;
		}
		if(i != 7)
			ds1302_clk();
	}
	*data8 = temp;
}

uint8_t ds1302_read(uint8_t addr)
{
	uint8_t data8bits = 0;

	//1. CE HIGH
	HAL_GPIO_WritePin(CE_DS1302_GPIO_Port, CE_DS1302_Pin, 1);
	//2. addr 전송
	ds1302_tx(addr+1);
	//3. data 읽어 들이는 작업
	ds1302_rx(&data8bits);
	//4. CE LOW
	HAL_GPIO_WritePin(CE_DS1302_GPIO_Port, CE_DS1302_Pin, 0);

	return bcd2dec(data8bits);
}

void ds1302_read_time(void)
{
	ds_time.seconds = ds1302_read(ADDR_SECONDS);
	ds_time.minutes = ds1302_read(ADDR_MINUTES);
	ds_time.hour = ds1302_read(ADDR_HOURS);
}

void ds1302_read_date(void)
{
	ds_time.date = ds1302_read(ADDR_DATE);
	ds_time.month = ds1302_read(ADDR_MONTH);
	ds_time.dayofweek = ds1302_read(ADDR_DAY_OF_WEEK);
	ds_time.year = ds1302_read(ADDR_YEAR);
}

void ds1302_main(void)
{
//	//initial 초기값
//	init_ds1302();
//
//	//CLK IO CE를 LOW로  적용
//	ds1302_gpio_init();
//
//	//time setting
//	ds1302_init_time_date();

	static int initialized = 0;
	if (!initialized)
	{
		//init_ds1302();
		ds1302_gpio_init();
		ds1302_init_time_date();
		initialized = 1;
	}

	//1. 시간을 읽고
	ds1302_read_time();
	//2. 날짜를 읽고
	ds1302_read_date();
	//3. 시간과 날짜를 printf
	printf("%4d-%2d-%2d %2d:%2d:%2d\n",ds_time.year+2000, ds_time.month, ds_time.date, ds_time.hour,
				ds_time.minutes, ds_time.seconds);
	//4. 1초 delay
	HAL_Delay(1000);

}


void ds1302_DataLine_Input(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/*Configure GPIO pin : PH0 */
  GPIO_InitStruct.Pin = IO_DS1302_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;			//Change Output to Input
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IO_DS1302_GPIO_Port, &GPIO_InitStruct);

	return;
}


void ds1302_DataLine_Output(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/*Configure GPIO pin : PH0 */
  GPIO_InitStruct.Pin = IO_DS1302_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;			//Change Input to Output
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; //LOW:2MHz , HIGH:25MHz ~ 100MHz
  HAL_GPIO_Init(IO_DS1302_GPIO_Port, &GPIO_InitStruct);

	return;
}
