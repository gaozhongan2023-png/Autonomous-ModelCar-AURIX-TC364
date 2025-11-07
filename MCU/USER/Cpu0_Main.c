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

#include "Cpu0_Main.h"
#include "headfile.h"
#include "HCI.h"
#include "IPS.h"
#include "Key.h"
#include "Track.h"
#include "PreDeal.h"
#include "Motor.h"
#pragma section all "cpu0_dsram"
//将本语句与#pragma section all restore语句之间的全局变量都放在CPU0的RAM中

//工程导入到软件之后，应该选中工程然后点击refresh刷新一下之后再编译

//工程默认设置为关闭优化，可以自己右击工程选择properties->C/C++ Build->Setting
//然后在右侧的窗口中找到C/C++ Compiler->Optimization->Optimization level处设置优化等级
//一般默认新建立的工程都会默认开2级优化，因此大家也可以设置为2级优化

//对于TC系列默认是不支持中断嵌套的，希望支持中断嵌套需要在中断内使用enableInterrupts();来开启中断嵌套
//简单点说实际上进入中断后TC系列的硬件自动调用了disableInterrupts();来拒绝响应任何的中断，因为需要我们自己手动调用enableInterrupts();来开启中断的响应。

extern uint16 nowtime;
extern uint16 time;
extern uint8 Thro;
extern int8 Error;
extern int8 NowError;
extern int8 CloseError;
extern uint8 RampFlag;
extern uint8 LowSpeedFlag;
extern uint8 SwitchFlag;
extern uint8 NoCircleFlag;

//uint8 Thro;
//int8 Error;
//uint16 nowtime;

extern uint8 LeftLine[256][2];
extern uint8 RightLine[256][2];
extern uint8 MiddleLine[256][2];
extern uint8 MidLine[256][2];
extern uint8 LeftTurn[4][3];
extern uint8 RightTurn[4][3];
extern uint8 TempLine1[256][2];
extern uint8 TempLine2[256][2];
uint8 testLine[120][2];

extern float LeftK[8];
extern float RightK[8];

extern uint8 CloseFlag;
extern uint8 EspFlag;
extern int8 Num[2];

extern struct imageInformation imgInfo;

extern uint32 SteerMid;

extern uint32 res1;
extern uint32 res2;
extern uint32 res3;
extern uint32 res4;
extern uint32 res5;
extern uint32 res6;
extern uint32 mid_speed;
extern uint32 high_speed;
extern uint32 PD_P;
extern uint32 b_value;
extern uint32 PD_D;

extern uint32 P_value;  //电机PID的P值
extern uint32 I_value;  //电机PID的I值
extern uint32 D_value;  //电机PID的D值

extern uint32 s_a_value;  //差速的a值
extern uint32 s_b_value;  //差速的b值
extern uint32 s_c_value;  //差速的c值

extern int Speed_pid_p, Speed_pid_i, Speed_pid_d;

extern uint8 turnpoint[3][2];

extern float PreError;
extern uint16 set_speed;
extern uint16 Steer;
extern uint8 middle;
extern int16 speed;
extern int16 ex_v1;
extern int16 ex_v2;

extern uint8 MiddleValue;
extern uint8 GarageFlag;
extern motor motor_l, motor_r;
extern pid RS_380;
extern uint8 ErrorBuffer[256];
extern uint8 StopFlag;
extern uint8 ThreeForkFlag;
extern uint8 CirCleOutFlag;
extern uint8 CircleFlag;
extern uint8 CrossFlag;
extern uint8 BadCrossFlag;
extern uint8 RightTFlag;
extern uint8 LeftTFlag;
extern float LeftCos, RightCos;
extern float increase;
extern float StraightFactor;
extern float StraightBuffer[11];
unsigned char mt9v03x_image_copy[60][94];
extern uint8 imageBin[MT9V03X_H][MT9V03X_W];

extern uint8 Mark[4];
extern uint16 BadCrossTime;
extern uint8 NoHeadFlag;
extern uint8 ThreeForkCount;
extern uint8 ThreeForkTime;

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

uint8 Cpu0InitFlag = 0;

extern int16 C1Num;
extern uint8 C2Num[2];
extern int8 Error5[5];
extern uint8 RepairFlag[2];
extern uint8 LeftMax;
extern uint8 RightMax;

extern float gpt_stop;
extern uint32 stop_time;

extern uint8 ThreeForkForesee[8];
extern uint8 LeftTForesee;
extern uint8 RightTForesee;
extern uint8 LeftCircleForesee;
extern uint8 RightCircleForesee;
extern uint8 LGForesee;
extern uint8 RGForesee;
extern uint8 ThreeForkOutFlag;
extern uint8 SpeedUpCount;
extern uint16 RampCount;

uint8 SceenFlag=0;

