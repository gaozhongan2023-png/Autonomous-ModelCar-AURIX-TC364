/*****************************************************************************************
文件名称：  190215_1913_HCI.c
项目名称：  人机交互.c文件
版 本：     V1.0
说 明：     无
修改记录：  无（初版）
*****************************************************************************************/
#include "headfile.h"
#include "Key.h"
#include "IPS.h"
#include "HCI.h"
#include "parm.h"

extern uint16 time;
extern uint8 EspFlag;
extern uint8 ErrorBuffer[256];
extern uint8 ErrorPlan[20][24];
extern uint8 PlanNum;

uint8 ErrorTemp[16];

unsigned char statue_flag;  //状态标志位
unsigned char screen_num;  //界面标号
unsigned char position1_x;  //设定界面1光标x坐标
unsigned char position1_y;  //设定界面1光标y坐标
unsigned char position;  //设定界面2光标的位置
unsigned char move_flag;  //光标移动标志位

/*****************参数类型待确定********************/
uint32 mid_speed;  //中等速度
uint32 high_speed;  //高速
uint32 low_speed;  //低速

uint32 s_a_value;  //差速的a值
uint32 s_b_value;  //差速的b值
uint32 s_c_value;  //差速的c值
/****************************************************/

uint32 P_value;  //电机PID的P值
uint32 I_value;  //电机PID的I值
uint32 D_value;  //电机PID的D值

uint32 PD_P;  //舵机变P的二次函数中的a
uint32 b_value;  //舵机变P的二次函数中的b
uint32 PD_D;  //舵机PD的D值

uint32 res1;  //双峰法阈值最小值
uint32 res2;  //双峰法阈值最大值

/*********************预留参数***********************/
uint32 res3;
uint32 res4;
uint32 res5;
uint32 res6;

uint32 P1P2;
uint32 P3P4;
uint32 P5P6;
uint32 D1D2;
uint32 D3D4;
uint32 D5D6;

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

uint32 SteerMid;

unsigned char value[6] = {'\0'};  //存放正在修改的位数为5的变量
unsigned char speed_value[4] = {'\0'};  //存放正在修改的位数为3的变量
uint32 temp_value;//暂存数值

//用于暂存原来的数据
//暂存电机PID值
unsigned int uiTempPID_P;
unsigned int uiTempPID_I;
unsigned int uiTempPID_D;
//暂存舵机PD值
unsigned int uiTempPD_a;
unsigned int uiTempPD_b;
unsigned int uiTempPD_D;
//暂存中高低速参数
unsigned int uiTempSpeed;
unsigned int uiTempMaxSpeed;
unsigned int uiTempMinSpeed;
//暂存差速参数
unsigned int uiTempS_A;
unsigned int uiTempS_B;
unsigned int uiTempS_C;
//暂存阈值参数
unsigned int uiTempres1;
unsigned int uiTempres2;
//预留参数
unsigned int uiTempres3;
unsigned int uiTempres4;
unsigned int uiTempres5;
unsigned int uiTempres6;

unsigned int uiTempP1P2;
unsigned int uiTempP3P4;
unsigned int uiTempP5P6;
unsigned int uiTempD1D2;
unsigned int uiTempD3D4;
unsigned int uiTempD5D6;

/*************************************************
函数名称: value_init();
函数功能: 标志位、变量初始化
调用函数: 无
入口参数: 无
出口参数: 无
返回值:   无
其他:
***********************************************/
void value_init(void)  //标志位、变量初始化
{
    statue_flag = 0;  //状态标志位
    screen_num = 0;  //界面标号
    position1_x = 0;  //设定界面1光标x坐标
    position1_y = 0;  //设定界面1光标y坐标
    position = 0;  //设定界面2光标的位置
    move_flag = 0;  //光标移动标志位
}


/*************************************************
 函数名称: HCI_init()
 函数功能: 人机交互初始化
 调用函数: 无
 入口参数: 无
 出口参数: 无
 返回值:   无
 其他:     无
*************************************************/
void HCI_init(void)
{
    systick_delay_ms(STM0,5);  //初始化后必要的延时
    Clr_Screen(8);  //清屏
    KEY_Init(KEY_ALL);  //LED和按键初始化函数（调试用）
    SWITCH_Init(SWITCH_ALL);
    value_init();
}


