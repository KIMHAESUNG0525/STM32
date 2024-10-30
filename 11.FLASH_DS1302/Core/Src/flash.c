#include "main.h"
#include "extern.h"
#include <stdio.h>
#include <string.h>
#include "button.h"

/**************************************************
     Flash module organization(STM32F411)
                               512KBytes

Name        Block base address              size
====      ==================   =======
Sector 0    0x8000000-0x8003FFF           16K bytes
Sector 1    0x8004000-0x8007FFF           16K bytes
Sector 2    0x8008000-0x800BFFF           16K bytes
Sector 3    0x800C000-0x800FFFF           16K bytes
Sector 4    0x8010000-0x801FFFF           64K bytes
Sector 5    0x8020000-0x803FFFF          128K bytes
Sector 6    0x8040000-0x805FFFF          128K bytes
Sector 7    0x8060000-0x807FFFF          128K bytes

******************************************************/

// 0x8060000-0x807FFFF 의 빈공간에 사용자 데이터를 flash programming
// 하도록 설정 한다.
#define ADDR_FLASH_SECTOR7      ((uint32_t) 0x8060000) //32BIT SECTOR7의 시작 번지
#define FLASH_USER_START_ADDR   ((uint32_t) 0x8060000)
#define USER_DATA_ADDRESS        0x8060000
#define FLASH_USER_END_ADDR     ((uint32_t) 0x807FFFF)
#define FLASH_INIT_STATUS       0xFFFFFFFF        // flash 초기 상태
#define FLASH_NOT_INIT_STATUS   0xAAAAAAAA        // flash 초기 상태가 아니다
#define DATA_32                 ((uint32_t) 0x00000001)

HAL_StatusTypeDef flash_write(uint32_t *data32, int size);
HAL_StatusTypeDef flash_read(uint32_t *addr32, int size);
HAL_StatusTypeDef flash_erase();
void flash_main();
void flash_set_time(void);
void set_alarm_time(char *time);
void real_time(void);
void flash_ds1302_set_time(void);
void ds1302_set_time(char *time);
void ds1302_time_flash_read(void);

extern RTC_TimeTypeDef sTime; // 시각 정보
extern RTC_DateTypeDef sDate; // 날짜 정보
extern unsigned char bcd2dec(unsigned char byte);
extern unsigned char dec2bcd(unsigned char byte);
extern void siren2(int siren);
extern void beep(int repeat);
extern void noTone();

extern volatile int TIM10_1ms_counter;
extern void ds1302_init_time_date(void);
extern void ds1302_gpio_init(void);

uint32_t flash_read_value=0;

typedef struct student //STUDENT 구조체 내
{
	uint32_t magic; //MAGIC : 플래시메모리가 공장 초기화 상태인지, 한 번이라도 쓴 기록이 있는지, 이거를 가리키기 위한 변수
    int num;        // hakbun
    char name[20];  // name
    double grade;   // hakjum
} t_student; //t_student 로 재정의 t_ : typedef에 주로 사용



typedef struct
{ //total 8byte = 32bit = 1word(with dummy)
	uint32_t magic; //4byte
	uint8_t Hours;  //1byte
	uint8_t Minutes;    // 1byte
	uint8_t Seconds; //1byte
	uint8_t dummy; //memory 얼라이먼트를 위해 잡아주기 위한 변수 // 1byte
	uint8_t Year;
	uint8_t Month;
	uint8_t Date;
	uint8_t dummy2;
}t_set_time;

t_student student; //student라는 이름으로 t_student를 할당
t_set_time set_time;



void flash_main()
{

	t_student *read_student;

	flash_read_value = *(__IO uint32_t *) USER_DATA_ADDRESS; //사용자가 쓰고자하는 uint32 타입의 주소를 read_value에 담음 0x806 = sector7

	if (flash_read_value == FLASH_INIT_STATUS)  // 초기에 아무런 데이터도 존재 하지 않을 경우
	{
		flash_erase(); //1개 sector만 지우는 동작
printf("flash EMPTY!!!\n");
		student.magic=0x55555555; //플래시메모리에 0x555555... 를 썻다는 의미 4byte = 32bit
		student.num=1101815; //학번 적용
		strncpy((char *)&student.name,"Hong_Gil_Dong", strlen("Hong_Gil_Dong")); //name의 주소를 가진 공간에 해당 값을 복사한다. 이 때 타입은 char(1byte 공간) 형변환
		student.grade=4.0;
		printf("w grade: %lf\n", student.grade);
		flash_write((uint32_t *) &student, sizeof(student)); //student 구조체 내에 할당 받은 값을 플래시 메모리에 쓰기
	}
	else   // 1번 이상 flash memory에 데이터를 write 한 경우
	{
		flash_read((uint32_t *)&student, sizeof(student));

		printf("magic: %08x\n", student.magic);
		printf("num: %08x\n", 	student.num);
		printf("name: %s\n", student.name);
		printf("r grade: %lf\n", student.grade);
	}
}

