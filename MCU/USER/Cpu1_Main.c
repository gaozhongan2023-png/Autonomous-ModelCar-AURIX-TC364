/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2020,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：三群：824575535
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file            main
 * @company         成都逐飞科技有限公司
 * @author          逐飞科技(QQ3184284598)
 * @version         查看doc内version文件 版本说明
 * @Software        ADS v1.2.2
 * @Target core     TC364DP
 * @Taobao          https://seekfree.taobao.com/
 * @date            2020-11-23
 ********************************************************************************************************************/

#include "headfile.h"
#include "HCI.h"
#include "IPS.h"
#include "Key.h"
#include "Motor.h"
#include "Track.h"
#include "PreDeal.h"
#include "ESP8266.h"
#pragma section all "cpu1_dsram"
//将本语句与#pragma section all restore语句之间的全局变量都放在CPU1的RAM中

extern uint8 Thro;
//int8 Error;
uint16 nowtime;
uint8 EspFlag;
int16 C1Num=0;
extern uint8 RampFlag;
extern uint8 NoRampFlag;
extern uint8 NoLowFlag;
extern int8 NowError;

extern uint8 CopyFlag;
extern uint8 Mark[4];
extern uint16 SpeedUpFlag;
extern int16 left_sum;
extern int16 ex_v1;
extern int16 ex_v2;
extern float RightCos;
extern uint8 RightTurn[4][3];

extern uint16 nowtime;
extern float speed;
extern float nowspeed;

extern motor motor_l,motor_r;
//
//extern uint16 time;
extern uint8 Cpu0InitFlag;
//
//extern uint8 LeftLine[256][2];
//extern uint8 RightLine[256][2];
extern uint8 MiddleLine[256][2];
//extern uint8 LeftTurn[4][3];
//extern uint8 RightTurn[4][3];
extern uint8 MiddleTurn[4][3];
//
//extern float LeftK[8];
//extern float RightK[8];
//
//extern uint32 mid_speed;
//extern uint32 res6;
//extern uint32 PD_P;
//
extern struct imageInformation imgInfo;

extern uint8 LeftLine1[256][2];
extern uint8 RightLine1[256][2];
extern uint8 MiddleLine1[256][2];
extern uint8 MidLine[256][2];
extern uint8 LeftTurn1[4][3];
extern uint8 RightTurn1[4][3];
extern uint8 TempLine11[256][2];
extern uint8 TempLine21[256][2];
extern int8 Error;
extern int8 preError;
extern int8 CircleCount;
extern uint16 Steer;
extern uint8 ThreeForkCount;
extern uint8 BadThreeForkFlag;
extern uint8 CircleFlag;
extern uint16 ThreeForkTime;
extern uint16 BadCrossTime;

extern struct imageInformation imgInfo1;
extern float LeftK1[8];
extern float RightK1[8];
extern uint8 RightTFlag;
extern uint8 ThreeForkFlag;
extern uint8 LeftTFlag;
extern uint8 PD_Plan;
extern uint8 GarageCount;
extern float Error_P;
extern uint16 SpeedUpFlag;
extern uint16 ChangeCount;
uint8 C2Num[2];
extern uint32 stop_time;
extern uint16 StopFlag;
extern uint32 gpt_sum;

extern int8 preError;

extern float Gyro_x;
extern float Gyro_y;
extern float Gyro_z;

extern int8 CircleMean[50];
extern uint8 CircleMeanCount;
extern uint8 SpeedUpCount;
extern uint8 ThreeForkOutFlag;
extern uint16 RampCount;

extern float LeftCos;
extern float RightCos;

int32 sumnum;

uint8 ad_sum[100]={0,};
uint8 ad_sample=0;

void core1_main(void)
{
    disableInterrupts();
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    //用户在此处调用各种初始化函数等

    while(!Cpu0InitFlag);

    //等待所有核心初始化完毕
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 0xFFFF);
    enableInterrupts();

    while(Cpu0InitFlag!=2);

    for(uint8 i=0;i<100;i++)
    {
        data_conversion(100, 0, 0, 0, virtual_scope_data);//使用虚拟示波器数据转换函数转换数据，至多同时转换四个数据
        seekfree_wireless_send_buff(virtual_scope_data, 10); //数据转换完成后，使用无线串口发送函数将数组的内容发送出去
    }

    while (TRUE)
    {
//        ips114_showint32(190,0,motor_l.real_speed,4);
//        ips114_showint32(190,1,motor_r.real_speed,4);
//        ips114_showint32(190,2,motor_l.set_speed,4);
//        ips114_showint32(190,3,motor_r.set_speed,4);
//
        if(CircleMeanCount)
        {
            sumnum = 0;
            for(uint8 i=0;i<20;i++)
            {
                sumnum += CircleMean[i];
            }
            sumnum/=20;
        }

//        data_conversion(speed*1000,imgInfo.MidLineCount ,stop_time, StopFlag+imgInfo.StopFlag*100 , virtual_scope_data);
        data_conversion(Error+imgInfo.StopFlag*100,imgInfo.nextroadType*10,(LeftTFlag+RightTFlag+CircleFlag+ThreeForkFlag+RampFlag)*10+ThreeForkOutFlag*5, CircleCount, virtual_scope_data);

        seekfree_wireless_send_buff(virtual_scope_data, 10); //数据转换完成后，使用无线串口发送函数将数组的内容发送出去

//        if(SpeedUpCount>50) gpio_set(P02_6, 0);
//        else gpio_set(P02_6, 1);

//        uart_putchar(UART_3,Mark[0]);

//        ips114_showfloat(190,4,speed,4,3);
//        ips114_showfloat(190,5,((float)(s_a_value%100))/1000,4,3);
//        ips114_showfloat(190,4,speed,4,3);

        if(!NoLowFlag && RampFlag<=1 && Gyro_x>150) RampFlag = 2;
        if(RampFlag==2)
        {
            if(Gyro_x<-50)RampFlag = 3;
        }
        if(RampFlag==3)
        {
            if(Gyro_x>150)
            {
                RampFlag = 4;

            }
        }
        if(RampFlag==4)
        {
            if(Gyro_x<20)
            {
                RampFlag = 0;
            }
            imgInfo.nextroadType = Straight;
        }

        ad_sample = 250;
        for(uint8 i=20;i>0;i--)
        {
            ad_sum[i] = ad_sum[i-1];
            if(ad_sample>ad_sum[i]) ad_sample = ad_sum[i];
        }
        ad_sum[0] = adc_convert(ADC_8, ADC8_CH12_A44, ADC_12BIT)/20;
        if(ad_sample>ad_sum[0]) ad_sample = ad_sum[0];

        if(!NoRampFlag && ad_sample>60 && ad_sample<200 && !RampFlag &&  RampCount>300)
        {
            RampFlag = 1;
            RampCount = 0;
        }


//        ad_sample = adc_convert(ADC_8, ADC8_CH12_A44, ADC_12BIT)/20;
    }
}



#pragma section all restore