void HCI_handle(void)
{
    data_flash_read();//读取flash中的数据

    //暂存电机PID值
    uiTempPID_P = Speed_pid_p;
    uiTempPID_I = Speed_pid_i;
    uiTempPID_D = Speed_pid_d;
    //暂存舵机PD值
    uiTempPD_a = PD_P;
    uiTempPD_b = b_value;
    uiTempPD_D = PD_D;
    //暂存中高低速参数
    uiTempSpeed = mid_speed;
    uiTempMaxSpeed = high_speed;
    uiTempMinSpeed = low_speed;
    //暂存差速参数
    uiTempS_A = s_a_value;
    uiTempS_B = s_b_value;
    uiTempS_C = s_c_value;

    //预留参数
    uiTempres1 = res1;
    uiTempres2 = res2;
    uiTempres3 = res3;
    uiTempres4 = res4;
    uiTempres5 = res5;
    uiTempres6 = res6;

    uiTempP1P2 = P1P2;
    uiTempP3P4 = P3P4;
    uiTempP5P6 = P5P6;
    uiTempD1D2 = D1D2;
    uiTempD3D4 = D3D4;
    uiTempD5D6 = D5D6;

    P1 = P1P2/100;
    P2 = P1P2%100;
    P3 = P3P4/100;
    P4 = P3P4%100;
    P5 = P5P6/100;
    P6 = P5P6%100;

    D1 = D1D2/100;
    D2 = D1D2%100;
    D3 = D3D4/100;
    D4 = D3D4%100;
    D5 = D5D6/100;
    D6 = D5D6%100;

    PlanNum = res6;

    while(KEY_Get_Input(KEY_BACK))
    {
        display_screen(screen_num);  //界面显示函数
        //切换界面键处理(上下键)
        if( 0 == KEY_Get_Input(KEY_DOWN))  //带延时消抖
        {
            clr_Screen(7,44,84);
            print_str8x16(7,44,1,"Shift");//反白显示shift键
            gpio_set(P02_6, 0);
            systick_delay_ms(STM0,50);
            gpio_set(P02_6, 1);
            Clr_Screen(8);

            if(5 == screen_num)  //4个主界面循环显示
            {
                screen_num = 0;
            }//end of if(3 == screen_num)
            else
            {
                screen_num++;
            }//end of else
        }//end of if( 0 == key_check(KEY_U))切换显示界面

        else if( 0 == KEY_Get_Input(KEY_UP))  //带延时消抖
        {

            clr_Screen(7,44,84);
            print_str8x16(7,44,1,"Shift");//反白显示shift键
            gpio_set(P02_6, 0);
            systick_delay_ms(STM0,50);
            gpio_set(P02_6, 1);
            Clr_Screen(8);

            if(0 == screen_num)  //4个主界面循环显示
            {
                screen_num = 5;
            }//end of if(3 == screen_num)
            else
            {
                screen_num--;
            }//end of else
        }

        //设定键处理
        else if(0 == KEY_Get_Input(KEY_SET))
        {
            clr_Screen(7,0,18);
            print_str8x16(7,0,1,"Set");  //反白显示Set
            gpio_set(P02_6, 0);
            systick_delay_ms(STM0,50);
            gpio_set(P02_6, 1);

            statue_flag = 1;//进入参数选择显示界面

            clr_Screen(7,0,128);
            print_str8x16(7,0,0,"Set");  //显示Set
            print_str8x16(7,96,0,"Exit");  //显示Exit
            select_display(screen_num,position1_x,position1_y,1);  //反白显示主界面0光标所在位置的参数

            while(0 != statue_flag)
            {

                while(1 == statue_flag)
                {
                    //选择显示处理
                    if(1 == move_flag)//如果光标移动了则更新显示
                    {
                        move_flag = 0;
                        select_display(screen_num,position1_x,position1_y,1);//更新显示移动后光标的位置
                    }//end of if(1 == statue_flag)

                    if(0 == KEY_Get_Input(KEY_UP))
                    {
                        select_display(screen_num,position1_x,position1_y,0);//清除移动前光标的反白
                        set1_up_process();
                    }//end of if(0 == key_check(KEY_U))

                    else if(0 == KEY_Get_Input(KEY_DOWN))
                    {
                        select_display(screen_num,position1_x,position1_y,0);//清除移动前光标的反白
                        set1_down_process();
                    }//end of else if(0 == key_check(KEY_D))

                    else if(0 == KEY_Get_Input(KEY_LEFT))
                    {
                        select_display(screen_num,position1_x,position1_y,0);//清除移动前光标的反白
                        set1_left_process();
                    }//end of else if(0 == key_check(KEY_L))

                    else if(0 == KEY_Get_Input(KEY_RIGHT))
                    {
                        select_display(screen_num,position1_x,position1_y,0);//清除移动前光标的反白
                        set1_right_process();
                    }//end of else if(0 == key_check(KEY_R))

                    //设定或退出处理
                    if(0 == KEY_Get_Input(KEY_SET))
                    {
                        set1_ld_process();
                    }//end of if(0 == key_check(KEY_R))

                    else if(0 == KEY_Get_Input(KEY_BACK))
                    {
                        set1_rd_process();
                    }//end of else if(0 == key_check(KEY_RD))

                }//end of while(1 == statue_flag)
                if(1 == screen_num)
                {
                    print_str8x16(0,40,0,".");
                    print_str8x16(2,40,0,".");
                    print_str8x16(4,40,0,".");
                }
                //具体参数设定处理
                while(2 == statue_flag)
                {
                    if(1 == move_flag)  //如果光标移动了则更新显示
                    {
                        renew_data(position1_x,position1_y);  //更新数据
                        move_flag = 0;
                        position_display(screen_num,position1_x,position1_y,position,1);
                    }//end of if(1 == move_flag)

                    if(0 == KEY_Get_Input(KEY_UP))
                    {
                        set2_up_process();
                    }//end of if(0 == key_check(KEY_U))

                    else if(0 == KEY_Get_Input(KEY_DOWN))
                    {
                        set2_down_process();
                    }//end of else if(0 == key_check(KEY_D))

                    else if(0 == KEY_Get_Input(KEY_LEFT))
                    {
                        position_display(screen_num,position1_x,position1_y,position,0);
                        set2_left_process();
                    }//end of else if(0 == key_check(KEY_L))

                    else if(0 == KEY_Get_Input(KEY_RIGHT))
                    {
                        position_display(screen_num,position1_x,position1_y,position,0);
                        set2_right_process();
                    }//end of else if(0 == key_check(KEY_R))

                    if(0 == KEY_Get_Input(KEY_SET))
                    {
                        set2_ld_process();
                    }//end of else if(0 == key_check(KEY_LD))

                    else if(0 == KEY_Get_Input(KEY_BACK))
                    {
                         set2_rd_process();
                    }//end of else if(0 == key_check(KEY_RD))

                }//end of while(2 == statue_flag)
            }//end of while(0 != statue_flag)
        }//end of else if(0 == key_check(KEY_R))设定键处理

        //起跑键设定
        else if(0 == KEY_Get_Input(KEY_BACK))
        {
            clr_Screen(7,104,112);
            print_str8x16(7,104,1,"Run");//反白显示Run键
            systick_delay_ms(STM0,50);
            Clr_Screen(8);
            break;
        }//end of else if(0 == key_check(KEY_RD))
    }//end of while(1)

    data_flash_read();  //获取人机交互最新的数据
/************************************************************************/
//    RS_380.kp_value = P_value;  //用于将人机交互中的参数转换为自己的用到的
//    RS_380.ti_value = I_value;
//    RS_380.td_value = D_value;

//    S3010.kp_value = PD_P * 0.001;
//    S3010.td_value = PD_D * 0.001;

//    RS_380.set_speed = mid_speed * 2.857;//oled上显示的速度转换成编码器数值
//    h_speed = high_speed * 2.857;//直道最高速度
//    l_speed = low_speed * 2.857;//弯道减速
/***********************************************************************/
}


void data_flash_read(void)  //读取flash中的数据
{
    Speed_pid_p = flash_read(SECTOR_NUM, 0, uint16);
    Speed_pid_i = flash_read(SECTOR_NUM, 1, uint16);
    Speed_pid_d = flash_read(SECTOR_NUM, 2, uint16);
    PD_P = flash_read(SECTOR_NUM, 3, uint16);
    b_value = flash_read(SECTOR_NUM, 4, uint16);
    PD_D = flash_read(SECTOR_NUM, 5, uint16);


    mid_speed = flash_read(SECTOR_NUM, 6, uint16);
    high_speed = flash_read(SECTOR_NUM, 7, uint16);
    low_speed = flash_read(SECTOR_NUM, 8, uint16);
    s_a_value = flash_read(SECTOR_NUM, 9, uint16);
    s_b_value = flash_read(SECTOR_NUM, 10, uint16);
    s_c_value = flash_read(SECTOR_NUM, 11, uint16);


    res1 = flash_read(SECTOR_NUM, 12, uint16);
    res2 = flash_read(SECTOR_NUM, 13, uint16);
    res3 = flash_read(SECTOR_NUM, 14, uint16);
    res4 = flash_read(SECTOR_NUM, 15, uint16);
    res5 = flash_read(SECTOR_NUM, 16, uint16);
    res6 = flash_read(SECTOR_NUM, 17, uint16);

    P1P2 = flash_read(SECTOR_NUM, 18, uint16);
    P3P4 = flash_read(SECTOR_NUM, 19, uint16);
    P5P6 = flash_read(SECTOR_NUM, 20, uint16);
    D1D2 = flash_read(SECTOR_NUM, 21, uint16);
    D3D4 = flash_read(SECTOR_NUM, 22, uint16);
    D5D6 = flash_read(SECTOR_NUM, 23, uint16);
}


