/*********************************************************************************************************************
* @file             ESP8266.c
* @author           Andreas WH
* @brief            ESP8266
* @Target core      TC364
* @board            TC364_MainBoard_V1.1
* @date             2022-1-3
********************************************************************************************************************/

#include "ESP8266.h"
#include "headfile.h"
#include <string.h>

uint8 RevBuffer[80] = {0,};
uint8 BufferCount = 0;

//-------------------------------------------------------------------------------------------------------------------
// @brief       按键初始化
// @param       keynum              选择的按键：KEY_UP,KEY_DOWN,KEY_LEFT,KEY_RIGHT,KEYSET,KEY_BACK
// @return      void
// Sample usage:            KEY_Init(KEY_UP);//初始化KEY_UP
//-------------------------------------------------------------------------------------------------------------------
void ESP8266_Init(void)
{
    uart_init(ESP8266_UART, 921600, ESP8266_TX, ESP8266_RX);
    gpio_init(ESP8266_RES, GPO, 0, PUSHPULL);
    systick_delay_ms(STM0, 100);
    gpio_set(ESP8266_RES, 1);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       按键初始化
// @param       keynum              选择的按键：KEY_UP,KEY_DOWN,KEY_LEFT,KEY_RIGHT,KEYSET,KEY_BACK
// @return      void
// Sample usage:            KEY_Init(KEY_UP);//初始化KEY_UP
//-------------------------------------------------------------------------------------------------------------------
void ESP8266_Send(const int8 *Send,int8 Rev[])
{
    uint8 RevFlag=0;
    while(!RevFlag)
    {
        memset(RevBuffer,0,BufferCount+1);
        BufferCount = 0;
        IfxAsclin_Asc_clearRx(&uart3_handle);
        uart_putstr(ESP8266_UART,Send);
        systick_delay_ms(STM0, 20);
        while(uart_query(ESP8266_UART,&RevBuffer[BufferCount]))
        {
            BufferCount++;
        }
        if(strstr((char*)RevBuffer, (char*)Rev) != NULL) RevFlag=1;
    }
}

void IMAGE_SEND(uint8 Thro)
{
    Package image;
    memcpy(image.name, "t97tt1x6", 8);
    uart_putstr(UART_3,(uint8*)&image.name);
    memset(image.data, 0, sizeof(image.data));

    uint16 i, j, k, p = 0;


//    for(i=0;i<MT9V03X_H;i++)
//    {
//        for(j=0;j<MT9V03X_W;j=j+8)
//        {//----------------------------------
//            for(k=0; k<8;k++)
//            {
//                image.data[p] = ((mt9v03x_image[i][j+k]>Thro)?1:0)|(image.data[p]<<1);
//            }
//            if(image.data[p]==0)
//                image.data[p]=1;
////----------------------------------二值化
//            p++;
//            if(p==1024||((i==(MT9V03X_H-1))&&(j==(MT9V03X_W-8))))
//            {
//                uart_putstr(UART_3,(uint8*)&image.data);
//                p = 0;
////                uart_putstr(UART_3,"PICTURE SEND");
////                systick_delay_ms(STM0, 50);
//            }
//        }
//    }

    for(j=0;j<MT9V03X_W;j++)
    {
        for(i=0;i<MT9V03X_H;i=i+8)
        {//----------------------------------
            for(k=0; k<8;k++)
            {
                image.data[p] = ((mt9v03x_image[i+k][j]>Thro)?1:0)|(image.data[p]<<1);
            }
            if(image.data[p]==0)
                image.data[p]=1;
//----------------------------------二值化
            p++;
            if(p==1024||((i==(MT9V03X_H-8))&&(j==(MT9V03X_W-1))))
            {
                uart_putstr(UART_3,(uint8*)&image.data);
                p = 0;
//                uart_putstr(UART_3,"PICTURE SEND");
                systick_delay_ms(STM0, 20);
            }
        }
    }


}

void IMAGE_parameter_send()
{


}

void IMAGE_parameter_set()
{

    uart_putstr(ESP8266_UART,"cmd IMAGE_set success\r\n");
}

void PID_parameter_send()
{

}

void PID_parameter_set()
{
    uart_putstr(ESP8266_UART,"cmd PID_set success\r\n");
}

void MOTOR_parameter_send()
{


}

void MOTOR_parameter_set()
{
    uart_putstr(ESP8266_UART,"cmd MOTOR_set success\r\n");
}

void COORD_parameter_send()
{


}

//void check_cmd()
//{
//    if(RevFlag){
//        Esp_Uart_Check_Recieve();
//      while(RevFlag){
//        if(strspn(URecv, "IMAGE") >= 5)
//            {
//                URecv_Index = 0;
//                if(strspn(URecv, "STOP") >= 4)
//                {
//                      Esp_Uart_Send_Data("cmd IMAGE STOP\r\n",16);
//                    break;
//                }
//                Esp_Uart_Send_Data("cmd IMAGE show\r\n",16);
//        IMAGE_SEND();
//                continue;
//            }
//        else if(strspn(URecv, "IMG_TS_G") >= 8)
//            {
//        IMAGE_parameter_send();
//                break;
//            }
//        else if(strspn(URecv, "IMG_TS_S") >= 8)
//            {
//        IMAGE_parameter_set();
//                break;
//            }
//        else if(strspn(URecv, "PID_G") >= 5)
//            {
//        PID_parameter_send();
//                break;
//            }
//        else if(strspn(URecv, "PID_S") >= 5)
//            {
//        PID_parameter_set();
//                break;
//            }
//        else if(strspn(URecv, "MOTOR_G") >= 7)
//            {
//        MOTOR_parameter_send();
//                break;
//            }
//        else if(strspn(URecv, "MOTOR_S") >= 7)
//            {
//        MOTOR_parameter_set();
//                break;
//            }
//        else if(strspn(URecv, "COORD") >= 5)
//            {
//        COORD_parameter_send();
//                break;
//            }
//        else
//        {
//            Esp_Uart_Send_Data("Parse Errors\r\n",14);
//        }
//    }
//}
//    URecv_Index = 0;
//}