int core0_main(void)
{
    get_clk();//获取时钟频率  务必保留
    //用户在此处调用各种初始化函数等

    //目前的库采集总钻风 图像最后一列为固定的黑色
    //这是由于单片机造成的，不是摄像头的问题

    uart_init(UART_3, 115200, UART3_TX_P15_6, UART3_RX_P15_7);
    uart_putchar(UART_3, 0xfe);
    uart_putchar(UART_3, 0x10);

    icm20602_init_spi();

//    ESP8266_Init();

    adc_init(ADC_8, ADC8_CH15_A47);//初始化ADC0 通道0 使用A0引脚
    adc_init(ADC_8, ADC8_CH12_A44);//初始化ADC0 通道0 使用A0引脚
    gpio_init(P02_6, GPO, 0, OPENDRAIN);
    systick_delay_ms(STM0, 100);
    gpio_set(P02_6, 1);

//    gtm_pwm_init(ATOM0_CH7_P02_7, 100, 1522);//中1510，左1670，右，1350
    gtm_pwm_init(ATOM0_CH7_P02_7, 50, 1496/2);//中1510，左1670，右，1350

    ips114_init();  //初始化IPS屏幕
    mt9v03x_init(); //初始化摄像头
    ips114_showstr(0, 0, "SEEKFREE MT9V03x");
    ips114_showstr(0, 1, "Initializing...");

    gtm_pwm_init(ATOM0_CH0_P21_2, 10000, 0);
    gtm_pwm_init(ATOM0_CH1_P21_3, 10000, 0);
    gtm_pwm_init(ATOM0_CH4_P02_4, 10000, 0);
    gtm_pwm_init(ATOM0_CH3_P21_5, 10000, 0);

    gpt12_init(GPT12_T4, GPT12_T4INA_P02_8, GPT12_T4EUDA_P00_9);
    gpt12_init(GPT12_T6, GPT12_T6INA_P20_3, GPT12_T6EUDA_P20_0);

    Cpu0InitFlag=1;

    //等待所有核心初始化完毕
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 0xFFFF);
    enableInterrupts();

    Clr_Screen(8);

    HCI_init();

    HCI_handle();

    if(SWITCH_ON == SWITCH_Get_Input(SWITCH2))
    {
        ThreeForkForesee[0] = res1 / 100;
        ThreeForkForesee[1] = res1 % 100;
        ThreeForkForesee[2] = res2 / 100;
        ThreeForkForesee[3] = res2 % 100;
        LeftTForesee = res3 / 100;
        RightTForesee = res3 % 100;
        LeftCircleForesee = res4 / 100;
        RightCircleForesee = res4 % 100;
    }

    MiddleValue = 94;
    speed = (float)mid_speed/100;

    gpio_set(P02_6, 0);
    systick_delay_ms(STM0, 100);
    gpio_set(P02_6, 1);
    systick_delay_ms(STM0, 100);
    gpio_set(P02_6, 0);
    systick_delay_ms(STM0, 100);
    gpio_set(P02_6, 1);

    systick_delay_ms(STM0, 1000);

    gpio_set(P02_6, 0);
    systick_delay_ms(STM0, 400);
    gpio_set(P02_6, 1);

    ErrorInit();

    Cpu0InitFlag = 2;

    pit_interrupt_us(CCU6_0, PIT_CH0, 100);
    pit_interrupt_ms(CCU6_0, PIT_CH1, 5);
    pit_interrupt_ms(CCU6_1, PIT_CH0, 1);
    imgInfo.ThreeForkDir = 0;

    if(SWITCH_ON == SWITCH_Get_Input(SWITCH3))
    {
        imgInfo.NoHeadDir = 1;//左拐出库
    }
    else
    {
        imgInfo.NoHeadDir = 0;//右拐出库
    }

//出库！
    if(SWITCH_ON != SWITCH_Get_Input(SWITCH1))
    {
        imgInfo.roadType = NoHead;
        imgInfo.nextroadType = NoHead;
        SceenFlag = 1;
    }

