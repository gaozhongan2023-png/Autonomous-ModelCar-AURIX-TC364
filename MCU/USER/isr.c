/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2020,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：三群：824575535
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file            isr
 * @company         成都逐飞科技有限公司
 * @author          逐飞科技(QQ3184284598)
 * @version         查看doc内version文件 版本说明
 * @Software        ADS v1.2.2
 * @Target core     TC364DP
 * @Taobao          https://seekfree.taobao.com/
 * @date            2020-11-23
 ********************************************************************************************************************/

#include "isr_config.h"
#include "isr.h"
#include "Key.h"
#include "HCI.h"
#include "IPS.h"
#include "Track.h"
#include "PreDeal.h"
#include "Motor.h"

uint16 time;
extern uint8 Thro;
int8 Error;
int8 Error5[5];
int8 CloseError;
//uint16 nowtime;
uint8 CopyFlag;
uint8 IcmFlag=0;
uint32 gpt_sum=0;
extern uint8 RampFlag;
extern uint8 NoRampFlag;
extern int16 C1Num;

extern uint8 MiddleValue;
extern uint8 GarageFlag;
extern motor motor_l, motor_r;
extern pid RS_380;
extern uint8 ErrorBuffer[256];
extern uint16 StopFlag;
extern uint8 ThreeForkFlag;
extern uint8 CirCleOutFlag;
extern uint8 CircleFlag;
extern uint8 LeftTFlag;
extern float LeftCos, RightCos;

extern uint8 LeftLine[256][2];
extern uint8 RightLine[256][2];
extern uint8 MiddleLine[256][2];
extern uint8 MidLine[256][2];
extern uint8 LeftTurn[4][3];
extern uint8 RightTurn[4][3];
extern uint8 MiddleTurn[4][3];
extern uint8 TempLine1[256][2];
extern uint8 TempLine2[256][2];

extern float LeftK[8];
extern float RightK[8];

extern uint8 CloseFlag;
extern uint8 RightTFlag;
extern uint8 EspFlag;
extern uint8 ThreeForkCount;

extern struct imageInformation imgInfo;

extern uint32 SteerMid;

extern uint32 res4;
extern uint32 res5;
extern uint32 res6;
extern uint32 mid_speed;
extern uint32 high_speed;
extern uint32 PD_P;
extern uint32 b_value;
extern uint32 PD_D;

extern uint32 s_a_value;  //差速的a值
extern uint32 s_b_value;  //差速的b值
extern uint32 s_c_value;  //差速的c值

extern uint32 P_value;  //电机PID的P值
extern uint32 I_value;  //电机PID的I值
extern uint32 D_value;  //电机PID的D值

extern uint8 turnpoint[3][2];


uint8 LeftLine1[256][2] = {{0,0},};
uint8 RightLine1[256][2] = {{0,0},};
uint8 MiddleLine1[256][2] = {{0,0},};
uint8 LeftTurn1[4][3] = {{0,0,0},};
uint8 RightTurn1[4][3] = {{0,0,0},};
uint8 TempLine11[256][2] = {{0,0},};
uint8 TempLine21[256][2] = {{0,0},};
struct imageInformation imgInfo1;
float LeftK1[8];
float RightK1[8];

extern uint8 LeftLine1[256][2];
extern uint8 RightLine1[256][2];
extern uint8 MiddleLine1[256][2];
extern uint8 LeftTurn1[4][3];
extern uint8 RightTurn1[4][3];
extern uint8 TempLine11[256][2];
extern uint8 TempLine21[256][2];
extern uint8 MiddleTurn[4][3];

extern struct imageInformation imgInfo1;
extern float LeftK1[8];
extern float RightK1[8];
extern uint8 NoHeadFlag;
extern uint8 TurnFlag;
extern uint8 BadCrossFlag;
extern uint8 RightTFlag;
extern uint8 C2Num[2];

extern uint8 P1;
extern uint8 P2;
extern uint8 P3;
extern uint8 P4;
extern uint8 P5;
extern uint8 P6;
extern uint8 D1;
extern uint8 D2;
extern uint8 D3;
extern uint8 D4;
extern uint8 D5;
extern uint8 D6;

extern int Speed_pid_p,Speed_pid_i,Speed_pid_d;

float Error_P=0;
float CloseError_P=0;

float left_gpt_flag;
float right_gpt_flag;
float gpt_stop = 0;
float gpt_start = 0;

uint8 PD_Plan;
extern uint8 P0;
extern uint8 D0;

extern uint8 SwitchFlag;
extern uint8 SpeedUpCount;

float speed;
float nowspeed;
float increase = 0;

