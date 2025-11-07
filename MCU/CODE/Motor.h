#ifndef _MOTOR_H
#define _MOTOR_H

#include "headfile.h"
#include "common.h"
//宏定义S3010通道参数
//#define S3010_TPM TPM2  //B3接舵机
//#define S3010_CH TPM_CH1
#define S3010_HZ 100
#define S3010_MID_DUTY 1350         //1310

#define RS_380_HZ 17*1000

//宏定义S3010极限参数
#define PD_OUT_DEATH 0.1   //后续修改？
#define PD_out_max 10      //后续修改？
#define S3010_OUT_MIN 1180 			//1130
#define S3010_OUT_MAX 1520 			//1490
#define S3010_OUT_MID S3010_MID_DUTY
#define motor_pid_out_max 500
#define motor_pid_out_min 100

typedef struct pid_control{
      float kp_value;  //P参数
      float ti_value;  //I参数
      float td_value;  //D参数
      float pid_out;
      float set_speed;
      float duty;
      float p_error;
      float d_error;
      float error[2];
}pid;
extern pid S3010,RS_380; //定义舵机和电机控制参数

extern float kp_a;
extern char stopflag;
typedef struct motor_control{

      uint16 set_speed;
//      float set_speed;
      int16 real_speed;

      float kp_error;
      float ti_error;
      float td_error;
      float PID_OUT;
      float real_duty;

}motor;
extern char  MID_flag;
extern motor motor_l,motor_r; //定义电机参数
extern float h_speed;
extern float l_speed;
extern int16 car_speed;
extern int16 l_filter_speed;
extern int16 r_filter_speed;
extern int16 l_real_speed;
extern int16 r_real_speed;
extern int32 r_s;
extern int32 l_s;
extern int32 S_s;
extern int32 a;
/******************函数声明**********************/
void motor_init(void);
void motor_error(void);
void motor_control(void);

#endif



