#include "common.h"

//-----------------------------------摄像头参数--------------------------------------//
uint8 label_max;                     //连通域的个数
uint8 location_x;                    //信标位置x坐标
uint8 location_y;                    //信标位置y坐标
uint8 find_light;                    //置1表明当前图像中有灯

//-----------------------------------速度参数--------------------------------------//
int left_pwm;                        //左轮pwm
int right_pwm;                       //右轮pwm

int last_left_pwm;                        //上次左轮pwm
int last_right_pwm;                       //上次右轮pwm

int left_want_speed;                 //左轮预期速度
int right_want_speed;                //右轮预期速度

int sum_error_max;                   //累计误差最大值
int sum_error_min;                   //累计误差最小值

int temp_left;                       //左轮编码器获得值
int temp_right;                      //右轮编码器获得值



uint16 RUN_SPEED;
uint16 TURN_SPEED;
uint16 TURN_FACTOR;

//-----------------------------------速度控制参数--------------------------------------//
int Speed_pid_p,Speed_pid_i,Speed_pid_d;
float left_error,right_error;
float last_left_error,last_right_error;
float previous_left_error,previous_right_error;
float sum_left_error,sum_right_error;


//-----------------------------------充电参数--------------------------------------//
uint16 power;                                //电量，1200为12V
uint16 last_power;
uint16 pre_power;
uint8 power_flag;                            //起跑标志，仅改变一次，电充好后由0变为1，之后不再变化
uint8 charge_flag;                           //中途充电标志，当电压下降到一定值时由0变1，车开始准备在下一个灯充电，当电充到规定值时才由1变0

uint8 low_flag;                              //慢行状态标志，当y坐标大到一定值时进入慢性状态
uint8 stop_flag;                             //停止标志，当在充电时停止运行
uint16 fall;                                 //自增量，当进入慢行状态时刻起便开始自增，若加到指定值时还没有触发停止状态，说明本次充电失败
//-----------------------------------控制参数初始化--------------------------------------//
void parm_init(void)
{
    left_want_speed = 0;                                       //设置左轮预期速度
    right_want_speed = 0;                                      //设置右轮预期速度

    left_pwm = 0;                                               //左轮PWM
    right_pwm = 0;                                              //右轮PWM

    last_left_pwm = 0;
    last_right_pwm = 0;

    temp_left = 0;                                              //左轮编码器初始化
    temp_right = 0;                                             //右轮编码器初始化

    sum_error_max = 100;                                        //积累误差最大值
    sum_error_min = -50;                                        //积累误差最小值

    power = 0;
}