void data_flash_write(void)//向flash中写入数据
{
    eeprom_erase_sector(SECTOR_NUM);  //擦除扇区,写入flash数据前，需要先擦除对应的扇区(不然数据会乱)
    eeprom_page_program(SECTOR_NUM, 0, &Speed_pid_p);
    eeprom_page_program(SECTOR_NUM, 1, &Speed_pid_i);
    eeprom_page_program(SECTOR_NUM, 2, &Speed_pid_d);
    eeprom_page_program(SECTOR_NUM, 3, &PD_P);
    eeprom_page_program(SECTOR_NUM, 4, &b_value);
    eeprom_page_program(SECTOR_NUM, 5, &PD_D);

    eeprom_page_program(SECTOR_NUM, 6, &mid_speed);
    eeprom_page_program(SECTOR_NUM, 7, &high_speed);
    eeprom_page_program(SECTOR_NUM, 8, &low_speed);
    eeprom_page_program(SECTOR_NUM, 9, &s_a_value);
    eeprom_page_program(SECTOR_NUM, 10, &s_b_value);
    eeprom_page_program(SECTOR_NUM, 11, &s_c_value);

    eeprom_page_program(SECTOR_NUM, 12, &res1);
    eeprom_page_program(SECTOR_NUM, 13, &res2);
    eeprom_page_program(SECTOR_NUM, 14, &res3);
    eeprom_page_program(SECTOR_NUM, 15, &res4);
    eeprom_page_program(SECTOR_NUM, 16, &res5);
    eeprom_page_program(SECTOR_NUM, 17, &res6);


    eeprom_page_program(SECTOR_NUM, 18, &P1P2);
    eeprom_page_program(SECTOR_NUM, 19, &P3P4);
    eeprom_page_program(SECTOR_NUM, 20, &P5P6);
    eeprom_page_program(SECTOR_NUM, 21, &D1D2);
    eeprom_page_program(SECTOR_NUM, 22, &D3D4);
    eeprom_page_program(SECTOR_NUM, 23, &D5D6);
}

void select_display(unsigned char page,unsigned char x,unsigned char y,unsigned char reverse)//参数选择界面显示程序
{
    if(0 == page)//主界面1
    {
        if( (0 == x)&&(0 == y) )  //反白显示PID的P值
        {
            clr_Screen(0,0,56);
            print_str8x16(0,0,reverse,"P:");
            show_data(0,16,reverse,Speed_pid_p);
        }//end of if( (0 == x)&&(0 == y) )
        else if( (0 == x)&&(1 == y) )  //反白显示PID的I值
        {
            clr_Screen(2,0,56);
            print_str8x16(2,0,reverse,"I:");
            show_data(2,16,reverse,Speed_pid_i);
        }//end of else if( (0 == x)&&(1 == y) )
        else if( (0 == x)&&(2 == y) )  //反白显示PID的D值
        {
            clr_Screen(4,0,56);
            print_str8x16(4,0,reverse,"D:");
            show_data(4,16,reverse,Speed_pid_d);
        }//end of else if( (0 == x)&&(2 == y) )
        else if( (1 == x)&&(0 == y) )  //反白显示变P的a
        {
            clr_Screen(0,72,128);
            print_str8x16(0,104,reverse,"P:");//72=7*8+2*8
            show_data(0,120,reverse,PD_P);//88=72+2*8
        }//end of else if( (1 == x)&&(0 == y) )
        else if( (1 == x)&&(1 == y) )  //反白显示变P的b
        {
            clr_Screen(2,72,128);
            print_str8x16(2,104,reverse,"b:");
            show_data(2,120,reverse,b_value);
        }//end of else if( (1 == x)&&(1 == y) )
        else if( (1 == x)&&(2 == y) )  //反白显示PD的D值
        {
            clr_Screen(4,72,128);
            print_str8x16(4,104,reverse,"D:");
            show_data(4,120,reverse,PD_D);
        }//end of else if( (1 == x)&&(2 == y) )
    }
    else if(1 == page)
    {
        if( (0 == x)&&(0 == y) )  //反白显示中等速度
        {
            clr_Screen(0,0,64);
            print_str8x16(0,0,reverse,"M_S:");
            show_data(0,32,reverse,mid_speed);  //显示小数函数
        }//end of if( (0 == x)&&(0 == y) )
        else if( (0 == x)&&(1 == y) )  //反白显示高速
        {
            clr_Screen(2,0,64);
            print_str8x16(2,0,reverse,"H_S:");
            show_data(2,32,reverse,high_speed);
        }//end of else if( (0 == x)&&(1 == y) )
        else if( (0 == x)&&(2 == y) )  //反白显示低速
        {
            clr_Screen(4,0,64);
            print_str8x16(4,0,reverse,"L_S:");
            show_data(4,32,reverse,low_speed);
        }//end of else if( (0 == x)&&(2 == y) )
        else if( (1 == x)&&(0 == y) )  //反白显示差速的a值
        {
            clr_Screen(0,64,128);
            print_str8x16(0,104,reverse,"a:");//72=7*8+2*8
            show_data(0,120,reverse,s_a_value);//88=72+2*8
        }//end of else if( (1 == x)&&(0 == y) )
        else if( (1 == x)&&(1 == y) )  //反白显示差速的b值
        {
            clr_Screen(2,64,128);
            print_str8x16(2,104,reverse,"b:");
            show_data(2,120,reverse,s_b_value);
        }//end of else if( (1 == x)&&(1 == y) )
        else if( (1 == x)&&(2 == y) )  //反白显示差速的c值
        {
            clr_Screen(4,64,128);
            print_str8x16(4,104,reverse,"c:");
            show_data(4,120,reverse,s_c_value);
        }//end of else if( (1 == x)&&(2 == y) )
    }
    else if(4 == page)
    {
        if( (0 == x)&&(0 == y) )  //反白显示预留参数1（根据需要自己修改）
        {
            clr_Screen(0,0,56);
            print_str8x16(0,0,reverse,"1:");
            show_data(0,16,reverse,res1);
        }//end of if( (0 == x)&&(0 == y) )
        else if( (0 == x)&&(1 == y) )  //反白显示预留参数2（根据需要自己修改）
        {
            clr_Screen(2,0,56);
            print_str8x16(2,0,reverse,"2:");
            show_data(2,16,reverse,res2);
        }//end of else if( (0 == x)&&(1 == y) )
        else if( (0 == x)&&(2 == y) )  //反白显示预留参数3（根据需要自己修改）
        {
            clr_Screen(4,0,56);
            print_str8x16(4,0,reverse,"3:");
            show_data(4,16,reverse,res3);
        }//end of else if( (0 == x)&&(2 == y) )
        else if( (1 == x)&&(0 == y) )  //反白显示预留参数4（根据需要自己修改）
        {
            clr_Screen(0,72,128);
            print_str8x16(0,104,reverse,"4:");//72=7*8+2*8
            show_data(0,120,reverse,res4);//88=72+2*8
        }//end of else if( (1 == x)&&(0 == y) )
        else if( (1 == x)&&(1 == y) )  //反白显示预留参数5（根据需要自己修改）
        {
            clr_Screen(2,56,128);
            print_str8x16(2,104,reverse,"5:");
            show_data(2,120,reverse,res5);
        }//end of else if( (1 == x)&&(1 == y) )
        else if( (1 == x)&&(2 == y) )  //反白显示预留参数6（根据需要自己修改）
        {
            clr_Screen(4,56,128);
            print_str8x16(4,104,reverse,"6:");
            show_data(4,120,reverse,res6);
        }//end of else if( (1 == x)&&(2 == y) )
    }
    else if(2 == page)
    {
        if( (0 == x)&&(0 == y) )  //反白显示预留参数1（根据需要自己修改）
        {
            clr_Screen(0,0,88);
            print_str8x16(0,0,reverse,"P1+P2:");
            show_data(0,48,reverse,P1P2);
        }//end of if( (0 == x)&&(0 == y) )
        else if( (0 == x)&&(1 == y) )  //反白显示预留参数2（根据需要自己修改）
        {
            clr_Screen(2,0,88);
            print_str8x16(2,0,reverse,"P3+P4:");
            show_data(2,48,reverse,P3P4);
        }//end of else if( (0 == x)&&(1 == y) )
        else if( (0 == x)&&(2 == y) )  //反白显示预留参数3（根据需要自己修改）
        {
            clr_Screen(4,0,88);
            print_str8x16(4,0,reverse,"P5+P6:");
            show_data(4,48,reverse,P5P6);
        }//end of else if( (0 == x)&&(2 == y) )
        else if( (1 == x)&&(0 == y) )  //反白显示预留参数4（根据需要自己修改）
        {
            clr_Screen(0,88,128);
            print_str8x16(0,128,reverse,"D1+D2:");//72=7*8+2*8
            show_data(0,176,reverse,D1D2);//88=72+2*8
        }//end of else if( (1 == x)&&(0 == y) )
        else if( (1 == x)&&(1 == y) )  //反白显示预留参数5（根据需要自己修改）
        {
            clr_Screen(2,88,128);
            print_str8x16(2,128,reverse,"D3+D4:");
            show_data(2,176,reverse,D3D4);
        }//end of else if( (1 == x)&&(1 == y) )
        else if( (1 == x)&&(2 == y) )  //反白显示预留参数6（根据需要自己修改）
        {
            clr_Screen(4,88,128);
            print_str8x16(4,128,reverse,"D5+D6:");
            show_data(4,176,reverse,D5D6);
        }//end of else if( (1 == x)&&(2 == y) )
    }
}


