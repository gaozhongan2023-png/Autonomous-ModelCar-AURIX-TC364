#ifndef PARM_H
#define PARM_H
#include "common.h"

//-----------------------------------摄像头参数--------------------------------------//
extern uint8 label_max;                     //连通域的个数
extern uint8 location_x;
extern uint8 location_y;                    //信标位置y坐标
extern uint8 find_light;

//-----------------------------------速度参数--------------------------------------//
extern uint16 RUN_SPEED;                                  //行驶时的基础速度
extern uint16 TURN_SPEED;                                  //原地找灯速度
extern uint16 TURN_FACTOR;                                     //左右轮速差比例系数

extern int left_pwm;
extern int right_pwm;

extern int last_left_pwm;                        //上次左轮pwm
extern int last_right_pwm;                       //上次右轮pwm

extern int left_want_speed;
extern int right_want_speed;

extern int sum_error_max;
extern int sum_error_min;

extern int temp_left;
extern int temp_right;

//-----------------------------------速度控制参数--------------------------------------//
#define PWM_MIN -9999
#define PWM_MAX 9999

extern int Speed_pid_p,Speed_pid_i,Speed_pid_d;
extern float left_error,right_error;
extern float last_left_error,last_right_error;
extern float previous_left_error,previous_right_error;
extern float sum_left_error,sum_right_error;

//-----------------------------------充电参数--------------------------------------//
extern uint16 power;
extern uint16 last_power;
extern uint16 pre_power;
extern uint8 power_flag;
extern uint8 charge_flag;

extern uint8 low_flag;                              //慢行状态标志，当y坐标大到一定值时进入慢性状态
extern uint8 stop_flag;                             //停止标志，当在充电时停止运行
extern uint16 fall;                                 //自增量，当进入慢行状态时刻起便开始自增，若加到指定值时还没有触发停止状态，说明本次充电失败
//-----------------------------------控制参数初始化--------------------------------------//
void parm_init(void);
#endif