extern uint32 stop_time;
uint32 nohead_time = 0;
uint16 SpeedUpFlag=0;

float gpt_out = 0;
int16 left_sum = 0;
int16 right_sum = 0;
float out_limit_r = 6000;
float out_limit_l = 6000;

int LeftTurnPID(int countl);
int RightTurnPID(int countr);

LeftTurnPID(int countl)
{
    float result_l;

    result_l = -countl * 12.856 + 1026.760;
    return result_l;
}

RightTurnPID(int countr)
{
    float result_r;

    result_r = -countr * 12.902 + 1018.612;
    return result_r;
}

float StraightBuffer[11] = {0.99,0.98,0.95,0.88,0.73,0.5,0.73,0.88,0.95,0.98,0.99};
float StraightFactor = 1;

float PreError;
float NowError;
uint16 set_speed;
uint16 Steer = 1490;
uint8 middle;
uint16 icmtime;
int16 ex_v1;
int16 ex_v2;
extern uint8 Mark[4];


//PIT中断函数  示例
IFX_INTERRUPT(cc60_pit_ch0_isr, CCU6_0_CH0_INT_VECTAB_NUM, CCU6_0_CH0_ISR_PRIORITY)
{
    enableInterrupts();//开启中断嵌套
    PIT_CLEAR_FLAG(CCU6_0, PIT_CH0);

    icmtime++;
    time++;
}