void set1_up_process(void)//参数选择界面上键处理程序
{
    move_flag = 1;

    if(0 == position1_y)
    {
        position1_y = 2;
    }
    else
    {
        position1_y--;
    }
}


void set1_down_process(void)//参数选择界面下键处理程序
{
    move_flag = 1;

    if(2 == position1_y)
    {
        position1_y = 0;
    }
    else
    {
        position1_y++;
    }
}


void set1_left_process(void)//参数选择界面左键处理程序
{
    move_flag = 1;
    if(0 == position1_x)
    {
        position1_x = 1;
    }
    else
    {
        position1_x--;
    }
}


void set1_right_process(void)//参数选择界面右键处理程序
{
    move_flag = 1;
    if(1 == position1_x)
    {
        position1_x = 0;
    }
    else
    {
        position1_x++;
    }
}


void set1_ld_process(void)//参数选择界面左下键处理程序
{
    clr_Screen(7,0,17);  //反白显示Set
    print_str8x16(7,0,1,"Set");
    gpio_set(P02_6, 0);
    systick_delay_ms(STM0,50);
    gpio_set(P02_6, 1);

    statue_flag = 3;

    data_split(screen_num,position1_x,position1_y);

    if((1 == screen_num)&&(0 == position1_x))
    {
        temp_value = speed_value[0] + 10*speed_value[2];
    }
    else
    {
        temp_value = value[0]+10*value[1]+100*value[2]+1000*value[3]+10000*value[4];
    }

    //切换到具体参数设定处理界面
    clr_Screen(7,0,128);  //具体参数设定处理
    print_str8x16(7,0,0,"SET");  //显示Ok
    print_str8x16(7,96,0,"BACK");  //显示Back
    select_display(screen_num,position1_x,position1_y,0);  //清光标位置的反白
    position_display(screen_num,position1_x,position1_y,position,1);  //反白显示待设定参数的待设定位
    statue_flag = 2;
}


