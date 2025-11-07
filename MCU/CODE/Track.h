/*********************************************************************************************************************
* @file             Track.h
* @author           Andreas WH
* @brief            路径识别
* @Target core      TC364
* @board            TC364_MainBoard_V1.2
* @date             2022-1-5
********************************************************************************************************************/
#pragma once
#ifndef TRACK_H_
#define TRACK_H_

#include "headfile.h"
#include "IPS.h"
#include "SEEKFREE_IPS114_SPI.h"
#include "SEEKFREE_MT9V03X.h"
//#include "mainInclude.h"

struct imageInformation
{
    //图像相关信息
    uint8 LeftLineCount;
    uint8 RightLineCount;
    uint8 MiddleLineCount;
    uint8 MidLineCount;

    uint8 bottom;
    uint8 top;
    uint8 roadType;
    uint8 nextroadType;
    uint8 turnDir;
    uint8 NoHeadDir;
    uint8 ThreeForkDir;
    uint8 StopFlag;
    int8 differ;



    uint8 LeftLineSum;
    uint8 RightLineSum;
};

static uint8 Straight = 0;
static uint8 NoHead = 1;
static uint8 Cross = 2;
static uint8 ThreeFork = 3;
static uint8 RightCircle = 4;
static uint8 Ramp = 5;
static uint8 TurnLeft = 6;
static uint8 TurnRight = 7;
static uint8 LeftT = 8;
static uint8 RightT = 9;
static uint8 LeftCircle = 10;
static uint8 LeftGarage = 11;
static uint8 RightGarage = 12;
static uint8 BadThreeFork = 13;
static uint8 Crooked = 14;
static uint8 BadCross = 15;
static uint8 Turn = 16;
static uint8 ToBeDone = 19;
static uint8 Stop = 20;


void InfoInit(void);
uint8 FindNext(uint8* LastPoint, uint8 NearNum, uint8 dir);
uint8 FindTurn(uint8* StartPoint, uint8 length, uint8 error, uint8 dir);
uint8 FindTurn1(uint8* StartPoint, uint8 length, uint8 error, uint8 dir);
uint8 FindTurn2(uint8* StartPoint, uint8 length, uint8 error, uint8 dir);
uint8 FindTurnMid(uint8* StartPoint,uint8 length, float error);
uint8 repair(uint8* StartPoint, uint8* EndPoint);
uint8 extend(uint8* StartPoint, uint8* EndPoint);
uint8 extend1(uint8* StartPoint, uint8* EndPoint);
uint8 extend2border(uint8* StartPoint, uint8* EndPoint);
uint16 my_sqrt(uint16 number);
float process_curvity(int x1, int y1, int x2, int y2, int x3, int y3);
void NearScan(void);
void GetMiddle(void);
void GetMid(void);
void SpeedUp(void);
//void FindTurn1(void);
void GetK(void);
void GetLeftCos(void);
void GetRightCos(void);
float FindCos(uint8 AngleY, uint8 AngleX, uint8 LineY1, uint8 LineX1, uint8 LineY2, uint8 LineX2);
void GetK1(void);
void GetWidth(void);
void GarageJudge(void);
void EazyTJudge(void);
uint8 BadThreeForkJudge(void);
void RoadJudge(void);
void NextRoad(void);
//void MiddleRepair(void);
void turn(void);
void nohead1(void);//
//void threefork(void);
void threefork1(void);//
//void badthreefork(void);
//void cross(void);
void cross1(void);//
//void leftt(void);
void leftt1(void);//
//void rightt(void);
void rightt1(void);//
//void rightcircle(void);
void rightcircle1(void);//
//void leftcircle(void);
void leftcircle1(void);//
void crooked(void);
//void badcross1(void);
void badcross(void);//
void ramp(void);
void ErrorInit(void);
uint8 ErrorSum(void);
void LoseLine(void);

#endif