IFX_INTERRUPT(cc60_pit_ch1_isr, CCU6_0_CH1_INT_VECTAB_NUM, CCU6_0_CH1_ISR_PRIORITY)
{
    enableInterrupts();//开启中断嵌套
    PIT_CLEAR_FLAG(CCU6_0, PIT_CH1);
    CopyFlag = 0;

    if(imgInfo.StopFlag && stop_time<1000) stop_time++;
    if(imgInfo.StopFlag && StopFlag>25) stop_time = 200;
    if(SWITCH_ON != SWITCH_Get_Input(SWITCH1))
    {
        if(imgInfo.nextroadType == NoHead)
        {
            speed = 0.16;
        }
        else if(ThreeForkFlag==1)
        {
            speed = 0.14;
        }
//        else if(!NoRampFlag && (RampFlag==1||RampFlag==2))
//        {
//            speed = 0.12;
//            RS_380.kp_value = (float)25000 * 0.01;  //用于将人机交互中的参数转换为自己的用到的
//            RS_380.ti_value = (float)1000 * 0.01;
//            RS_380.td_value = (float)10000 * 0.01;
//        }
        else if(!NoRampFlag && RampFlag==1)
        {
//            speed = 0.10;
            speed = ((float)(s_a_value%100))/1000;
            RS_380.kp_value = (float)25000 * 0.01;  //用于将人机交互中的参数转换为自己的用到的
            RS_380.ti_value = (float)1000 * 0.01;
            RS_380.td_value = (float)10000 * 0.01;
        }
        else if(RampFlag==2 || RampFlag==3)
        {
            speed = ((float)(s_a_value/100))/1000;
        }
        else if((!imgInfo.NoHeadDir&&LeftTFlag>=10)||(imgInfo.NoHeadDir&&RightTFlag>=10))
        {
            speed = 0.07;
        }
        else if(LeftTFlag==3 || RightTFlag==3)//断头配速
        {
            speed = (float)low_speed/100;
        }
        else if(LeftTFlag==4 || RightTFlag==4)//断头配速
        {
            speed = 0.08;
        }
        else if(LeftTFlag==5 || RightTFlag==5 || LeftTFlag==6 || RightTFlag==6)
        {
            speed = 0.14;
        }
        else
        {
            if(SpeedUpCount>50)
            {
                if(speed<(float)mid_speed/100+(float)(SpeedUpCount-50)/50*((float)high_speed-mid_speed)/100)
                {
                    speed+=0.003;
                }
                else if(speed>(float)mid_speed / 100)
                {
                    speed-=0.01;
                }
                else if (speed<=(float)mid_speed / 100) speed = (float)mid_speed / 100;
            }
            else speed = (float)mid_speed / 100;
        }

//        if(SWITCH_ON != SWITCH_Get_Input(SWITCH2))
//        {
//            //速度控制
//            if(imgInfo.nextroadType == NoHead)
//            {
//                speed = 0.16;
//            }
//            else if(ThreeForkCount < 1)
//            {
//                speed = 0.21;
//            }
//            else if(ThreeForkCount == 4)
//            {
//                speed = (float)high_speed / 100;
//            }
//            else if(ThreeForkCount==1&&!RampFlag)
//            {
//                speed = ((float)(s_a_value/100))/1000;
//                IcmFlag = 1;
//            }
//            else if(RampFlag==1||RampFlag==2)
//            {
//                speed = ((float)(s_a_value%100))/1000;
//                RS_380.kp_value = (float)25000 * 0.01;  //用于将人机交互中的参数转换为自己的用到的
//                RS_380.ti_value = (float)1000 * 0.01;
//                RS_380.td_value = (float)10000 * 0.01;
//            }
//            else if(LeftTFlag==10)
//            {
//                speed = 0.07;
//            }
//            else if(LeftTFlag==3 || RightTFlag==3)//断头配速
//            {
//                speed = (float)mid_speed / 100;
//            }
//            else if(LeftTFlag==4 || RightTFlag==4)//断头配速
//            {
//                speed = 0.12;
//            }
//            else if(LeftTFlag==5 || RightTFlag==5 || LeftTFlag==6 || RightTFlag==6)
//            {
//                speed = ((float)low_speed*0.2+(float)mid_speed*0.8)/100;
//            }
//            else if(SwitchFlag==1 && !LeftTFlag)
//            {
//                speed = (float)high_speed / 100;
//            }
//            else
//            {
//                speed = (float)mid_speed / 100;
//            }
//        }
//        else
//        {
//            if(imgInfo.nextroadType == NoHead)
//            {
//                speed = 0.17;
//            }
//            else if(ThreeForkCount==1&&!RampFlag)
//            {
//                speed = 0.14;
//                IcmFlag = 1;
//            }
//            else if(RampFlag==1||RampFlag==2)
//            {
//                speed = 0.08;
//                RS_380.kp_value = (float)25000 * 0.01;  //用于将人机交互中的参数转换为自己的用到的
//                RS_380.ti_value = (float)1000 * 0.01;
//                RS_380.td_value = (float)10000 * 0.01;
//            }
//            else if(LeftTFlag==10)
//            {
//                speed = 0.07;
//            }
//            else if(LeftTFlag==3 || RightTFlag==3)//断头配速
//            {
//                speed = 0.17;
//            }
//            else if(LeftTFlag==4 || RightTFlag==4)//断头配速
//            {
//                speed = 0.12;
//            }
//            else if(LeftTFlag==5 || RightTFlag==5 || LeftTFlag==6 || RightTFlag==6)
//            {
//                speed = 0.14;
//            }
//            else speed = 0.17;
//        }



        if(SWITCH_ON == SWITCH_Get_Input(SWITCH4) && !imgInfo.StopFlag)
        {
            nowspeed = speed;
            if(gpt_start<120 && imgInfo.nextroadType != NoHead) gpt_start++;

            if(Steer >= 1490)
            {
                motor_l.set_speed = (int)(speed * LeftTurnPID((Steer - 1490) / 10));
                motor_r.set_speed = (int)(1000 * speed);
            }
            else
            {
                motor_l.set_speed = (int)(1000 * speed);
                motor_r.set_speed = (int)(speed * RightTurnPID((1490 - Steer) / 10));
            }

            motor_l.real_speed = gpt12_get(GPT12_T6);
            motor_r.real_speed = 0 - gpt12_get(GPT12_T4);

            gpt_sum += motor_l.real_speed + motor_r.real_speed;

            if(gpt_start >= 100)
            {
                if(RampFlag!=1)
                {
                    RS_380.kp_value = (float)Speed_pid_p * 0.01;  //用于将人机交互中的参数转换为自己的用到的
                    RS_380.ti_value = (float)Speed_pid_i * 0.01;
                    RS_380.td_value = (float)Speed_pid_d * 0.01;
                }

                if(!RampFlag && motor_l.set_speed-motor_l.real_speed >= 60)
                {
                    left_gpt_flag++;
                }
                else
                {
                    left_gpt_flag = 0;
                }

                if(!RampFlag && motor_r.set_speed-motor_r.real_speed >= 60)
                {
                    right_gpt_flag++;
                }
                else
                {
                   right_gpt_flag = 0;
                }

                if(left_gpt_flag>=75 || right_gpt_flag>=75)
                {
                    gpt_stop = 1;
                }
            }
            else
            {
                RS_380.kp_value = (float)5000 * 0.01;  //用于将人机交互中的参数转换为自己的用到的
                RS_380.ti_value = (float)50 * 0.01;
                RS_380.td_value = (float)1000 * 0.01;
            }

            if(NoHeadFlag == 2)
            {
                if(gpt_out == 0)
                {
                    left_sum = left_sum + motor_l.real_speed;
                    right_sum = right_sum + motor_r.real_speed;
                }

                if((left_sum>=out_limit_l && !imgInfo.NoHeadDir)||(right_sum>=out_limit_r && !imgInfo.NoHeadDir))
                {
                    gpt_out =1;
                    left_sum = 0;
                    right_sum = 0;
                    NoHeadFlag = 0;
                    gpt_sum=0;
                    imgInfo.nextroadType = Straight;
                }
            }


            gpt12_clear(GPT12_T4);
            gpt12_clear(GPT12_T6);
            motor_error();
            motor_control();
        }
        else
        {
//            if(imgInfo.StopFlag && stop_time<82)
//            {
//                motor_r.set_speed = 100;
//                motor_l.set_speed = 100;
//            }
//            else
            if(imgInfo.StopFlag && stop_time>120)
            {
                motor_r.set_speed = 0;
                motor_l.set_speed = 0;
                if(SWITCH_ON == SWITCH_Get_Input(SWITCH4)) gpt_stop = 1;
            }

            if(imgInfo.StopFlag && stop_time>240)
            {
                pwm_duty(ATOM0_CH0_P21_2, 0);
                pwm_duty(ATOM0_CH1_P21_3, 0);
                pwm_duty(ATOM0_CH4_P02_4, 0);
                pwm_duty(ATOM0_CH3_P21_5, 0);
            }
            else
            {
                RS_380.ti_value = 400 * 0.01;
                motor_l.real_speed = gpt12_get(GPT12_T6);
                motor_r.real_speed = 0 - gpt12_get(GPT12_T4);
                gpt12_clear(GPT12_T4);
                gpt12_clear(GPT12_T6);
                motor_error();
                motor_control();
            }
        }

//        if(SWITCH_ON == SWITCH_Get_Input(SWITCH3))
//        {
//            imgInfo.NoHeadDir = 1;//左拐出库
//        }
//        else
//        {
//            imgInfo.NoHeadDir = 0;//右拐出库
//        }

//        nowtime = time;
    }
}

