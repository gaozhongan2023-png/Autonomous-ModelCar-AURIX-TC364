#include "math.h"
#include "Motor.h"
#include "stdlib.h"
#include "string.h"
#include "headfile.h"
#include "common.h"
#include "HCI.h"
motor motor_l,motor_r; //定义电机参数
pid S3010,RS_380; //定义舵机和电机控制参数
float kp_a;
char  MID_flag=0;
float h_speed;//直道最高速度
float l_speed;//弯道最低速度
char l_transpose=0;
char r_transpose=0;
char stopflag = 1;
int16 l_filter_speed = 0;
int16 r_filter_speed = 0;
int16 l_real_speed = 0;
int16 r_real_speed = 0;
int16 l_l_filter_speed = 0;
int16 l_r_filter_speed = 0;
extern uint8 GarageFlag;

/*************************************************
函数名称: motor_init()
函数功能: 舵机、电机、编码器初始化
调用函数: 无
入口参数: 无
出口参数: 无
返回值:   无
其他:     无
***********************************************/
void motor_init(void)  //舵机、电机、编码器初始化
{
//    PWM_Config(41,271);
    RS_380.kp_value = 500 * 0.01;  //用于将人机交互中的参数转换为自己的用到的
    RS_380.ti_value = 10 * 0.01;
    RS_380.td_value = 0 * 0.01;
}

/*************************************************
函数名称: void motor_error(void)
函数功能: 电机偏差数据计算
调用函数: 无
入口参数: 无
出口参数: 无
返回值:   无
其他:     放在5ms定时中断里面
***********************************************/
void motor_error(void)
{
    static int16 l_real_speed[3] = {0};
    static int16 r_real_speed[3] = {0};
    float a[3]={0.1,0.3,0.6};
    int16 l_speed_sum = 0;
    int16 r_speed_sum = 0;

    uint8 i;
    for(i = 0; i<2; i++)
    {
        l_real_speed[i] = l_real_speed[i+1];
        r_real_speed[i] = r_real_speed[i+1];
    }
    l_real_speed[2] = motor_l.real_speed;
    r_real_speed[2] = motor_r.real_speed;
    for(i = 0; i < 3; i++)
    {
        l_speed_sum += l_real_speed[i]*a[i];
        r_speed_sum += r_real_speed[i]*a[i];
    }
    l_filter_speed =  l_speed_sum;
    r_filter_speed =  r_speed_sum;


    static int l_current_error = 0;  //左电机
    static int l_last_error = 0;
    static int l_last_last_error = 0;

    static int r_current_error = 0;  //右电机
    static int r_last_error = 0;
    static int r_last_last_error = 0;

    l_current_error = motor_l.set_speed - l_filter_speed;  //****************************************8
    r_current_error = motor_r.set_speed - r_filter_speed;

    //增量式PID控制
    //左电机
    motor_l.kp_error = l_current_error - l_last_error;
    motor_l.ti_error = l_current_error;
    motor_l.td_error = l_current_error - 2*l_last_error + l_last_last_error;
    //右电机
    motor_r.kp_error = r_current_error - r_last_error;
    motor_r.ti_error = r_current_error;
    motor_r.td_error = r_current_error - 2*r_last_error + r_last_last_error;

    if(GarageFlag)
    {
        //计算  左电机PID
           motor_l.PID_OUT = RS_380.kp_value * motor_l.kp_error * 2 + RS_380.ti_value * motor_l.ti_error + RS_380.td_value * motor_l.td_error;
           motor_l.real_duty = (int)(motor_l.real_duty + motor_l.PID_OUT);

           //计算  右电机PID
           motor_r.PID_OUT = RS_380.kp_value * motor_r.kp_error * 2 + RS_380.ti_value * motor_r.ti_error + RS_380.td_value * motor_r.td_error;
           motor_r.real_duty = (int)(motor_r.real_duty + motor_r.PID_OUT);
    }
    else
    {
        //计算  左电机PID
        motor_l.PID_OUT = RS_380.kp_value * motor_l.kp_error + RS_380.ti_value * motor_l.ti_error + RS_380.td_value * motor_l.td_error;
        motor_l.real_duty = (int)(motor_l.real_duty + motor_l.PID_OUT);

        //计算  右电机PID
        motor_r.PID_OUT = RS_380.kp_value * motor_r.kp_error + RS_380.ti_value * motor_r.ti_error + RS_380.td_value * motor_r.td_error;
        motor_r.real_duty = (int)(motor_r.real_duty + motor_r.PID_OUT);
    }
//    ips114_showint32(190,0,motor_l.real_duty,4);
//    ips114_showint32(190,1,motor_r.real_duty,4);
//    ips114_showfloat(190,1,RS_380.kp_value,4,1);
    //计算左电机
    l_last_last_error = l_last_error;
    l_last_error = l_current_error;
    //计算右电机
    r_last_last_error = r_last_error;
    r_last_error = r_current_error;
//      a = RS_380.kp_value * motor_l.kp_error;
}


/*************************************************
函数名称: void motor_control(void)
函数功能: 电机偏差数据计算
调用函数: 无
入口参数: 无
出口参数: 无
返回值:   无
其他:     占空比范围为0-10000 即0对应占空比为0  5000对应占空比50%
           放在5ms定时中断里面
***********************************************/
void motor_control(void)
{
    int setspeed_l, setspeed_r;
    char l_transpose, r_transpose;

    if(motor_l.real_duty < 0)  //左轮反转设置
    {
        setspeed_l = 0 - 5 * stopflag * motor_l.real_duty;   //stopflag 默认为0  因为智能车平时不需要电机反转 刹车时令 stopflag=1 可以制动刹车（刹车效果较强）
        l_transpose = 1;
    }
    else
    {
        l_transpose=0;
        setspeed_l = motor_l.real_duty;
    }

    if(setspeed_l > 8000)  //设置左轮上限
    {
        setspeed_l = 8000;
    }

     if(motor_r.real_duty < 0)  //右轮反转设置
     {
              setspeed_r = 0 - 5 * stopflag * motor_r.real_duty;
              r_transpose = 1;
     }
     else
     {
              setspeed_r = motor_r.real_duty;
              r_transpose=0;
     }


    if(setspeed_r > 8000)  //设置右轮上限
    {
        setspeed_r = 8000;
    }

      if(l_transpose == 1)
      {
          pwm_duty(ATOM0_CH4_P02_4, setspeed_l);               //左电机反转
          pwm_duty(ATOM0_CH3_P21_5, 0);
      }
      else
      {
            pwm_duty(ATOM0_CH4_P02_4, 0);                        //左电机正转
            pwm_duty(ATOM0_CH3_P21_5, setspeed_l);
      }

      if(r_transpose == 1)
      {
            pwm_duty(ATOM0_CH0_P21_2, setspeed_r);                        //右电机反转
            pwm_duty(ATOM0_CH1_P21_3, 0);
      }
      else
      {
            pwm_duty(ATOM0_CH0_P21_2, 0);                //右电机正转
            pwm_duty(ATOM0_CH1_P21_3, setspeed_r);
      }
}


