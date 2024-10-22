#include "ele.h"
#include "extint.h"

void button_check0(void);
void button_check1(void);
void button_check2(void);
void ele_move(void);
void dot_floor(void);
void ELEVATOR_TASK2();
void door_open(void);

extern void servo_motor_control(void);

void(*fp_dot3[])() =
{
   dotmatrix_off,
   up_arrow_display,
   down_arrow_display,
   floor_dot_f1,
   floor_dot_f2,
   floor_dot_f3,
   floor_dot_f4
};

void(*fp_step2[])() =
{
   stop_motor,
   forward_step,
   backward_step
};

int dot3 = DOT_OFF;
int ele_step = STOP;
int ele_state = 0;
int floor_led;
int floor_state = 0;
int bt_stat = 0;

void button_check0(void)
{
    if(get_button(GPIOC, GPIO_PIN_0, BUTTON0) == BUTTON_PRESS)
    {

    	for(int i = 0; i < 4; i++)
    	{
    	    GPIOB->BSRR = (1 << (i + 16)); //Write Pin HW Direct Access
    	}
        switch(floor_led)
        {
            case 0:
            	*(unsigned int *) 0x40020414 |= 0x0001; //1층 LED
                floor_state = 1;
                break;
            case 1:
            	*(unsigned int *) 0x40020414 |= 0x0002; //2층 LED
                floor_state = 2;
                break;
            case 2:
            	*(unsigned int *) 0x40020414 |= 0x0004; //3층 LED
                floor_state = 3;
                break;
            case 3:
            	*(unsigned int *) 0x40020414 |= 0x0008; //4층 LED
                floor_state = 4;
                break;
        }
        floor_led = (floor_led + 1) % 4; // BTN0 floor Setting Cycle
    }
    fp_dot3[dot3]();
}

void button_check1(void)
{

   if(get_button(GPIOC, GPIO_PIN_1, BUTTON1) == BUTTON_PRESS)
   {
	  bt_stat = 1;
   }
}

void button_check2(void)
{

   if(get_button(GPIOC, GPIO_PIN_2, BUTTON2) == BUTTON_PRESS)
   {
	  bt_stat = 2;
   }
}


void dot_floor(void) //floor_ 1&2&3&4 포토 커플러가 인식한 실제 floor, 외부 Dot Matrix
{

	if(current_floor == floor_1)
	{
		dot3 = f_1F;
	}
	else if(current_floor == floor_2)
	{
		dot3 = f_2F;
	}
	else if(current_floor == floor_3)
	{
		dot3 = f_3F;
	}
	else if(current_floor == floor_4)
	{
		dot3 = f_4F;
	}
    switch(dot3) //외부 ELE의 위치 및 동작에 따른 Dot Matrix 동작 원리
    {
        case f_1F: //ELE가 현재 위치하는 층 , 1층
        	if((bt_stat == 1 || bt_stat == 2) && (floor_state == 2 || floor_state == 3 || floor_state == 4))
			{
				dot3 = UP_ARROW1;
			}
            break;

        case f_2F: //ELE가 현재 위치하는 층 , 2층
        	if(bt_stat == 1 && floor_state == 1)
			{
				dot3 = DOWN_ARROW1;
			}
        	if((bt_stat == 1 || bt_stat == 2) && (floor_state == 3 || floor_state == 4))
			{
				dot3 = UP_ARROW1;
			}
            break;
        case f_3F: //ELE가 현재 위치하는 층 , 3층
        	if((bt_stat == 1 || bt_stat == 2) && (floor_state == 1 || floor_state == 2))
			{
				dot3 = DOWN_ARROW1;
			}
        	if(bt_stat == 2 && floor_state == 4)
			{
				dot3 = UP_ARROW1;
			}

            break;
        case f_4F: //ELE가 현재 위치하는 층 , 4층
        	if((bt_stat == 1 || bt_stat == 2) && (floor_state ==1 ||floor_state == 2 || floor_state == 3))
			{
				dot3 = DOWN_ARROW1;
			}
            break;
        default:
            break;
    }
    fp_dot3[dot3]();
}


void ele_move(void) //외부 ELE 스텝모터 동작 함수
{
    switch (floor_state) //floor_state BTN0 으로 지정한 현재 층
    {
        case 1: //1층 동작
            if (current_floor == floor_1)
            {
                ele_step = STOP;
                bt_stat = 0;
            }
            else if (bt_stat == 1)
            {
                ele_step = FORWARD;  // 아래 방향
            }
            break;

        case 2: //2층 동작
            if (current_floor == floor_1 && (bt_stat == 1 || bt_stat == 2))
            {
                ele_step = BACKWARD;  // 위 방향
            }
            else if ((current_floor == floor_3 || current_floor == floor_4) && (bt_stat == 1 || bt_stat == 2))
            {
                ele_step = FORWARD;  // 아래 방향
            }
            else if (current_floor == floor_2)
            {
                ele_step = STOP;
                bt_stat = 0;
            }
            break;

        case 3://3층 동작
            if (current_floor == floor_4 && (bt_stat == 1 || bt_stat == 2))
            {
                ele_step = FORWARD;  // 아래 방향
            }
            else if ((current_floor == floor_1 || current_floor == floor_2) && (bt_stat == 1 || bt_stat == 2))
            {
                ele_step = BACKWARD;  // 위 방향
            }
            else if (current_floor == floor_3)
            {
                ele_step = STOP;
                bt_stat = 0;
            }
            break;

        case 4://4층 동작
            if (current_floor == floor_4)
            {
                ele_step = STOP;
                bt_stat = 0;
            }
            else if (bt_stat == 2)
            {
                ele_step = BACKWARD;  // 위 방향
            }
            break;
    }

    fp_step2[ele_step]();  // 모터 제어 함수 호출
}

void ELEVATOR_TASK2() //Demo Board Button이 Toggle 되기 전 동작 함수 = 외부 ELE 동작 , MAIN Thread에서 실행
{
	if(function_state == 0)
	{
		button_check0();
		button_check1();
		button_check2();

		servo_motor_control();
		dot_floor();
		ele_move();
	}
}