void set_alarm_time(char *time)
{
	//flash_erase(); //1개 sector만 지우는 동작 //쓰레기 값 방지하기 위해 한 번 더 erase



	char hh[4], min[4], ss[4]; //time 정보 //ascii를 int로 변환하기 위해 작성 atoi를 활용하기 위함

	strncpy(hh, time, 2);
	strncpy(min, time+2, 2);
	strncpy(ss, time+4, 2);

	flash_read_value = *(__IO uint32_t *) USER_DATA_ADDRESS;



	set_time.magic=0x55555555; //사용자 임의 지정 값
	set_time.Hours = atoi(hh); //플래시메모리에 0x555555... 를 썻다는 의미 4byte = 32bit
	set_time.Minutes = atoi(min); //minute
	set_time.Seconds = atoi(ss);
	flash_write((uint32_t *) &set_time, sizeof(set_time)); //student 구조체 내에 할당 받은 값을 플래시 메모리에 쓰기
}

//uint8_t alarm_active = 0;

extern t_ds1302 ds_time;

void real_time(void)
{
	static int time_state = 0;

	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);

	uint8_t hour = ds_time.hour;
	uint8_t minute = ds_time.minutes;
	uint8_t sec = ds_time.seconds;

	if(set_time.Hours == hour && set_time.Minutes == minute && set_time.Seconds == sec)
	{
		time_state = 1;
		TIM10_1ms_counter = 0;
	}


	if(time_state == 1 && TIM10_1ms_counter < 30000)
	{
		beep(1000);
		if(get_button(GPIOC, GPIO_PIN_0, BUTTON0) == BUTTON_PRESS)
		{
			//alarm_active = !alarm_active;
			time_state = 0;
		}
	}
}
void ds1302_time_flash_read(void)
{
	flash_read((uint32_t *)&set_time, sizeof(set_time));
	printf("DATE:%04d:%02d:%02d Time:%02d:%02d:%02d\n",set_time.Year+2000,set_time.Month,set_time.Date,set_time.Hours,set_time.Minutes,set_time.Seconds);
}


void flash_ds1302_set_time(void)
{
	flash_read_value = *(__IO uint32_t *) USER_DATA_ADDRESS;

	if(TIM10_1ms_counter >= 300000) // 5분 이상 flash memory에 데이터를 write 한 경우
	{
		TIM10_1ms_counter = 0;

		set_time.Year= ds_time.year;
		set_time.Month= ds_time.month;
		set_time.Date= ds_time.date;
		set_time.Hours= ds_time.hour;
		set_time.Minutes= ds_time.minutes;
		set_time.Seconds= ds_time.seconds;
		//set_time = ds_time;
		flash_write((uint32_t *) &set_time, sizeof(set_time));
		printf("DATE:%04d:%02d:%02d Time:%02d:%02d:%02d\n",set_time.Year+2000,set_time.Month,set_time.Date,set_time.Hours,set_time.Minutes,set_time.Seconds);
	}

	if(ds_time.year <=10)
	{
		flash_read((uint32_t *)&set_time, sizeof(set_time));
		ds_time.year = set_time.Year;
		ds_time.month = set_time.Month;
		ds_time.date = set_time.Date;
		ds_time.hour =	set_time.Hours;
		ds_time.minutes = set_time.Minutes;
		ds_time.seconds = set_time.Seconds;

		ds1302_gpio_init();
		ds1302_init_time_date();

	}
}

