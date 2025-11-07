/*********************************************************************************************************************
* @file             KEY.h
* @author           Andreas WH
* @brief            按键的初始化与读取
* @Target core      TC364
* @board            TC364_MainBoard_V1.2
* @date             2022-1-3
********************************************************************************************************************/

#ifndef KEY_H
#define KEY_H

#include "headfile.h"


/* 板载按键定义V1.1 */
#define KEYn                            6U

#define KEY_UP_PIN                      P15_2

#define KEY_DOWN_PIN                    P15_1

#define KEY_LEFT_PIN                    P15_3

#define KEY_RIGHT_PIN                   P15_0

#define KEY_SET_PIN                     P15_5

#define KEY_BACK_PIN                    P15_4

#define KEY_PRESS                       0
#define KEY_RELEASE                     1


/* 板载拨码开关定义 */
#define SWITCHn                         4U

#define SWITCH1_PIN                     P20_6

#define SWITCH2_PIN                     P20_9

#define SWITCH3_PIN                     P20_8

#define SWITCH4_PIN                     P20_7

#define SWITCH_ON                       0
#define SWITCH_OFF                      1


typedef enum
{
    KEY_UP = 0,
    KEY_DOWN = 1,
    KEY_LEFT = 2,
    KEY_RIGHT = 3,
    KEY_SET = 4,
    KEY_BACK = 5,
    KEY_ALL = 99
} key_typedef_enum;

typedef enum
{
    SWITCH1 = 0,
    SWITCH2 = 1,
    SWITCH3 = 2,
    SWITCH4 = 3,
    SWITCH_ALL = 99,
} switch_typedef_enum;



/* 函数定义 */

/* 初始化按键 */
void KEY_Init(key_typedef_enum keynum);
/* 获取按键输入(无延时) */
uint8 KEY_Get(key_typedef_enum keynum);
/* 获取按键输入 */
uint8 KEY_Get_Input(key_typedef_enum keynum);
/* 读取输入的按键值 */
uint8 KEY_Read(uint8 mode);
/* 初始化拨码开关 */
void SWITCH_Init(switch_typedef_enum switchnum);
/* 获取拨码开关输入 */
uint8 SWITCH_Get_Input(switch_typedef_enum switchnum);
/* 读取输入的拨码开关值 */
uint8 SWITCH_Read(void);


#endif
