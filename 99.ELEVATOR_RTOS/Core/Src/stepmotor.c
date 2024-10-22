#include "ele.h"
#include "button.h"

void STEPMOTOR_DRIVE(int step);
void BUTTON0_CHECK();
void BUTTON1_CHECK();
void BUTTON2_CHECK();
void BUTTON3_CHECK();
void MOVE_ELEVATOR();
void ELEVATOR_TASK();
void servo_motor_main(void);
void servo_motor_control(void);
void stepmotor_driver(int step);
void forward_step(void);
void backward_step(void);
void stop_motor(void);
void dot_dis_main(void);
void demo_check();

extern int function_state;

ElevatorState current_state = ELEVATOR_IDLE;
int step = STOP;
Floor target_floor;
int count = 0;

extern void(*fp_step[])();
extern void(*fp_dot2[])();


void set_rpm(int rpm) // rpm : 1~13
{
	delay_us(60000000/4096/rpm);
	//최대 speed 기준 (13) : delay_us(1126)
}


void BUTTON0_CHECK() { //내부 ELE Floor 1 BTN
    if (get_button(GPIOC, GPIO_PIN_0, BUTTON0) == BUTTON_PRESS)
    {

        target_floor = floor_1;  // 1층을 목표로 설정
        if (current_floor == floor_1)
        {
            set_rpm(0);  // 1층에 이미 있으면 정지
        }
        else
        {
            current_state = ELEVATOR_BACKWARD;  // 더 높은 층에 있으면 내려감
        }

    }
}


void demo_check()
{
    if(get_button(GPIOC, GPIO_PIN_13, BUTTON13) == BUTTON_PRESS)
    {
        count++;

        if(count == 2)
        {
            siren(5);
        }
    }
}


void BUTTON1_CHECK() { //내부 ELE Floor 2 BTN
    if (get_button(GPIOC, GPIO_PIN_1, BUTTON1) == BUTTON_PRESS)
    {
        target_floor = floor_2;  // 2층을 목표로 설정
        if (current_floor == floor_2)
        {
            set_rpm(0);  // 2층에 이미 있으면 정지
        }
        else if (current_floor < floor_2)
        {
            current_state = ELEVATOR_FORWARD;  // 더 낮은 층에 있으면 올라감
        }
        else
        {
            current_state = ELEVATOR_BACKWARD;  // 더 높은 층에 있으면 내려감
        }
    }
}

void BUTTON2_CHECK() { //내부 ELE Floor 3 BTN
    if (get_button(GPIOC, GPIO_PIN_2, BUTTON2) == BUTTON_PRESS)
    {
        target_floor = floor_3;  // 3층을 목표로 설정
        if (current_floor == floor_3)
        {
            set_rpm(0);  // 3층에 이미 있으면 정지
        }
        else if (current_floor < floor_3)
        {
            current_state = ELEVATOR_FORWARD;  // 더 낮은 층에 있으면 올라감
        }
        else
        {
            current_state = ELEVATOR_BACKWARD;  // 더 높은 층에 있으면 내려감
        }
    }
}

void BUTTON3_CHECK() { //내부 ELE Floor 4 BTN
    if (get_button(GPIOC, GPIO_PIN_3, BUTTON3) == BUTTON_PRESS)
    {

        target_floor = floor_4;  // 4층을 목표로 설정
        if (current_floor == floor_4)
        {
            set_rpm(0);  // 4층에 이미 있으면 정지
        }
        else
        {
            current_state = ELEVATOR_FORWARD;  // 더 낮은 층에 있으면 올라감
        }
    }
}