void set1_rd_process(void)//参数选择界面右下键处理程序
{
    unsigned char renew_flag;
    renew_flag = 0;

    clr_Screen(7,64,128);  //反白显示Exit
    print_str8x16(7,96,1,"BACK");
    gpio_set(P02_6, 0);
    systick_delay_ms(STM0,50);
    gpio_set(P02_6, 1);

    statue_flag = 0;  //返回到主界面显示
    position1_x = 0;
    position1_y = 0;
    Clr_Screen(8);
    if(Speed_pid_p != uiTempPID_P)
    {
        renew_flag = 1;
    }
    else if(Speed_pid_i != uiTempPID_I)
    {
        renew_flag = 1;
    }
    else if(Speed_pid_d != uiTempPID_D)
    {
        renew_flag = 1;
    }
    else if(PD_P != uiTempPD_a)
    {
        renew_flag = 1;
    }
    else if(b_value != uiTempPD_b)
    {
        renew_flag = 1;
    }
    else if(PD_D != uiTempPD_D)
    {
        renew_flag = 1;
    }
    else if(mid_speed != uiTempSpeed)
    {
        renew_flag = 1;
    }
    else if(high_speed != uiTempMaxSpeed)
    {
        renew_flag = 1;
    }
    else if(low_speed != uiTempMinSpeed)
    {
        renew_flag = 1;
    }
    else if(s_a_value != uiTempS_A)
    {
        renew_flag = 1;
    }
    else if(s_b_value != uiTempS_B)
    {
        renew_flag = 1;
    }
    else if(s_c_value != uiTempS_C)
    {
        renew_flag = 1;
    }
    else if(res1 != uiTempres1)
    {
        renew_flag = 1;
    }
    else if(res2 != uiTempres2)
    {
        renew_flag = 1;
    }
    else if(res3 != uiTempres3)
    {
        renew_flag = 1;
    }
    else if(res4 != uiTempres4)
    {
        renew_flag = 1;
    }
    else if(res5 != uiTempres5)
    {
        renew_flag = 1;
    }
    else if(res6 != uiTempres6)
    {
        renew_flag = 1;
    }
    else if(P1P2 != uiTempP1P2)
    {
        renew_flag = 1;
    }
    else if(P3P4 != uiTempP3P4)
    {
        renew_flag = 1;
    }
    else if(P5P6 != uiTempP5P6)
    {
        renew_flag = 1;
    }
    else if(D1D2 != uiTempD1D2)
    {
        renew_flag = 1;
    }
    else if(D3D4 != uiTempD3D4)
    {
        renew_flag = 1;
    }
    else if(D5D6 != uiTempD5D6)
    {
        renew_flag = 1;
    }

    if(1  == renew_flag)
    {
        renew_flag = 0;
        data_flash_write();//向flash中写入数据
    }//end of if(1 == renew_flag)
}


void data_split(unsigned char page,unsigned char x,unsigned char y)//更新value数组中的数值
{
    unsigned char i;
    if(0 == page)
    {
        if( (0 == x)&&(0 == y) )
        {
            Data_Split(Speed_pid_p,5,value,0);  //拆分为单纯的数字，更新value数组中的数值
        }
        else if( (0 == x)&&(1 == y) )
        {
            Data_Split(Speed_pid_i,5,value,0);  //拆分为单纯的数字，更新value数组中的数值
        }
        else if( (0 == x)&&(2 == y) )
        {
            Data_Split(Speed_pid_d,5,value,0);  //拆分为单纯的数字，更新value数组中的数值
        }
        else if( (1 == x)&&(0 == y) )
        {
            Data_Split(PD_P,5,value,0);  //拆分为单纯的数字，更新value数组中的数值
        }
        else if( (1 == x)&&(1 == y) )
        {
            Data_Split(b_value,5,value,0);  //拆分为单纯的数字，更新value数组中的数值
        }
        else if( (1 == x)&&(2 == y) )
        {
            Data_Split(PD_D,5,value,0);  //拆分为单纯的数字，更新value数组中的数值
        }

        i = value[4];//Data_Split()拆分完的数组的第0个元素是最高位因此位置要调换一下
        value[4] = value[0];
        value[0] = i;
        i = value[3];
        value[3] = value[1];
        value[1] = i;
    }
    else if(1 == page)
    {
        if( (0 == x)&&(0 == y) )
        {
            Data_Split(mid_speed,5,value,0);  //拆分为单纯的数字，更新value数组中的数值
        }
        else if( (0 == x)&&(1 == y) )
        {
            Data_Split(high_speed,5,value,0);  //拆分为单纯的数字，更新value数组中的数值
        }
        else if( (0 == x)&&(2 == y) )
        {
            Data_Split(low_speed,5,value,0);  //拆分为单纯的数字，更新value数组中的数值
        }
        else if( (1 == x)&&(0 == y) )
        {
            Data_Split(s_a_value,5,value,0);  //拆分为单纯的数字，更新value数组中的数值
        }
        else if( (1 == x)&&(1 == y) )
        {
            Data_Split(s_b_value,5,value,0);  //拆分为单纯的数字，更新value数组中的数值
        }
        else if( (1 == x)&&(2 == y) )
        {
            Data_Split(s_c_value,5,value,0);  //拆分为单纯的数字，更新value数组中的数值
        }
        i = value[4];//Data_Split()拆分完的数组的第0个元素是最高位因此位置要调换一下
        value[4] = value[0];
        value[0] = i;
        i = value[3];
        value[3] = value[1];
        value[1] = i;
        if(0 == x)
        {
            speed_value[0] = value[0];
            speed_value[2] = value[1];
        }
    }
    else if(4 == page)
    {
        if( (0 == x)&&(0 == y) )
        {
            Data_Split(res1,5,value,0);  //拆分为单纯的数字，更新value数组中的数值
        }
        else if( (0 == x)&&(1 == y) )
        {
            Data_Split(res2,5,value,0);  //拆分为单纯的数字，更新value数组中的数值
        }
        else if( (0 == x)&&(2 == y) )
        {
            Data_Split(res3,5,value,0);  //拆分为单纯的数字，更新value数组中的数值
        }
        else if( (1 == x)&&(0 == y) )
        {
            Data_Split(res4,5,value,0);  //拆分为单纯的数字，更新value数组中的数值
        }
        else if( (1 == x)&&(1 == y) )
        {
            Data_Split(res5,5,value,0);  //拆分为单纯的数字，更新value数组中的数值
        }
        else if( (1 == x)&&(2 == y) )
        {
            Data_Split(res6,5,value,0);  //拆分为单纯的数字，更新value数组中的数值
        }

        i = value[4];//Data_Split()拆分完的数组的第0个元素是最高位因此位置要调换一下
        value[4] = value[0];
        value[0] = i;
        i = value[3];
        value[3] = value[1];
        value[1] = i;
    }
    else if(2 == page)
    {
        if( (0 == x)&&(0 == y) )
        {
            Data_Split(P1P2,5,value,0);  //拆分为单纯的数字，更新value数组中的数值
        }
        else if( (0 == x)&&(1 == y) )
        {
            Data_Split(P3P4,5,value,0);  //拆分为单纯的数字，更新value数组中的数值
        }
        else if( (0 == x)&&(2 == y) )
        {
            Data_Split(P5P6,5,value,0);  //拆分为单纯的数字，更新value数组中的数值
        }
        else if( (1 == x)&&(0 == y) )
        {
            Data_Split(D1D2,5,value,0);  //拆分为单纯的数字，更新value数组中的数值
        }
        else if( (1 == x)&&(1 == y) )
        {
            Data_Split(D3D4,5,value,0);  //拆分为单纯的数字，更新value数组中的数值
        }
        else if( (1 == x)&&(2 == y) )
        {
            Data_Split(D5D6,5,value,0);  //拆分为单纯的数字，更新value数组中的数值
        }

        i = value[4];//Data_Split()拆分完的数组的第0个元素是最高位因此位置要调换一下
        value[4] = value[0];
        value[0] = i;
        i = value[3];
        value[3] = value[1];
        value[1] = i;
    }
}