//    imgInfo.roadType = Straight;
//    imgInfo.nextroadType = Straight;

    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_changeCpuWatchdogReload(IfxScuWdt_getCpuWatchdogPassword(), 0xF000); /* Set CPU0WD timer to ~1.3 sec */
    IfxScuWdt_enableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_clearCpuEndinit(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_serviceCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword()); // 喂狗

    while (TRUE)
    {
        if(mt9v03x_finish_flag)
        {
            mt9v03x_finish_flag = 0;
            Thro = ostu();
            Binarization(Thro);
            NearScan();
            GetK();
            GetMiddle();
            RoadJudge();
            NextRoad();
            GetMid();
            SpeedUp();
            LoseLine();


            C2Num[0] = LeftLine[imgInfo.LeftLineSum][1];
            C2Num[1] = RightLine[imgInfo.RightLineSum][1];
            Error = MiddleValue - ErrorSum();

            PreError = ((float)(Error5[4]+Error5[3]+Error5[2]+Error5[1]+Error5[0]))/5;
            Error5[4] = Error5[3];
            Error5[3] = Error5[2];
            Error5[2] = Error5[1];
            Error5[1] = Error5[0];
            Error5[0] = Error;
            NowError = ((float)(Error5[4]+Error5[3]+Error5[2]+Error5[1]+Error5[0]))/5;


            nowtime = time;
            time = 0;

            if(gpt_stop == 0)
            {
                IfxScuWdt_serviceCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword()); // 喂狗
            }


//            if(LeftTFlag == 2 || RightTFlag == 2)
//            {
//                Steer = (uint16)(1496 + (int16)Error*(int16)P2/10 + (NowError-PreError)*D2);
//            }
//            else if(LeftTFlag == 3 || RightTFlag == 3)
//            {
//                Steer = (uint16)(1496 + (int16)Error*(int16)P2/10 + (NowError-PreError)*D2);
//            }
////            else if(BadThreeForkFlag == 1)
////            {
////                Steer = (uint16)(1496 + (int16)Error*(int16)P3/10 + (NowError-PreError)*D3);
////            }
//            else
//            {
//                Steer = (uint16)(1496 + (int16)Error*(int16)PD_P/10 + (NowError-PreError)*PD_D);
//            }


            if(Error<0)//左
            {
                //1.7 33 15
                Steer = (uint16)(1496 + (int16)Error*(int16)P1/10 + (NowError-PreError)*D1);
            }
            else//右
            {
                //1.7 33 15
                Steer = (uint16)(1496 + (int16)Error*(int16)PD_P/10 + (NowError-PreError)*PD_D);
            }

            if(Steer>1665) Steer = 1665;
            else if(Steer<1340) Steer = 1340;

            pwm_duty(ATOM0_CH7_P02_7, Steer/2);

            if(RampCount<5005) RampCount++;
        }

        if(!SceenFlag)
        {
            create_Img(Thro);
            print_line(LeftLine[0],0x0010,imgInfo.LeftLineCount);
            print_line(RightLine[0],0x0010,imgInfo.RightLineCount);
            print_line(MidLine[0],0x0010,imgInfo.MidLineCount);

            if(KEY_PRESS == KEY_Get(KEY_UP))
            {
                print_big_point(LeftTurn[0], 0xd8a7);//红
                print_big_point(RightTurn[0], 0xd8a7);
            }
            else if(KEY_PRESS == KEY_Get(KEY_LEFT))
            {
                print_big_point(LeftTurn[1], 0x07ef);//绿
                print_big_point(RightTurn[1], 0x07ef);
            }
            else if(KEY_PRESS == KEY_Get(KEY_RIGHT))
            {
                print_big_point(LeftTurn[2], 0x1c9f);//蓝
                print_big_point(RightTurn[2], 0x1c9f);
            }
            else
            {
                print_big_point(LeftTurn[1], 0x07ef);//绿
                print_big_point(RightTurn[1], 0x07ef);
                print_big_point(LeftTurn[2], 0x1c9f);//蓝
                print_big_point(RightTurn[2], 0x1c9f);
                print_big_point(LeftTurn[0], 0xd8a7);//红
                print_big_point(RightTurn[0], 0xd8a7);
            }

            print_big_line1(TempLine1[0],0xd8a7,0);
            print_big_line1(TempLine2[0],0x1c9f,0);
            for(uint8 i1=0;i1<120;i1++)
            {
                testLine[i1][0] = i1;
                testLine[i1][1] = 94;
            }
            print_line(testLine[0],0x0010,120);

            print_Img_color();

            ips114_showint32(190,0, Error,4);
            ips114_showint32(190,1, LeftTFlag+RightTFlag+CircleFlag+NoHeadFlag+ThreeForkFlag,4);

            ips114_showint32(190,2,LeftTurn[0][0],4);
            ips114_showint32(190,3,RightTurn[0][0],4);

            ips114_showfloat(190,4,LeftCos,4,3);
            ips114_showfloat(190,5,RightCos,4,3);

//            ips114_showint32(190,4,RightMax,4);
//            ips114_showint32(190,5,RightTurn[0][0],4);

            print_road_type(190,6,imgInfo.roadType);
            print_road_type(190,7,imgInfo.nextroadType);

//            ips114_showint32(190,0,ThreeForkForesee[0],4);
//            ips114_showint32(190,1,ThreeForkForesee[1],4);
//            ips114_showint32(190,2,ThreeForkForesee[2],4);
//            ips114_showint32(190,3,ThreeForkForesee[3],4);
//            ips114_showint32(190,4,LeftTForesee,4);
//            ips114_showint32(190,5,RightTForesee,4);

//            ips114_showfloat(190,4,((float)(s_a_value/100))/1000,4,3);
//            ips114_showfloat(190,5,((float)(s_a_value%100))/1000,4,3);

        }
    }
}

#pragma section all restore