void MOVE_ELEVATOR() {
    switch (current_state)
    {
        case ELEVATOR_IDLE:
            set_rpm(0);

            break;
        case ELEVATOR_FORWARD:
            if (current_floor == target_floor)
            {
            	current_state = ELEVATOR_IDLE;
            	ELEVATOR_BUZZER();

            	servo_step = 1;
            	function_state = !function_state;
            	if(servo_step = 1)
					servo_step = 5;
            }
            else
            {
            	STEPMOTOR_DRIVE(ELEVATOR_FORWARD);  // 계속 위로 이동
                set_rpm(13);
            }
            break;
        case ELEVATOR_BACKWARD:
            if (current_floor == target_floor)
            {
            	current_state = ELEVATOR_IDLE;  // 목표층에 도달하면 정지
            	ELEVATOR_BUZZER();

            	servo_step = 1;
				function_state = !function_state;
				if(servo_step = 1)
					servo_step = 5;
            }
            else
            {
            	STEPMOTOR_DRIVE(ELEVATOR_BACKWARD);  // 계속 아래로 이동
                set_rpm(13);
            }
            break;
    }
}


void ELEVATOR_TASK()
{
	if(function_state == 1)
	{
    BUTTON0_CHECK();
    BUTTON1_CHECK();
    BUTTON2_CHECK();
    BUTTON3_CHECK();
    demo_check();
    MOVE_ELEVATOR();
	}
}


void STEPMOTOR_DRIVE(int direction)
{
	static int step= 0;

	switch(step)
	{
		 	case 0:
		 		HAL_GPIO_WritePin(GPIOC, IN1_Pin, 1);
		 		HAL_GPIO_WritePin(GPIOC, IN2_Pin, 0);
		 		HAL_GPIO_WritePin(GPIOC, IN3_Pin, 0);
		 		HAL_GPIO_WritePin(GPIOC, IN4_Pin, 0);
		 		break;
		 	case 1:
		 		HAL_GPIO_WritePin(GPIOC, IN1_Pin, 1);
		 		HAL_GPIO_WritePin(GPIOC, IN2_Pin, 1);
		 		HAL_GPIO_WritePin(GPIOC, IN3_Pin, 0);
		 		HAL_GPIO_WritePin(GPIOC, IN4_Pin, 0);
		 		break;
		 	case 2:
		 		HAL_GPIO_WritePin(GPIOC, IN1_Pin, 0);
		 		HAL_GPIO_WritePin(GPIOC, IN2_Pin, 1);
		 		HAL_GPIO_WritePin(GPIOC, IN3_Pin, 0);
		 		HAL_GPIO_WritePin(GPIOC, IN4_Pin, 0);
		 		break;
		 	case 3:
		 		HAL_GPIO_WritePin(GPIOC, IN1_Pin, 0);
		 		HAL_GPIO_WritePin(GPIOC, IN2_Pin, 1);
		 		HAL_GPIO_WritePin(GPIOC, IN3_Pin, 1);
		 		HAL_GPIO_WritePin(GPIOC, IN4_Pin, 0);
		 		break;
		 	case 4:
		 		HAL_GPIO_WritePin(GPIOC, IN1_Pin, 0);
		 		HAL_GPIO_WritePin(GPIOC, IN2_Pin, 0);
		 		HAL_GPIO_WritePin(GPIOC, IN3_Pin, 1);
		 		HAL_GPIO_WritePin(GPIOC, IN4_Pin, 0);
		 		break;
		 	case 5:
		 		HAL_GPIO_WritePin(GPIOC, IN1_Pin, 0);
		 		HAL_GPIO_WritePin(GPIOC, IN2_Pin, 0);
		 		HAL_GPIO_WritePin(GPIOC, IN3_Pin, 1);
		 		HAL_GPIO_WritePin(GPIOC, IN4_Pin, 1);
		 		break;
		 	case 6:
		 		HAL_GPIO_WritePin(GPIOC, IN1_Pin, 0);
		 		HAL_GPIO_WritePin(GPIOC, IN2_Pin, 0);
		 		HAL_GPIO_WritePin(GPIOC, IN3_Pin, 0);
		 		HAL_GPIO_WritePin(GPIOC, IN4_Pin, 1);
		 		break;
		 	case 7:
		 		HAL_GPIO_WritePin(GPIOC, IN1_Pin, 1);
		 		HAL_GPIO_WritePin(GPIOC, IN2_Pin, 0);
		 		HAL_GPIO_WritePin(GPIOC, IN3_Pin, 0);
		 		HAL_GPIO_WritePin(GPIOC, IN4_Pin, 1);
		 		break;
	}

	if (direction == ELEVATOR_BACKWARD)
	{
		step++;
		step %= 8; //다음진행할 step을 준비
	}
	if (direction == ELEVATOR_FORWARD)
	{
		step--;
		if (step < 0)
				step += 8;
	}
}
void stop_motor(void)
{
    HAL_GPIO_WritePin(GPIOC, IN1_Pin, 0);
    HAL_GPIO_WritePin(GPIOC, IN2_Pin, 0);
    HAL_GPIO_WritePin(GPIOC, IN3_Pin, 0);
    HAL_GPIO_WritePin(GPIOC, IN4_Pin, 0);
}