void ds1302_set_time(char *time)
{
	//flash_erase(); //1개 sector만 지우는 동작 //쓰레기 값 방지하기 위해 한 번 더 erase

	char yy[4], mm[4], dd[4];
	char hh[4], min[4], ss[4]; //time 정보 //ascii를 int로 변환하기 위해 작성 atoi를 활용하기 위함

	strncpy(yy, time, 2);
	strncpy(mm, time+2, 2);
	strncpy(dd, time+4, 2);

	strncpy(hh, time+6, 2);
	strncpy(min, time+8, 2);
	strncpy(ss, time+10, 2);

	set_time.magic=0x55555555; //사용자 임의 지정 값

	set_time.Year = atoi(yy);
	set_time.Month = atoi(mm);
	set_time.Date = atoi(dd);
	set_time.Hours = atoi(hh); //플래시메모리에 0x555555... 를 썻다는 의미 4byte = 32bit
	set_time.Minutes = atoi(min); //minute
	set_time.Seconds = atoi(ss);

	ds_time.year = atoi(yy);
	ds_time.month = atoi(mm);
	ds_time.date = atoi(dd);
	ds_time.hour = atoi(hh); //플래시메모리에 0x555555... 를 썻다는 의미 4byte = 32bit
	ds_time.minutes = atoi(min); //minute
	ds_time.seconds = atoi(ss);

	ds1302_gpio_init();
	ds1302_init_time_date();
	flash_write((uint32_t *) &set_time, sizeof(set_time)); //student 구조체 내에 할당 받은 값을 플래시 메모리에 쓰기

}

void flash_set_time(void)
{


	t_set_time *read_set_time;

	flash_read_value = *(__IO uint32_t *) USER_DATA_ADDRESS; //사용자가 쓰고자하는 uint32 타입의 주소를 read_value에 담음 0x806 = sector7

	if (flash_read_value == FLASH_INIT_STATUS)  // 초기에 아무런 데이터도 존재 하지 않을 경우
	{
		flash_erase(); //1개 sector만 지우는 동작 //쓰레기 값 방지하기 위해 한 번 더 erase
printf("flash EMPTY!!!\n");
		set_time.magic=0x55555555; //사용자 임의 지정 값
		set_time.Hours=11; //플래시메모리에 0x555555... 를 썻다는 의미 4byte = 32bit
		set_time.Minutes=30; //minute
		set_time.Seconds=00;
		flash_write((uint32_t *) &set_time, sizeof(set_time)); //student 구조체 내에 할당 받은 값을 플래시 메모리에 쓰기
	}
	else   // 1번 이상 flash memory에 데이터를 write 한 경우
	{
		flash_read((uint32_t *)&set_time, sizeof(set_time));

		printf("magic: %08x\n", set_time.magic);
		printf("Hours: %02d\n", set_time.Hours);
		printf("Minutes: %02d\n", set_time.Minutes);
		printf("Seconds: %02d\n",set_time.Seconds);
	}

}

HAL_StatusTypeDef flash_write(uint32_t *data32, int size) //data32는 주소를 저장하는 공간으로 현재 주소인 806이 넘어온다.
{
	uint32_t *mem32 = data32; //*를 치는 순간 806의 내용을 가지고 오는 것임.

  /* Unlock to control */
  HAL_FLASH_Unlock();

  uint32_t Address = FLASH_USER_START_ADDR;

  printf("t_student size: %d\n", size);

  /* Writing data to flash memory */
  for (int i=0; i < size; )
  {
	  if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, *mem32) == HAL_OK)
	  {//1word write 후 mem32를 증가 시킨다.
		  printf("mem32: %0x\n", *mem32);
		  mem32++;
		  Address = Address + 4;
		  i += 4; //4byte 이므로 +4를 수행
	  }
	  else
	  {
		  uint32_t errorcode = HAL_FLASH_GetError();
		  return HAL_ERROR;
	  }
  }
  /* Lock flash control register */
  HAL_FLASH_Lock();

  return HAL_OK;
}

HAL_StatusTypeDef flash_read(uint32_t *addr32, int size)
{
  uint32_t *data32 = addr32; //*data32는 주소를 저장
  uint32_t address = FLASH_USER_START_ADDR;
  uint32_t end_address = FLASH_USER_START_ADDR + size;

  while(address < end_address)
  {
    *data32 = *(uint32_t*) address;
    data32++;
    address = address + 4;
  }

  return HAL_OK;

}


HAL_StatusTypeDef flash_erase()
{
	uint32_t SectorError = 0;

	/* Unlock to control */
	HAL_FLASH_Unlock();

	/* Calculate sector index */
	uint32_t UserSector = 7;     // sector 번호
	uint32_t NbOfSectors = 1;    // sector 수

	/* Erase sectors */
	FLASH_EraseInitTypeDef EraseInitStruct;
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector = UserSector;
	EraseInitStruct.NbSectors = NbOfSectors;

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
	{
		uint32_t errorcode = HAL_FLASH_GetError();
		return HAL_ERROR;
	}

	/* Lock flash control register */
	HAL_FLASH_Lock();

	return HAL_OK;
}
