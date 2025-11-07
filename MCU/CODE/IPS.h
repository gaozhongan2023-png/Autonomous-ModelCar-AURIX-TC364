/*********************************************************************************************************************
* @file             IPS.h
* @author           Andreas WH
* @brief            IPS部分显示函数用于人机交互移植
* @Target core      TC364
* @board            TC364_MainBoard_V1.1
* @date             2021-12-26
********************************************************************************************************************/
#ifndef     _IPS_H_
#define     _IPS_H_

#include "common.h"
#include "SEEKFREE_FONT.h"

void Clr_Screen(unsigned char row);
void clr_Screen(unsigned char row,unsigned char start,unsigned char end);
void print_char8x16(uint16 row,uint16 column,unsigned char reverse,const int8 dat);
void print_str8x16(uint16 row,uint16 column,unsigned char reverse,const int8 dat[]);
void print_Img_color(void);
void create_Img(uint8 thro);
void print_line(uint8 *p, uint16 color, uint8 num);
void print_big_line(uint8 *p, uint16 color, uint8 num);
void print_big_line1(uint8 *p, uint16 color, uint8 num);
void print_big_point(uint8 *p, uint16 color);
void print_road_type(uint16 x, uint16 y ,uint8 type);

#endif
