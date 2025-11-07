/*********************************************************************************************************************
* @file             ESP8266.h
* @author           Andreas WH
* @brief            ESP8266
* @Target core      TC364
* @board            TC364_MainBoard_V1.1
* @date             2022-1-3
********************************************************************************************************************/

#ifndef ESP8266_H
#define ESP8266_H

#include "headfile.h"

/* ESP8266串口定义 */
#define         ESP8266_UART        UART_3
#define         ESP8266_TX          UART3_TX_P15_6
#define         ESP8266_RX          UART3_RX_P15_7
#define         ESP8266_RES         P20_10

/* 函数定义 */
typedef struct
{
    uint8 name[8];
    uint8 data[1024];
} Package;

/* 初始化按键 */
void ESP8266_Init(void);

void ESP8266_Send(const int8 *Send,int8 Rev[]);

void check_cmd();

void IMAGE_SEND(uint8 Thro);
void IMAGE_parameter_send();
void IMAGE_parameter_set();
void PID_parameter_send();
void PID_parameter_set();
void MOTOR_parameter_send();
void MOTOR_parameter_set();
void COORD_parameter_send();

#endif