void position_display(unsigned char page,unsigned char x,unsigned char y,unsigned char position,unsigned char reverse)//具体参数设定显示程序
{
    unsigned char column;
    unsigned char begin;
    unsigned char final;

    if(3==page)
    {
        if(0 == x)//确定光标（反白）的位置
        {
            column = 10 - position;
        }
        else
        {
            column = 26 - position;
        }
    }
    else
    {
        if(0 == x)//确定光标（反白）的位置
        {
            column = 6 - position;
        }
        else
        {
            column = 19 - position;
        }
    }


    begin = 8*column;//清除一个点
    final = begin + 8;

    if((1 == page)&&(0 == x))
    {
        speed_value[position] = speed_value[position] + 0x30;
        if(0 == y)  //反白显示PID中P的某一位
        {
            clr_Screen(0,begin,final);
            print_char8x16(0,begin,reverse,speed_value[position]);
        }
        else if(1 == y)  //反白显示PID中I的某一位
        {
            clr_Screen(2,begin,final);
            print_char8x16(2,begin,reverse,speed_value[position]);
        }
        else if(2 == y)  //反白显示PID中D的某一位
        {
            clr_Screen(4,begin,final);
            print_char8x16(4,begin,reverse,speed_value[position]);
        }
        speed_value[position] = speed_value[position] - 0x30;
    }//end of if((1 == page)&&(0 == x))
    else
    {
        value[position] = value[position] + 0x30;
        if( (0 == x)&&(0 == y) )  //反白显示PID中P的某一位
        {
            clr_Screen(0,begin,final);
            print_char8x16(0,begin,reverse,value[position]);
        }
        else if((0 == x)&&(1 == y))  //反白显示PID中I的某一位
        {
            clr_Screen(2,begin,final);
            print_char8x16(2,begin,reverse,value[position]);
        }
        else if( (0 == x)&&(2 == y) )  //反白显示PID中D的某一位
        {
            clr_Screen(4,begin,final);
            print_char8x16(4,begin,reverse,value[position]);
        }
        else if( (1 == x)&&(0 == y) )  //反白显示变P中a的某一位
        {
            clr_Screen(0,begin,final);
            print_char8x16(0,begin,reverse,value[position]);
        }
        else if( (1 == x)&&(1 == y) )  //反白显示变P中b的某一位
        {
            clr_Screen(2,begin,final);
            print_char8x16(2,begin,reverse,value[position]);
        }
        else if( (1 == x)&&(2 == y) )  //反白显示PD中D的某一位
        {
            clr_Screen(4,begin,final);
            print_char8x16(4,begin,reverse,value[position]);
        }
        value[position] = value[position] - 0x30;
    }//end of else
}


//参数修改界面各键的处理程序
void set2_up_process(void)//具体参数设定上键处理程序
{
    move_flag = 1;
    if((1 == screen_num)&&(0 == position1_x))
    {
        if(9 == speed_value[position])
        {
            speed_value[position] = 0;
        }
        else
        {
            speed_value[position]++;
        }
    }
    else
    {
        if(9 == value[position])
        {
            value[position] = 0;
        }
        else
        {
            value[position]++;
        }
    }
}


void set2_down_process(void)//具体参数设定下键处理程序
{
    move_flag = 1;
    if((1 == screen_num)&&(0 == position1_x))
    {
        if(0 == speed_value[position])
        {
            speed_value[position] = 9;
        }
        else
        {
            speed_value[position]--;
        }
    }
    else
    {
        if(0 == value[position])
        {
            value[position] = 9;
        }
        else
        {
            value[position]--;
        }
    }
}


void set2_left_process(void)//具体参数设定左键处理程序
{
    move_flag = 1;
    if((1 == screen_num)&&(0 == position1_x))
    {
        if(0 == position)
        {
            position = 2;
        }
        else
        {
            position = 0;
        }
    }
    else
    {
        if(4 == position)
        {
            position = 0;
        }
        else
        {
            position++;
        }
    }
}


void set2_right_process(void)//具体参数设定右键处理程序
{
    move_flag = 1;

    if((1 == screen_num)&&(0 == position1_x))
    {
        if(0 == position)
        {
            position = 2;
        }
        else
        {
            position = 0;
        }
    }
    else
    {
        if(0 == position)
        {
            position = 4;
        }
        else
        {
            position--;
        }
    }
}


void renew_data(unsigned char x,unsigned char y)//更新修改后数据
{
    if(0 == screen_num)
    {
        if( (0 == x)&&(0 == y) )
        {
            Speed_pid_p = value[0]+10*value[1]+100*value[2]+1000*value[3]+10000*value[4];
        }//end of if( (0 == x)&&(0 == y) )

        else if( (0 == x)&&(1 == y) )
        {
            Speed_pid_i = value[0]+10*value[1]+100*value[2]+1000*value[3]+10000*value[4];
        }//end of if( (0 == x)&&(1 == y) )

        else if( (0 == x)&&(2 == y) )
        {
            Speed_pid_d = value[0]+10*value[1]+100*value[2]+1000*value[3]+10000*value[4];
        }//end of if( (0 == x)&&(2 == y) )

        else if( (1 == x)&&(0 == y) )
        {
            PD_P = value[0]+10*value[1]+100*value[2]+1000*value[3]+10000*value[4];
        }//end of if( (1 == x)&&(0 == y) )

        else if( (1 == x)&&(1 == y) )
        {
            b_value = value[0]+10*value[1]+100*value[2]+1000*value[3]+10000*value[4];
        }//end of if( (1 == x)&&(1 == y) )

        else if( (1 == x)&&(2 == y) )
        {
            PD_D = value[0]+10*value[1]+100*value[2]+1000*value[3]+10000*value[4];
        }//end of if( (1 == x)&&(2 == y) )
    }
    else if(1 == screen_num)
    {
        if(0 == x)
        {
            if(0 == y)
            {
                mid_speed = speed_value[0] + 10*speed_value[2];
            }
            else if(1 == y)
            {
                high_speed = speed_value[0] + 10*speed_value[2];
            }
            else if(2 == y)
            {
                low_speed = speed_value[0] + 10*speed_value[2];
            }
        }
        else
        {
            if(0 == y)
            {
                s_a_value = value[0]+10*value[1]+100*value[2]+1000*value[3]+10000*value[4];
            }
            else if(1 == y)
            {
                s_b_value = value[0]+10*value[1]+100*value[2]+1000*value[3]+10000*value[4];
            }
            else if(2 == y)
            {
                s_c_value = value[0]+10*value[1]+100*value[2]+1000*value[3]+10000*value[4];
            }
        }
    }
    else if(4 == screen_num)
    {
        if( (0 == x)&&(0 == y) )
        {
            res1 = value[0]+10*value[1]+100*value[2]+1000*value[3]+10000*value[4];
        }//end of if( (0 == x)&&(0 == y) )

        else if( (0 == x)&&(1 == y) )
        {
            res2 = value[0]+10*value[1]+100*value[2]+1000*value[3]+10000*value[4];
        }//end of if( (0 == x)&&(1 == y) )

        else if( (0 == x)&&(2 == y) )
        {
            res3 = value[0]+10*value[1]+100*value[2]+1000*value[3]+10000*value[4];
        }//end of if( (0 == x)&&(2 == y) )

        else if( (1 == x)&&(0 == y) )
        {
            res4 = value[0]+10*value[1]+100*value[2]+1000*value[3]+10000*value[4];
        }//end of if( (1 == x)&&(0 == y) )

        else if( (1 == x)&&(1 == y) )
        {
            res5 = value[0]+10*value[1]+100*value[2]+1000*value[3]+10000*value[4];
        }//end of if( (1 == x)&&(1 == y) )

        else if( (1 == x)&&(2 == y) )
        {
            res6 = value[0]+10*value[1]+100*value[2]+1000*value[3]+10000*value[4];
        }//end of if( (1 == x)&&(2 == y) )

    }
    else if(2 == screen_num)
    {
        if( (0 == x)&&(0 == y) )
        {
            P1P2 = value[0]+10*value[1]+100*value[2]+1000*value[3]+10000*value[4];
        }//end of if( (0 == x)&&(0 == y) )

        else if( (0 == x)&&(1 == y) )
        {
            P3P4 = value[0]+10*value[1]+100*value[2]+1000*value[3]+10000*value[4];
        }//end of if( (0 == x)&&(1 == y) )

        else if( (0 == x)&&(2 == y) )
        {
            P5P6 = value[0]+10*value[1]+100*value[2]+1000*value[3]+10000*value[4];
        }//end of if( (0 == x)&&(2 == y) )

        else if( (1 == x)&&(0 == y) )
        {
            D1D2 = value[0]+10*value[1]+100*value[2]+1000*value[3]+10000*value[4];
        }//end of if( (1 == x)&&(0 == y) )

        else if( (1 == x)&&(1 == y) )
        {
            D3D4 = value[0]+10*value[1]+100*value[2]+1000*value[3]+10000*value[4];
        }//end of if( (1 == x)&&(1 == y) )

        else if( (1 == x)&&(2 == y) )
        {
            D5D6 = value[0]+10*value[1]+100*value[2]+1000*value[3]+10000*value[4];
        }//end of if( (1 == x)&&(2 == y) )

    }
}


