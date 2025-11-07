/*********************************************************************************************************************
* @file             IPS.c
* @author           Andreas WH
* @brief            IPS部分显示函数用于人机交互移植
* @Target core      TC364
* @board            TC364_MainBoard_V1.1
* @date             2021-12-26
********************************************************************************************************************/

#include "headfile.h"
#include <string.h>
#include "zf_spi.h"
#include "zf_gpio.h"
#include "zf_assert.h"
#include "zf_stm_systick.h"
#include "SEEKFREE_PRINTF.h"
#include "SEEKFREE_IPS114_SPI.h"
#include "IPS.h"
#include "Track.h"

uint16 color[10] = {0xfdf8,0xdb92,0x997c};
uint16 Img_color[MT9V03X_H][MT9V03X_W];

//-------------------------------------------------------------------------------------------------------------------
//  @brief      清除row行位置的屏幕
//  @param      row             行数，参数范围 0-7，8为清全屏
//  @return     void
//  Sample usage:               Clr_Screen(0);//清除第0行
//-------------------------------------------------------------------------------------------------------------------
void Clr_Screen(unsigned char row)
{
    if(row!=8)
    {
        uint16 i,j;
        ips114_set_region(0,row*16,IPS114_X_MAX-1,row*16+15);
        for(i=0;i<IPS114_X_MAX;i++)
        {
            for (j=0;j<16;j++)
            {
                ips114_writedata_16bit(BLACK);
            }
        }
    }
    else
    {
        uint16 i,j;
        ips114_set_region(0,0,IPS114_X_MAX-1,IPS114_Y_MAX-1);
        for(i=0;i<IPS114_X_MAX;i++)
        {
            for (j=0;j<IPS114_Y_MAX;j++)
            {
                ips114_writedata_16bit(BLACK);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      清除row行位置从start-end的屏幕
//  @param      row             行数，参数范围 0-7
//  @param      start           坐标x方向的起点 参数范围 0 -（IPS114_X_MAX-1）
//  @param      end           坐标x方向的终点 参数范围 0 -（IPS114_X_MAX-1）
//  @return     void
//  Sample usage:               clr_Screen(0,0,64);//清除第0行0-64格
//-------------------------------------------------------------------------------------------------------------------
void clr_Screen(unsigned char row,unsigned char start,unsigned char end)
{
    uint16 i,j;
    ips114_set_region(start,row*16,end,row*16+15);
    for(i=start;i<end;i++)
    {
        for (j=0;j<16;j++)
        {
            ips114_writedata_16bit(BLACK);
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      液晶显示字符
//  @param      row             行数，参数范围 0-7
//  @param      column          列数， 参数范围 0 -（IPS114_X_MAX-1）
//  @param      reverse         是否反白显示（0为正常，1为反白）
//  @param      dat             需要显示的字符
//  @return     void
//  @since      v1.0
//  Sample usage:               print_char8x16(0,0,0,'x');//坐标0,0写一个字符x,正常显示
//-------------------------------------------------------------------------------------------------------------------
void print_char8x16(uint16 row,uint16 column,unsigned char reverse,const int8 dat)
{
    uint8 i,j;
    uint8 temp;

    for(i=0; i<16; i++)
    {
        ips114_set_region(column,row*16+i,column+7,row*16+i);
        if(reverse) temp = ~tft_ascii[dat-32][i];
        else temp = tft_ascii[dat-32][i];//反白显示
        for(j=0; j<8; j++)
        {
            if(temp&0x01)
            {
                ips114_writedata_16bit(IPS114_PENCOLOR);
            }
            else ips114_writedata_16bit(IPS114_BGCOLOR);
            temp>>=1;
        }
    }
}

/***********************************************************************************
//  @brief      液晶显示字符
//  @param      row             行数，参数范围 0-7
//  @param      column          列数， 参数范围 0 -（IPS114_X_MAX-1）
//  @param      reverse         是否反白显示（0为正常，1为反白）
//  @param      dat             需要显示的字符
//  @return     void
//  @since      v1.0
//  Sample usage:               print_str8x16(0,0,0,'xyz');//坐标0,0写一个字符x,正常显示
************************************************************************************/
void print_str8x16(uint16 row,uint16 column,unsigned char reverse,const int8 dat[])
{
    uint16 j;

    j = 0;
    while(dat[j] != '\0')
    {
        print_char8x16(row,column+8*j,reverse,dat[j]);
        j++;
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      总钻风(灰度摄像头)液晶显示函数
//  @param      *p              图像数组地址
//  @param      width           图像宽度
//  @param      height          图像高度
//  @return     void
//  @since      v1.0
//  Sample usage:               ips114_displayimage032(mt9v03x_csi_image[0], MT9V03X_CSI_W, MT9V03X_CSI_H)//显示灰度摄像头 图像
//  @note       图像的宽度如果超过液晶的宽度，则自动进行缩放显示。这样可以显示全视野
//-------------------------------------------------------------------------------------------------------------------
void print_Img_color(void)
{
    uint32 i,j;

    uint16 temp = 0;

    ips114_set_region(0,0,MT9V03X_W-1,MT9V03X_H-1);

    for(j=0;j<MT9V03X_H;j++)
    {
        for(i=0;i<MT9V03X_W;i++)
        {
            temp = *(Img_color[0]+j*MT9V03X_W+i);//读取像素点
            ips114_writedata_16bit(temp);
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      总钻风(灰度摄像头)液晶显示函数
//  @param      *p              图像数组地址
//  @param      width           图像宽度
//  @param      height          图像高度
//  @return     void
//  @since      v1.0
//  Sample usage:               ips114_displayimage032(mt9v03x_csi_image[0], MT9V03X_CSI_W, MT9V03X_CSI_H)//显示灰度摄像头 图像
//  @note       图像的宽度如果超过液晶的宽度，则自动进行缩放显示。这样可以显示全视野
//-------------------------------------------------------------------------------------------------------------------
void create_Img(uint8 thro)
{
    uint32 i,j;

    uint16 temp = 0;

    for(j=0;j<MT9V03X_H;j++)
    {
        for(i=0;i<MT9V03X_W;i++)
        {
            temp = *(mt9v03x_image[0]+j*MT9V03X_W+i);//读取像素点
            if(temp>=thro) *(Img_color[0]+j*MT9V03X_W+i)=0xfdf8;
            else *(Img_color[0]+j*MT9V03X_W+i)=0xfb76;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      液晶显示边线
//  @param      *p              图像数组地址
//  @param      width           图像宽度
//  @param      height          图像高度
//  @return     void
//  @since      v1.0
//  Sample usage:               ips114_displayimage032(mt9v03x_csi_image[0], MT9V03X_CSI_W, MT9V03X_CSI_H)//显示灰度摄像头 图像
//  @note       图像的宽度如果超过液晶的宽度，则自动进行缩放显示。这样可以显示全视野
//-------------------------------------------------------------------------------------------------------------------
void print_line(uint8 *p, uint16 color, uint8 num)
{
    uint16 count=0;

    if(!num)
    {
        while(*(p+count)!=0 || *(p+count+1)!=0)
        {
            *(Img_color[0]+*(p+count)*MT9V03X_W+*(p+count+1)) = color;
            count+=2;
        }
    }
    else
    {
        while(count<num*2)
        {
            *(Img_color[0]+*(p+count)*MT9V03X_W+*(p+count+1)) = color;
            count+=2;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      液晶显示边线
//  @param      *p              图像数组地址
//  @param      width           图像宽度
//  @param      height          图像高度
//  @return     void
//  @since      v1.0
//  Sample usage:               ips114_displayimage032(mt9v03x_csi_image[0], MT9V03X_CSI_W, MT9V03X_CSI_H)//显示灰度摄像头 图像
//  @note       图像的宽度如果超过液晶的宽度，则自动进行缩放显示。这样可以显示全视野
//-------------------------------------------------------------------------------------------------------------------
void print_big_line(uint8 *p, uint16 color, uint8 num)
{
    uint16 count=0;

    if(!num)
    {
        while(*(p+count)!=0 || *(p+count+1)!=0)
        {
            *(Img_color[0]+*(p+count)*MT9V03X_W+*(p+count+1)) = color;
            *(Img_color[0]+*(p+count)*MT9V03X_W+*(p+count+1)-1) = color;
            *(Img_color[0]+*(p+count)*MT9V03X_W+*(p+count+1)+1) = color;
            count+=2;
        }
    }
    else
    {
        while(count<num*2)
        {
            *(Img_color[0]+*(p+count)*MT9V03X_W+*(p+count+1)) = color;
            *(Img_color[0]+*(p+count)*MT9V03X_W+*(p+count+1)-1) = color;
            *(Img_color[0]+*(p+count)*MT9V03X_W+*(p+count+1)+1) = color;
            count+=2;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      液晶显示边线
//  @param      *p              图像数组地址
//  @param      width           图像宽度
//  @param      height          图像高度
//  @return     void
//  @since      v1.0
//  Sample usage:               ips114_displayimage032(mt9v03x_csi_image[0], MT9V03X_CSI_W, MT9V03X_CSI_H)//显示灰度摄像头 图像
//  @note       图像的宽度如果超过液晶的宽度，则自动进行缩放显示。这样可以显示全视野
//-------------------------------------------------------------------------------------------------------------------
void print_big_line1(uint8 *p, uint16 color, uint8 num)
{
    uint16 count=0;

    if(!num)
    {
        while(*(p+count)!=0 || *(p+count+1)!=0)
        {
            *(Img_color[0]+(*(p+count)-1)*MT9V03X_W+*(p+count+1)) = color;
            *(Img_color[0]+*(p+count)*MT9V03X_W+*(p+count+1)) = color;
            *(Img_color[0]+(*(p+count)+1)*MT9V03X_W+*(p+count+1)) = color;
            count+=2;
        }
    }
    else
    {
        while(count<num*2)
        {
            *(Img_color[0]+(*(p+count)-1)*MT9V03X_W+*(p+count+1)) = color;
            *(Img_color[0]+*(p+count)*MT9V03X_W+*(p+count+1)) = color;
            *(Img_color[0]+(*(p+count)+1)*MT9V03X_W+*(p+count+1)) = color;
            count+=2;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      液晶显示边线
//  @param      *p              图像数组地址
//  @param      width           图像宽度
//  @param      height          图像高度
//  @return     void
//  @since      v1.0
//  Sample usage:               ips114_displayimage032(mt9v03x_csi_image[0], MT9V03X_CSI_W, MT9V03X_CSI_H)//显示灰度摄像头 图像
//  @note       图像的宽度如果超过液晶的宽度，则自动进行缩放显示。这样可以显示全视野
//-------------------------------------------------------------------------------------------------------------------
void print_big_point(uint8 *p, uint16 color)
{
        *(Img_color[0]+(*p-1)*MT9V03X_W+*(p+1)) = color;
        *(Img_color[0]+(*p-1)*MT9V03X_W+*(p+1)-1) = color;
        *(Img_color[0]+(*p-1)*MT9V03X_W+*(p+1)+1) = color;
        *(Img_color[0]+*p*MT9V03X_W+*(p+1)) = color;
        *(Img_color[0]+*p*MT9V03X_W+*(p+1)-1) = color;
        *(Img_color[0]+*p*MT9V03X_W+*(p+1)+1) = color;
        *(Img_color[0]+(*p+1)*MT9V03X_W+*(p+1)) = color;
        *(Img_color[0]+(*p+1)*MT9V03X_W+*(p+1)-1) = color;
        *(Img_color[0]+(*p+1)*MT9V03X_W+*(p+1)+1) = color;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      液晶显示边线
//  @param      *p              图像数组地址
//  @param      width           图像宽度
//  @param      height          图像高度
//  @return     void
//  @since      v1.0
//  Sample usage:               ips114_displayimage032(mt9v03x_csi_image[0], MT9V03X_CSI_W, MT9V03X_CSI_H)//显示灰度摄像头 图像
//  @note       图像的宽度如果超过液晶的宽度，则自动进行缩放显示。这样可以显示全视野
//-------------------------------------------------------------------------------------------------------------------
void print_road_type(uint16 x, uint16 y ,uint8 type)
{
    if(type == Straight) ips114_showstr(x,y,"Straight");
    else if(type == NoHead) ips114_showstr(x,y,"NoHead    ");
    else if(type == Cross) ips114_showstr(x,y,"Cross    ");
    else if(type == ThreeFork) ips114_showstr(x,y,"ThreeFork");
    else if(type == RightCircle) ips114_showstr(x,y,"RightCircle    ");
    else if(type == Ramp) ips114_showstr(x,y,"Ramp     ");
    else if(type == TurnLeft) ips114_showstr(x,y,"TurnLeft  ");
    else if(type == TurnRight) ips114_showstr(x,y,"TurnRight");
    else if(type == LeftT) ips114_showstr(x,y,"LeftT    ");
    else if(type == RightT) ips114_showstr(x,y,"RightT    ");
    else if(type == LeftCircle) ips114_showstr(x,y,"LeftCircle  ");
    else if(type == LeftGarage) ips114_showstr(x,y,"LeftGarage    ");
    else if(type == RightGarage) ips114_showstr(x,y,"RightGarage    ");
    else if(type == Crooked) ips114_showstr(x,y,"Crooked    ");
    else if(type == BadCross) ips114_showstr(x,y,"BadCross    ");
    else if(type == ToBeDone) ips114_showstr(x,y,"ToBeDone");
    else if(type == BadThreeFork) ips114_showstr(x,y,"BadThreeFork");
    else if(type == Turn) ips114_showstr(x,y,"Turn      ");
    else if(type == Stop) ips114_showstr(x,y,"Stop      ");
    else ips114_showstr(x,y,"Cannot Tell");
}