void forward_step(void)
{
	static int j=0;
	if(TIM10_1ms_counter>0)
	{
	  TIM10_1ms_counter=0;
	  stepmotor_driver(j);
	  j++;
	}
	j%=8;
}

void backward_step(void)
{
	static int j=8;
	if(j==0)
	{
	  j=8;
	}
	if(TIM10_1ms_counter>0)
	{
	  TIM10_1ms_counter=0;

	  stepmotor_driver(j);
	  j--;
	}
}

void stepmotor_driver(int direction)
{
	switch(direction)
	{
		case 0:
			HAL_GPIO_WritePin(GPIOC, IN1_Pin, 1);
			HAL_GPIO_WritePin(GPIOC, IN2_Pin, 0);
			HAL_GPIO_WritePin(GPIOC, IN3_Pin, 0);
			HAL_GPIO_WritePin(GPIOC, IN4_Pin, 0);
			break;
		case 1:
			HAL_GPIO_WritePin(GPIOC, IN1_Pin, 1);
			HAL_GPIO_WritePin(GPIOC, IN2_Pin, 1);
			HAL_GPIO_WritePin(GPIOC, IN3_Pin, 0);
			HAL_GPIO_WritePin(GPIOC, IN4_Pin, 0);
			break;
		case 2:
			HAL_GPIO_WritePin(GPIOC, IN1_Pin, 0);
			HAL_GPIO_WritePin(GPIOC, IN2_Pin, 1);
			HAL_GPIO_WritePin(GPIOC, IN3_Pin, 0);
			HAL_GPIO_WritePin(GPIOC, IN4_Pin, 0);
			break;
		case 3:
			HAL_GPIO_WritePin(GPIOC, IN1_Pin, 0);
			HAL_GPIO_WritePin(GPIOC, IN2_Pin, 1);
			HAL_GPIO_WritePin(GPIOC, IN3_Pin, 1);
			HAL_GPIO_WritePin(GPIOC, IN4_Pin, 0);
			break;
		case 4:
			HAL_GPIO_WritePin(GPIOC, IN1_Pin, 0);
			HAL_GPIO_WritePin(GPIOC, IN2_Pin, 0);
			HAL_GPIO_WritePin(GPIOC, IN3_Pin, 1);
			HAL_GPIO_WritePin(GPIOC, IN4_Pin, 0);
			break;
		case 5:
			HAL_GPIO_WritePin(GPIOC, IN1_Pin, 0);
			HAL_GPIO_WritePin(GPIOC, IN2_Pin, 0);
			HAL_GPIO_WritePin(GPIOC, IN3_Pin, 1);
			HAL_GPIO_WritePin(GPIOC, IN4_Pin, 1);
			break;
		case 6:
			HAL_GPIO_WritePin(GPIOC, IN1_Pin, 0);
			HAL_GPIO_WritePin(GPIOC, IN2_Pin, 0);
			HAL_GPIO_WritePin(GPIOC, IN3_Pin, 0);
			HAL_GPIO_WritePin(GPIOC, IN4_Pin, 1);
			break;
		case 7:
			HAL_GPIO_WritePin(GPIOC, IN1_Pin, 1);
			HAL_GPIO_WritePin(GPIOC, IN2_Pin, 0);
			HAL_GPIO_WritePin(GPIOC, IN3_Pin, 0);
			HAL_GPIO_WritePin(GPIOC, IN4_Pin, 1);
			break;
		}
}