void set2_ld_process(void)//具体参数设定左下键处理程序
{
    clr_Screen(7,0,16);
    print_str8x16(7,0,1,"SET");  //反白显示显示Ok
    gpio_set(P02_6, 0);
    systick_delay_ms(STM0,50);
    gpio_set(P02_6, 1);

    statue_flag = 1;//返回到参数选择界面
    renew_data(position1_x,position1_y);//更新数据
    select_display(screen_num,position1_x,position1_y,1);
    clr_Screen(7,0,128);
    print_str8x16(7,0,0,"SET");  //显示Set
    print_str8x16(7,96,0,"BACK");  //显示Exit
    position = 0;
}


void set2_rd_process(void)//具体参数设定右下键处理程序
{
    unsigned char i;
    clr_Screen(7,64,128);
    print_str8x16(7,96,1,"BACK");  //反白显示显示Back
    gpio_set(P02_6, 0);
    systick_delay_ms(STM0,50);
    gpio_set(P02_6, 1);

    //back按键按下则不保存刚修改的数据
    Data_Split(temp_value,5,value,0);  //拆分为单纯的数字，更新value数组中的数值
    i = value[4];//Data_Split()拆分完的数组的第0个元素是最高位因此位置要调换一下
    value[4] = value[0];
    value[0] = i;
    i = value[3];
    value[3] = value[1];
    value[1] = i;
    speed_value[0] = value[0];
    speed_value[2] = value[1];
    renew_data(position1_x,position1_y);//更新修改后数据

    statue_flag = 1;
    select_display(screen_num,position1_x,position1_y,1);
    position = 0;
    clr_Screen(7,0,128);
    print_str8x16(7,0,0,"SET");  //显示Set
    print_str8x16(7,96,0,"BACK");  //显示Exit
}