IFX_INTERRUPT(cc61_pit_ch0_isr, CCU6_1_CH0_INT_VECTAB_NUM, CCU6_1_CH0_ISR_PRIORITY)
{
    //陀螺仪中断
    enableInterrupts();//开启中断嵌套
    PIT_CLEAR_FLAG(CCU6_1, PIT_CH0);
//    if(RampFlag>2) IcmFlag = 0;
    static uint8 count = 0;
    count++;
    if(count >=10)//每10ms读取一次数据`
    {
        count = 0;
//        if(IcmFlag) icmGetValues();
        icmGetValues();
    }
}

IFX_INTERRUPT(cc61_pit_ch1_isr, CCU6_1_CH1_INT_VECTAB_NUM, CCU6_1_CH1_ISR_PRIORITY)
{
    enableInterrupts();//开启中断嵌套
    PIT_CLEAR_FLAG(CCU6_1, PIT_CH1);

}

IFX_INTERRUPT(eru_ch0_ch4_isr, ERU_CH0_CH4_INT_VECTAB_NUM, ERU_CH0_CH4_INT_PRIO)
{
    enableInterrupts();//开启中断嵌套
    if(GET_GPIO_FLAG(ERU_CH0_REQ4_P10_7))//通道0中断
    {
        CLEAR_GPIO_FLAG(ERU_CH0_REQ4_P10_7);
    }

    if(GET_GPIO_FLAG(ERU_CH4_REQ13_P15_5))//通道4中断
    {
        CLEAR_GPIO_FLAG(ERU_CH4_REQ13_P15_5);
    }
}

IFX_INTERRUPT(eru_ch1_ch5_isr, ERU_CH1_CH5_INT_VECTAB_NUM, ERU_CH1_CH5_INT_PRIO)
{
    enableInterrupts();//开启中断嵌套
    if(GET_GPIO_FLAG(ERU_CH1_REQ5_P10_8))//通道1中断
    {
        CLEAR_GPIO_FLAG(ERU_CH1_REQ5_P10_8);
    }

    if(GET_GPIO_FLAG(ERU_CH5_REQ1_P15_8))//通道5中断
    {
        CLEAR_GPIO_FLAG(ERU_CH5_REQ1_P15_8);
    }
}

//由于摄像头pclk引脚默认占用了 2通道，用于触发DMA，因此这里不再定义中断函数
//IFX_INTERRUPT(eru_ch2_ch6_isr, 0, ERU_CH2_CH6_INT_PRIO)
//{
//  enableInterrupts();//开启中断嵌套
//  if(GET_GPIO_FLAG(ERU_CH2_REQ7_P00_4))//通道2中断
//  {
//      CLEAR_GPIO_FLAG(ERU_CH2_REQ7_P00_4);
//
//  }
//  if(GET_GPIO_FLAG(ERU_CH6_REQ9_P20_0))//通道6中断
//  {
//      CLEAR_GPIO_FLAG(ERU_CH6_REQ9_P20_0);
//
//  }
//}

