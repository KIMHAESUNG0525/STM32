#include "main.h"
#include "button.h"
#include "stepmotor.h"


extern void up_arrow_display(void);
extern void down_arrow_display(void);
extern void dotmatrix_off();
extern void forward_step(void);
extern void backward_step(void);
extern void stop_motor(void);
extern void set_rpm(int rpm);
extern void delay_us(unsigned int us);
extern void ELEVATOR_BUZZER();
extern void ELEVATOR_FND(void);
extern void floor_dot_f1();
extern void floor_dot_f2();
extern void floor_dot_f3();
extern void floor_dot_f4();
extern void servo_motor_control(void);
extern void siren(int siren);
extern void noTone();
extern unsigned char display_data[8];

extern volatile int TIM10_1ms_counter;
extern volatile int TIM10_servo_motor_counter;
extern volatile int TIM10_1ms_counter1;


extern int step;
extern int current_floor;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim1;
extern uint8_t up[];
extern uint8_t down[];
extern int servo_step;
int function_state;
extern uint8_t servo_state;

enum {STOP, FORWARD, BACKWARD};
enum {DOT_OFF, UP_ARROW1, DOWN_ARROW1, f_1F, f_2F, f_3F, f_4F};

typedef enum {
    floor_1 = 1,
    floor_2,
    floor_3,
    floor_4
} Floor;


typedef enum {
    ELEVATOR_IDLE,
	ELEVATOR_FORWARD,
	ELEVATOR_BACKWARD
} ElevatorState;
