void display_screen(unsigned char screen_num)//显示主界面处理程序
{
    if(0 == screen_num)  //主界面1显示
    {
//        show_data(6,150,0,EspFlag);
        show_data(7, 128, 0, adc_convert(ADC_8, ADC8_CH15_A47, ADC_12BIT)*9900/4096+400);//电量显示

        show_data(7, 170, 0, adc_convert(ADC_8, ADC8_CH12_A44, ADC_12BIT));//距离显示

        print_str8x16(0,0,0,"P:");
        show_data(0,16,0,Speed_pid_p);
        print_str8x16(2,0,0,"I:");
        show_data(2,16,0,Speed_pid_i);
        print_str8x16(4,0,0,"D:");
        show_data(4,16,0,Speed_pid_d);

        print_str8x16(0,104,0,"P:");//72=7*8+2*8
        show_data(0,120,0,PD_P);//88=72+2*8
        print_str8x16(2,104,0,"b:");
        show_data(2,120,0,b_value);
        print_str8x16(4,104,0,"D:");
        show_data(4,120,0,PD_D);

        print_str8x16(7,0,0,"SET");
        print_str8x16(7,36,0,"SHIFT");//Set+14小格
        print_str8x16(7,90,0,"RUN");
    }//end of if(0 == screen_num)

    else if(1 == screen_num)
    {
        //设定速度参数
        print_str8x16(0,0,0,"M_S:");
        show_data(0,32,0,mid_speed);
        print_str8x16(2,0,0,"H_S:");
        show_data(2,32,0,high_speed);
        print_str8x16(4,0,0,"L_S:");
        show_data(4,32,0,low_speed);

        position1_x = 1;//仅仅为了使abc不显示小数
        //差速参数
        print_str8x16(0,104,0,"a:");//72=7*8+2*8
        show_data(0,120,0,s_a_value);//88=72+2*8
        print_str8x16(2,104,0,"b:");
        show_data(2,120,0,s_b_value);
        print_str8x16(4,104,0,"c:");
        show_data(4,120,0,s_c_value);
        position1_x = 0;

        print_str8x16(7,0,0,"SET");
        print_str8x16(7,44,0,"SHIFT");//Set+14小格
        print_str8x16(7,104,0,"RUN");
    }//end of if(1 == screen_num)

    else if(2 == screen_num)
    {
        print_str8x16(0,0,0,"P1+P2:");
        show_data(0,48,0,P1P2);
        print_str8x16(2,0,0,"P3+P4:");
        show_data(2,48,0,P3P4);
        print_str8x16(4,0,0,"P5+P6:");
        show_data(4,48,0,P5P6);

        //差速参数
        print_str8x16(0,128,0,"D1+D2:");//72=7*8+2*8
        show_data(0,176,0,D1D2);//88=72+2*8
        print_str8x16(2,128,0,"D3+D4:");
        show_data(2,176,0,D3D4);
        print_str8x16(4,128,0,"D5+D6:");
        show_data(4,176,0,D5D6);

        print_str8x16(7,0,0,"SET");
        print_str8x16(7,44,0,"SHIFT");//Set+14小格
        print_str8x16(7,104,0,"RUN");
    }//end of else if(3 == screen_num)

    else if(3 == screen_num)
    {
        if(SWITCH_OFF == SWITCH_Get_Input(SWITCH1))
        {
            int16 encoder1,encoder2;
            encoder1 = gpt12_get(GPT12_T4);
            encoder2 = gpt12_get(GPT12_T6);
            gpt12_clear(GPT12_T4);
            gpt12_clear(GPT12_T6);
            ips114_showint32(0,0,encoder1,4);
            ips114_showint32(0,1,encoder2,4);
            print_str8x16(7,44,0,"Shift");//Set+14小格
            print_str8x16(7,104,0,"RUN");
            ips114_showint32(7,190,time,4);
        }
        else
        {
            ips114_displayimage032(mt9v03x_image[0], MT9V03X_W, MT9V03X_H);
            mt9v03x_finish_flag = 0;
        }
    }//end of else if(3 == screen_num)

    else if(4 == screen_num)
    {
        //设定阈值参数
        print_str8x16(0,0,0,"1:");
        show_data(0,16,0,res1);
        print_str8x16(2,0,0,"2:");
        show_data(2,16,0,res2);
        print_str8x16(4,0,0,"3:");
        show_data(4,16,0,res3);

        //预留参数
        print_str8x16(0,104,0,"4:");//72=7*8+2*8
        show_data(0,120,0,res4);//88=72+2*8
        print_str8x16(2,104,0,"5:");
        show_data(2,120,0,res5);
        print_str8x16(4,104,0,"6:");
        show_data(4,120,0,res6);

        print_str8x16(7,0,0,"SET");
        print_str8x16(7,44,0,"SHIFT");//Set+14小格
        print_str8x16(7,104,0,"RUN");

        if(KEY_PRESS == KEY_Get_Input(KEY_LEFT))
        {
            res1 = 4045;
            res2 = 4045;
            res3 = 3232;
            res4 = 2020;
            res5 = 8080;
            data_flash_write();
        }
        else if(KEY_PRESS == KEY_Get_Input(KEY_RIGHT))
        {
            res1 = 3037;
            res2 = 3037;
            res3 = 3030;
            res4 = 2020;
            res5 = 7575;
            data_flash_write();
        }
    }

    else if(5 == screen_num)//显示中线权重
    {
        if(KEY_PRESS == KEY_Get_Input(KEY_LEFT))
        {
            if(!PlanNum)  PlanNum = 19;
            else PlanNum--;
            res6 = PlanNum;
            data_flash_write();
        }
        else if(KEY_PRESS == KEY_Get_Input(KEY_RIGHT))
        {
            if(PlanNum==19)  PlanNum = 0;
            else PlanNum++;
            res6 = PlanNum;
            data_flash_write();
        }

        for(uint8 i=0;i<24;i++)
        {
            for(uint8 j=0;j<5;j++)
            {
                ErrorBuffer[i*5+j] = ErrorPlan[PlanNum][i];
            }
        }

        for(uint8 i=0;i<24;i++)
        {
            uint16 j=0;
            uint16 k;
            j = ErrorBuffer[i*5] * 10;
            ips114_set_region(i*10,0,i*10+7,99);
            for(k=0;k<j*8;k++) ips114_writedata_16bit(IPS114_PENCOLOR);
            for(k=0;k<(100-j)*8;k++) ips114_writedata_16bit(IPS114_BGCOLOR);
//            ips114_showchar(i*10,95,ErrorBuffer[i*5]+48);

        }

        ips114_showchar(0,111,'P');
        ips114_showchar(8,111,'L');
        ips114_showchar(16,111,'A');
        ips114_showchar(24,111,'N');
        ips114_showchar(36,111,':');
        ips114_showint32(48,7,PlanNum,2);

    }//end of else if(4 == screen_num)
}


/***********************************************************************************
* 函数名: show_data;
*
* 描述: 显示变量;
*
* 入口参数:  unsigned char page, unsigned char column, unsigned char reverse, uint16 data;
             页地址;             列地址;               是否反白显示;          要显示的变量名;
*
* 出口参数: 无;
*
* 备注:  调用函数print_str8x16（）;
************************************************************************************/

void show_data(unsigned char page, unsigned char column, unsigned char reverse, uint32 data)
{
    unsigned char show_data[6] ={' '};
    unsigned char show_float[4];
    unsigned char display[6];
    unsigned char i = 0;
    unsigned char j = 0;
    unsigned char statue =0;
    Data_Split(data,5,show_data,1);
    for(i = 0; i < 6; i++)
    {
        display[i] = ' ';
    }
    if((1 == screen_num)&&(0 == position1_x))
    {
        show_float[3] = show_data[5];
        show_float[2] = show_data[4];
        show_float[1] = '.';
        show_float[0] = show_data[3];
        print_str8x16(page,column,reverse,show_float);
    }
    else
    {
        if(3 == statue_flag)
        {
            print_str8x16(page,column,reverse,show_data);
        }
        else
        {
            for(i = 0; i < 5; i++)
            {
                if( ('0' == show_data[i]) && (0 == statue) )
                {
                    ;
                }
                else
                {
                    statue = 1;
                    display[j] = show_data[i];
                    j++;
                }
            }

            if(0 == j)
            {
                display[0] = '0';
            }
            display[5] = '\0';

            print_str8x16(page,column,reverse,display);
        }
    }
    statue = 0;
}


//************拆分数据(暂定最多拆分5位，可调)************
//asc为1时，uidata 拆分为asc码，即0x30~0x39；asc为0时，拆分单纯的数字,即0~9
void Data_Split(unsigned int uiData, unsigned char ucSize, unsigned char *dp,unsigned char asc)
{
     switch(ucSize)
     {
        case 5:
        *(dp++) = (uiData/10000)%10 + 0x30 * asc;
        case 4:
        *(dp++) = (uiData/1000)%10 + 0x30 * asc;
        case 3:
        *(dp++) = (uiData/100)%10 + 0x30 * asc;
        case 2:
        *(dp++) = (uiData/10)%10 + 0x30 * asc;
        case 1:
        *(dp++) = (uiData)%10 + 0x30 * asc;
     }
}