IFX_INTERRUPT(eru_ch3_ch7_isr, ERU_CH3_CH7_INT_VECTAB_NUM, ERU_CH3_CH7_INT_PRIO)
{
    enableInterrupts();//开启中断嵌套
    if(GET_GPIO_FLAG(ERU_CH3_REQ6_P02_0))//通道3中断
    {
        CLEAR_GPIO_FLAG(ERU_CH3_REQ6_P02_0);
        if      (CAMERA_GRAYSCALE == camera_type)   mt9v03x_vsync();
        else if (CAMERA_BIN_UART  == camera_type)   ov7725_uart_vsync();
        else if (CAMERA_BIN       == camera_type)   ov7725_vsync();

    }
    if(GET_GPIO_FLAG(ERU_CH7_REQ16_P15_1))//通道7中断
    {
        CLEAR_GPIO_FLAG(ERU_CH7_REQ16_P15_1);

    }
}

IFX_INTERRUPT(dma_ch5_isr, ERU_DMA_INT_VECTAB_NUM, ERU_DMA_INT_PRIO)
{
    enableInterrupts();//开启中断嵌套

    if      (CAMERA_GRAYSCALE == camera_type)   mt9v03x_dma();
    else if (CAMERA_BIN_UART  == camera_type)   ov7725_uart_dma();
    else if (CAMERA_BIN       == camera_type)   ov7725_dma();
}

//串口中断函数  示例
IFX_INTERRUPT(uart0_tx_isr, UART0_INT_VECTAB_NUM, UART0_TX_INT_PRIO)
{
    enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrTransmit(&uart0_handle);
}
IFX_INTERRUPT(uart0_rx_isr, UART0_INT_VECTAB_NUM, UART0_RX_INT_PRIO)
{
    enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrReceive(&uart0_handle);
}
IFX_INTERRUPT(uart0_er_isr, UART0_INT_VECTAB_NUM, UART0_ER_INT_PRIO)
{
    enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrError(&uart0_handle);
}

//串口1默认连接到摄像头配置串口
IFX_INTERRUPT(uart1_tx_isr, UART1_INT_VECTAB_NUM, UART1_TX_INT_PRIO)
{
    enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrTransmit(&uart1_handle);
}
IFX_INTERRUPT(uart1_rx_isr, UART1_INT_VECTAB_NUM, UART1_RX_INT_PRIO)
{
    enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrReceive(&uart1_handle);
    if      (CAMERA_GRAYSCALE == camera_type)   mt9v03x_uart_callback();
    else if (CAMERA_BIN_UART  == camera_type)   ov7725_uart_callback();
}
IFX_INTERRUPT(uart1_er_isr, UART1_INT_VECTAB_NUM, UART1_ER_INT_PRIO)
{
    enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrError(&uart1_handle);
}

//串口2默认连接到无线转串口模块
IFX_INTERRUPT(uart2_tx_isr, UART2_INT_VECTAB_NUM, UART2_TX_INT_PRIO)
{
    enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrTransmit(&uart2_handle);
}
IFX_INTERRUPT(uart2_rx_isr, UART2_INT_VECTAB_NUM, UART2_RX_INT_PRIO)
{
    enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrReceive(&uart2_handle);
    switch(wireless_type)
    {
        case WIRELESS_SI24R1:
        {
            wireless_uart_callback();
        }break;

        case WIRELESS_CH9141:
        {
            bluetooth_ch9141_uart_callback();
        }break;
        default:break;
    }
}
IFX_INTERRUPT(uart2_er_isr, UART2_INT_VECTAB_NUM, UART2_ER_INT_PRIO)
{
    enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrError(&uart2_handle);
}

IFX_INTERRUPT(uart3_tx_isr, UART3_INT_VECTAB_NUM, UART3_TX_INT_PRIO)
{
    enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrTransmit(&uart3_handle);
}
IFX_INTERRUPT(uart3_rx_isr, UART3_INT_VECTAB_NUM, UART3_RX_INT_PRIO)
{
    enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrReceive(&uart3_handle);
}
IFX_INTERRUPT(uart3_er_isr, UART3_INT_VECTAB_NUM, UART3_ER_INT_PRIO)
{
    enableInterrupts();//开启中断嵌套
    IfxAsclin_Asc_isrError(&uart3_handle);
}
