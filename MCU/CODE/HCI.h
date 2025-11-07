#ifndef _HCI_H_
#define _HCI_H_

#include "common.h"
#include "zf_eeprom.h"

#define DIP_switch1 gpio_get(PTB10)
#define DIP_switch2 gpio_get(PTB16)
#define DIP_switch3 gpio_get(PTB17)
#define DIP_switch4 gpio_get(PTB18)
//#define DIP_switch5 gpio_get(PTA17)
//#define DIP_switch6 gpio_get(PTA16)
//#define DIP_switch7 gpio_get(PTA15)
//#define DIP_switch8 gpio_get(PTA14)

#define SECTOR_NUM  (EEPROM_SECTOR_NUM-1)

void HCI_init(void);  //人机交互初始化
//void key_init(void);  //按键初始化函数
void switch_init(void);  //拨码开关初始化函数
void value_init(void);  //标志位、变量初始化
void HCI_handle(void);  //人机交互主程序
void data_flash_read(void);  //读取flash中的数据
void data_flash_write(void);  //向flash中写入数据

void select_display(unsigned char page,unsigned char x,unsigned char y,unsigned char reverse);  //选择显示界面处理
void set1_up_process(void);  //选择参数上键处理程序
void set1_down_process(void);  //选择参数下键处理程序
void set1_left_process(void);  //选择参数左键处理程序
void set1_right_process(void);  //选择参数右键处理程序
void set1_ld_process(void);  //选择参数左下键处理程序
void set1_rd_process(void);  //选择参数右下键处理程序

void data_split(unsigned char page,unsigned char x,unsigned char y);//更新value数组中的数值
void renew_data(unsigned char x,unsigned char y);//更新数据

void position_display(unsigned char page,unsigned char x,unsigned char y,unsigned char position,unsigned char reverse);  //参数修改界面处理
void set2_up_process(void);  //参数修改上键处理程序
void set2_down_process(void);  //参数修改下键处理程序
void set2_left_process(void);  //参数修改左键处理程序
void set2_right_process(void);  //参数修改右键处理程序
void set2_ld_process(void);  //参数修改左下键处理程序
void set2_rd_process(void);  //参数修改右下键处理程序

void display_screen(unsigned char screen_num);  //主界面显示处理

void show_data(unsigned char page, unsigned char column, unsigned char reverse, uint32 data);
void Data_Split(unsigned int uiData, unsigned char ucSize, unsigned char *dp,unsigned char asc);


/*****************第二界面参数********************/
extern uint32 mid_speed;  //中等速度
extern uint32 high_speed;  //高速
extern uint32 low_speed;  //低速
extern uint32 s_a_value;  //差速的a值
extern uint32 s_b_value;  //差速的b值
extern uint32 s_c_value;  //差速的c值
/****************************************************/

/*****************第一界面参数********************/
extern uint32 P_value;  //电机PID的P值
extern uint32 I_value;  //电机PID的I值
extern uint32 D_value;  //电机PID的D值
extern uint32 PD_P;  //舵机PD的P值
extern uint32 b_value;  //舵机变P的二次函数中的b
extern uint32 PD_D;  //舵机PD的D值
/****************************************************/


/*********************预留参数***********************/
extern uint32 res1; 
extern uint32 res2;  
extern uint32 res3;
extern uint32 res4;
extern uint32 res5;
extern uint32 res6;

#endif
