/*********************************************************************************************************************
* @file             Track.c
* @author           Andreas WH
* @brief            路径识别
* @Target core      TC364
* @board            TC364_MainBoard_V1.2
* @date             2022-5-23
********************************************************************************************************************/
#include "Track.h"
#include <string.h>

#pragma section all "cpu0_dsram"
extern uint8 imageBin[MT9V03X_H][MT9V03X_W];

uint8 NoCircleFlag = 0;
uint8 NoLowFlag = 0;
uint8 NoRampFlag = 1;
uint8 NoGarageFlag = 0;

////1.2
//uint8 ThreeForkForesee[8] = { 40,45,40,45
//                             ,33,33,33,33 };
//uint8 LeftTForesee = 32;
//uint8 RightTForesee = 32;
//uint8 LeftCircleForesee = 20;
//uint8 RightCircleForesee = 20;
//uint8 LGForesee = 80;
//uint8 RGForesee = 80;

////1.7+2.1
uint8 ThreeForkForesee[8] = { 30,37,30,37
                             ,33,33,33,33 };
uint8 LeftTForesee = 30;
uint8 RightTForesee = 30;
uint8 LeftCircleForesee = 20;
uint8 RightCircleForesee = 20;
uint8 LGForesee = 75;
uint8 RGForesee = 75;

////1.9+2.3
//uint8 ThreeForkForesee[8] = { 28,32,28,32
//                             ,33,33,33,33 };
//uint8 LeftTForesee = 23;
//uint8 RightTForesee = 23;
//uint8 LeftCircleForesee = 16;
//uint8 RightCircleForesee = 16;
//uint8 LGForesee = 65;
//uint8 RGForesee = 65;

uint8 ThroLim = 50;

uint8 RampFlag;
uint8 RampInCount;
uint16 RampCount = 5010;
uint8 LowSpeedFlag;

int8 NearBuffer[8][2] = { {0,-1},{-1,-1} ,{-1,0}, {-1,1},{0,1},{1,1},{1,0},{1,-1} };

uint8 LeftLine[256][2] = { {0,0} , };
uint8 RightLine[256][2] = { {0,0} , };
uint8 MiddleLine[256][2] = { {0,0} , };
uint8 MidLine[256][2] = { {0,0} , };

uint8 LeftJudge[20][2] = { {0,0}, };
uint8 LeftTurn[4][3] = { {0,0,0}, };
uint8 RightJudge[20][2] = { {0,0}, };
uint8 RightTurn[4][3] = { {0,0,0}, };
uint8 MiddleTurn[4][3] = { {0,0,0}, };
uint8 MarkPoint[12][2] = { {0,0}, };

float LeftK[8] = { 0, };
float RightK[8] = { 0, };

uint8 RoadTypeSum[20] = { 0, };
uint8 RoadTypeCount = 0;
uint8 LastMiddleLine[100][2] = { {0,0} , };

uint8 TempLine1[256][2] = { {0,0} , };
uint8 TempLine2[256][2] = { {0,0} , };
uint8 TempLine3[256][2] = { {0,0} , };
uint8 TempNum[2] = { 0,0 };
int8 Num[2] = { 0,0 };
uint8 CrossFlag = 0;
uint8 RepairFlag[2] = { 0,0 };
uint16 StopFlag = 0;
uint8 BadCrossFlag = 0;
uint8 BadThreeForkFlag = 0;
uint8 CircleFlag = 0;
uint8 CircleCount = 0;
uint8 GarageFlag = 0;
uint8 GarageCount = 0;
uint8 StraightFlag = 0;
uint8 ThreeForkCount = 0;
uint16 ThreeForkTime = 0;
uint16 BadCrossTime = 0;
uint16 ChangeCount = 0;
uint32 stop_time = 0;
uint8 Thro;

float NoHeadK;
float ThreeForkK = 0;
uint8 ThreeForkFlag = 0;
uint8 ThreeForkOutFlag = 0;
uint8 CirCleOutFlag = 0;
uint8 CrookedOutFlag = 0;
uint8 CrossOutFlag = 0;
uint8 NoHeadFlag = 0;
uint8 LeftTFlag = 0;
uint8 RightTFlag = 0;
uint8 SwitchFlag = 0;
uint8 LeftCloseCount = 0;
uint8 RightCloseCount = 0;
uint8 TurnFlag = 0;
uint8 SpeedFlag = 0;
uint8 LeftMax = 0;
uint8 RightMax = 0;

uint8 LeftStart[2][3] = { {0,0,0}, };
uint8 RightStart[2][3] = { {0,0,0}, };

float LeftCos = 2, RightCos = 2;
float LGCos = 2, RGCos = 2;

uint8 turnpoint[4][2] = { {0,0}, };

float TransBuffer[3][3] = { {158.452381, -0.000000, -0.000000},{154.130952, 25.744681, -0.000000},{0.063690, 0.000000, 1.000000 } };
float InTransBuffer[3][3] = { {0.006311, -0.000000, -0.000000},{-0.037784, 0.038843, -0.000000},{-0.000402, 0.000000, 1.000000 } };
float TransTemp[2] = { 0,0 };
uint8 InTransTemp[2] = { 0,0 };

uint8 ErrorBuffer[256] =
{ 0,0,0,0,0,9,9,9,9,9 //10
,9,9,9,9,9,9,9,9,9,9 //20
,9,9,9,9,9,9,9,9,9,9  //30
,6,6,6,6,6,5,5,5,5,5 //40
,4,4,4,4,4,3,3,3,3,3 //50
,2,2,2,2,2,1,1,1,1,1 //60
,1,1,1,1,1,0,0,0,0,0 //70
,0,0,0,0,0,0,0,0,0,0 //80
,0, };

uint8 ErrorBuffer_Garage[256] =
{ 0,0,0,0,0,9,9,9,9,9 //10
,8,8,8,8,8,7,7,7,7,7 //20
,6,6,6,6,6,5,5,5,5,5  //30
,4,4,4,4,4,3,3,3,3,3 //50
,2,2,2,2,2,1,1,1,1,1
,0,0,0,0,0,0,0,0,0,0 //80
,0,0,0,0,0,0,0,0,0,0 //80
,0,0,0,0,0,0,0,0,0,0 //80
,0, };

//uint8 WidthBuffer[120] =
//{
//    0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B,     //10
//    0x1B, 0x1D, 0x1D, 0x1F, 0x21, 0x21, 0x23, 0x24, 0x25, 0x27,     //20
//    0x27, 0x29, 0x29, 0x2B, 0x2B, 0x2D, 0x2E, 0x2F, 0x31, 0x31,     //30
//    0x33, 0x34, 0x35, 0x36, 0x37, 0x39, 0x39, 0x3B, 0x3B, 0x3D,     //40
//    0x3E, 0x3F, 0x41, 0x41, 0x43, 0x43, 0x45, 0x46, 0x47, 0x48,     //50
//    0x49, 0x4A, 0x4B, 0x4D, 0x4D, 0x4F, 0x4F, 0x51, 0x51, 0x53,     //60
//    0x54, 0x55, 0x56, 0x57, 0x59, 0x59, 0x5B, 0x5B, 0x5D, 0x5D,     //70
//    0x5F, 0x60, 0x60, 0x62, 0x62, 0x64, 0x65, 0x66, 0x67, 0x68,     //80
//    0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x71, 0x72, 0x73,     //90
//    0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7B, 0x7B, 0x7D, 0x7D,     //100
//    0x7F, 0x80, 0x81, 0x81, 0x83, 0x83, 0x84, 0x85, 0x86, 0x87,     //110
//    0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8C, 0x8E, 0x8E, 0x8E, 0x8E,     //120
//};

uint8 WidthBuffer[120] =
{
    0x16, 0x16, 0x16, 0x16, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B,     //10
    0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x23, 0x23, 0x25, 0x25,     //20
    0x27, 0x27, 0x29, 0x29, 0x2B, 0x2B, 0x2D, 0x2D, 0x2F, 0x30,     //30
    0x31, 0x31, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A,     //40
    0x3A, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44,     //50
    0x45, 0x46, 0x48, 0x48, 0x4A, 0x4A, 0x4C, 0x4C, 0x4E, 0x4E,     //60
    0x50, 0x50, 0x52, 0x52, 0x54, 0x54, 0x56, 0x56, 0x58, 0x58,     //70
    0x5A, 0x5A, 0x5C, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x61, 0x62,     //80
    0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C,     //90
    0x6D, 0x6E, 0x6F, 0x71, 0x71, 0x72, 0x73, 0x74, 0x75, 0x77,     //100
    0x77, 0x79, 0x79, 0x7A, 0x7B, 0x7B, 0x7D, 0x7E, 0x7F, 0x7F,     //110
    0x81, 0x81, 0x83, 0x83, 0x85, 0x85, 0x85, 0x85, 0x85, 0x85,     //120
};

uint8 ErrorPlan[20][24] =
{
   {1,9,9,9,9, 9,8,7,6,5, 4,3,2,1,1, 1,1,1,1,1, 0,0,0,0},  //0
   {1,6,7,8,9, 9,9,9,9,8, 7,6,5,4,3, 2,1,1,1,1, 0,0,0,0},  //1
   {1,3,5,6,7, 9,9,9,9,9, 7,7,5,3,2, 1,1,1,1,1, 1,0,0,0},  //2
   {1,1,1,3,5, 6,7,9,9,9, 9,9,8,7,6, 5,4,3,2,1, 1,0,0,0},  //3
   {1,1,1,2,3, 4,5,6,7,7, 9,9,9,9,8, 7,5,4,3,2, 1,0,0,0},  //4
   {1,1,1,1,2, 2,3,5,6,6, 7,9,9,9,9, 8,7,6,5,3, 2,1,0,0},  //5
   {1,1,1,1,1, 1,2,3,5,6, 7,8,9,9,9, 9,8,7,6,5, 3,2,1,0},  //6
   {1,1,1,1,1, 1,1,2,3,4, 5,6,7,8,9, 9,9,9,8,7, 6,5,3,2},  //7
   {1,1,1,1,1, 1,1,1,2,2, 3,3,4,4,5, 7,8,8,9,9, 9,9,7,5},  //8
   {1,1,1,1,1, 1,1,1,1,1, 2,2,3,4,5, 6,7,7,8,8, 9,9,9,9},  //9

   {1,9,9,9,7, 4,2,1,1,1, 1,1,1,1,1, 1,1,1,1,1, 0,0,0,0},  //10
   {1,9,9,9,8, 7,5,3,2,1, 1,1,1,1,1, 1,1,1,1,1, 0,0,0,0},  //11
   {1,9,9,9,9, 9,8,7,5,4, 3,2,1,1,1, 1,1,1,1,1, 1,0,0,0},  //12
   {1,9,9,9,9, 9,9,9,8,7, 6,5,4,3,2, 1,1,1,1,1, 1,0,0,0},  //13
   {1,3,5,9,9, 9,9,9,8,7, 6,5,4,3,2, 1,1,1,1,1, 1,1,1,1},  //14
   {0,0,0,9,9, 9,9,9,8,8, 7,7,6,5,4, 4,3,2,1,1, 1,1,1,1},  //15
   {1,3,5,9,9, 9,9,9,8,8, 7,7,6,5,4, 4,3,3,3,3, 2,2,1,1},  //16
   {1,3,5,9,9, 9,9,9,8,8, 7,7,6,5,4, 4,4,4,4,3, 3,3,2,2},  //17
   {1,3,5,9,9, 9,9,9,9,8, 8,7,7,6,6, 5,5,4,4,4, 4,4,3,3},  //18
   {1,3,5,9,9, 9,9,9,9,8, 8,7,7,6,6, 5,5,5,5,5, 4,4,4,3},  //19
};

uint16 PlanNum = 0;

uint8 P0 = 35;
uint8 D0 = 16;

uint8 P1 = 30;
uint8 P2 = 30;
uint8 P3 = 30;
uint8 P4 = 30;
uint8 P5 = 30;
uint8 P6 = 30;
uint8 D1 = 10;
uint8 D2 = 10;
uint8 D3 = 10;
uint8 D4 = 10;
uint8 D5 = 10;
uint8 D6 = 10;

uint16 ErrorBufferSum = 0;

uint8 CloseFlag;

uint8 MiddleValue = 94;

struct imageInformation imgInfo;

uint8 Mark[4] = { 0, };

int8 CircleMean[50] = { 0, };
uint8 CircleMeanCount;
uint8 SpeedUpCount;

//-------------------------------------------------------------------------------------------------------------------
// @brief       初始化信息
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void InfoInit(void)
{
    imgInfo.bottom = MT9V03X_H - 3;
    imgInfo.top = 0;
    imgInfo.roadType = Straight;
    imgInfo.nextroadType = Straight;
    imgInfo.turnDir = 0;
    imgInfo.differ = 0;
    imgInfo.LeftLineCount = 0;
    imgInfo.RightLineCount = 0;
    imgInfo.MiddleLineCount = 0;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       寻找下一个有效赛道点
// @param       StartPoint          起始点的地址（如LeftLine[0]）
// @param       NearNum             起始的寻找位置（参考邻域数组）
// @param       dir                 寻找方向：0，顺时针；1，逆时针
// @return      uint8               找到的邻域点的位置：0~8，正常；255，未找到
// Sample usage:           uint8 NearNum=2; NearNum = FindNext(LeftLine[0],NearNum,0);//寻找LeftLine[0]的邻域点，起始位置为2，方向为顺时针
//-------------------------------------------------------------------------------------------------------------------
uint8 FindNext(uint8* StartPoint, uint8 NearNum, uint8 dir)
{
    uint8 i = NearNum;
    uint8 count = 0;        //迭代计数器，若超过8说明未找到
    if (!dir)
    {
        while (count < 8)
        {
            if (i == 0)         //NearNum=0需要跨越
            {
                //顺时针寻找黑白跳变邻域点
                if (imageBin[*StartPoint + NearBuffer[0][0]][*(StartPoint + 1) + NearBuffer[0][1]] && !imageBin[*StartPoint + NearBuffer[7][0]][*(StartPoint + 1) + NearBuffer[7][1]])
                {
                    *(StartPoint + 2) = *StartPoint + NearBuffer[0][0];             //将邻域点纵坐标赋值给下一个点
                    *(StartPoint + 3) = *(StartPoint + 1) + NearBuffer[0][1];       //将邻域点横坐标赋值给下一个点
                    return 0;
                }
                count++;
                i++;
            }
            else
            {
                if (imageBin[*StartPoint + NearBuffer[i][0]][*(StartPoint + 1) + NearBuffer[i][1]] && !imageBin[*StartPoint + NearBuffer[i - 1][0]][*(StartPoint + 1) + NearBuffer[i - 1][1]])
                {
                    *(StartPoint + 2) = *StartPoint + NearBuffer[i][0];
                    *(StartPoint + 3) = *(StartPoint + 1) + NearBuffer[i][1];
                    return i;
                }
                count++;
                i++;
                if (i == 8) i = 0;  //NearNum=7需要跨越
            }
        }
    }
    else
    {
        while (count < 8)
        {
            if (i == 7)         //NearNum=7需要跨越
            {
                //逆时针寻找黑白跳变邻域点
                if (imageBin[*StartPoint + NearBuffer[7][0]][*(StartPoint + 1) + NearBuffer[7][1]] && !imageBin[*StartPoint + NearBuffer[0][0]][*(StartPoint + 1) + NearBuffer[0][1]])
                {
                    *(StartPoint + 2) = *StartPoint + NearBuffer[7][0];
                    *(StartPoint + 3) = *(StartPoint + 1) + NearBuffer[7][1];
                    return 0;
                }
                count++;
                i--;
            }
            else
            {
                if (imageBin[*StartPoint + NearBuffer[i][0]][*(StartPoint + 1) + NearBuffer[i][1]] && !imageBin[*StartPoint + NearBuffer[i + 1][0]][*(StartPoint + 1) + NearBuffer[i + 1][1]])
                {
                    *(StartPoint + 2) = *StartPoint + NearBuffer[i][0];
                    *(StartPoint + 3) = *(StartPoint + 1) + NearBuffer[i][1];
                    return i;
                }
                count++;
                if (i == 0) i = 8;  //NearNum=0需要跨越
                i--;
            }
        }
    }
    return 99;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       对起始点进行邻域扫描，得到线段
// @param       StartPoint          起始点的地址（如LeftLine[0]）
// @param       NearNum             起始的寻找位置（参考邻域数组）
// @param       dir                 寻找方向：0，顺时针；1，逆时针
// @return      uint8               线段中点的数量
// Sample usage:           uint8 NearNum=2; NearNum = ScanLine(LeftLine[0],NearNum,0);//扫描LeftLine[0]的邻域线段，起始位置为2，方向为顺时针
//-------------------------------------------------------------------------------------------------------------------
uint8 ScanLine(uint8* StartPoint, uint8 NearNum, uint8 dir)
{
    uint8 count = 0;
    uint8 StartY = *StartPoint;
    uint8 StartX = *(StartPoint + 1);
    if (!dir)
    {
        while (*(StartPoint + count * 2) > 3 && *(StartPoint + count * 2 + 1) > 3 && *(StartPoint + count * 2 + 1) < 184 && count < 250)
        {
            if (NearNum < 2) NearNum = FindNext(StartPoint + count * 2, NearNum + 6, 0);
            else NearNum = FindNext(StartPoint + count * 2, NearNum - 2, 0);

            count++;
            if (NearNum == 255) break;
            if (StartY == *(StartPoint + count * 2) && StartX == *(StartPoint + count * 2 + 1)) break;
        }
        return count;
    }
    else
    {
        while (*(StartPoint + count * 2) > 3 && *(StartPoint + count * 2 + 1) > 3 && *(StartPoint + count * 2 + 1) < 184 && count < 250)
        {
            if (NearNum >= 6) NearNum = FindNext(StartPoint + count * 2, NearNum - 6, 1);
            else NearNum = FindNext(StartPoint + count * 2, NearNum + 2, 1);

            count++;
            if (NearNum == 255) break;
            if (StartY == *(StartPoint + count * 2) && StartX == *(StartPoint + count * 2 + 1)) break;
        }
        return count;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       根据左右线拟合中线
// @param       LeftCount           使用左线点的数量
// @param       RightCount          使用右线点的数量
// @return      uint8               拟合得到中线点的数量
// Sample usage:           FindMiddleLine(imgInfo.LeftLineCount,imgInfo.RightLineCount);//选取左右线拟合中线
//-------------------------------------------------------------------------------------------------------------------
void FindMiddleLine(uint8 LeftCount, uint8 RightCount)
{
    uint8 i;

    if (LeftCount <= RightCount)
    {
        for (i = 0; i < LeftCount; i++)
        {
            MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
            MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
        }
        for (i = LeftCount; i < RightCount; i++)
        {
            MiddleLine[i][0] = (LeftLine[LeftCount][0] + RightLine[i][0]) / 2;
            MiddleLine[i][1] = (LeftLine[LeftCount][1] + RightLine[i][1]) / 2;
        }
    }
    else
    {
        for (i = 0; i < RightCount; i++)
        {
            MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
            MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
        }
        for (i = RightCount; i < LeftCount; i++)
        {
            MiddleLine[i][0] = (LeftLine[i][0] + RightLine[RightCount][0]) / 2;
            MiddleLine[i][1] = (LeftLine[i][1] + RightLine[RightCount][1]) / 2;
        }
    }
    imgInfo.MiddleLineCount = i - 1;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       寻找一条线的拐点(斜率)
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 FindTurn(uint8* StartPoint, uint8 length, uint8 error, uint8 dir)
{
    if (*StartPoint == 0) return 0;
    int8 i, j, k, l;
    uint8 temp[2] = { 0,0 };
    i = *(StartPoint + length * 2) - *StartPoint;
    j = *(StartPoint + length * 2 + 1) - *(StartPoint + 1);
    l = 0;
    if (!dir)
    {
        if (i >= 0)
        {
            for (k = length; k <= 120; k++)
            {
                temp[0] = *StartPoint + k;
                temp[1] = (float)j * k / i + *(StartPoint + 1);
                while (*(StartPoint + k * 2 + l * 2) != temp[0] && *(StartPoint + k * 2 + l) != 0)
                {
                    l++;
                }
                if (*(StartPoint + k * 2 + l * 2 + 1) - temp[1]< -error || *(StartPoint + k * 2 + l * 2 + 1) - temp[1] > error) return k + l;
            }
            return 0;
        }
        else
        {
            for (k = length; k <= 120; k++)
            {
                temp[0] = *StartPoint - k;
                temp[1] = (float)j * k / -i + *(StartPoint + 1);
                while (*(StartPoint + k * 2 + l * 2) != temp[0] && *(StartPoint + k * 2 + l) != 0)
                {
                    l++;
                }
                if (*(StartPoint + k * 2 + l * 2 + 1) - temp[1]< -error || *(StartPoint + k * 2 + l * 2 + 1) - temp[1] > error) return k + l;
            }
            return 0;
        }
    }
    else
    {
        if (j >= 0)
        {
            for (k = length; k <= 120; k++)
            {
                temp[1] = *(StartPoint + 1) + k;
                temp[0] = (float)i * k / j + *StartPoint;
                while (*(StartPoint + k * 2 + l * 2 + 1) != temp[1] && *(StartPoint + k * 2 + l + 1) != 0)
                {
                    l++;
                }
                if (*(StartPoint + k * 2 + l * 2) - temp[0]< -error || *(StartPoint + k * 2 + l * 2) - temp[0] > error) return k + l;
            }
            return 0;
        }
        else
        {
            for (k = length; k <= 120; k++)
            {
                temp[1] = *(StartPoint + 1) - k;
                temp[0] = (float)i * k / -j + *StartPoint;
                while (*(StartPoint + k * 2 + l * 2 + 1) != temp[1] && *(StartPoint + k * 2 + l + 1) != 0)
                {
                    l++;
                }
                if (*(StartPoint + k * 2 + l * 2) - temp[0]< -error || *(StartPoint + k * 2 + l * 2) - temp[0] > error) return k + l;
            }
            return 0;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       寻找一条线的拐点(偏移)
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 FindTurn1(uint8* StartPoint, uint8 length, uint8 error, uint8 dir)
{
    int8 now = 0;
    int8 next = 0;
    uint8 i, j, output;
    output = 0;

    next = *(StartPoint + length * 2 + 1) - *(StartPoint + 1);
    for (i = 2; i < 40; i++)
    {
        now = next;
        if (*(StartPoint + length * 2 * i + 1) == 0) break;
        next = *(StartPoint + length * 2 * i + 1) - *(StartPoint + length * 2 * (i - 1) + 1);
        if ((next - now) > error || (next - now) < -error)
        {
            output = length * (i - 2);
            for (j = length * (i - 2); j < length * i; j++)
            {
                if (!dir)
                {
                    if (*(StartPoint + output * 2 + 1) <= *(StartPoint + j * 2 + 3)) output = j + 1;
                }
                else
                {
                    if (*(StartPoint + output * 2 + 1) >= *(StartPoint + j * 2 + 3)) output = j + 1;
                }
            }
            return output;
        }
    }
    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       寻找一条线的拐点(偏移)
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 FindTurn2(uint8* StartPoint, uint8 length, uint8 error, uint8 dir)
{
    int8 now = 0;
    int8 next = 0;
    uint8 i, j, output;
    output = 0;

    next = *(StartPoint + length * 2 + 1) - *(StartPoint + 1);
    for (i = 2; i < 40; i++)
    {
        now = next;
        if (*(StartPoint + length * 2 * i + 1) == 0) break;
        next = *(StartPoint + length * 2 * i + 1) - *(StartPoint + length * 2 * (i - 1) + 1);
        if ((now > 0 && next > 0) && !dir) continue;
        if ((now < 0 && next < 0) && dir) continue;
        if ((next - now) > error || (next - now) < -error)
        {
            output = length * (i - 2);
            for (j = length * (i - 2); j < length * i; j++)
            {
                if (!dir)
                {
                    if (*(StartPoint + output * 2 + 1) <= *(StartPoint + j * 2 + 3)) output = j + 1;
                }
                else
                {
                    if (*(StartPoint + output * 2 + 1) >= *(StartPoint + j * 2 + 3)) output = j + 1;
                }
            }
            return output;
        }
    }
    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       寻找中线的拐点(斜率)
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 FindMidTurn(uint8* StartPoint, uint8 length, float error)
{
    uint8 i;

    float k1, k2;
    k2 = (float)(*(StartPoint + length * 2 + 1) - *(StartPoint + 1)) / (float)(*(StartPoint + length * 2) - *StartPoint);

    for (i = 2; i < 40; i++)
    {
        k1 = k2;
        if (*(StartPoint + length * 2 * i) == 0) break;
        k2 = (float)(*(StartPoint + length * 2 * i + 1) - *(StartPoint + 1)) / (float)(*(StartPoint + length * 2 * i) - *StartPoint);
        if (k1 - k2 > error || k1 - k2 < -error) return (i - 1) * length;
    }
    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       寻找边线的拐点(八方向)
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 FindLineTurn(uint8* StartPoint, uint8 length, uint8 error)
{
    uint8 i, flag = 0;
    int8 value = 0;

    for (i = 1; i < 40; i++)
    {
        if (!*(StartPoint + length * 2 * i + 1)) return 0;
        value = *(StartPoint + length * 2 * i + 1) - *(StartPoint + length * 2 * (i - 1) + 1);
        if (value < -error)
        {
            if (!flag) flag = 1;
            if (flag == 2) return length * (i - 1);
        }
        if (value > error)
        {
            if (!flag) flag = 2;
            if (flag == 1) return length * (i - 1);
        }
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       根据两点进行补线
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 repair(uint8* StartPoint, uint8* EndPoint)
{
    if (*StartPoint == 0 || *EndPoint == 0) return 0;
    int8 i, j, k;
    i = *EndPoint - *StartPoint;
    j = *(EndPoint + 1) - *(StartPoint + 1);
    if (i >= 0)
    {
        for (k = 1; k < i; k++)
        {
            *(StartPoint + k * 2) = *StartPoint + k;
            *(StartPoint + k * 2 + 1) = (float)j * k / i + *(StartPoint + 1);
        }
        *(StartPoint + i * 2) = *EndPoint;
        *(StartPoint + i * 2 + 1) = *(EndPoint + 1);
        return i;
    }
    else
    {
        for (k = 1; k < -i; k++)
        {
            *(StartPoint + k * 2) = *StartPoint - k;
            *(StartPoint + k * 2 + 1) = (float)-j * k / i + *(StartPoint + 1);
        }
        *(StartPoint - i * 2) = *EndPoint;
        *(StartPoint - i * 2 + 1) = *(EndPoint + 1);
        return -i;
    }
    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       根据两点进行延长
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 extend(uint8* StartPoint, uint8* EndPoint)
{
    if (*StartPoint == 0 || *EndPoint == 0) return 0;

    int8 i, j;
    uint8 k;
    i = *EndPoint - *StartPoint;
    j = *(EndPoint + 1) - *(StartPoint + 1);
    if (i >= 0)
    {
        for (k = 1; k < 200; k++)
        {
            if (*EndPoint + k > 117 || (float)j * k / i + *(EndPoint + 1) > 184 || (float)j * k / i + *(EndPoint + 1) < 3) break;
            *(EndPoint + k * 2) = *EndPoint + k;
            *(EndPoint + k * 2 + 1) = (float)j * k / i + *(EndPoint + 1);
        }
        return k;
    }
    else
    {
        for (k = 1; k < 200; k++)
        {
            if (*EndPoint - k < 10 || (float)-j * k / i + *(EndPoint + 1) > 184 || (float)-j * k / i + *(EndPoint + 1) < 3) break;
            *(EndPoint + k * 2) = *EndPoint - k;
            *(EndPoint + k * 2 + 1) = (float)-j * k / i + *(EndPoint + 1);
        }
        return k;
    }
    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       根据两点进行延长
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 extend1(uint8* StartPoint, uint8* EndPoint)
{
    if (*StartPoint == 0 || *EndPoint == 0) return 0;

    int16 temp;

    int8 i, j;
    uint8 k;
    i = *EndPoint - *StartPoint;
    j = *(EndPoint + 1) - *(StartPoint + 1);

    if (j >= 0)
    {
        for (k = 1; k < 200; k++)
        {
            if (*(EndPoint + 1) + k > 184 || (float)i * k / j + *EndPoint > 117 || (float)i * k / j + *EndPoint < 3) break;
            *(EndPoint + k * 2 + 1) = *(EndPoint + 1) + k;
            *(EndPoint + k * 2) = (float)i * k / j + *EndPoint;
        }
        return k;
    }
    else
    {
        for (k = 1; k < 200; k++)
        {
            if (*(EndPoint + 1) - k < 3 || (float)-i * k / j + *EndPoint > 117 || (float)-i * k / j + *EndPoint < 3) break;
            *(EndPoint + k * 2 + 1) = *(EndPoint + 1) - k;
            *(EndPoint + k * 2) = (float)-i * k / j + *EndPoint;
            temp = *(EndPoint + 1) - k;
            temp = (float)-i * k / j + *EndPoint;
        }
        return k;
    }
    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       根据两点进行延长
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint8 extend2border(uint8* StartPoint, uint8* EndPoint)
{
    if (*StartPoint == 0 || *EndPoint == 0) return 0;

    int8 i, j;
    uint8 k, l = 0;
    i = *EndPoint - *StartPoint;
    j = *(EndPoint + 1) - *(StartPoint + 1);
    if (i >= 0)
    {
        for (k = 1; k < 200; k++)
        {
            if (*EndPoint + k > 119 || (float)j * k / i + *(EndPoint + 1) > 187 || (float)j * k / i + *(EndPoint + 1) < 1) break;
            *(EndPoint + k * 2) = *EndPoint + k;
            *(EndPoint + k * 2 + 1) = (float)j * k / i + *(EndPoint + 1);
            if (!imageBin[*(EndPoint + k * 2)][*(EndPoint + k * 2 + 1)])
            {
                k--;
                l = *(EndPoint + k * 2 + 1);
                break;
            }
        }
        while (imageBin[*(EndPoint + k * 2)][*(EndPoint + k * 2 + 1)])
        {
            k++;
            *(EndPoint + k * 2) = *EndPoint + k;
            *(EndPoint + k * 2 + 1) = l;
        }
        *(EndPoint + k * 2) = 0;
        *(EndPoint + k * 2 + 1) = 0;
        return k;
    }
    else
    {
        for (k = 1; k < 200; k++)
        {
            if (*EndPoint - k < 1 || (float)-j * k / i + *(EndPoint + 1) > 187 || (float)-j * k / i + *(EndPoint + 1) < 1) break;
            *(EndPoint + k * 2) = *EndPoint - k;
            *(EndPoint + k * 2 + 1) = (float)-j * k / i + *(EndPoint + 1);
            if (!imageBin[*(EndPoint + k * 2)][*(EndPoint + k * 2 + 1)])
            {
                k--;
                l = *(EndPoint + k * 2 + 1);
                break;
            }
        }
        while (imageBin[*(EndPoint + k * 2)][*(EndPoint + k * 2 + 1)])
        {
            k++;
            *(EndPoint + k * 2) = *EndPoint - k;
            *(EndPoint + k * 2 + 1) = l;
        }
        *(EndPoint + k * 2) = 0;
        *(EndPoint + k * 2 + 1) = 0;
        return k;
    }
    return 0;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       开平方根简算版
// @param
// @return
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
uint16 my_sqrt(uint16 number)
{
    uint8 ans = 0, p = 0x80;
    while (p != 0)
    {
        ans += p;
        if (ans * ans > number)
        {
            ans -= p;
        }
        p = (uint8)(p / 2);
    }
    return(ans);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       三点求曲率
// @param       x1 y1 x2 y2 x3 y3 三点
// @return      曲率
// Sample usage:            qulv=process_curvity(1,3,5,5,7,8);
//-------------------------------------------------------------------------------------------------------------------
float process_curvity(int x1, int y1, int x2, int y2, int x3, int y3)   //三点求曲率
{
    float K;
    int S_of_ABC = ((x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1)) / 2;
    //面积的符号表示方向
    int q1 = (int)((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
    int AB = my_sqrt(q1);
    q1 = (int)((x3 - x2) * (x3 - x2) + (y3 - y2) * (y3 - y2));
    int BC = my_sqrt(q1);
    q1 = (int)((x3 - x1) * (x3 - x1) + (y3 - y1) * (y3 - y1));
    int AC = my_sqrt(q1);
    if (AB * BC * AC == 0)
    {
        K = 0;
    }
    else
        K = (float)4 * S_of_ABC / (AB * BC * AC);
    return K;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       对一点求逆透视变换后坐标
// @param       x 纵坐标，y 横坐标
// @return      void
// Sample usage:            Trans(0,93);
//-------------------------------------------------------------------------------------------------------------------
void Trans(uint8 x, uint8 y)
{
    x++;
    y++;
    TransTemp[0] = (x * TransBuffer[0][0] + y * TransBuffer[0][1] + TransBuffer[0][2]) / (x * TransBuffer[2][0] + y * TransBuffer[2][1] + TransBuffer[2][2]);
    TransTemp[1] = (x * TransBuffer[1][0] + y * TransBuffer[1][1] + TransBuffer[1][2]) / (x * TransBuffer[2][0] + y * TransBuffer[2][1] + TransBuffer[2][2]);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       对逆透视变换后坐标反变换
// @return      void
// Sample usage:            InTrans();
//-------------------------------------------------------------------------------------------------------------------
void InTrans(void)
{
    float x, y;
    x = TransTemp[0];
    y = TransTemp[1];
    InTransTemp[0] = (uint8)((x * InTransBuffer[0][0] + y * InTransBuffer[0][1] + InTransBuffer[0][2]) / (x * InTransBuffer[2][0] + y * InTransBuffer[2][1] + InTransBuffer[2][2]));
    InTransTemp[1] = (uint8)((x * InTransBuffer[1][0] + y * InTransBuffer[1][1] + InTransBuffer[1][2]) / (x * InTransBuffer[2][0] + y * InTransBuffer[2][1] + InTransBuffer[2][2]));
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       计算斜率
// @param       无
// @return      void
// Sample usage:            GetK();//计算斜率
//-------------------------------------------------------------------------------------------------------------------
void GetK(void)
{
    uint8 i;

    if (LeftLine[0][0])
    {
        if (LeftLine[imgInfo.LeftLineCount + 20][0] != 0)
        {
            if (LeftLine[imgInfo.LeftLineCount + 20][1] == LeftTurn[0][1]) LeftK[0] = 0;
            else if (LeftLine[imgInfo.LeftLineCount + 20][0] == LeftTurn[0][0]) LeftK[0] = 20;
            else LeftK[0] = ((float)LeftLine[imgInfo.LeftLineCount + 20][1] - LeftTurn[0][1]) / ((float)LeftLine[imgInfo.LeftLineCount + 20][0] - LeftTurn[0][0]);
        }
        else
        {
            i = imgInfo.LeftLineCount;
            while (LeftLine[i + 1][0] != 0 && i < 200) i++;
            if (LeftLine[i][1] == LeftTurn[0][1]) LeftK[0] = 0;
            else if (LeftLine[i][0] == LeftTurn[0][0]) LeftK[0] = 20;
            else LeftK[0] = ((float)LeftLine[i][1] - LeftTurn[0][1]) / ((float)LeftLine[i][0] - LeftTurn[0][0]);
        }

        if (LeftLine[imgInfo.LeftLineSum][0] != 0)
        {
            if (LeftLine[imgInfo.LeftLineSum][1] == LeftLine[0][1]) LeftK[1] = 0;
            else if (LeftLine[imgInfo.LeftLineSum][0] == LeftLine[0][0]) LeftK[1] = 20;
            else LeftK[1] = ((float)LeftLine[imgInfo.LeftLineSum][1] - LeftLine[0][1]) / ((float)LeftLine[imgInfo.LeftLineSum][0] - LeftLine[0][0]);
        }
    }
    else
    {
        LeftK[0] = 0;
        LeftK[1] = 0;
    }

    if (RightLine[0][0])
    {
        if (RightLine[imgInfo.RightLineCount + 20][0] != 0)
        {
            if (RightLine[imgInfo.RightLineCount + 20][1] == RightTurn[0][1]) RightK[0] = 0;
            else if (RightLine[imgInfo.RightLineCount + 20][0] == RightTurn[0][0]) RightK[0] = -20;
            else RightK[0] = ((float)RightLine[imgInfo.RightLineCount + 20][1] - RightTurn[0][1]) / ((float)RightLine[imgInfo.RightLineCount + 20][0] - RightTurn[0][0]);
        }
        else
        {
            i = imgInfo.RightLineCount;
            while (RightLine[i + 1][0] != 0 && i < 200) i++;
            if (RightLine[i][1] == RightTurn[0][1]) RightK[0] = 0;
            else if (RightLine[i][0] == RightTurn[0][0]) RightK[0] = -20;
            else RightK[0] = ((float)RightLine[i][1] - RightTurn[0][1]) / ((float)RightLine[i][0] - RightTurn[0][0]);
        }

        if (RightLine[imgInfo.RightLineSum][0] != 0)
        {
            if (RightLine[imgInfo.RightLineSum][1] == RightLine[0][1]) RightK[1] = 0;
            else if (RightLine[imgInfo.RightLineSum][0] == RightLine[0][0]) RightK[1] = 20;
            else RightK[1] = ((float)RightLine[imgInfo.RightLineSum][1] - RightLine[0][1]) / ((float)RightLine[imgInfo.RightLineSum][0] - RightLine[0][0]);
        }
    }
    else
    {
        RightK[0] = 0;
        RightK[1] = 0;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       计算斜率
// @param       无
// @return      void
// Sample usage:            GetK1();//计算斜率
//-------------------------------------------------------------------------------------------------------------------
void GetK1(void)
{
    if (LeftTurn[0][0] != 0)
    {
        if (LeftTurn[0][0] - LeftLine[imgInfo.LeftLineCount][0])
        {
            LeftK[0] = (float)(LeftTurn[0][1] - LeftLine[imgInfo.LeftLineCount][1]) / (LeftTurn[0][0] - LeftLine[imgInfo.LeftLineCount][0]);
        }
        else
        {
            if (LeftTurn[0][1] - LeftLine[imgInfo.LeftLineCount][1]) LeftK[0] = 10;
            else LeftK[0] = -10;
        }
    }
    else
    {
        if (LeftJudge[10][0] - LeftLine[imgInfo.LeftLineCount][0])
        {
            LeftK[0] = (float)(LeftJudge[10][1] - LeftLine[imgInfo.LeftLineCount][1]) / (LeftJudge[10][0] - LeftLine[imgInfo.LeftLineCount][0]);
        }
        else
        {
            if (LeftJudge[10][1] - LeftLine[imgInfo.LeftLineCount][1]) LeftK[0] = 10;
            else LeftK[0] = -10;
        }
    }
    if (RightTurn[0][0] != 0)
    {
        if (RightTurn[0][0] - RightLine[imgInfo.RightLineCount][0])
        {
            RightK[0] = (float)(RightTurn[0][1] - RightLine[imgInfo.RightLineCount][1]) / (RightTurn[0][0] - RightLine[imgInfo.RightLineCount][0]);
        }
        else
        {
            if (RightTurn[0][0] - RightLine[imgInfo.RightLineCount][0]) RightK[0] = 10;
            else RightK[0] = -10;
        }
    }
    else
    {
        if (RightJudge[10][0] - RightLine[imgInfo.RightLineCount][0])
        {
            RightK[0] = (float)(RightJudge[10][1] - RightLine[imgInfo.RightLineCount][1]) / (RightJudge[10][0] - RightLine[imgInfo.RightLineCount][0]);
        }
        else
        {
            if (RightJudge[10][1] - RightLine[imgInfo.RightLineCount][1]) RightK[0] = 10;
            else RightK[0] = -10;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       计算斜率
// @param       无
// @return      void
// Sample usage:            GetK1();//计算斜率
//-------------------------------------------------------------------------------------------------------------------
//void GetWidth(void)
//{
//    uint8 i,j;
//
//    uart_putchar(UART_3,0x00);
//    uart_putchar(UART_3,0x00);
//    uart_putchar(UART_3,0x00);
//
//    j=0;
//    for(i=imgInfo.LeftLineSum;i>=1;i--)
//    {
//        if(RightLine[i][0]) uart_putchar(UART_3,RightLine[i][1] - LeftLine[i][1]);
//    }
//
//    uart_putchar(UART_3,0xff);
//    uart_putchar(UART_3,0xff);
//    uart_putchar(UART_3,0xff);
//}

//-------------------------------------------------------------------------------------------------------------------
// @brief       计算左余弦
// @param       无
// @return      void
// Sample usage:            GetLeftCos();//
//-------------------------------------------------------------------------------------------------------------------
void GetLeftCos(void)
{
    int16 Y_Move, y_Move, X_Move, x_Move;

    if (LeftLine[imgInfo.LeftLineCount + 20][0] != 0 && LeftLine[imgInfo.LeftLineCount - 20][0] != 0)
    {
        if (LeftLine[imgInfo.LeftLineCount + 30][0])
        {
            Y_Move = LeftLine[imgInfo.LeftLineCount + 30][0] - LeftLine[imgInfo.LeftLineCount + 10][0];
            X_Move = LeftLine[imgInfo.LeftLineCount + 30][1] - LeftLine[imgInfo.LeftLineCount + 10][1];
        }
        else if (LeftLine[imgInfo.LeftLineCount + 25][0])
        {
            Y_Move = LeftLine[imgInfo.LeftLineCount + 25][0] - LeftLine[imgInfo.LeftLineCount + 5][0];
            X_Move = LeftLine[imgInfo.LeftLineCount + 25][1] - LeftLine[imgInfo.LeftLineCount + 5][1];
        }
        else if (LeftLine[imgInfo.LeftLineCount + 20][0])
        {
            Y_Move = LeftLine[imgInfo.LeftLineCount + 20][0] - LeftLine[imgInfo.LeftLineCount + 5][0];
            X_Move = LeftLine[imgInfo.LeftLineCount + 20][1] - LeftLine[imgInfo.LeftLineCount + 5][1];
        }
        else
        {
            Y_Move = LeftLine[imgInfo.LeftLineSum][0] - LeftLine[imgInfo.LeftLineCount][0];
            X_Move = LeftLine[imgInfo.LeftLineSum][1] - LeftLine[imgInfo.LeftLineCount][1];
        }


        if (imgInfo.LeftLineCount > 30)
        {
            y_Move = LeftLine[imgInfo.LeftLineCount - 30][0] - LeftLine[imgInfo.LeftLineCount][0];
            x_Move = LeftLine[imgInfo.LeftLineCount - 30][1] - LeftLine[imgInfo.LeftLineCount][1];
        }
        else if (imgInfo.LeftLineCount > 25)
        {
            y_Move = LeftLine[imgInfo.LeftLineCount - 25][0] - LeftLine[imgInfo.LeftLineCount][0];
            x_Move = LeftLine[imgInfo.LeftLineCount - 25][1] - LeftLine[imgInfo.LeftLineCount][1];
        }
        else if (imgInfo.LeftLineCount > 20)
        {
            y_Move = LeftLine[imgInfo.LeftLineCount - 20][0] - LeftLine[imgInfo.LeftLineCount][0];
            x_Move = LeftLine[imgInfo.LeftLineCount - 20][1] - LeftLine[imgInfo.LeftLineCount][1];
        }
        else
        {
            y_Move = LeftLine[0][0] - LeftLine[imgInfo.LeftLineCount][0];
            x_Move = LeftLine[0][1] - LeftLine[imgInfo.LeftLineCount][1];
        }

        LeftCos = FindCos(LeftLine[imgInfo.LeftLineCount][0], LeftLine[imgInfo.LeftLineCount][1],
            LeftLine[imgInfo.LeftLineCount][0] + Y_Move, LeftLine[imgInfo.LeftLineCount][1] + X_Move,
            LeftLine[imgInfo.LeftLineCount][0] + y_Move, LeftLine[imgInfo.LeftLineCount][1] + x_Move);

        if (LeftCos < 0.6 && LeftCos>0.4 && LeftTurn[0][1] < 94 && LeftTurn[0][0] > 50)
        {
            X_Move = FindTurn1(LeftLine[imgInfo.LeftLineCount], 5, 1, 0);
            if (!X_Move || X_Move > 25) RepairFlag[0] = 1;
        }

        LGCos = LeftCos;
        if (LeftTurn[0][0] < 30) LeftCos = 2;
    }
    else LeftCos = 2;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       计算右余弦
// @param       无
// @return      void
// Sample usage:            GetRightCos();//
//-------------------------------------------------------------------------------------------------------------------
void GetRightCos(void)
{
    int16 Y_Move, y_Move, X_Move, x_Move;

    if (RightLine[imgInfo.RightLineCount + 20][0] != 0 && RightLine[imgInfo.RightLineCount - 20][0] != 0)
    {
        if (RightLine[imgInfo.RightLineCount + 30][0])
        {
            Y_Move = RightLine[imgInfo.RightLineCount + 30][0] - RightLine[imgInfo.RightLineCount + 10][0];
            X_Move = RightLine[imgInfo.RightLineCount + 30][1] - RightLine[imgInfo.RightLineCount + 10][1];
        }
        else if (RightLine[imgInfo.RightLineCount + 25][0])
        {
            Y_Move = RightLine[imgInfo.RightLineCount + 25][0] - RightLine[imgInfo.RightLineCount + 5][0];
            X_Move = RightLine[imgInfo.RightLineCount + 25][1] - RightLine[imgInfo.RightLineCount + 5][1];
        }
        else if (RightLine[imgInfo.RightLineCount + 20][0])
        {
            Y_Move = RightLine[imgInfo.RightLineCount + 20][0] - RightLine[imgInfo.RightLineCount + 5][0];
            X_Move = RightLine[imgInfo.RightLineCount + 20][1] - RightLine[imgInfo.RightLineCount + 5][1];
        }
        else
        {
            Y_Move = RightLine[imgInfo.RightLineSum][0] - RightLine[imgInfo.RightLineCount][0];
            X_Move = RightLine[imgInfo.RightLineSum][1] - RightLine[imgInfo.RightLineCount][1];
        }


        if (imgInfo.RightLineCount > 30)
        {
            y_Move = RightLine[imgInfo.RightLineCount - 30][0] - RightLine[imgInfo.RightLineCount][0];
            x_Move = RightLine[imgInfo.RightLineCount - 30][1] - RightLine[imgInfo.RightLineCount][1];
        }
        else if (imgInfo.RightLineCount > 25)
        {
            y_Move = RightLine[imgInfo.RightLineCount - 25][0] - RightLine[imgInfo.RightLineCount][0];
            x_Move = RightLine[imgInfo.RightLineCount - 25][1] - RightLine[imgInfo.RightLineCount][1];
        }
        else if (imgInfo.RightLineCount > 20)
        {
            y_Move = RightLine[imgInfo.RightLineCount - 20][0] - RightLine[imgInfo.RightLineCount][0];
            x_Move = RightLine[imgInfo.RightLineCount - 20][1] - RightLine[imgInfo.RightLineCount][1];
        }
        else
        {
            y_Move = RightLine[0][0] - RightLine[imgInfo.RightLineCount][0];
            x_Move = RightLine[0][1] - RightLine[imgInfo.RightLineCount][1];
        }

        RightCos = FindCos(RightLine[imgInfo.RightLineCount][0], RightLine[imgInfo.RightLineCount][1],
            RightLine[imgInfo.RightLineCount][0] + Y_Move, RightLine[imgInfo.RightLineCount][1] + X_Move,
            RightLine[imgInfo.RightLineCount][0] + y_Move, RightLine[imgInfo.RightLineCount][1] + x_Move);

        if (RightCos < 0.6 && RightCos > 0.4 && RightTurn[0][1] > 94 && RightTurn[0][0] > 50)
        {
            X_Move = FindTurn1(RightLine[imgInfo.RightLineCount], 5, 1, 1);
            if (!X_Move || X_Move > 25) RepairFlag[1] = 1;
        }

        RGCos = RightCos;
        if (RightTurn[0][0] < 30)
        {
            RightCos = 2;
        }
    }
    else RightCos = 2;
}


//-------------------------------------------------------------------------------------------------------------------
// @brief       计算余弦
// @param       无
// @return      void
// Sample usage:            GetK();//计算斜率
//-------------------------------------------------------------------------------------------------------------------
float FindCos(uint8 AngleY, uint8 AngleX, uint8 LineY1, uint8 LineX1, uint8 LineY2, uint8 LineX2)
{
    int16 a, b, c, a1, b1, c1;
    float Cos;

    if (LineY1 != 0 && LineY2 != 0)
    {
        a = (LineX1 - AngleX) * (LineX1 - AngleX)
            + (LineY1 - AngleY) * (LineY1 - AngleY);

        b = (LineX2 - AngleX) * (LineX2 - AngleX)
            + (LineY2 - AngleY) * (LineY2 - AngleY);

        c = (LineX1 - LineX2) * (LineX1 - LineX2)
            + (LineY1 - LineY2) * (LineY1 - LineY2);

        if (a != 0 && b != 0 && a != 0)
        {
            a1 = my_sqrt(a);
            b1 = my_sqrt(b);
            c1 = my_sqrt(c);

            Cos = (float)(a + b - c) / a1 / b1 / 2;
        }
        else Cos = 2;
    }
    else Cos = 2;

    return Cos;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       按行扫线
// @param       无
// @return      void
// Sample usage:            NearScan();//寻找有效直道
//-------------------------------------------------------------------------------------------------------------------
void ColScan(void)
{
    uint8 i;
    uint8 j;
    uint8 Count = 0;//扫线序号
    uint8 NearNum = 0;//邻域序号
    uint8 flag1 = 0, flag2 = 0;

    //临近标志位，0：直道；1：靠近；2：紧贴
    CloseFlag = 0;

    memset(LeftLine, 0, 512);
    memset(RightLine, 0, 512);
    memset(MiddleLine, 0, 512);

    memset(LeftTurn, 0, 12);
    memset(RightTurn, 0, 12);
    memset(RepairFlag, 0, 2);
    imgInfo.LeftLineSum = 0;
    imgInfo.RightLineSum = 0;

    Count = 0;
    for (j = 115; j > 5; j--)
    {
        //寻找左起点
        for (i = 3; i < 80; i++)
        {
            if (!imageBin[j][i] && imageBin[j][i + 1] && imageBin[j][i + 2])
            {
                LeftLine[Count][0] = j;
                LeftLine[Count][1] = i + 1;
                flag1 = 1;
                break;
            }
        }

        if (flag1) break;
        else
        {
            LeftLine[Count][0] = j;
            LeftLine[Count][1] = 3;
            Count++;
        }
    }

    if (flag1)
    {
        NearNum = 2;
        //左线邻域扫线
        while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 200)
        {
            if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
            else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);

            Count++;
        }

        imgInfo.LeftLineSum = Count - 1;
    }

    Count = 0;
    j = 115;
    while (j > 5)
    {
        //寻找左起点
        for (i = 184; i > 110; i--)
        {
            if (!imageBin[j][i] && imageBin[j][i - 1] && imageBin[j][i - 2])
            {
                RightLine[Count][0] = j;
                RightLine[Count][1] = i - 1;
                flag2 = 1;
                break;
            }
        }

        if (flag2) break;
        else
        {
            RightLine[Count][0] = j;
            RightLine[Count][1] = 184;
            Count++;
        }
        j--;
    }

    if (flag2)
    {
        NearNum = 2;

        //右线邻域扫线
        while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 200)
        {
            NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

            Count++;
        }
        imgInfo.RightLineSum = Count - 1;
    }

    j = RightLine[Count - 1][0];
    flag2 = 0;
    while (j > 5)
    {
        //寻找左起点
        for (i = 184; i > 160; i--)
        {
            if (!imageBin[j][i] && imageBin[j][i - 1] && imageBin[j][i - 2])
            {
                RightLine[Count][0] = j;
                RightLine[Count][1] = i - 1;
                flag2 = 1;
                break;
            }
        }

        if (flag2) break;
        else
        {
            RightLine[Count][0] = j;
            RightLine[Count][1] = 184;
            Count++;
        }
        j--;
    }

    if (flag2)
    {
        NearNum = 2;

        //右线邻域扫线
        while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 200)
        {
            NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

            Count++;
        }
        imgInfo.RightLineSum = Count - 1;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       寻找有效直道
// @param       无
// @return      void
// Sample usage:            NearScan();//寻找有效直道
//-------------------------------------------------------------------------------------------------------------------
void NearScan(void)
{
    uint8 i;
    uint8 j;
    uint8 Count = 0;//扫线序号
    uint8 NearNum = 0;//邻域序号
    uint8 EndCount = 0;//退出标志位
    uint8 flag1 = 0, flag2 = 0;

    //临近标志位，0：直道；1：靠近；2：紧贴
    CloseFlag = 0;

    memset(LeftLine, 0, 512);
    memset(RightLine, 0, 512);
    memset(MiddleLine, 0, 512);

    memset(LeftTurn, 0, 12);
    memset(RightTurn, 0, 12);
    memset(RepairFlag, 0, 2);

    imgInfo.LeftLineSum = 0;
    imgInfo.RightLineSum = 0;

    LeftCos = 2;
    RightCos = 2;

    ////老版扫线------------------------------------------------------------------------------------------
    for (i = 115; i > 100; i--)
    {
        if (!imageBin[i][15]) break;
    }

    Count = 0;
    for (j = i; j > 70; j--)
    {
        //寻找左起点
        for (i = 3; i < 94; i++)
        {
            if (!imageBin[j][i] && imageBin[j][i + 1] && imageBin[j][i + 2])
            {
                LeftLine[Count][0] = j;
                LeftLine[Count][1] = i + 1;
                flag1 = 1;
                break;
            }
        }

        if (flag1) break;
        else
        {
            LeftLine[Count][0] = j;
            LeftLine[Count][1] = 3;
            Count++;
        }
    }

    if (flag1)
    {
        imgInfo.LeftLineSum = ScanLine(LeftLine[Count], 2, 0) + Count;
        if (imgInfo.LeftLineSum) imgInfo.LeftLineSum -= 1;
    }

    for (i = 115; i > 100; i--)
    {
        if (!imageBin[i][172]) break;
    }

    Count = 0;
    for (j = i; j > 70; j--)
    {
        //寻找左起点
        for (i = 184; i > 94; i--)
        {
            if (!imageBin[j][i] && imageBin[j][i - 1] && imageBin[j][i - 2])
            {
                RightLine[Count][0] = j;
                RightLine[Count][1] = i - 1;
                flag2 = 1;
                break;
            }
        }

        if (flag2) break;
        else
        {
            RightLine[Count][0] = j;
            RightLine[Count][1] = 184;
            Count++;
        }
    }

    if (flag2)
    {
        imgInfo.RightLineSum = ScanLine(RightLine[Count], 2, 1) + Count;
        if (imgInfo.RightLineSum) imgInfo.RightLineSum -= 1;
    }
    ////老版扫线------------------------------------------------------------------------------------------




    //新版扫线------------------------------------------------------------------------------------------
        //对于前15个点，线终点必须跨越100纵坐标，否则认为是黑斑
//    memset(LeftStart, 0, 6);
//    memset(RightStart, 0, 6);
//
//    Count = 0;
//    for (i = 115; i > 100; i--)
//    {
//        if (!imageBin[i][3])
//        {
//            //寻找左起点
//            for (j = 3; j < 94; j++)
//            {
//                if (!imageBin[i][j] && imageBin[i][j + 1] && imageBin[i][j + 2])
//                {
//                    LeftStart[0][0] = i;
//                    LeftStart[0][1] = j + 1;
//                    break;
//                }
//            }
//
//            //扫描左边线
//            if (LeftStart[0][0])
//            {
//                TempLine1[0][0] = LeftStart[0][0];
//                TempLine1[0][1] = LeftStart[0][1];
//                LeftStart[0][2] = ScanLine(TempLine1[0], 2, 0) - 1;
//
//                //根据线终点纵坐标判断是否为黑斑
//                if (TempLine1[LeftStart[0][2] - 3][0] < 100 && TempLine1[LeftStart[0][2] - 3][0] > 3)
//                {
//                    LeftLine[0][0] = LeftStart[0][0];
//                    LeftLine[0][1] = LeftStart[0][1];
//                    imgInfo.LeftLineSum = ScanLine(LeftLine[0], 2, 0) - 1;
//                    flag1 = 1;
//                }
//                break;
//            }
//        }
//    }
//
//    //判断为黑斑或未找到起点，从100为起点进行扫描
//    if (!flag1)
//    {
//        for (i = 100; i > 70; i--)
//        {
//            //寻找左起点
//            for (j = 3; j < 94; j++)
//            {
//                if (!imageBin[i][j] && imageBin[i][j + 1] && imageBin[i][j + 2])
//                {
//                    LeftStart[1][0] = i;
//                    LeftStart[1][1] = j + 1;
//                    break;
//                }
//            }
//
//            //找到起点
//            if (LeftStart[1][0])
//            {
//                TempLine1[0][0] = LeftStart[1][0];
//                TempLine1[0][1] = LeftStart[1][1];
//                LeftStart[1][2] = ScanLine(TempLine1[0], 2, 0) - 1;
//
//                //根据线终点纵坐标判断是否为黑斑
//                if (LeftStart[1][2]> LeftStart[0][2])
//                {
//                    LeftLine[0][0] = LeftStart[1][0];
//                    LeftLine[0][1] = LeftStart[1][1];
//                    imgInfo.LeftLineSum = ScanLine(LeftLine[0], 2, 0) - 1;
//                    flag1 = 1;
//                }
//                else if(LeftStart[0][2])
//                {
//                    LeftLine[0][0] = LeftStart[0][0];
//                    LeftLine[0][1] = LeftStart[0][1];
//                    imgInfo.LeftLineSum = ScanLine(LeftLine[0], 2, 0) - 1;
//                    flag1 = 1;
//                }
//                break;
//            }
//            else if (LeftStart[0][2])
//            {
//                LeftLine[0][0] = LeftStart[0][0];
//                LeftLine[0][1] = LeftStart[0][1];
//                imgInfo.LeftLineSum = ScanLine(LeftLine[0], 2, 0) - 1;
//                flag1 = 1;
//            }
//        }
//
//        //找到起点
//        if (!flag1)
//        {
//            LeftLine[0][0] = 117;
//            LeftLine[0][1] = 3;
//        }
//    }
//
//    Count = 0;
//    for (i = 115; i > 100; i--)
//    {
//        if (!imageBin[i][184])
//        {
//            //寻找右起点
//            for (j = 184; j > 94; j--)
//            {
//                if (!imageBin[i][j] && imageBin[i][j - 1] && imageBin[i][j - 2])
//                {
//                    RightStart[0][0] = i;
//                    RightStart[0][1] = j - 1;
//                    break;
//                }
//            }
//
//            //扫描右边线
//            if (RightStart[0][0])
//            {
//                TempLine1[0][0] = RightStart[0][0];
//                TempLine1[0][1] = RightStart[0][1];
//                RightStart[0][2] = ScanLine(TempLine1[0], 2, 1) - 1;
//
//                //根据线终点纵坐标判断是否为黑斑
//                if (TempLine1[RightStart[0][2] - 3][0] < 100 && TempLine1[RightStart[0][2] - 3][0] > 3)
//                {
//                    RightLine[0][0] = RightStart[0][0];
//                    RightLine[0][1] = RightStart[0][1];
//                    imgInfo.RightLineSum = ScanLine(RightLine[0], 2, 1) - 1;
//                    flag2 = 1;
//                }
//                break;
//            }
//        }
//    }
//
//    //判断为黑斑或未找到，从100为起点进行扫描
//    if (!flag2)
//    {
//        for (i = 100; i > 70; i--)
//        {
//            //寻找右起点
//            for (j = 184; j > 94; j--)
//            {
//                if (!imageBin[i][j] && imageBin[i][j - 1] && imageBin[i][j - 2])
//                {
//                    RightStart[1][0] = i;
//                    RightStart[1][1] = j - 1;
//                    break;
//                }
//            }
//
//            //找到起点
//            if (RightStart[1][0])
//            {
//                TempLine1[0][0] = RightStart[1][0];
//                TempLine1[0][1] = RightStart[1][1];
//                RightStart[1][2] = ScanLine(TempLine1[0], 2, 1) - 1;
//
//                //根据线终点纵坐标判断是否为黑斑
//                if (RightStart[1][2] > RightStart[0][2])
//                {
//                    RightLine[0][0] = RightStart[1][0];
//                    RightLine[0][1] = RightStart[1][1];
//                    imgInfo.RightLineSum = ScanLine(RightLine[0], 2, 1) - 1;
//                    flag2 = 1;
//                }
//                else if(RightStart[0][2])
//                {
//                    RightLine[0][0] = RightStart[0][0];
//                    RightLine[0][1] = RightStart[0][1];
//                    imgInfo.RightLineSum = ScanLine(RightLine[0], 2, 1) - 1;
//                    flag2 = 1;
//                }
//                break;
//            }
//            else if (RightStart[0][2])
//            {
//                RightLine[0][0] = RightStart[0][0];
//                RightLine[0][1] = RightStart[0][1];
//                imgInfo.RightLineSum = ScanLine(RightLine[0], 2, 1) - 1;
//                flag2 = 1;
//            }
//        }
//        if(!flag2)
//        {
//            RightLine[0][0] = 117;
//            RightLine[0][1] = 183;
//        }
//    }
//
//    memset(TempLine1, 0, sizeof(TempLine1));
    //新版扫线------------------------------------------------------------------------------------------

    //左侧拐点补线
    Count = 0;
    if (flag1) Count = FindTurn2(LeftLine[0], 10, 4, 0);
    if (Count)
    {
        LeftTurn[0][0] = LeftLine[Count][0];
        LeftTurn[0][1] = LeftLine[Count][1];
        LeftTurn[0][2] = Count;
        imgInfo.LeftLineCount = Count;

        //？？
        GetLeftCos();


        if (Count > 40)
        {
            TempLine3[0][0] = LeftLine[Count - 40][0];
            TempLine3[0][1] = LeftLine[Count - 40][1];
        }
        else if (Count > 10)
        {
            TempLine3[0][0] = LeftLine[0][0];
            TempLine3[0][1] = LeftLine[0][1];
        }

        if (Count > 10)
        {
            TempLine3[1][0] = LeftLine[Count][0];
            TempLine3[1][1] = LeftLine[Count][1];
            Count = extend(TempLine3[0], TempLine3[1]);

            if (Count > 30)
            {
                j = Count;
                Count = 0;
                for (i = 10; i < j; i++)
                {
                    if (TempLine3[i][1] < 182 && TempLine3[i][1]>8)
                    {
                        for (int k = 5; k > -5; k--)
                        {
                            if (!imageBin[TempLine3[i][0]][TempLine3[i][1] + k] && imageBin[TempLine3[i][0]][TempLine3[i][1] + k + 1])
                            {
                                TempLine1[Count][0] = TempLine3[i][0];
                                TempLine1[Count][1] = TempLine3[i][1] + k + 1;
                                Count++;
                                break;
                            }
                        }
                    }
                    if (Count > 13) break;
                }

                if (Count > 12 && imgInfo.LeftLineCount > 12)
                {
                    LeftK[2] = (float)(TempLine1[12][1] - TempLine1[2][1]) / (TempLine1[12][0] - TempLine1[2][0]);
                    LeftK[3] = (float)(LeftLine[imgInfo.LeftLineCount - 2][1] - LeftLine[imgInfo.LeftLineCount - 12][1]) / (LeftLine[imgInfo.LeftLineCount - 2][0] - LeftLine[imgInfo.LeftLineCount - 12][0]);

                    if (LeftK[2] - LeftK[3] > -0.4 && LeftK[2] - LeftK[3] < 0.4 && LeftCos>0.3 && LeftCos < 0.7)
                    {
                        RepairFlag[0] = 1;
                        Count = imgInfo.LeftLineCount + repair(LeftLine[imgInfo.LeftLineCount], TempLine1[10]);

                        while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 200)
                        {
                            if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
                            else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);

                            Count++;
                        }
                        imgInfo.LeftLineSum = Count;
                    }
                }
            }
        }
    }
    else
    {
        imgInfo.LeftLineCount = imgInfo.LeftLineSum;
    }

    //右侧拐点补线
    Count = 0;
    if (flag2) Count = FindTurn2(RightLine[0], 10, 4, 1);
    if (Count)
    {
        RightTurn[0][0] = RightLine[Count][0];
        RightTurn[0][1] = RightLine[Count][1];
        RightTurn[0][2] = Count;
        imgInfo.RightLineCount = Count;

        //？？
        GetRightCos();

        if (Count > 40)
        {
            TempLine3[0][0] = RightLine[Count - 40][0];
            TempLine3[0][1] = RightLine[Count - 40][1];
        }
        else if (Count > 10)
        {
            TempLine3[0][0] = RightLine[0][0];
            TempLine3[0][1] = RightLine[0][1];
        }

        if (Count > 10)
        {
            TempLine3[1][0] = RightLine[Count][0];
            TempLine3[1][1] = RightLine[Count][1];
            Count = extend(TempLine3[0], TempLine3[1]);

            if (Count > 30)
            {
                j = Count;
                Count = 0;
                for (i = 10; i < j; i++)
                {
                    if (TempLine3[i][1] < 182 && TempLine3[i][1]>8)
                    {
                        for (int k = 5; k > -5; k--)
                        {
                            if (!imageBin[TempLine3[i][0]][TempLine3[i][1] - k] && imageBin[TempLine3[i][0]][TempLine3[i][1] - k - 1])
                            {
                                TempLine2[Count][0] = TempLine3[i][0];
                                TempLine2[Count][1] = TempLine3[i][1] - k - 1;
                                Count++;
                                break;
                            }
                        }
                    }
                }

                if (Count > 12 && imgInfo.RightLineCount > 12)
                {
                    RightK[2] = (float)(TempLine2[12][1] - TempLine2[2][1]) / (TempLine2[12][0] - TempLine2[2][0]);
                    RightK[3] = (float)(RightLine[imgInfo.RightLineCount - 2][1] - RightLine[imgInfo.RightLineCount - 12][1]) / (RightLine[imgInfo.RightLineCount - 2][0] - RightLine[imgInfo.RightLineCount - 12][0]);

                    if (RightK[2] - RightK[3] > -0.4 && RightK[2] - RightK[3] < 0.4 && RightCos>0.3 && RightCos < 0.7)
                    {
                        RepairFlag[1] = 1;
                        Count = imgInfo.RightLineCount + repair(RightLine[imgInfo.RightLineCount], TempLine2[10]);

                        while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 200)
                        {
                            NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);
                            Count++;

                            if (NearNum == 99) break;
                        }
                        imgInfo.RightLineSum = Count;
                    }
                }

            }
        }
    }
    else
    {
        imgInfo.RightLineCount = imgInfo.RightLineSum;
    }

    LeftMax = 120;
    RightMax = 120;
    for (i = 0; i < imgInfo.LeftLineCount; i++)
    {
        if (LeftMax > LeftLine[i][0])
        {
            if (!LeftLine[i][0]) break;
            else LeftMax = LeftLine[i][0];
        }
    }
    for (i = 0; i < imgInfo.RightLineCount; i++)
    {
        if (RightMax > RightLine[i][0])
        {
            if (!RightLine[i][0]) break;
            else RightMax = RightLine[i][0];
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       拟合中线
// @param       无
// @return      void
// Sample usage:            FindTurn1();//寻找拐点
//-------------------------------------------------------------------------------------------------------------------
void GetMiddle(void)
{
    uint8 i;
    //拟合有效直道中线
    if (imgInfo.LeftLineCount && imgInfo.RightLineCount)
    {
        if (imgInfo.LeftLineCount <= imgInfo.RightLineCount)
        {
            for (i = 0; i < imgInfo.LeftLineCount; i++)
            {
                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
            }
            for (i = imgInfo.LeftLineCount; i < imgInfo.RightLineCount; i++)
            {
                MiddleLine[i][0] = (LeftLine[imgInfo.LeftLineCount][0] + RightLine[i][0]) / 2;
                MiddleLine[i][1] = (LeftLine[imgInfo.LeftLineCount][1] + RightLine[i][1]) / 2;
            }
            imgInfo.MiddleLineCount = imgInfo.LeftLineCount;
            imgInfo.top = LeftLine[imgInfo.LeftLineCount][0];
        }
        else
        {
            for (i = 0; i < imgInfo.RightLineCount; i++)
            {
                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
            }
            for (i = imgInfo.RightLineCount; i < imgInfo.LeftLineCount; i++)
            {
                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineCount][0]) / 2;
                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineCount][1]) / 2;
            }
            imgInfo.MiddleLineCount = imgInfo.RightLineCount;
            imgInfo.top = RightLine[imgInfo.RightLineCount][0];
        }
    }
    else imgInfo.MiddleLineCount = 0;

    if (imgInfo.MiddleLineCount > 30 && (imgInfo.nextroadType != NoHead || imgInfo.MiddleLineCount > 80) && (imgInfo.nextroadType != LeftCircle && CircleFlag == 0) && (imgInfo.nextroadType != RightCircle && CircleFlag == 0) && ThreeForkCount != 3)
    {
        StraightFlag++;
        if ((imgInfo.nextroadType != TurnRight && imgInfo.nextroadType != TurnLeft) || StraightFlag > 5)
        {
            imgInfo.roadType = Straight;
            imgInfo.nextroadType = Straight;
        }
    }

    //if (RightTFlag == 7)
    //{
    //    RightTFlag = 0;
    //}

    if (imgInfo.MiddleLineCount < 30)
    {
        CloseFlag = 2;

        //for (i = 0; i < 20; i++)
        //{
        //    if (RoadTypeSum[i] > RoadTypeSum[imgInfo.nextroadType]) imgInfo.nextroadType = i;
        //}
    }
    if (imgInfo.MiddleLineCount < 60)
    {
        if (CloseFlag != 2) CloseFlag = 1;

    }
    else
    {
        BadCrossTime = 0;

        if (imgInfo.MiddleLineCount > 80)
        {
            NoHeadFlag = 0;
            //CircleFlag = 0;
            //RightTFlag = 0;
            CrossFlag = 0;
            BadCrossFlag = 0;
            //            ThreeForkFlag = 0;
                        //LeftTFlag = 0;
                        //imgInfo.roadType = Straight;
                        //imgInfo.nextroadType = Straight;
        }

        //if (RightTFlag == 6 || RightTFlag == 7)
        //{
        //    RightTFlag = 0;
        //}
//        if (LeftTFlag == 6 || LeftTFlag == 7)
//        {
//            LeftTFlag = 0;
//        }

        if (CrossFlag >= 1) CrossFlag = 0;
        if (BadCrossFlag >= 1) BadCrossFlag = 0;

        memset(RoadTypeSum, 0, 20);
        if (TempLine1[0][0] != 0) TempLine1[0][0] = 0;
        if (TempLine2[0][0] != 0) TempLine2[0][0] = 0;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       根据拐点斜率判断赛道类型
// @param       无
// @return      void
// Sample usage:            RoadJudge();//根据拐点斜率判断赛道类型
//-------------------------------------------------------------------------------------------------------------------
uint8 BadThreeForkJudge(void)
{
    uint8 i;
    uint8 j;
    uint8 Count = 0;//扫线序号
    uint8 Bias1 = 0, Bias2 = 0;//扫线序号
    int16 Y_Move, y_Move, X_Move, x_Move;
    uint8 error = 99;

    if (BadThreeForkFlag == 1)
    {
        Count = 0;
        if (imgInfo.LeftLineSum) Count = FindTurn2(LeftLine[0], 10, 4, 0);
        if (Count)
        {
            LeftTurn[0][0] = LeftLine[Count][0];
            LeftTurn[0][1] = LeftLine[Count][1];
            LeftTurn[0][2] = Count;
            imgInfo.LeftLineCount = Count;

            if (LeftTurn[0][0] > 20 && LeftTurn[0][0] < 100 && LeftLine[imgInfo.LeftLineCount + 20][0] != 0 && LeftLine[imgInfo.LeftLineCount - 20][0] != 0)
            {
                if (LeftLine[imgInfo.LeftLineCount + 30][0]) Bias1 = 30;
                else if (LeftLine[imgInfo.LeftLineCount + 25][0]) Bias1 = 25;
                else if (LeftLine[imgInfo.LeftLineCount + 20][0]) Bias1 = 20;
                else if (LeftLine[imgInfo.LeftLineCount + 10][0]) Bias1 = 10;

                if (imgInfo.LeftLineCount > 30) Bias2 = 30;
                else if (imgInfo.LeftLineCount > 25) Bias2 = 25;
                else if (imgInfo.LeftLineCount > 20) Bias2 = 20;
                else if (imgInfo.LeftLineCount > 10) Bias2 = 10;

                if (Bias1 > 10)
                {
                    Y_Move = LeftLine[imgInfo.LeftLineCount + Bias1][0] - LeftLine[imgInfo.LeftLineCount][0];
                    X_Move = LeftLine[imgInfo.LeftLineCount + Bias1][1] - LeftLine[imgInfo.LeftLineCount][1];
                }
                else
                {
                    Y_Move = LeftLine[imgInfo.LeftLineSum][0] - LeftLine[imgInfo.LeftLineCount][0];
                    X_Move = LeftLine[imgInfo.LeftLineSum][1] - LeftLine[imgInfo.LeftLineCount][1];
                    Bias1 = imgInfo.LeftLineSum - imgInfo.LeftLineCount;
                }

                if (Bias2 > 10)
                {
                    y_Move = LeftLine[imgInfo.LeftLineCount - Bias2][0] - LeftLine[imgInfo.LeftLineCount][0];
                    x_Move = LeftLine[imgInfo.LeftLineCount - Bias2][1] - LeftLine[imgInfo.LeftLineCount][1];
                }
                else
                {
                    y_Move = LeftLine[0][0] - LeftLine[imgInfo.LeftLineCount][0];
                    x_Move = LeftLine[0][1] - LeftLine[imgInfo.LeftLineCount][1];
                    Bias2 = imgInfo.LeftLineCount;
                }

                //拐点后线点
                MarkPoint[0][0] = LeftLine[imgInfo.LeftLineCount + Bias1][0];
                MarkPoint[0][1] = LeftLine[imgInfo.LeftLineCount + Bias1][1];

                //拐点前线点
                MarkPoint[1][0] = LeftLine[imgInfo.LeftLineCount - Bias2][0];
                MarkPoint[1][1] = LeftLine[imgInfo.LeftLineCount - Bias2][1];

                //拐点后线中点
                MarkPoint[4][0] = (MarkPoint[0][0] + LeftTurn[0][0]) / 2;
                MarkPoint[4][1] = (MarkPoint[0][1] + LeftTurn[0][1]) / 2;

                //拐点前线中点
                MarkPoint[5][0] = (MarkPoint[1][0] + LeftTurn[0][0]) / 2;
                MarkPoint[5][1] = (MarkPoint[1][1] + LeftTurn[0][1]) / 2;

                X_Move = 0;
                Y_Move = 0;
                for (i = 0; i < Bias1; i++)
                {
                    X_Move += LeftLine[imgInfo.LeftLineCount + i][1];
                    Y_Move += LeftLine[imgInfo.LeftLineCount + i][0];
                }
                X_Move /= i;
                Y_Move /= i;

                //拐点后线平均点
                MarkPoint[8][0] = Y_Move;
                MarkPoint[8][1] = X_Move;

                X_Move = 0;
                Y_Move = 0;
                for (i = 0; i < Bias2; i++)
                {
                    X_Move += LeftLine[imgInfo.LeftLineCount - i][1];
                    Y_Move += LeftLine[imgInfo.LeftLineCount - i][0];
                }
                X_Move /= i;
                Y_Move /= i;

                //拐点前线平均点
                MarkPoint[9][0] = Y_Move;
                MarkPoint[9][1] = X_Move;

                if (MarkPoint[4][0] && MarkPoint[5][0] && MarkPoint[8][0] && MarkPoint[9][0])
                {
                    error = 0;

                    if (MarkPoint[8][0] > MarkPoint[4][0]) error += MarkPoint[8][0] - MarkPoint[4][0];
                    else error += MarkPoint[4][0] - MarkPoint[8][0];
                    if (MarkPoint[8][1] > MarkPoint[4][1]) error += MarkPoint[8][1] - MarkPoint[4][1];
                    else error += MarkPoint[4][1] - MarkPoint[8][1];

                    if (MarkPoint[9][0] > MarkPoint[5][0]) error += MarkPoint[9][0] - MarkPoint[5][0];
                    else error += MarkPoint[5][0] - MarkPoint[9][0];
                    if (MarkPoint[9][1] > MarkPoint[5][1]) error += MarkPoint[9][1] - MarkPoint[5][1];
                    else error += MarkPoint[5][1] - MarkPoint[9][1];
                }
            }
        }
    }
    else
    {
        if (imgInfo.RightLineSum) Count = FindTurn2(RightLine[0], 10, 4, 1);
        if (Count)
        {
            RightTurn[0][0] = RightLine[Count][0];
            RightTurn[0][1] = RightLine[Count][1];
            RightTurn[0][2] = Count;
            imgInfo.RightLineCount = Count;

            if (RightTurn[0][0] > 20 && RightTurn[0][0] < 100 && RightLine[imgInfo.RightLineCount + 20][0] != 0 && RightLine[imgInfo.RightLineCount - 20][0] != 0)
            {
                if (RightLine[imgInfo.RightLineCount + 30][0]) Bias1 = 30;
                else if (RightLine[imgInfo.RightLineCount + 25][0]) Bias1 = 25;
                else if (RightLine[imgInfo.RightLineCount + 20][0]) Bias1 = 20;
                else if (RightLine[imgInfo.RightLineCount + 10][0]) Bias1 = 10;

                if (imgInfo.RightLineCount > 30) Bias2 = 30;
                else if (imgInfo.RightLineCount > 25) Bias2 = 25;
                else if (imgInfo.RightLineCount > 20) Bias2 = 20;
                else if (imgInfo.RightLineCount > 10) Bias2 = 10;

                if (Bias1 > 10)
                {
                    Y_Move = RightLine[imgInfo.RightLineCount + Bias1][0] - RightLine[imgInfo.RightLineCount][0];
                    X_Move = RightLine[imgInfo.RightLineCount + Bias1][1] - RightLine[imgInfo.RightLineCount][1];
                }
                else
                {
                    Y_Move = RightLine[imgInfo.RightLineSum][0] - RightLine[imgInfo.RightLineSum][0];
                    X_Move = RightLine[imgInfo.RightLineSum][1] - RightLine[imgInfo.RightLineSum][1];
                    Bias1 = imgInfo.RightLineSum - imgInfo.RightLineCount;
                }

                if (Bias2 > 10)
                {
                    y_Move = RightLine[imgInfo.RightLineCount - Bias2][0] - RightLine[imgInfo.RightLineCount][0];
                    x_Move = RightLine[imgInfo.RightLineCount - Bias2][1] - RightLine[imgInfo.RightLineCount][1];
                }
                else
                {
                    y_Move = RightLine[0][0] - RightLine[imgInfo.RightLineCount][0];
                    x_Move = RightLine[0][1] - RightLine[imgInfo.RightLineCount][1];
                    Bias2 = imgInfo.RightLineCount;
                }

                //拐点后线点
                MarkPoint[2][0] = RightLine[imgInfo.RightLineCount + Bias1][0];
                MarkPoint[2][1] = RightLine[imgInfo.RightLineCount + Bias1][1];
                //拐点前线点
                MarkPoint[3][0] = RightLine[imgInfo.RightLineCount - Bias2][0];
                MarkPoint[3][1] = RightLine[imgInfo.RightLineCount - Bias2][1];
                //拐点后线中点
                MarkPoint[6][0] = (MarkPoint[2][0] + RightTurn[0][0]) / 2;
                MarkPoint[6][1] = (MarkPoint[2][1] + RightTurn[0][1]) / 2;
                //拐点前线中点
                MarkPoint[7][0] = (MarkPoint[3][0] + RightTurn[0][0]) / 2;
                MarkPoint[7][1] = (MarkPoint[3][1] + RightTurn[0][1]) / 2;

                X_Move = 0;
                Y_Move = 0;
                for (i = 0; i < Bias1; i++)
                {
                    X_Move += RightLine[imgInfo.RightLineCount + i][1];
                    Y_Move += RightLine[imgInfo.RightLineCount + i][0];
                }
                X_Move /= i;
                Y_Move /= i;

                //拐点后线平均点
                MarkPoint[10][0] = Y_Move;
                MarkPoint[10][1] = X_Move;

                X_Move = 0;
                Y_Move = 0;
                for (i = 0; i < Bias2; i++)
                {
                    X_Move += RightLine[imgInfo.RightLineCount - i][1];
                    Y_Move += RightLine[imgInfo.RightLineCount - i][0];
                }
                X_Move /= i;
                Y_Move /= i;

                //拐点前线平均点
                MarkPoint[11][0] = Y_Move;
                MarkPoint[11][1] = X_Move;

                if (MarkPoint[6][0] && MarkPoint[7][0] && MarkPoint[10][0] && MarkPoint[11][0])
                {
                    error = 0;

                    if (MarkPoint[10][0] > MarkPoint[6][0]) error += MarkPoint[10][0] - MarkPoint[6][0];
                    else error += MarkPoint[6][0] - MarkPoint[10][0];
                    if (MarkPoint[10][1] > MarkPoint[6][1]) error += MarkPoint[10][1] - MarkPoint[6][1];
                    else error += MarkPoint[6][1] - MarkPoint[10][1];

                    if (MarkPoint[11][0] > MarkPoint[7][0]) error += MarkPoint[11][0] - MarkPoint[7][0];
                    else error += MarkPoint[7][0] - MarkPoint[11][0];
                    if (MarkPoint[11][1] > MarkPoint[7][1]) error += MarkPoint[11][1] - MarkPoint[7][1];
                    else error += MarkPoint[7][1] - MarkPoint[11][1];
                }
            }
        }
    }
    return error;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       根据拐点斜率判断赛道类型
// @param       无
// @return      void
// Sample usage:            RoadJudge();//根据拐点斜率判断赛道类型
//-------------------------------------------------------------------------------------------------------------------
void GarageJudge(void)
{
    uint8 i, j, NearNum, Count = 0;

    if (!imgInfo.NoHeadDir)
    {
        //        if (!LeftMax) LeftMax = 119;
        if (LeftMax > 20 && LeftTFlag <= 2)
        {
            uint8 RightI = 0;
            for (i = LeftMax - 5; i > LeftMax - 50 && i > 10; i--)
            {
                while (RightLine[RightI][0] > i && RightI < 240) RightI++;
                if (RightI > 230) break;
                NearNum = Count;
                Count = 0;

                for (j = RightLine[RightI][1] - 2; j > RightLine[RightI][1] - WidthBuffer[i] + 5; j--)
                {
                    if (imageBin[i][j] && !imageBin[i][j - 1])
                    {
                        Count++;
                        j -= 3;
                    }
                }
                if (Count)
                {
                    i -= 3;
                    if (NearNum < Count) NearNum = Count;
                }

                if (NearNum > 2)
                {
                    //                    gpio_set(P02_6, 0);
                    LeftTFlag = 10;
                    break;
                }
            }
        }
    }
    else
    {
        if (!RightMax) RightMax = 119;
        if (RightMax && RightTFlag <= 2)
        {
            uint8 LeftI = 0;
            for (i = RightMax - 5; i > RightMax - 50 && i > 10; i--)
            {
                while (LeftLine[LeftI][0] > i) LeftI++;
                NearNum = Count;
                Count = 0;

                for (j = LeftLine[LeftI][1] + 2; j < LeftLine[LeftI][1] + WidthBuffer[i] - 5; j++)
                {
                    if (imageBin[i][j] && !imageBin[i][j - 1])
                    {
                        Count++;
                        j += 3;
                    }
                }
                if (Count)
                {
                    i -= 3;
                    if (NearNum < Count) NearNum = Count;
                }

                if (NearNum > 2)
                {
                    //                    gpio_set(P02_6, 0);
                    RightTFlag = 10;
                    break;
                }
            }
        }
    }

    //    if(NearNum<=2)     gpio_set(P02_6, 1);
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       根据拐点斜率判断赛道类型
// @param       无
// @return      void
// Sample usage:            RoadJudge();//根据拐点斜率判断赛道类型
//-------------------------------------------------------------------------------------------------------------------
void EazyTJudge(void)
{
    uint8 i, j, NearNum, Count = 0;

    if (LeftMax < 20)
    {
        for (i = 117; i > 20; i--)
        {
            turnpoint[0][0] = 0;
            if (imageBin[i][183] && !imageBin[i - 1][183] && !imageBin[i - 2][183])
            {
                turnpoint[0][0] = i;
                break;
            }
        }
        if (turnpoint[0][0] && turnpoint[0][0] - LeftMax > 40)
        {
            imgInfo.nextroadType = RightT;
            RightTFlag = 1;
        }
    }
    else if (RightMax < 20)
    {
        for (i = 117; i > 20; i--)
        {
            turnpoint[0][0] = 0;
            if (imageBin[i][4] && !imageBin[i - 1][4] && !imageBin[i - 2][4])
            {
                turnpoint[0][0] = i;
                break;
            }

            if (turnpoint[0][0] && turnpoint[0][0] - RightMax > 40)
            {
                imgInfo.nextroadType = LeftT;
                LeftTFlag = 1;
            }
        }
    }

    turnpoint[0][0] = 0;
    turnpoint[0][1] = 0;

}

//-------------------------------------------------------------------------------------------------------------------
// @brief       根据拐点斜率判断赛道类型
// @param       无
// @return      void
// Sample usage:            RoadJudge();//根据拐点斜率判断赛道类型
//-------------------------------------------------------------------------------------------------------------------
void RoadJudge(void)
{
    uint8 i = 0;
    uint8 Count = 0;

    if (RoadTypeCount > 200)
    {
        while (i < 20)
        {
            if (RoadTypeSum[i] > 10) RoadTypeSum[i] = 10;
            i++;
        }
        RoadTypeCount = 0;
    }

    if (RampFlag == 1 || RampFlag == 2)
    {
        RoadTypeSum[Ramp]++;
        RoadTypeCount++;
        imgInfo.roadType = Ramp;
        imgInfo.nextroadType = Ramp;
    }
    else if (imgInfo.nextroadType == RightCircle || (CircleFlag && CircleFlag <= 10))
    {
        RoadTypeSum[RightCircle]++;
        RoadTypeCount++;
        imgInfo.roadType = RightCircle;
        imgInfo.nextroadType = RightCircle;
    }
    else if (imgInfo.nextroadType == LeftCircle || CircleFlag > 10)
    {
        RoadTypeSum[LeftCircle]++;
        RoadTypeCount++;
        imgInfo.roadType = LeftCircle;
        imgInfo.nextroadType = LeftCircle;
    }
    else if (RightTFlag || (GarageFlag && imgInfo.nextroadType == RightT))
    {
        RoadTypeSum[RightT]++;
        RoadTypeCount++;
        imgInfo.roadType = RightT;
        imgInfo.nextroadType = RightT;
    }
    else if (LeftTFlag || (GarageFlag && imgInfo.nextroadType == LeftT))
    {
        RoadTypeSum[LeftT]++;
        RoadTypeCount++;
        imgInfo.roadType = LeftT;
        imgInfo.nextroadType = LeftT;
    }
    else if (CrossFlag)
    {
        RoadTypeSum[Cross]++;
        RoadTypeCount++;
        imgInfo.roadType = Cross;
        imgInfo.nextroadType = Cross;
    }
    else if (BadCrossFlag)
    {
        RoadTypeSum[BadCross]++;
        RoadTypeCount++;
        imgInfo.roadType = BadCross;
        imgInfo.nextroadType = BadCross;
    }
    else if (imgInfo.nextroadType == NoHead)
    {
        RoadTypeSum[NoHead]++;
        RoadTypeCount++;
        imgInfo.roadType = NoHead;
        imgInfo.nextroadType = NoHead;
    }
    else if (imgInfo.nextroadType == ThreeFork || ThreeForkFlag)
    {
        //        if (ThreeForkCount >= 3 && ThreeForkTime > 10 && ThreeForkFlag) ThreeForkCount = 4;

        RoadTypeSum[ThreeFork]++;
        RoadTypeSum[TurnRight] = 0;
        RoadTypeSum[TurnLeft] = 0;
        RoadTypeCount++;

        imgInfo.nextroadType = ThreeFork;
    }
    else if (!ThreeForkOutFlag)
    {
        if (LeftTurn[0][0] - RightTurn[0][0] > -15 && LeftTurn[0][0] - RightTurn[0][0] < 15 && ((RepairFlag[0] == 1 && RepairFlag[1] == 1) || (RightCos < 0.55 && RightCos > 0.3 && LeftCos < 0.55 && LeftCos > 0.3)))
        {
            RoadTypeSum[Cross]++;
            RoadTypeCount++;
            imgInfo.roadType = Cross;
            imgInfo.nextroadType = Cross;
        }
        else if (imgInfo.nextroadType != Cross && (RepairFlag[0] == 1 && RightMax > 40 && LeftMax > 40) || (RepairFlag[1] == 1 && LeftMax > 40 && RightMax > 40))
        {
            RoadTypeSum[BadCross]++;
            RoadTypeCount++;
            imgInfo.roadType = BadCross;
            imgInfo.nextroadType = BadCross;
        }
        else if ((RepairFlag[0] == 1) && RightMax < 20 && RightMax + 40 < LeftTurn[0][0])
        {
            RoadTypeSum[LeftT]++;
            RoadTypeCount++;
            imgInfo.roadType = LeftT;
            imgInfo.nextroadType = LeftT;
        }
        else if ((RepairFlag[1] == 1) && LeftMax < 20 && LeftMax + 40 < RightTurn[0][0])
        {
            RoadTypeSum[RightT]++;
            RoadTypeCount++;
            imgInfo.roadType = RightT;
            imgInfo.nextroadType = RightT;
        }
        else if (LeftCos > 0.65 && LeftCos < 0.95 && LeftTurn[0][1] < 94 && LeftTurn[0][0] > 40 && RightMax < 20 && RightMax)
        {
            RoadTypeSum[LeftCircle]++;
            RoadTypeCount++;
            imgInfo.roadType = LeftCircle;
            imgInfo.nextroadType = LeftCircle;
        }
        else if (RightCos > 0.65 && RightCos < 0.95 && RightTurn[0][1]>94 && RightTurn[0][0] > 40 && LeftMax < 20 && LeftMax)
        {
            RoadTypeSum[RightCircle]++;
            RoadTypeCount++;
            imgInfo.roadType = RightCircle;
            imgInfo.nextroadType = RightCircle;
        }
        else if (LeftCos < 0.3 && LeftCos > -0.3 && RightCos < 0.3 && RightCos > -0.3 && RightTurn[0][1] > 50 && LeftTurn[0][1] < 140)
        {
            RoadTypeSum[ThreeFork]++;
            RoadTypeCount++;
            imgInfo.roadType = ThreeFork;
            imgInfo.nextroadType = ThreeFork;
        }
        else if ((LeftCos < 0.3 && LeftCos > -0.4) || (RightCos < 0.3 && RightCos > -0.4))
        {
            BadThreeForkFlag = 0;
            if (LeftCos < 0.3 && LeftCos > -0.3 && !(imgInfo.RightLineSum > 20 && RightMax < LeftTurn[0][0])) BadThreeForkFlag = 1;
            else if (RightCos < 0.3 && RightCos > -0.3 && !(imgInfo.LeftLineSum > 20 && LeftMax < RightTurn[0][0])) BadThreeForkFlag = 2;
            Count = 0;
            Count = BadThreeForkJudge();
            //            Mark[2] = Count;
            if (Count < 4)
            {
                RoadTypeSum[ThreeFork]++;
                RoadTypeCount++;
                imgInfo.roadType = BadThreeFork;
                imgInfo.nextroadType = ThreeFork;
            }
        }
        else
        {
            RoadTypeCount++;
            imgInfo.roadType = Turn;
            if (ThreeForkCount > 3 && ((!imgInfo.NoHeadDir && LGCos > 0.3 && LGCos < 0.6 && RightMax < 20 && RightLine[imgInfo.RightLineSum-2][1]>50) || (imgInfo.NoHeadDir && RGCos > 0.3 && RGCos < 0.6 && LeftMax < 20 && LeftLine[imgInfo.LeftLineSum-2][1]<130)))
            {
                if (!RampFlag) GarageJudge();
            }

            if ((LeftMax < 20 && RightMax>100) || (RightMax < 20 && LeftMax>100))
            {
                EazyTJudge();
            }
        }
    }
    else
    {
        if (LeftCos < 0.3 && LeftCos > -0.3 && RightCos < 0.3 && RightCos > -0.3 && RightTurn[0][1] > 50 && LeftTurn[0][1] < 140)
        {
            RoadTypeSum[ThreeFork]++;
            RoadTypeCount++;
            imgInfo.roadType = ThreeFork;
            imgInfo.nextroadType = ThreeFork;
        }
        else if ((LeftCos < 0.3 && LeftCos > -0.4) || (RightCos < 0.3 && RightCos > -0.4))
        {
            BadThreeForkFlag = 0;
            if (LeftCos < 0.3 && LeftCos > -0.3 && !(imgInfo.RightLineSum > 20 && RightMax < LeftTurn[0][0])) BadThreeForkFlag = 1;
            else if (RightCos < 0.3 && RightCos > -0.3 && !(imgInfo.LeftLineSum > 20 && LeftMax < RightTurn[0][0])) BadThreeForkFlag = 2;
            Count = 0;
            Count = BadThreeForkJudge();
            //            Mark[2] = Count;
            if (Count < 4)
            {
                RoadTypeSum[ThreeFork]++;
                RoadTypeCount++;
                imgInfo.roadType = BadThreeFork;
                imgInfo.nextroadType = ThreeFork;
            }
        }
        else
        {
            imgInfo.roadType = Turn;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       根据后续赛道类型扫线
// @param       无
// @return      void
// Sample usage:           uint8 Error = ErrorSum();//计算偏差
//-------------------------------------------------------------------------------------------------------------------
void NextRoad(void)
{
    if (imgInfo.nextroadType == Straight || imgInfo.nextroadType == TurnLeft || imgInfo.nextroadType == TurnRight)
    {
        turn();
        if (LeftLine[imgInfo.LeftLineSum][1] > 140 && RightLine[imgInfo.RightLineSum][1] > 140)
        {
            imgInfo.nextroadType = TurnRight;
            StraightFlag = 0;
        }
        if (LeftLine[imgInfo.LeftLineSum][1] < 50 && RightLine[imgInfo.RightLineSum][1] < 50)
        {
            imgInfo.nextroadType = TurnLeft;
            StraightFlag = 0;
        }
    }
    else if (imgInfo.nextroadType == NoHead)//车库
    {
        nohead1();
    }
    else if (imgInfo.nextroadType == ThreeFork)
    {
        threefork1();
    }
    else if (imgInfo.nextroadType == Cross)
    {
        cross1();
    }
    else if (imgInfo.nextroadType == LeftT)
    {
        leftt1();
    }
    else if (imgInfo.nextroadType == RightT)
    {
        rightt1();
    }
    else if (imgInfo.nextroadType == RightCircle)
    {
        rightcircle1();
    }
    else if (imgInfo.nextroadType == LeftCircle)
    {
        leftcircle1();
    }
    else if (imgInfo.nextroadType == Ramp)
    {
        ramp();
    }
    else if (imgInfo.nextroadType == BadCross)
    {
        badcross();
    }
    else
    {
        turn();
    }

    if(RampFlag)
    {
        RampInCount++;
        if(RampInCount>80)
        {
            RampInCount = 0;
            RampFlag = 0;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       拟合固定纵坐标中线
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void GetMid(void)
{
    //    uart_putchar(UART_3,0x61);
    memset(MidLine, 0, 512);
    imgInfo.MidLineCount = 0;
    uint8 i, j, Count = 0;
    uint8 Tempnum = 0;
    uint8 gap = 1;
    uint8 col = MiddleLine[0][0];
    MidLine[0][0] = MiddleLine[0][0];
    MidLine[0][1] = MiddleLine[0][1];
    Count++;

    if (LeftTFlag != 1 && LeftTFlag != 2 && LeftTFlag != 10 && LeftTFlag != 11 && RightTFlag != 1 && RightTFlag != 2 && RightTFlag != 10 && RightTFlag != 11)
    {
        if (imgInfo.MiddleLineCount)
        {
            //            uart_putchar(UART_3,0x62);
            for (i = 0; i < imgInfo.MiddleLineCount; i++)
            {
                if (MiddleLine[i][0] < col)
                {
                    Tempnum = 0;
                    for (j = 0; j < 35; j++)
                    {
                        if (!MiddleLine[i + j][0]) break;

                        if (MiddleLine[i + j][0] < col - 1)
                        {
                            Tempnum = MiddleLine[i + j - 1][1];

                            if (Tempnum > MidLine[Count - 1][1] + 1)
                            {
                                while (Tempnum > MidLine[Count - 1][1] + 1)
                                {
                                    MidLine[Count][0] = MiddleLine[i + j - 1][0];
                                    MidLine[Count][1] = MidLine[Count - 1][1] + gap;
                                    Count++;
                                }
                            }
                            else if (Tempnum < MidLine[Count - 1][1] - 1)
                            {
                                while (Tempnum < MidLine[Count - 1][1] - 1)
                                {
                                    MidLine[Count][0] = MiddleLine[i + j - 1][0];
                                    MidLine[Count][1] = MidLine[Count - 1][1] - gap;
                                    Count++;
                                }
                            }
                            else
                            {
                                MidLine[Count][0] = MiddleLine[i + j - 1][0];
                                MidLine[Count][1] = MiddleLine[i + j - 1][1];
                                Count++;
                            }
                            col--;
                            break;
                        }
                        else if (MiddleLine[i + j][0] == col)
                        {
                            Tempnum = MiddleLine[i + j - 1][1];

                            if (Tempnum > MidLine[Count - 1][1] + 1)
                            {
                                while (Tempnum > MidLine[Count - 1][1] + 1)
                                {
                                    MidLine[Count][0] = MiddleLine[i + j - 1][0];
                                    MidLine[Count][1] = MidLine[Count - 1][1] + gap;
                                    Count++;
                                }
                            }
                            else if (Tempnum < MidLine[Count - 1][1] - 1)
                            {
                                while (Tempnum < MidLine[Count - 1][1] - 1)
                                {
                                    MidLine[Count][0] = MiddleLine[i + j - 1][0];
                                    MidLine[Count][1] = MidLine[Count - 1][1] - gap;
                                    Count++;
                                }
                            }

                            MidLine[Count][0] = MiddleLine[i + j][0];
                            MidLine[Count][1] = MiddleLine[i + j][1];
                            Count++;

                            i += j;

                            break;
                        }
                    }
                }
            }
            //            uart_putchar(UART_3,0x63);

            imgInfo.MidLineCount = Count - 1;
            if ((MiddleLine[imgInfo.MiddleLineCount - 1][0] - MidLine[imgInfo.MidLineCount][0] < 2) && (MiddleLine[imgInfo.MiddleLineCount - 1][0] - MidLine[imgInfo.MidLineCount][0] > -2))
            {
                //                uart_putchar(UART_3,0x64);
                i = MiddleLine[imgInfo.MiddleLineCount - 1][0] - MidLine[imgInfo.MidLineCount][0];

                Tempnum = MiddleLine[imgInfo.MiddleLineCount - 1][1];

                if (Tempnum > MidLine[Count - 1][1] + 5 && Tempnum < MidLine[Count - 1][1] + 40)
                {
                    while (Tempnum > MidLine[Count - 1][1] + 1)
                    {
                        MidLine[Count][0] = MidLine[Count - 1][0];
                        MidLine[Count][1] = MidLine[Count - 1][1] + gap;
                        Count++;
                    }
                    imgInfo.MidLineCount = Count - 1;
                }
                else if (Tempnum < MidLine[Count - 1][1] - 5 && Tempnum > MidLine[Count - 1][1] - 40)
                {
                    while (Tempnum < MidLine[Count - 1][1] - 1)
                    {
                        MidLine[Count][0] = MidLine[Count - 1][0];
                        MidLine[Count][1] = MidLine[Count - 1][1] - gap;
                        Count++;
                    }
                    imgInfo.MidLineCount = Count - 1;
                }
                //                uart_putchar(UART_3,0x65);
            }
        }

        //        uart_putchar(UART_3,0x66);

        //        MiddleTurn[0][0] = 0;
        //        MiddleTurn[0][1] = 0;
        //
        //        i = FindTurn1(MidLine[0], 10, 2, 0);
        //        if (i)
        //        {
        //            MiddleTurn[0][0] = MidLine[i][0];
        //            MiddleTurn[0][1] = MidLine[i][1];
        //        }

                //if (imgInfo.MiddleLineCount > 120)
                //{
                //    for (i = 0; i < imgInfo.MiddleLineCount; i++)
                //    {
                //        if ((float)i / imgInfo.MiddleLineCount * 120 >= Count)
                //        {
                //            MidLine[Count][0] = MiddleLine[i][0];
                //            MidLine[Count][1] = MiddleLine[i][1];
                //            Count++;
                //        }
                //    }
                //    imgInfo.MidLineCount = Count;
                //}
                //else
                //{
                //    for (i = 0; i < imgInfo.MiddleLineCount; i++)
                //    {
                //        while ((float)i * 120 / imgInfo.MiddleLineCount >= Count)
                //        {
                //            MidLine[Count][0] = MiddleLine[i][0];
                //            MidLine[Count][1] = MiddleLine[i][1];
                //            Count++;
                //        }
                //    }
                //    imgInfo.MidLineCount = Count;
                //}
    }
    else
    {
        for (i = 0; i < imgInfo.MiddleLineCount; i++)
        {
            MidLine[i][0] = MiddleLine[i][0];
            MidLine[i][1] = MiddleLine[i][1];
        }
        imgInfo.MidLineCount = imgInfo.MiddleLineCount;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       加速检测
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void SpeedUp(void)
{
    uint8 err = 2;
    uint8 i, x0, x1, y0, y1, temp = 0, temp1 = 0, Count = 0;
    int16 xb, yb, xn;

    temp1 = 0;
    Count++;
    x0 = MidLine[0][1];
    y0 = MidLine[0][0];
    for (Count = 0; Count < imgInfo.MidLineCount - 1; Count += 5)
    {
        x1 = MidLine[Count][1];
        y1 = MidLine[Count][0];
        if (!x1 || !y1) break;
        xb = x1 - x0;
        yb = y1 - y0;
        for (i = 1; i < Count; i++)
        {
            if (yb) xn = ((int16)MidLine[i][1] - x0) - xb * ((int16)MidLine[i][0] - y0) / yb;
            else xn = 99;

            if (xn > err || xn < -err)
            {
                temp = i;
                break;
            }

            if (i == Count - 1) temp = Count;
        }

        if (temp > temp1) temp1 = temp;
    }

    if ((imgInfo.nextroadType == RightCircle && CircleFlag < 1) || (imgInfo.nextroadType == RightT && RightTFlag < 3))
    {
        if (temp1 > 120 - RightTurn[0][0]) SpeedUpCount = 120 - RightTurn[0][0];
        else SpeedUpCount = temp1;
    }
    else if ((imgInfo.nextroadType == LeftCircle && CircleFlag < 11) || (imgInfo.nextroadType == LeftT && LeftTFlag < 3))
    {
        if (temp1 > 120 - LeftTurn[0][0]) SpeedUpCount = 120 - LeftTurn[0][0];
        else SpeedUpCount = temp1;
    }
    else if ((CircleFlag >= 1 && CircleFlag < 9) || CircleFlag >= 11 || RightTFlag >= 3 || LeftTFlag >= 3) SpeedUpCount = 0;
    else SpeedUpCount = temp1;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       转向扫线
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void turn(void)
{
    uint8 i, j;
    uint8 Count = 0;//扫线序号
    uint8 NearNum = 2;//邻域序号

    if (LeftTurn[0][2] && imgInfo.RightLineSum < 5 && !RepairFlag[0])
    {
        if (imgInfo.LeftLineCount > 40) imgInfo.LeftLineSum = imgInfo.LeftLineCount + extend(LeftLine[imgInfo.LeftLineCount - 40], LeftLine[imgInfo.LeftLineCount]);
        else if (imgInfo.LeftLineCount > 10) imgInfo.LeftLineSum = imgInfo.LeftLineCount + extend(LeftLine[0], LeftLine[imgInfo.LeftLineCount]);
    }
    else if (RightTurn[0][2] && imgInfo.LeftLineSum < 5 && !RepairFlag[1])
    {
        if (imgInfo.RightLineCount > 40) imgInfo.RightLineSum = imgInfo.RightLineCount + extend(RightLine[imgInfo.RightLineCount - 40], RightLine[imgInfo.RightLineCount]);
        else if (imgInfo.RightLineCount > 10) imgInfo.RightLineSum = imgInfo.RightLineCount + extend(RightLine[0], RightLine[imgInfo.RightLineCount]);
    }
    else if (LeftTurn[0][2] && LeftCos > 0.3 && LeftCos < 1) imgInfo.LeftLineSum = imgInfo.LeftLineCount;
    else if (RightTurn[0][2] && RightCos > 0.3 && RightCos < 1) imgInfo.RightLineSum = imgInfo.RightLineCount;

    if (!LeftLine[0][0])
    {
        LeftLine[0][0] = 117;
        LeftLine[0][1] = 3;
    }

    if (!RightLine[0][0])
    {
        RightLine[0][0] = 117;
        RightLine[0][1] = 184;
    }

    Count = 0;
    j = 0;
    i = FindLineTurn(LeftLine[0], 10, 4);
    while (i != 0)
    {
        LeftTurn[Count][0] = LeftLine[j + i][0];
        LeftTurn[Count][1] = LeftLine[j + i][1];
        LeftTurn[Count][2] = j + i;
        Count++;
        if (Count > 2) break;
        j += i;
        i = FindLineTurn(LeftLine[j], 10, 4);
    }

    Count = 0;
    j = 0;
    i = FindLineTurn(RightLine[0], 10, 4);
    while (i != 0)
    {
        RightTurn[Count][0] = RightLine[j + i][0];
        RightTurn[Count][1] = RightLine[j + i][1];
        RightTurn[Count][2] = j + i;
        Count++;
        if (Count > 2) break;
        j += i;
        i = FindLineTurn(RightLine[j], 10, 4);
    }

    if (RightTurn[0][0] && !LeftTurn[0][0] && RightTurn[0][2] > imgInfo.LeftLineSum)
    {
        for (i = 0; i < imgInfo.LeftLineSum; i++)
        {
            MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
            MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
        }
        for (i = imgInfo.LeftLineSum; i < RightTurn[0][2]; i++)
        {
            MiddleLine[i][0] = (LeftLine[imgInfo.LeftLineSum][0] + RightLine[i][0]) / 2;
            MiddleLine[i][1] = (LeftLine[imgInfo.LeftLineSum][1] + RightLine[i][1]) / 2;
        }
        imgInfo.MiddleLineCount = RightTurn[0][2];
    }
    else if (LeftTurn[0][0] && !RightTurn[0][0] && LeftTurn[0][2] > imgInfo.RightLineSum)
    {
        for (i = 0; i < imgInfo.RightLineSum; i++)
        {
            MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
            MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
        }
        for (i = imgInfo.RightLineSum; i < LeftTurn[0][2]; i++)
        {
            MiddleLine[i][0] = (RightLine[imgInfo.RightLineSum][0] + LeftLine[i][0]) / 2;
            MiddleLine[i][1] = (RightLine[imgInfo.RightLineSum][1] + LeftLine[i][1]) / 2;
        }
        imgInfo.MiddleLineCount = LeftTurn[0][2];
    }
    else
    {
        //拟合有效直道中线
        if (imgInfo.LeftLineSum <= imgInfo.RightLineSum)
        {
            for (i = 0; i < imgInfo.LeftLineSum; i++)
            {
                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
            }
            for (i = imgInfo.LeftLineSum; i < imgInfo.RightLineSum; i++)
            {
                MiddleLine[i][0] = (LeftLine[imgInfo.LeftLineSum][0] + RightLine[i][0]) / 2;
                MiddleLine[i][1] = (LeftLine[imgInfo.LeftLineSum][1] + RightLine[i][1]) / 2;
            }
            imgInfo.MiddleLineCount = imgInfo.RightLineSum;
        }
        else
        {
            for (i = 0; i < imgInfo.RightLineSum; i++)
            {
                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
            }
            for (i = imgInfo.RightLineSum; i < imgInfo.LeftLineSum; i++)
            {
                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineSum][0]) / 2;
                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineSum][1]) / 2;
            }
            imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
        }
    }

    //Count = 0;
    //j = 0;
    //i = FindLineTurn(MiddleLine[0], 10, 4);
    //while (i != 0)
    //{
    //    MiddleTurn[Count][0] = MiddleLine[j + i][0];
    //    MiddleTurn[Count][1] = MiddleLine[j + i][1];
    //    MiddleTurn[Count][2] = j + i;
    //    Count++;
    //    if (Count > 2) break;
    //    j += i;
    //    i = FindLineTurn(MiddleTurn[j], 10, 4);
    //}
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       断头路扫线
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void nohead1(void)
{
    uint8 i = 0;
    if (!NoHeadFlag) NoHeadFlag = 1;

    if (imgInfo.NoHeadDir)
    {
        LeftTurn[0][0] = 0;
        for (i = 10; i < 90; i++)
        {
            if (!imageBin[i][40] && imageBin[i + 1][40])
            {
                LeftTurn[0][0] = i;
                LeftTurn[0][1] = 40;
                break;
            }
        }
        if (LeftTurn[0][0] > 40) NoHeadFlag = 2;
    }
    else
    {
        RightTurn[0][0] = 0;
        for (i = 10; i < 90; i++)
        {
            if (!imageBin[i][147] && imageBin[i + 1][147])
            {
                RightTurn[0][0] = i;
                RightTurn[0][1] = 147;
                break;
            }
        }
        if (RightTurn[0][0] > 40) NoHeadFlag = 2;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       三叉路扫线
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void threefork1(void)
{
    uint8 i, j, Count, NearNum = 2;

    if (ThreeForkFlag == 0)
    {
        Count = 0;
        if (LeftTurn[0][1] > 10)
        {
            memset(&LeftTurn[1][0], 0, 9);
            for (j = LeftTurn[0][1] + 5; j < LeftTurn[0][1] + WidthBuffer[LeftTurn[0][0]] - 5; j += 3)
            {
                for (i = LeftTurn[0][0]; i > 25; i--)
                {
                    if (!imageBin[i - 11][j] && imageBin[i][j] && imageBin[i - 10][j])
                    {
                        LeftTurn[2][0] = LeftTurn[1][0];
                        LeftTurn[2][1] = LeftTurn[1][1];

                        LeftTurn[1][0] = i - 10;
                        LeftTurn[1][1] = j;
                        break;
                    }
                }
                if (LeftTurn[1][0] == LeftTurn[2][0]) Count++;
                else if (LeftTurn[1][0] < LeftTurn[2][0] || Count>5) break;
                else Count = 0;
            }
        }
        else
        {
            if (!LeftLine[0][1]) LeftLine[0][1] = 3;
            memset(&LeftTurn[1][0], 0, 9);
            for (j = LeftLine[0][1] + 5; j < LeftLine[0][1] + WidthBuffer[119] - 5; j += 3)
            {
                for (i = 117; i > 25; i--)
                {
                    if (!imageBin[i - 11][j] && imageBin[i][j] && imageBin[i - 10][j])
                    {
                        LeftTurn[2][0] = LeftTurn[1][0];
                        LeftTurn[2][1] = LeftTurn[1][1];

                        LeftTurn[1][0] = i - 10;
                        LeftTurn[1][1] = j;
                        break;
                    }
                }
                if (LeftTurn[1][0] == LeftTurn[2][0]) Count++;
                else if (LeftTurn[1][0] < LeftTurn[2][0] || Count>5) break;
                else Count = 0;
            }
        }

        Count = 0;
        if (RightTurn[0][1] < 177 && RightTurn[0][1])
        {
            memset(&RightTurn[1][0], 0, 9);
            for (j = RightTurn[0][1] - 5; j > RightTurn[0][1] - WidthBuffer[RightTurn[0][0]] + 5; j -= 3)
            {
                for (i = RightTurn[0][0]; i > 25; i--)
                {
                    if (!imageBin[i - 11][j] && imageBin[i][j] && imageBin[i - 10][j])
                    {
                        RightTurn[2][0] = RightTurn[1][0];
                        RightTurn[2][1] = RightTurn[1][1];

                        RightTurn[1][0] = i - 10;
                        RightTurn[1][1] = j;
                        break;
                    }
                }
                if (RightTurn[1][0] == RightTurn[2][0]) Count++;
                else if (RightTurn[1][0] < RightTurn[2][0] || Count>5) break;
                else Count = 0;
            }
        }
        else
        {
            if (!RightLine[0][1]) RightLine[0][1] = 187;
            memset(&RightTurn[1][0], 0, 9);
            for (j = RightLine[0][1] - 5; j > RightLine[0][1] - WidthBuffer[119] + 5; j -= 3)
            {
                for (i = 117; i > 25; i--)
                {
                    if (!imageBin[i - 11][j] && imageBin[i][j] && imageBin[i - 10][j])
                    {
                        RightTurn[2][0] = RightTurn[1][0];
                        RightTurn[2][1] = RightTurn[1][1];

                        RightTurn[1][0] = i - 10;
                        RightTurn[1][1] = j;
                        break;
                    }
                }
                if (RightTurn[1][0] == RightTurn[2][0]) Count++;
                else if (RightTurn[1][0] < RightTurn[2][0] || Count>5) break;
                else Count = 0;
            }
        }

        turnpoint[0][0] = 0;
        turnpoint[0][1] = 0;
        if (LeftTurn[2][0] - RightTurn[2][0] > -10 && LeftTurn[2][0] - RightTurn[2][0] < 10)
        {
            if (!imgInfo.ThreeForkDir)
            {
                turnpoint[0][0] = LeftTurn[2][0];
                turnpoint[0][1] = LeftTurn[2][1];
            }
            else
            {
                turnpoint[0][0] = RightTurn[2][0];
                turnpoint[0][1] = RightTurn[2][1];
            }
        }

        if (turnpoint[0][0])
        {
            if (!ThreeForkOutFlag)
            {
                if (!imgInfo.ThreeForkDir)
                {
                    imgInfo.RightLineCount += repair(RightLine[imgInfo.RightLineCount], turnpoint[0]);
                    imgInfo.LeftLineCount = imgInfo.LeftLineSum;

                    i = imgInfo.RightLineCount;
                    while (RightLine[i][0] > 3 && RightLine[i][1] > 4 && RightLine[i][1] < 184 && i < imgInfo.LeftLineCount)
                    {
                        NearNum = FindNext(&RightLine[i][0], NearNum + 2, 1);

                        if (NearNum == 99) break;

                        i++;
                    }
                    imgInfo.RightLineCount = i;
                }
                else
                {
                    imgInfo.LeftLineCount += repair(LeftLine[imgInfo.LeftLineCount], turnpoint[0]);
                    imgInfo.RightLineCount = imgInfo.RightLineSum;

                    i = imgInfo.LeftLineCount;
                    while (LeftLine[i][0] > 3 && LeftLine[i][1] > 4 && LeftLine[i][1] < 184 && i < imgInfo.RightLineCount)
                    {
                        if (NearNum < 2) NearNum = FindNext(&LeftLine[i][0], NearNum + 6, 0);
                        else NearNum = FindNext(&LeftLine[i][0], NearNum - 2, 0);

                        if (NearNum == 99) break;

                        i++;
                    }
                    imgInfo.LeftLineCount = i;
                }

                //拟合有效直道中线
                if (imgInfo.LeftLineCount <= imgInfo.RightLineCount)
                {
                    for (i = 0; i < imgInfo.LeftLineCount - 1; i++)
                    {
                        MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
                        MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
                    }
                    for (i = imgInfo.LeftLineCount - 1; i < imgInfo.RightLineCount; i++)
                    {
                        MiddleLine[i][0] = (LeftLine[imgInfo.LeftLineCount - 1][0] + RightLine[i][0]) / 2;
                        MiddleLine[i][1] = (LeftLine[imgInfo.LeftLineCount - 1][1] + RightLine[i][1]) / 2;
                    }
                    imgInfo.MiddleLineCount = imgInfo.RightLineCount;
                }
                else
                {
                    for (i = 0; i < imgInfo.RightLineCount - 1; i++)
                    {
                        MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
                        MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
                    }
                    for (i = imgInfo.RightLineCount - 1; i < imgInfo.LeftLineCount; i++)
                    {
                        MiddleLine[i][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineCount - 1][0]) / 2;
                        MiddleLine[i][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineCount - 1][1]) / 2;
                    }
                    imgInfo.MiddleLineCount = imgInfo.LeftLineCount;
                }
            }
            else
            {
                MiddleLine[0][0] = 117;
                MiddleLine[0][1] = 94;
                imgInfo.MiddleLineCount = repair(MiddleLine[0], turnpoint[0]);
                imgInfo.MiddleLineCount += extend(MiddleLine[0], MiddleLine[imgInfo.MiddleLineCount - 3]) - 4;
            }
        }

        //！！三叉
//        if (turnpoint[0][0] > 40) ThreeForkFlag = 1;//1.7

        if (ThreeForkCount == 0 && turnpoint[0][0] > ThreeForkForesee[0]) ThreeForkFlag = 1;
        else if (ThreeForkCount == 1 && turnpoint[0][0] > ThreeForkForesee[1]) ThreeForkFlag = 1;
        else if (ThreeForkCount == 2 && turnpoint[0][0] > ThreeForkForesee[2]) ThreeForkFlag = 1;
        else if (ThreeForkCount == 3 && turnpoint[0][0] > ThreeForkForesee[3]) ThreeForkFlag = 1;
        else if (ThreeForkCount == 4 && turnpoint[0][0] > ThreeForkForesee[4]) ThreeForkFlag = 1;
        else if (ThreeForkCount == 5 && turnpoint[0][0] > ThreeForkForesee[5]) ThreeForkFlag = 1;
        else if (ThreeForkCount == 6 && turnpoint[0][0] > ThreeForkForesee[6]) ThreeForkFlag = 1;
        else if (ThreeForkCount == 7 && turnpoint[0][0] > ThreeForkForesee[7]) ThreeForkFlag = 1;

    }
    else if (ThreeForkFlag == 1)
    {
        BadThreeForkFlag = 0;
        ThreeForkTime++;

        if (ThreeForkTime > 13)
        {
            if (imgInfo.LeftLineSum > 60 || imgInfo.RightLineSum > 60 || ThreeForkTime > 20) ThreeForkFlag = 2;
        }
    }
    else if (ThreeForkFlag == 2)
    {
        if (imgInfo.LeftLineSum > imgInfo.RightLineSum)
        {
            for (i = 0; i < imgInfo.LeftLineSum; i++)
            {
                if (LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] / 2 > 180) break;
                MiddleLine[i][0] = LeftLine[i][0];
                MiddleLine[i][1] = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] / 2;
            }
            imgInfo.MiddleLineCount = i;
        }
        else if (imgInfo.RightLineSum > imgInfo.LeftLineSum)
        {
            for (i = 0; i < imgInfo.RightLineSum; i++)
            {
                if (RightLine[i][1] - WidthBuffer[RightLine[i][0]] / 2 < 10) break;
                MiddleLine[i][0] = RightLine[i][0];
                MiddleLine[i][1] = RightLine[i][1] - WidthBuffer[RightLine[i][0]] / 2;
            }
            imgInfo.MiddleLineCount = i;
        }

        if (imgInfo.LeftLineSum && imgInfo.RightLineSum)
        {
            uint8 j1, j2;

            Count = 0;
            if (imgInfo.LeftLineSum > 70 && imgInfo.RightLineSum > 70) Count = 50;
            else
            {
                if (imgInfo.LeftLineSum > imgInfo.RightLineSum)
                {
                    Count = 0;
                    for (i = 0; i < imgInfo.LeftLineSum && i < 120; i++)
                    {
                        j1 = LeftLine[i][0];
                        if (LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] + 10 > 175) break;
                        for (j2 = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] - 5; j2 < 175 && j2 < LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] + 10 && Count < 50; j2++)
                        {
                            if (imageBin[j1][j2] && !imageBin[j1][j2 + 1] && !imageBin[j1][j2 + 2])
                            {
                                TempLine1[Count][0] = j1;
                                TempLine1[Count][1] = j2;
                                Count++;
                                break;
                            }
                        }
                    }
                }
                else
                {
                    Count = 0;
                    for (i = 0; i < imgInfo.RightLineSum && i < 120; i++)
                    {
                        j1 = RightLine[i][0];
                        if (RightLine[i][1] - WidthBuffer[RightLine[i][0]] - 10 < 10) break;
                        for (j2 = RightLine[i][1] - WidthBuffer[RightLine[i][0]] + 5; j2 > 15 && j2 > RightLine[i][1] - WidthBuffer[RightLine[i][0]] - 10 && Count < 50; j2--)
                        {
                            if (imageBin[j1][j2] && !imageBin[j1][j2 - 1] && !imageBin[j1][j2 - 2])
                            {
                                TempLine1[Count][0] = j1;
                                TempLine1[Count][1] = j2;
                                Count++;
                                break;
                            }
                        }
                    }
                }
            }

            if (Count > 30)
            {
                //                if(ThreeForkCount)
                //                {
                //                    if(imgInfo.ThreeForkDir!=0) imgInfo.ThreeForkDir = 0;
                //                    else imgInfo.ThreeForkDir = 1;
                //                }
                ThreeForkTime = 0;
                ThreeForkFlag = 0;
                if (!ThreeForkOutFlag) ThreeForkOutFlag = 1;
                else ThreeForkOutFlag = 0;
                imgInfo.nextroadType = Straight;
                ThreeForkCount++;
            }
        }
    }
}


////-------------------------------------------------------------------------------------------------------------------
//// @brief       十字扫线
//// @param       无
//// @return      void
//// Sample usage:
////-------------------------------------------------------------------------------------------------------------------
//void cross(void)
//{
//    uint8 i;
//    if (CloseFlag == 1)
//    {
//        int8 j, k;
//        memset(turnpoint, 0, 6);
//        j = FindTurn(TempLine1[0], 15, 2, 1) - 2;
//        k = FindTurn(TempLine2[0], 15, 2, 1) - 2;
//        if (j != -2 && k != -2)
//        {
//            imgInfo.LeftLineCount += repair(LeftLine[imgInfo.LeftLineCount], TempLine1[j]);
//            imgInfo.RightLineCount += repair(RightLine[imgInfo.RightLineCount], TempLine2[k]);
//            turnpoint[0][0] = TempLine1[j][0];
//            turnpoint[0][1] = TempLine1[j][1];
//            turnpoint[1][0] = TempLine2[k][0];
//            turnpoint[1][1] = TempLine2[k][1];
//        }
//    }
//    else
//    {
//        uint8 j, k, NearNum;
//        LeftLine[0][0] = 115;
//        LeftLine[0][1] = 5;
//
//        RightLine[0][0] = 115;
//        RightLine[0][1] = 183;
//
//        turnpoint[0][0] -= 5;
//        j = turnpoint[0][0];
//        for (k = turnpoint[0][1] - 20; k < turnpoint[0][1] + 20; k++)
//        {
//            if (!imageBin[j][k] && imageBin[j][k + 1])
//            {
//                TempLine1[0][0] = j;
//                TempLine1[0][1] = k + 1;
//            }
//            if (k > 254) break;
//        }
//
//        NearNum = 0;
//        for (i = 0; i < 20; i++)
//        {
//            NearNum = FindNext(&TempLine1[i][0], NearNum + 2, 1);
//            if (NearNum == 99) break;
//
//            if (TempLine1[i][0] > turnpoint[0][0])
//            {
//                turnpoint[0][0] = TempLine1[i][0];
//                turnpoint[0][1] = TempLine1[i][1];
//            }
//        }
//        TempLine1[i][0] = 0;
//        imgInfo.LeftLineCount = repair(LeftLine[0], turnpoint[0]);
//
//        NearNum = 0;
//        j = imgInfo.LeftLineCount;
//        for (i = 0; i < 20; i++)
//        {
//            if (NearNum < 2) NearNum = FindNext(&LeftLine[i + j][0], NearNum + 6, 0);
//            else NearNum = FindNext(&LeftLine[i + j][0], NearNum - 2, 0);
//            if (NearNum == 99) break;
//        }
//        imgInfo.LeftLineCount += i;
//
//        turnpoint[1][0] -= 5;
//        j = turnpoint[1][0];
//        for (k = turnpoint[1][1] + 20; k > turnpoint[1][1] - 20; k--)
//        {
//            if (!imageBin[j][k] && imageBin[j][k - 1])
//            {
//                TempLine2[0][0] = j;
//                TempLine2[0][1] = k - 1;
//            }
//            if (k == 0) break;
//        }
//
//        NearNum = 4;
//        for (i = 0; i < 20; i++)
//        {
//            if (NearNum < 2) NearNum = FindNext(&TempLine2[i][0], NearNum + 6, 0);
//            else NearNum = FindNext(&TempLine2[i][0], NearNum - 2, 0);
//            if (NearNum == 99) break;
//
//            if (TempLine2[i][0] > turnpoint[1][0])
//            {
//                turnpoint[1][0] = TempLine2[i][0];
//                turnpoint[1][1] = TempLine2[i][1];
//            }
//        }
//        TempLine2[i][0] = 0;
//        imgInfo.RightLineCount = repair(RightLine[0], turnpoint[1]);
//
//        NearNum = 4;
//        j = imgInfo.RightLineCount;
//        for (i = 0; i < 20; i++)
//        {
//            NearNum = FindNext(&RightLine[i + j][0], NearNum + 2, 1);
//            if (NearNum == 99) break;
//        }
//        imgInfo.RightLineCount += i;
//    }
//
//    //拟合有效直道中线
//    if (imgInfo.LeftLineCount <= imgInfo.RightLineCount)
//    {
//        for (i = 0; i < imgInfo.LeftLineCount; i++)
//        {
//            MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
//            MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
//        }
//        for (i = imgInfo.LeftLineCount; i < imgInfo.RightLineCount; i++)
//        {
//            MiddleLine[i][0] = (LeftLine[imgInfo.LeftLineCount][0] + RightLine[i][0]) / 2;
//            MiddleLine[i][1] = (LeftLine[imgInfo.LeftLineCount][1] + RightLine[i][1]) / 2;
//        }
//        imgInfo.MiddleLineCount = imgInfo.RightLineCount;
//        imgInfo.top = LeftLine[imgInfo.LeftLineCount][0];
//    }
//    else
//    {
//        for (i = 0; i < imgInfo.RightLineCount; i++)
//        {
//            MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
//            MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
//        }
//        for (i = imgInfo.RightLineCount; i < imgInfo.LeftLineCount; i++)
//        {
//            MiddleLine[i][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineCount][0]) / 2;
//            MiddleLine[i][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineCount][1]) / 2;
//        }
//        imgInfo.MiddleLineCount = imgInfo.LeftLineCount;
//        imgInfo.top = RightLine[imgInfo.RightLineCount][0];
//    }
//}

//-------------------------------------------------------------------------------------------------------------------
// @brief       十字扫线
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void cross1(void)
{
    uint8 i, j, Count, NearNum;
    if (CrossFlag == 0 || CrossFlag == 1)
    {
        if (!CrossFlag)
        {
            for (i = 90; i > 70; i--)
            {
                if (imageBin[i][5] || imageBin[i][182])
                {
                    CrossFlag = 1;
                    break;
                }
            }
        }
        else
        {
            CrossFlag = 2;
            for (i = 90; i > 70; i--)
            {
                if (!imageBin[i][10] || !imageBin[i][174])
                {
                    CrossFlag = 1;
                    break;
                }
            }
        }

        if (imgInfo.LeftLineCount > 30) imgInfo.LeftLineSum = imgInfo.LeftLineCount + extend(LeftLine[imgInfo.LeftLineCount - 15], LeftLine[imgInfo.LeftLineCount]);
        if (imgInfo.RightLineCount > 30) imgInfo.RightLineSum = imgInfo.RightLineCount + extend(RightLine[imgInfo.RightLineCount - 15], RightLine[imgInfo.RightLineCount]);

        if (imgInfo.LeftLineSum <= imgInfo.RightLineSum)
        {
            for (i = 0; i < imgInfo.LeftLineSum; i++)
            {
                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
            }
            for (i = imgInfo.LeftLineSum; i < imgInfo.RightLineSum; i++)
            {
                MiddleLine[i][0] = (LeftLine[imgInfo.LeftLineSum][0] + RightLine[i][0]) / 2;
                MiddleLine[i][1] = (LeftLine[imgInfo.LeftLineSum][1] + RightLine[i][1]) / 2;
            }
            imgInfo.MiddleLineCount = imgInfo.RightLineSum;
        }
        else
        {
            for (i = 0; i < imgInfo.RightLineSum; i++)
            {
                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
            }
            for (i = imgInfo.RightLineSum; i < imgInfo.LeftLineSum; i++)
            {
                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineSum][0]) / 2;
                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineSum][1]) / 2;
            }
            imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
        }
    }

    if (CrossFlag == 2)
    {
        CrossFlag = 3;
        for (i = 100; i > 80; i--)
        {
            if (imageBin[i][3] || imageBin[i][184])
            {
                CrossFlag = 2;
                break;
            }
        }

        //if (LeftTurn[0][0] && RightTurn[0][0])
        //{
        //    j = LeftTurn[0][1];
        //    for (i=LeftTurn[0][0]; i > 20; i--)
        //    {
        //        if (imageBin[i][j] && !imageBin[i - 1][j] && !imageBin[i - 2][j])
        //        {
        //            TempLine3[0][0] = i;
        //            TempLine3[0][1] = j;
        //        }
        //    }
        //}

        i = 100;
        while (!imageBin[i][20] || !imageBin[i][167])
        {
            i--;
            if (i < 10) break;
        }
        while (imageBin[i][20] || imageBin[i][167])
        {
            i--;
            if (i < 10) break;
        }//找到十字前端

        if (i > 10)//能找到
        {
            memset(TempLine1[0], 0, 40);
            memset(TempLine2[0], 0, 40);

            if (!imgInfo.LeftLineSum) CrossOutFlag = 1;
            i -= 10;
            for (j = 3; j < 184; j++)
            {
                if (!imageBin[i][j] && imageBin[i][j + 1] && imageBin[i][j + 2])
                {
                    TempLine1[0][0] = i;
                    TempLine1[0][1] = j + 1;
                    break;
                }
            }
            for (j = 184; j > 3; j--)
            {
                if (!imageBin[i][j] && imageBin[i][j - 1] && imageBin[i][j - 2])
                {
                    TempLine2[0][0] = i;
                    TempLine2[0][1] = j - 1;
                    break;
                }
            }//找起始点

            Count = 0;
            NearNum = 0;
            while (Count < 10)
            {
                if (NearNum > 5) NearNum = FindNext(&TempLine1[Count][0], NearNum - 6, 1);
                else NearNum = FindNext(&TempLine1[Count][0], NearNum + 2, 1);

                Count++;
            }

            Count = 0;
            NearNum = 4;
            while (Count < 10)
            {
                NearNum = FindNext(&TempLine2[Count][0], NearNum - 2, 0);

                Count++;
            }

            i = extend(TempLine1[0], TempLine1[9]) + 8;
            j = extend(TempLine2[0], TempLine2[9]) + 8;

            while (TempLine1[i][0] < 115)
            {
                TempLine1[i + 1][0] = TempLine1[i][0] + 1;
                TempLine1[i + 1][1] = 3;
                i++;
            }
            while (TempLine2[j][0] < 115)
            {
                TempLine2[j + 1][0] = TempLine2[j][0] + 1;
                TempLine2[j + 1][1] = 184;
                j++;
            }

            Count = 0;
            while (i > 1 && j > 1)
            {
                MiddleLine[Count][0] = (TempLine1[i][0] + TempLine2[j][0]) / 2;
                MiddleLine[Count][1] = (TempLine1[i][1] + TempLine2[j][1]) / 2;
                Count++;
                i--;
                j--;
            }
            imgInfo.MiddleLineCount = Count;

            LeftLine[0][0] = TempLine1[0][0];
            LeftLine[0][1] = TempLine1[0][1];
            i = 0;
            NearNum = 2;
            while (LeftLine[i][0] > 3 && LeftLine[i][1] > 3 && LeftLine[i][1] < 184 && i + Count < 200)
            {
                if (NearNum < 2) NearNum = FindNext(&LeftLine[i][0], NearNum + 6, 0);
                else NearNum = FindNext(&LeftLine[i][0], NearNum - 2, 0);

                if (NearNum == 99) break;
                i++;
            }
            imgInfo.LeftLineSum = i;

            RightLine[0][0] = TempLine2[0][0];
            RightLine[0][1] = TempLine2[0][1];

            i = 0;
            NearNum = 2;
            while (RightLine[i][0] > 3 && RightLine[i][1] > 3 && RightLine[i][1] < 184 && i + Count < 200)
            {
                NearNum = FindNext(&RightLine[i][0], NearNum + 2, 1);

                if (NearNum == 99) break;
                i++;
            }
            imgInfo.RightLineSum = i;

            if (imgInfo.LeftLineSum <= imgInfo.RightLineSum)
            {
                for (i = 0; i < imgInfo.LeftLineSum; i++)
                {
                    MiddleLine[i + Count][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
                    MiddleLine[i + Count][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
                    if (i + Count > 200)break;
                }
                for (i = imgInfo.LeftLineSum; i < imgInfo.RightLineSum; i++)
                {
                    MiddleLine[i + Count][0] = (LeftLine[imgInfo.LeftLineSum][0] + RightLine[i][0]) / 2;
                    MiddleLine[i + Count][1] = (LeftLine[imgInfo.LeftLineSum][1] + RightLine[i][1]) / 2;
                    if (i + Count > 200)break;
                }
                imgInfo.MiddleLineCount = imgInfo.RightLineSum + Count;
            }
            else
            {
                for (i = 0; i < imgInfo.RightLineSum; i++)
                {
                    MiddleLine[i + Count][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
                    MiddleLine[i + Count][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
                    if (i + Count > 200)break;
                }
                for (i = imgInfo.RightLineSum; i < imgInfo.LeftLineSum; i++)
                {
                    MiddleLine[i + Count][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineSum][0]) / 2;
                    MiddleLine[i + Count][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineSum][1]) / 2;
                    if (i + Count > 200)break;
                }
                imgInfo.MiddleLineCount = imgInfo.LeftLineSum + Count;
            }
        }
    }
    if (CrossFlag == 3)
    {
        uint8 j, Count, NearNum;

        CrossFlag = 0;
        for (i = 119; i > 90; i--)
        {
            if (imageBin[i][4] || imageBin[i][183])
            {
                CrossFlag = 2;
                break;
            }
        }

        for (i = 80; i > 60; i--)
        {
            if (!imageBin[i][4] && !imageBin[i][183])
            {
                for (j = 3; j < 90; j++)
                {
                    if (!imageBin[i][j] && imageBin[i][j + 1] && imageBin[i][j + 2])
                    {
                        LeftLine[0][0] = i;
                        LeftLine[0][1] = j + 1;
                        break;
                    }
                }

                for (j = 184; j > 90; j--)
                {
                    if (!imageBin[i][j] && imageBin[i][j - 1] && imageBin[i][j - 2])
                    {
                        RightLine[0][0] = i;
                        RightLine[0][1] = j - 1;
                        break;
                    }
                }
            }
            if (LeftLine[0][0] && RightLine[0][0]) break;
        }

        Count = 0;
        NearNum = 2;

        while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 200)
        {
            if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
            else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);

            if (NearNum == 99) break;
            Count++;
        }
        imgInfo.LeftLineSum = Count;

        Count = 0;
        NearNum = 2;

        //右线邻域扫线
        while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 200)
        {
            NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

            if (NearNum == 99) break;
            Count++;
        }
        imgInfo.RightLineSum = Count;

        if (imgInfo.LeftLineSum <= imgInfo.RightLineSum)
        {
            for (i = 0; i < imgInfo.LeftLineSum; i++)
            {
                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
            }
            for (i = imgInfo.LeftLineSum; i < imgInfo.RightLineSum; i++)
            {
                MiddleLine[i][0] = (LeftLine[imgInfo.LeftLineSum][0] + RightLine[i][0]) / 2;
                MiddleLine[i][1] = (LeftLine[imgInfo.LeftLineSum][1] + RightLine[i][1]) / 2;
            }
            imgInfo.MiddleLineCount = imgInfo.RightLineSum;
            imgInfo.top = RightLine[imgInfo.RightLineSum][0];
        }
        else
        {
            for (i = 0; i < imgInfo.RightLineSum; i++)
            {
                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
            }
            for (i = imgInfo.RightLineSum; i < imgInfo.LeftLineSum; i++)
            {
                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineSum][0]) / 2;
                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineSum][1]) / 2;
            }
            imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
            imgInfo.top = LeftLine[imgInfo.LeftLineSum][0];
        }
    }
}


//-------------------------------------------------------------------------------------------------------------------
// @brief       左岔路扫线
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
//void leftt(void)
//{
//    uint8 i, j, NearNum;
//    int8 k;
//    if (CloseFlag == 1)
//    {
//        j = LeftTurn[0][1] - 5;
//        for (i = LeftTurn[0][0] - 5; i > 0; i--)
//        {
//            if (imageBin[i][j] && !imageBin[i - 1][j])
//            {
//                TempLine1[0][0] = i;
//                TempLine1[0][1] = j;
//                break;
//            }
//        }
//
//        NearNum = 2;
//        for (i = 0; i < 40; i++)
//        {
//            if (NearNum < 2) NearNum = FindNext(&TempLine1[i][0], NearNum + 6, 0);
//            else NearNum = FindNext(&TempLine1[i][0], NearNum - 2, 0);
//            if (NearNum == 99) break;
//        }
//
//        memset(turnpoint[0], 0, 2);
//        k = FindTurn(TempLine1[0], 10, 2, 1) - 2;
//        turnpoint[0][0] = TempLine1[k][0];
//        turnpoint[0][1] = TempLine1[k][1];
//
//        if (k != -2)
//        {
//            imgInfo.LeftLineCount += repair(LeftLine[imgInfo.LeftLineCount], TempLine1[k]);
//        }
//
//        //拟合有效直道中线
//        if (imgInfo.LeftLineCount <= imgInfo.RightLineCount)
//        {
//            for (i = 0; i < imgInfo.LeftLineCount; i++)
//            {
//                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
//                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
//            }
//            for (i = imgInfo.LeftLineCount; i < imgInfo.RightLineCount; i++)
//            {
//                MiddleLine[i][0] = (LeftLine[imgInfo.LeftLineCount][0] + RightLine[i][0]) / 2;
//                MiddleLine[i][1] = (LeftLine[imgInfo.LeftLineCount][1] + RightLine[i][1]) / 2;
//            }
//            imgInfo.MiddleLineCount = imgInfo.LeftLineCount;
//            imgInfo.top = LeftLine[imgInfo.LeftLineCount][0];
//        }
//        else
//        {
//            for (i = 0; i < imgInfo.RightLineCount; i++)
//            {
//                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
//                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
//            }
//            for (i = imgInfo.RightLineCount; i < imgInfo.LeftLineCount; i++)
//            {
//                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineCount][0]) / 2;
//                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineCount][1]) / 2;
//            }
//            imgInfo.MiddleLineCount = imgInfo.RightLineCount;
//            imgInfo.top = RightLine[imgInfo.RightLineCount][0];
//        }
//        TempLine2[0][0] = LeftLine[0][0];
//        TempLine2[0][1] = LeftLine[0][1];
//    }
//    else
//    {
//        j = turnpoint[0][1] - 20;
//        for (i = turnpoint[0][0] + 15; i > 0; i--)
//        {
//            if (imageBin[i][j] && !imageBin[i - 1][j])
//            {
//                TempLine1[0][0] = i;
//                TempLine1[0][1] = j;
//                break;
//            }
//        }
//
//        NearNum = 2;
//        for (i = 0; i < 40; i++)
//        {
//            if (NearNum < 2) NearNum = FindNext(&TempLine1[i][0], NearNum + 6, 0);
//            else NearNum = FindNext(&TempLine1[i][0], NearNum - 2, 0);
//            if (NearNum == 99) break;
//        }
//
//        memset(turnpoint[0], 0, 2);
//        k = FindTurn(TempLine1[0], 10, 2, 1) - 2;
//        if (k != -2)
//        {
//            turnpoint[0][0] = TempLine1[k][0];
//            turnpoint[0][1] = TempLine1[k][1];
//            LeftLine[0][0] = TempLine2[0][0];
//            LeftLine[0][1] = TempLine2[0][1];
//            imgInfo.LeftLineCount += repair(LeftLine[0], TempLine1[k]);
//        }
//
//        i = LeftLine[0][0];
//        //寻找右起点
//        for (j = 184; j > 90; j--)
//        {
//            if (!imageBin[i][j] && imageBin[i][j - 1] && imageBin[i][j - 2])
//            {
//                RightLine[0][0] = i;
//                RightLine[0][1] = j - 1;
//                break;
//            }
//        }
//
//        i = 0;
//        //右线邻域扫线
//        while (RightLine[i][0] > 3 && RightLine[i][1] > 3 && RightLine[i][1] < 184 && i < 200)
//        {
//            NearNum = FindNext(&RightLine[i][0], NearNum + 2, 1);
//            if (NearNum == 99) break;
//            i++;
//        }
//        imgInfo.RightLineCount = i;
//
//        //拟合有效直道中线
//        if (imgInfo.LeftLineCount <= imgInfo.RightLineCount)
//        {
//            for (i = 0; i < imgInfo.LeftLineCount; i++)
//            {
//                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
//                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
//            }
//            for (i = imgInfo.LeftLineCount; i < imgInfo.RightLineCount; i++)
//            {
//                MiddleLine[i][0] = (LeftLine[imgInfo.LeftLineCount][0] + RightLine[i][0]) / 2;
//                MiddleLine[i][1] = (LeftLine[imgInfo.LeftLineCount][1] + RightLine[i][1]) / 2;
//            }
//            imgInfo.MiddleLineCount = imgInfo.LeftLineCount;
//            imgInfo.top = LeftLine[imgInfo.LeftLineCount][0];
//        }
//        else
//        {
//            for (i = 0; i < imgInfo.RightLineCount; i++)
//            {
//                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
//                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
//            }
//            for (i = imgInfo.RightLineCount; i < imgInfo.LeftLineCount; i++)
//            {
//                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineCount][0]) / 2;
//                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineCount][1]) / 2;
//            }
//            imgInfo.MiddleLineCount = imgInfo.RightLineCount;
//            imgInfo.top = RightLine[imgInfo.RightLineCount][0];
//        }
//    }
//}

//-------------------------------------------------------------------------------------------------------------------
// @brief       左岔路扫线
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void leftt1(void)
{
    uint8 i, j, NearNum, Count = 0;

    if (!LeftMax) RightMax = 119;
    if (LeftMax > 60 && LeftTFlag <= 2)
    {
        uint8 RightI = 0;
        for (i = LeftMax - 5; i > LeftMax - 50; i--)
        {
            while (RightLine[RightI][0] > i) RightI++;
            NearNum = Count;
            Count = 0;

            for (j = RightLine[RightI][1] - 2; j > RightLine[RightI][1] - WidthBuffer[i] + 5; j--)
            {
                if (imageBin[i][j] && !imageBin[i][j - 1])
                {
                    Count++;
                    j -= 3;
                }
            }
            if (Count)
            {
                i -= 3;
                if (NearNum < Count) NearNum = Count;
            }

            if (NearNum > 4)
            {
                LeftTFlag = 10;
                break;
            }
        }
    }

    if (LeftTFlag == 0 || LeftTFlag == 1 || LeftTFlag == 2)
    {
        if (LeftTFlag == 0)
        {
            for (i = 100; i > 50; i--) if (!imageBin[i][4]) LeftTFlag = 1;
        }
        else if (LeftTFlag == 1)
        {
            LeftTFlag = 2;
            for (i = 110; i > 80; i--) if (!imageBin[i][4]) LeftTFlag = 1;
        }
        else if (LeftTFlag == 2)
        {
            LeftTFlag = 3;
            for (i = 110; i > 80; i--) if (imageBin[i][3]) LeftTFlag = 2;
        }

        if (imgInfo.RightLineSum < 60 || RightMax>60)
        {
            turnpoint[0][0] = 0;
            RightLine[0][0] = 117;
            RightLine[0][1] = 183;
            for (i = 110; i > 10; i--)
            {
                if (imageBin[i + 1][183] && !imageBin[i][183] && !imageBin[i - 1][183]) break;
            }

            if (i > 15)
            {
                for (j = 183; j > 60; j--)
                {
                    if (!imageBin[i][j] && imageBin[i][j - 1] && imageBin[i][j - 2])
                    {
                        turnpoint[0][0] = i;
                        turnpoint[0][1] = j - 1;
                        break;
                    }
                }

                if (!turnpoint[0][0])
                {
                    turnpoint[0][0] = i;
                    turnpoint[0][1] = 183;
                }
            }

            if (turnpoint[0][0])
            {
                imgInfo.RightLineCount = repair(RightLine[0], turnpoint[0]);
                Count = ScanLine(RightLine[imgInfo.RightLineCount], 2, 1);
                if (imgInfo.RightLineCount + Count > 150) imgInfo.RightLineSum = 150;
                else imgInfo.RightLineSum = imgInfo.RightLineCount + Count;
            }
        }

        for (i = 0; i < imgInfo.RightLineSum; i++)
        {
            MiddleLine[i][0] = RightLine[i][0];
            MiddleLine[i][1] = RightLine[i][1] - WidthBuffer[RightLine[i][0]] / 2;
        }
        imgInfo.MiddleLineCount = imgInfo.RightLineSum;
    }
    //    if (LeftTFlag == 2)
    //    {
    //        LeftTFlag = 3;
    //        for (i = 110; i > 80; i--) if (imageBin[i][3]) LeftTFlag = 2;
    //
    //        NearNum = 3;
    //        Count = 0;
    //
    //        for (j = 184; j > 60; j--)
    //        {
    //            if (!imageBin[115][j] && imageBin[115][j - 1] && imageBin[115][j - 2])
    //            {
    //                RightLine[0][0] = 115;
    //                RightLine[0][1] = j - 1;
    //                break;
    //            }
    //        }
    //
    //        //右线邻域扫线
    //        while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 200)
    //        {
    //            NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);
    //
    //            Count++;
    //
    //            if (NearNum == 99) break;
    //        }
    //        imgInfo.RightLineSum = Count;
    //
    //        for (i = 0; i < imgInfo.RightLineSum; i++)
    //        {
    //            MiddleLine[i][0] = RightLine[i][0];
    //            MiddleLine[i][1] = RightLine[i][1] - WidthBuffer[RightLine[i][0]] / 2;
    //        }
    //        imgInfo.MiddleLineCount = imgInfo.RightLineSum;
    //        ChangeCount = 0;
    //    }
    if (LeftTFlag == 3)
    {
        ChangeCount++;
        uint8 j1, j2;

        //拟合有效直道中线
        for (i = 0; i < imgInfo.LeftLineSum; i++)
        {
            MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
            MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
        }
        for (i = imgInfo.LeftLineSum; i < imgInfo.RightLineSum; i++)
        {
            MiddleLine[i][0] = (RightLine[i][0] + LeftLine[imgInfo.LeftLineSum][0]) / 2;
            MiddleLine[i][1] = (RightLine[i][1] + LeftLine[imgInfo.LeftLineSum][1]) / 2;
        }
        imgInfo.MiddleLineCount = imgInfo.RightLineSum;
        imgInfo.top = RightLine[imgInfo.RightLineSum][0];


        for (i = 0; i < imgInfo.RightLineCount; i++)
        {
            if (RightLine[i][1] < 182) break;
        }

        j = FindTurn1(RightLine[i], 5, 5, 1);
        if (j && RightLine[i + j][1] > 15)
        {
            turnpoint[0][0] = RightLine[i + j][0];
            turnpoint[0][1] = RightLine[i + j][1];

            LeftTFlag = 4;
            for (i = 0; i < 5; i++) if (!imageBin[turnpoint[0][0] - i][turnpoint[0][1]]) LeftTFlag = 3;
        }

        Count = 0;
        for (i = imgInfo.RightLineCount; i > imgInfo.RightLineCount - 30; i--)
        {
            j1 = RightLine[i][0];
            if (RightLine[i][1] - WidthBuffer[RightLine[i][0]] - 10 < 15) break;
            for (j2 = RightLine[i][1] - WidthBuffer[RightLine[i][0]] + 5; j2 > 15 && j2 > LeftLine[i][1] - WidthBuffer[LeftLine[i][0]] - 10 && Count < 20; j2--)
            {
                if (imageBin[j1][j2] && !imageBin[j1][j2 - 1] && !imageBin[j1][j2 - 2])
                {
                    TempLine1[Count][0] = j1;
                    TempLine1[Count][1] = j2;
                    Count++;
                    break;
                }
            }
        }

        if (Count > 8)
        {
            LeftTFlag = 0;
            CircleFlag = 11;
            CircleCount = 0;
            imgInfo.nextroadType = LeftCircle;
        }
    }
    if (LeftTFlag == 4)
    {
        if (!imgInfo.RightLineCount)
        {
            LeftLine[0][0] = 115;
            LeftLine[0][1] = 3;

            for (i = 115; i > 10; i--)
            {
                if (!imageBin[i][184]) break;
            }

            for (j = 184; j > 60; j--)
            {
                if (!imageBin[i][j] && imageBin[i][j - 1] && imageBin[i][j - 2])
                {
                    RightLine[0][0] = i;
                    RightLine[0][1] = j - 1;
                    break;
                }
            }

            Count = 0;
            NearNum = 3;

            //右线邻域扫线
            while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 200)
            {
                NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

                Count++;

                if (NearNum == 99) break;
            }
            imgInfo.RightLineSum = Count;
        }

        for (i = 0; i < imgInfo.RightLineCount; i++)
        {
            if (RightLine[i][1] < 180) break;
        }

        i = FindTurn1(RightLine[i], 6, 4, 1);

        if (i)
        {
            imgInfo.RightLineSum = extend(RightLine[i - 20], RightLine[i - 5]) + i;
            imgInfo.RightLineSum -= 5;
        }

        for (i = 0; i < imgInfo.RightLineSum; i++)
        {
            MiddleLine[i][0] = RightLine[i][0] / 2;
            MiddleLine[i][1] = RightLine[i][1] / 2;
        }
        imgInfo.MiddleLineCount = imgInfo.RightLineSum;
        imgInfo.top = LeftLine[imgInfo.RightLineSum][0];

        TempLine1[0][0] = 0;
        TempLine1[0][1] = 0;
        for (i = 5; i < 50; i++)
        {
            if (!imageBin[i][7] && imageBin[i + 1][7] && imageBin[i + 2][7])
            {
                TempLine1[0][0] = i + 1;
                TempLine1[0][1] = 7;
            }
        }

        if (TempLine1[0][0])
        {
            Count = 0;
            NearNum = 2;
            while (TempLine1[Count][0] > 3 && TempLine1[Count][1] > 3 && TempLine1[Count][1] < 183 && Count < 40)
            {
                if (NearNum < 2) NearNum = FindNext(&TempLine1[Count][0], NearNum + 6, 0);
                else NearNum = FindNext(&TempLine1[Count][0], NearNum - 2, 0);

                if (NearNum == 99) break;
                Count++;
            }

            turnpoint[0][0] = 0;
            turnpoint[0][1] = 0;

            if (Count > 35)
            {
                Count = extend1(TempLine1[0], TempLine1[34]) + 32;
                turnpoint[0][0] = TempLine1[0][0] / 2 + TempLine1[Count][0] / 2;
                turnpoint[0][1] = TempLine1[0][1] / 2 + TempLine1[Count][1] / 2;
            }

            Mark[0] = turnpoint[0][0];
            Mark[1] = turnpoint[0][1];
            //？出左断头
            if (turnpoint[0][0] && turnpoint[0][0] > LeftTForesee && turnpoint[0][1] > 45) LeftTFlag = 5;
        }
    }
    if (LeftTFlag == 5 || LeftTFlag == 6)
    {
        if (LeftTFlag == 5)
        {
            LeftTFlag = 6;
            for (i = 115; i > 90; i--)
            {
                if (!imageBin[i][3])
                {
                    LeftTFlag = 5;
                    break;
                }
            }
        }

        if (LeftTFlag == 6)
        {
            LeftTFlag = 7;
            for (i = 105; i > 70; i--)
            {
                if (imageBin[i][3])
                {
                    LeftTFlag = 6;
                    break;
                }
            }
        }

        for (i = 20; i < 115; i++)
        {
            turnpoint[1][0] = 0;
            turnpoint[1][1] = 0;
            if (!imageBin[i][MiddleValue] && imageBin[i + 1][MiddleValue])
            {
                turnpoint[1][0] = (i + 120) / 2;
                turnpoint[1][1] = 184;
                break;
            }
        }

        if (turnpoint[1][0] > LeftTForesee)
        {
            turnpoint[0][0] = turnpoint[1][0];
            turnpoint[0][1] = turnpoint[1][1];
        }

        MiddleLine[0][0] = 115;
        MiddleLine[0][1] = MiddleValue;

        imgInfo.MiddleLineCount = repair(MiddleLine[0], turnpoint[0]);
    }
    if (LeftTFlag == 7)
    {
        uint8 j1, j2;
        Count = 0;
        if (imgInfo.RightLineSum > imgInfo.LeftLineSum)
        {
            for (i = 0; i < imgInfo.RightLineSum && i < 120; i++)
            {
                MiddleLine[i][0] = RightLine[i][0];
                MiddleLine[i][1] = RightLine[i][1] - WidthBuffer[RightLine[i][0]] / 2;

                j1 = RightLine[i][0];
                for (j2 = RightLine[i][1] - WidthBuffer[RightLine[i][0]] + 5; j2 > 15 && j2 > RightLine[i][1] + WidthBuffer[RightLine[i][0]] - 10 && Count < 20; j2--)
                {
                    if (imageBin[j1][j2] && !imageBin[j1][j2 - 1] && !imageBin[j1][j2 - 2])
                    {
                        TempLine1[Count][0] = j1;
                        TempLine1[Count][1] = j2;
                        Count++;
                        break;
                    }
                }
            }
            if (imgInfo.RightLineSum > 120) imgInfo.MiddleLineCount = 120;
            else imgInfo.MiddleLineCount = imgInfo.RightLineSum;
        }
        else
        {
            for (i = 0; i < imgInfo.LeftLineSum && i < 120; i++)
            {
                MiddleLine[i][0] = LeftLine[i][0];
                MiddleLine[i][1] = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] / 2;

                j1 = LeftLine[i][0];
                for (j2 = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] - 5; j2 < 175 && j2 < LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] + 10 && Count < 20; j2++)
                {
                    if (imageBin[j1][j2] && !imageBin[j1][j2 + 1] && !imageBin[j1][j2 + 2])
                    {
                        TempLine1[Count][0] = j1;
                        TempLine1[Count][1] = j2;
                        Count++;
                        break;
                    }
                }
            }
            if (imgInfo.LeftLineSum > 120) imgInfo.MiddleLineCount = 120;
            else imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
        }

        if (Count > 10 || (imgInfo.LeftLineCount > 80 && imgInfo.RightLineCount > 80))
        {
            LeftTFlag = 0;
            imgInfo.nextroadType = Straight;
        }
    }

    if (LeftTFlag == 10 || LeftTFlag == 11)
    {
        //        uart_putchar(UART_3,0x51);
        if (LeftTFlag == 10)
        {
            //            uart_putchar(UART_3,0x52);
            if (!imgInfo.NoHeadDir)
            {
                LeftTFlag = 15;
                for (i = LGForesee; i > LGForesee - 15; i--)
                {
                    if (!imageBin[i][3])
                    {
                        LeftTFlag = 10;
                        break;
                    }
                }
            }
            else
            {
                LeftTFlag = 11;
                for (i = 115; i > 90; i--)
                {
                    if (!imageBin[i][3])
                    {
                        LeftTFlag = 10;
                        break;
                    }
                }
            }
            //            uart_putchar(UART_3,0x53);
        }
        else if (LeftTFlag == 11)
        {
            //            uart_putchar(UART_3,0x54);
            LeftTFlag = 0;
            for (i = 119; i > 90; i--)
            {
                if (imageBin[i][3])
                {
                    LeftTFlag = 11;
                    break;
                }
            }
            //            uart_putchar(UART_3,0x55);
        }

        //        uart_putchar(UART_3,0x56);
        for (j = 184; j > 60; j--)
        {
            if (!imageBin[115][j] && imageBin[115][j - 1] && imageBin[115][j - 2])
            {
                RightLine[0][0] = 115;
                RightLine[0][1] = j - 1;
                break;
            }
        }

        //        uart_putchar(UART_3,0x57);
        NearNum = 2;
        Count = 0;

        //右线邻域扫线
        while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 200)
        {
            NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

            Count++;

            if (NearNum == 99) break;
        }
        imgInfo.RightLineSum = Count;
        for (i = 0; i < imgInfo.RightLineSum; i++)
        {
            MiddleLine[i][0] = RightLine[i][0];
            MiddleLine[i][1] = RightLine[i][1] - WidthBuffer[RightLine[i][0]] / 2;
        }
        imgInfo.MiddleLineCount = imgInfo.RightLineSum;
        //        uart_putchar(UART_3,0x58);
    }

    if (LeftTFlag == 15) imgInfo.StopFlag = 1;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       右岔路扫线
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
//void rightt(void)
//{
//    uint8 i, j, NearNum;
//    int8 k;
//    if (CloseFlag == 1)
//    {
//        j = RightTurn[0][1] + 5;
//        for (i = RightTurn[0][0] - 5; i > 0; i--)
//        {
//            if (imageBin[i][j] && !imageBin[i - 1][j])
//            {
//                TempLine1[0][0] = i;
//                TempLine1[0][1] = j;
//                break;
//            }
//        }
//
//        NearNum = 2;
//        for (i = 0; i < 40; i++)
//        {
//            NearNum = FindNext(&TempLine1[i][0], NearNum + 2, 1);
//            if (NearNum == 99) break;
//        }
//
//        memset(turnpoint[0], 0, 2);
//        k = FindTurn(TempLine1[0], 10, 2, 1) - 2;
//        turnpoint[0][0] = TempLine1[k][0];
//        turnpoint[0][1] = TempLine1[k][1];
//
//        if (k != -2)
//        {
//            imgInfo.RightLineCount += repair(RightLine[imgInfo.RightLineCount], TempLine1[k]);
//        }
//
//        //拟合有效直道中线
//        if (imgInfo.LeftLineCount <= imgInfo.RightLineCount)
//        {
//            for (i = 0; i < imgInfo.LeftLineCount; i++)
//            {
//                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
//                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
//            }
//            for (i = imgInfo.LeftLineCount; i < imgInfo.RightLineCount; i++)
//            {
//                MiddleLine[i][0] = (LeftLine[imgInfo.LeftLineCount][0] + RightLine[i][0]) / 2;
//                MiddleLine[i][1] = (LeftLine[imgInfo.LeftLineCount][1] + RightLine[i][1]) / 2;
//            }
//            imgInfo.MiddleLineCount = imgInfo.LeftLineCount;
//            imgInfo.top = LeftLine[imgInfo.LeftLineCount][0];
//        }
//        else
//        {
//            for (i = 0; i < imgInfo.RightLineCount; i++)
//            {
//                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
//                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
//            }
//            for (i = imgInfo.RightLineCount; i < imgInfo.LeftLineCount; i++)
//            {
//                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineCount][0]) / 2;
//                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineCount][1]) / 2;
//            }
//            imgInfo.MiddleLineCount = imgInfo.RightLineCount;
//            imgInfo.top = RightLine[imgInfo.RightLineCount][0];
//        }
//        TempLine2[0][0] = RightLine[0][0];
//        TempLine2[0][1] = RightLine[0][1];
//    }
//    else
//    {
//        j = turnpoint[0][1] + 20;
//        for (i = turnpoint[0][0] + 15; i > 0; i--)
//        {
//            if (imageBin[i][j] && !imageBin[i - 1][j])
//            {
//                TempLine1[0][0] = i;
//                TempLine1[0][1] = j;
//                break;
//            }
//        }
//
//        NearNum = 2;
//        for (i = 0; i < 40; i++)
//        {
//            NearNum = FindNext(&TempLine1[i][0], NearNum + 2, 1);
//            if (NearNum == 99) break;
//        }
//
//        memset(turnpoint[0], 0, 2);
//        k = FindTurn(TempLine1[0], 10, 2, 1) - 2;
//        if (k != -2)
//        {
//            turnpoint[0][0] = TempLine1[k][0];
//            turnpoint[0][1] = TempLine1[k][1];
//            RightLine[0][0] = TempLine2[0][0];
//            RightLine[0][1] = TempLine2[0][1];
//            imgInfo.RightLineCount += repair(RightLine[0], TempLine1[k]);
//        }
//
//        i = RightLine[0][0];
//        //寻找左起点
//        for (j = 0; j < 90; j++)
//        {
//            if (!imageBin[i][j] && imageBin[i][j + 1] && imageBin[i][j + 2])
//            {
//                LeftLine[0][0] = i;
//                LeftLine[0][1] = j + 1;
//                break;
//            }
//        }
//
//        i = 0;
//        //左线邻域扫线
//        while (LeftLine[i][0] > 3 && LeftLine[i][1] > 3 && LeftLine[i][1] < 184 && i < 200)
//        {
//            if (NearNum < 2) NearNum = FindNext(&LeftLine[i][0], NearNum + 6, 0);
//            else NearNum = FindNext(&LeftLine[i][0], NearNum - 2, 0);
//            if (NearNum == 99) break;
//            i++;
//        }
//        imgInfo.LeftLineCount = i;
//
//        //拟合有效直道中线
//        if (imgInfo.LeftLineCount <= imgInfo.RightLineCount)
//        {
//            for (i = 0; i < imgInfo.LeftLineCount; i++)
//            {
//                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
//                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
//            }
//            for (i = imgInfo.LeftLineCount; i < imgInfo.RightLineCount; i++)
//            {
//                MiddleLine[i][0] = (LeftLine[imgInfo.LeftLineCount][0] + RightLine[i][0]) / 2;
//                MiddleLine[i][1] = (LeftLine[imgInfo.LeftLineCount][1] + RightLine[i][1]) / 2;
//            }
//            imgInfo.MiddleLineCount = imgInfo.LeftLineCount;
//            imgInfo.top = LeftLine[imgInfo.LeftLineCount][0];
//        }
//        else
//        {
//            for (i = 0; i < imgInfo.RightLineCount; i++)
//            {
//                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
//                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
//            }
//            for (i = imgInfo.RightLineCount; i < imgInfo.LeftLineCount; i++)
//            {
//                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineCount][0]) / 2;
//                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineCount][1]) / 2;
//            }
//            imgInfo.MiddleLineCount = imgInfo.RightLineCount;
//            imgInfo.top = RightLine[imgInfo.RightLineCount][0];
//        }
//    }
//}

//-------------------------------------------------------------------------------------------------------------------
// @brief       右岔路扫线
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void rightt1(void)
{
    uint8 i, j, NearNum, Count, Flag;
    Flag = 0;
    Count = 0;

    //！
    if (!RightMax) RightMax = 119;
    if (RightMax > 60 && RightTFlag <= 2)
    {
        uint8 LeftI = 0;
        for (i = RightMax - 5; i > RightMax - 50; i--)
        {
            while (LeftLine[LeftI][0] > i) LeftI++;
            NearNum = Count;
            Count = 0;

            for (j = LeftLine[LeftI][1] + 2; j < LeftLine[LeftI][1] + WidthBuffer[i] - 5; j++)
            {
                if (imageBin[i][j] && !imageBin[i][j - 1])
                {
                    Count++;
                    j += 3;
                }
            }
            if (Count)
            {
                i -= 3;
                if (NearNum < Count) NearNum = Count;
            }

            if (NearNum > 2)
            {
                RightTFlag = 10;
                break;
            }
        }
    }

    if (RightTFlag == 0 || RightTFlag == 1 || RightTFlag == 2)
    {
        if (RightTFlag == 0)
        {
            for (i = 100; i > 50; i--) if (!imageBin[i][184]) RightTFlag = 1;
        }
        else if (RightTFlag == 1)
        {
            RightTFlag = 2;
            for (i = 110; i > 80; i--) if (!imageBin[i][184]) RightTFlag = 1;
        }
        else if (RightTFlag == 2)
        {
            RightTFlag = 3;
            for (i = 110; i > 80; i--) if (imageBin[i][184]) RightTFlag = 2;
        }


        if (imgInfo.LeftLineCount < 60 || LeftMax>60)
        {
            turnpoint[0][0] = 0;
            LeftLine[0][0] = 117;
            LeftLine[0][1] = 4;
            for (i = 110; i > 10; i--)
            {
                if (imageBin[i - 1][4] && !imageBin[i][4] && !imageBin[i + 1][4]) break;
            }

            if (i > 15)
            {
                for (j = 4; j < 120; j++)
                {
                    if (!imageBin[i][j] && imageBin[i][j + 1] && imageBin[i][j + 2])
                    {
                        turnpoint[0][0] = i;
                        turnpoint[0][1] = j + 1;
                        break;
                    }
                }

                if (!LeftLine[0][0])
                {
                    turnpoint[0][0] = i;
                    turnpoint[0][1] = 4;
                }
            }

            if (turnpoint[0][0])
            {
                imgInfo.LeftLineCount = repair(LeftLine[0], turnpoint[0]);
                Count = ScanLine(LeftLine[imgInfo.LeftLineCount], 2, 0);
                if (imgInfo.LeftLineCount + Count > 150) imgInfo.LeftLineSum = 150;
                else imgInfo.LeftLineSum = imgInfo.LeftLineCount + Count;
            }
        }

        for (i = 0; i < imgInfo.LeftLineSum; i++)
        {
            MiddleLine[i][0] = LeftLine[i][0];
            MiddleLine[i][1] = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] / 2;
        }
        imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
    }
    //    if (RightTFlag == 2)
    //    {
    //        RightTFlag = 3;
    //        for (i = 110; i > 80; i--) if (imageBin[i][184]) RightTFlag = 2;
    //
    //        NearNum = 2;
    //        Count = 0;
    //
    //        for (j = 3; j < 160; j++)
    //        {
    //            if (!imageBin[115][j] && imageBin[115][j + 1] && imageBin[115][j + 2])
    //            {
    //                LeftLine[0][0] = 115;
    //                LeftLine[0][1] = j + 1;
    //                break;
    //            }
    //        }
    //
    //        //左线邻域扫线
    //        while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 200)
    //        {
    //            if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
    //            else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);
    //
    //            Count++;
    //
    //            if (NearNum == 99) break;
    //        }
    //        imgInfo.LeftLineSum = Count;
    //
    //        for (i = 0; i < imgInfo.LeftLineSum; i++)
    //        {
    //            MiddleLine[i][0] = LeftLine[i][0];
    //            MiddleLine[i][1] = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] / 2;
    //        }
    //        imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
    //
    //        ChangeCount = 0;
    //    }
    if (RightTFlag == 3)
    {
        //！
        ChangeCount++;
        uint8 j1, j2;

        //拟合有效直道中线
        for (i = 0; i < imgInfo.RightLineSum; i++)
        {
            MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
            MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
        }
        for (i = imgInfo.RightLineSum; i < imgInfo.LeftLineSum; i++)
        {
            MiddleLine[i][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineSum][0]) / 2;
            MiddleLine[i][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineSum][1]) / 2;
        }
        imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
        imgInfo.top = LeftLine[imgInfo.LeftLineSum][0];

        for (i = 0; i < imgInfo.LeftLineCount; i++)
        {
            if (LeftLine[i][1] > 5) break;
        }

        j = FindTurn1(LeftLine[i], 5, 5, 1);
        if (j)
        {
            for (Count = 20; Count < imgInfo.LeftLineSum; Count++)
            {
                if (LeftLine[Count][1] > 172)
                {
                    Count = 10;
                    break;
                }
            }

            if (Count != 10 && ChangeCount > 30)
            {
                turnpoint[0][0] = LeftLine[i + j][0];
                turnpoint[0][1] = LeftLine[i + j][1] - 1;

                RightTFlag = 4;
                for (i = 0; i < 5; i++) if (!imageBin[turnpoint[0][0] - i][turnpoint[0][1]]) RightTFlag = 3;
            }
        }

        Count = 0;
        for (i = imgInfo.LeftLineCount; i > imgInfo.LeftLineCount - 30; i--)
        {
            j1 = LeftLine[i][0];
            if (LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] + 10 > 175) break;
            for (j2 = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] - 5; j2 < 175 && j2 < LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] + 10 && Count < 20; j2++)
            {
                if (imageBin[j1][j2] && !imageBin[j1][j2 + 1] && !imageBin[j1][j2 + 2])
                {
                    TempLine1[Count][0] = j1;
                    TempLine1[Count][1] = j2;
                    Count++;
                    break;
                }
            }
        }

        if (Count > 8)
        {
            RightTFlag = 0;
            CircleFlag = 1;
            CircleCount = 0;
            imgInfo.nextroadType = RightCircle;
        }
    }
    if (RightTFlag == 4)
    {
        uint8 i, j;
        uint8 Count = 0;//扫线序号
        uint8 NearNum = 2;//邻域序号

        if (!imgInfo.RightLineCount)
        {
            RightLine[0][0] = 115;
            RightLine[0][1] = 184;

            for (i = 115; i > 10; i--)
            {
                if (!imageBin[i][3]) break;
            }

            for (j = 3; j < 160; j++)
            {
                if (!imageBin[i][j] && imageBin[i][j + 1] && imageBin[i][j + 2])
                {
                    LeftLine[0][0] = i;
                    LeftLine[0][1] = j + 1;
                    break;
                }
            }

            //左线邻域扫线
            while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 200)
            {
                if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
                else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);

                Count++;

                if (NearNum == 99) break;

                imgInfo.LeftLineCount = Count;
            }
        }

        for (i = 0; i < imgInfo.LeftLineCount; i++)
        {
            if (LeftLine[i][1] > 5) break;
        }

        i += FindTurn1(LeftLine[i], 6, 4, 1);

        if (i)
        {
            imgInfo.LeftLineSum = extend(LeftLine[i - 20], LeftLine[i - 5]) + i;
            imgInfo.LeftLineSum -= 5;
        }

        for (i = 0; i < imgInfo.LeftLineSum; i++)
        {
            MiddleLine[i][0] = LeftLine[i][0];
            MiddleLine[i][1] = LeftLine[i][1] / 2 + 94 - 4;
        }
        imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
        imgInfo.top = LeftLine[imgInfo.LeftLineSum][0];

        TempLine1[0][0] = 0;
        TempLine1[0][1] = 0;
        for (i = 5; i < 50; i++)
        {
            if (!imageBin[i][180] && imageBin[i + 1][180] && imageBin[i + 2][180])
            {
                TempLine1[0][0] = i + 1;
                TempLine1[0][1] = 180;
                //                break;
            }
        }

        if (TempLine1[0][0])
        {
            Count = 0;
            NearNum = 2;
            while (TempLine1[Count][0] > 3 && TempLine1[Count][1] > 3 && TempLine1[Count][1] < 183 && Count < 40)
            {
                if (NearNum > 5) NearNum = FindNext(&TempLine1[Count][0], NearNum - 6, 1);
                else NearNum = FindNext(&TempLine1[Count][0], NearNum + 2, 1);

                if (NearNum == 99) break;
                Count++;
            }

            turnpoint[0][0] = 0;
            turnpoint[0][1] = 0;

            if (Count > 35)
            {
                Count = extend1(TempLine1[0], TempLine1[34]) + 32;
                turnpoint[0][0] = TempLine1[0][0] / 2 + TempLine1[Count][0] / 2;
                turnpoint[0][1] = TempLine1[0][1] / 2 + TempLine1[Count][1] / 2;
            }

            //？出右断头
            Mark[0] = turnpoint[0][0];
            Mark[1] = turnpoint[0][1];
            if (turnpoint[0][0] && turnpoint[0][0] > RightTForesee && turnpoint[0][1] < 135) RightTFlag = 5;
        }
    }
    if (RightTFlag == 5 || RightTFlag == 6)
    {
        if (RightTFlag == 5)
        {
            RightTFlag = 6;
            for (i = 115; i > 90; i--)
            {
                if (!imageBin[i][184])
                {
                    RightTFlag = 5;
                    break;
                }
            }
        }

        if (RightTFlag == 6)
        {
            RightTFlag = 7;
            for (i = 105; i > 70; i--)
            {
                if (imageBin[i][184])
                {
                    RightTFlag = 6;
                    break;
                }
            }
        }

        for (i = 20; i < 115; i++)
        {
            turnpoint[1][0] = 0;
            turnpoint[1][1] = 0;
            if (!imageBin[i][MiddleValue] && imageBin[i + 1][MiddleValue])
            {
                turnpoint[1][0] = (i + 120) / 2;
                turnpoint[1][1] = 3;
                break;
            }
        }

        if (turnpoint[1][0] > RightTForesee)
        {
            turnpoint[0][0] = turnpoint[1][0];
            turnpoint[0][1] = turnpoint[1][1];
        }

        MiddleLine[0][0] = 115;
        MiddleLine[0][1] = MiddleValue;

        imgInfo.MiddleLineCount = repair(MiddleLine[0], turnpoint[0]);
    }
    if (RightTFlag == 7)
    {
        uint8 j1, j2;
        Count = 0;
        if (imgInfo.RightLineSum > imgInfo.LeftLineSum)
        {
            for (i = 0; i < imgInfo.RightLineSum && i < 120; i++)
            {
                MiddleLine[i][0] = RightLine[i][0];
                MiddleLine[i][1] = RightLine[i][1] - WidthBuffer[RightLine[i][0]] / 2;

                j1 = RightLine[i][0];
                for (j2 = RightLine[i][1] - WidthBuffer[RightLine[i][0]] + 5; j2 > 15 && j2 > RightLine[i][1] + WidthBuffer[RightLine[i][0]] - 10 && Count < 20; j2--)
                {
                    if (imageBin[j1][j2] && !imageBin[j1][j2 - 1] && !imageBin[j1][j2 - 2])
                    {
                        TempLine1[Count][0] = j1;
                        TempLine1[Count][1] = j2;
                        Count++;
                        break;
                    }
                }
            }
            if (imgInfo.RightLineSum > 120) imgInfo.MiddleLineCount = 120;
            else imgInfo.MiddleLineCount = imgInfo.RightLineSum;
        }
        else
        {
            for (i = 0; i < imgInfo.LeftLineSum && i < 120; i++)
            {
                MiddleLine[i][0] = LeftLine[i][0];
                MiddleLine[i][1] = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] / 2;

                j1 = LeftLine[i][0];
                for (j2 = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] - 5; j2 < 175 && j2 < LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] + 10 && Count < 20; j2++)
                {
                    if (imageBin[j1][j2] && !imageBin[j1][j2 + 1] && !imageBin[j1][j2 + 2])
                    {
                        TempLine1[Count][0] = j1;
                        TempLine1[Count][1] = j2;
                        Count++;
                        break;
                    }
                }
            }
            if (imgInfo.LeftLineSum > 120) imgInfo.MiddleLineCount = 120;
            else imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
        }

        if (Count > 10 || (imgInfo.LeftLineCount > 80 && imgInfo.RightLineCount > 80))
        {
            RightTFlag = 0;
            imgInfo.nextroadType = Straight;
        }
    }

    if (RightTFlag == 10 || RightTFlag == 11)
    {
        if (RightTFlag == 10)
        {
            CircleCount = 0;
            if (imgInfo.NoHeadDir)
            {
                RightTFlag = 15;
                for (i = RGForesee; i > RGForesee - 15; i--)
                {
                    if (!imageBin[i][184])
                    {
                        RightTFlag = 10;
                        break;
                    }
                }
            }
            else
            {
                RightTFlag = 11;
                for (i = 115; i > 90; i--)
                {
                    if (!imageBin[i][184])
                    {
                        RightTFlag = 10;
                        break;
                    }
                }
            }
        }

        if (RightTFlag == 11)
        {
            RightTFlag = 0;
            for (i = 119; i > 90; i--)
            {
                if (imageBin[i][184] && CircleCount < 18)
                {
                    RightTFlag = 11;
                    break;
                }
            }
        }

        NearNum = 2;
        Count = 0;

        for (j = 3; j < 160; j++)
        {
            if (!imageBin[115][j] && imageBin[115][j + 1] && imageBin[115][j + 2])
            {
                LeftLine[0][0] = 115;
                LeftLine[0][1] = j + 1;
                break;
            }
        }

        //左线邻域扫线
        while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 200)
        {
            if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
            else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);

            Count++;

            if (NearNum == 99) break;
        }
        imgInfo.LeftLineSum = Count;

        for (i = 0; i < imgInfo.LeftLineSum; i++)
        {
            MiddleLine[i][0] = LeftLine[i][0];
            MiddleLine[i][1] = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] / 2;
        }
        imgInfo.MiddleLineCount = imgInfo.LeftLineSum;

        if (LeftTFlag == 1) CircleCount = 0;
    }

    if (RightTFlag == 15) imgInfo.StopFlag = 1;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       右环岛扫线
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
//void rightcircle(void)
//{
//    uint8 i, j, Count = 0;
//    uint8 j1 = 0, j2 = 0;
//    uint8 NearNum = 2;
//
//    if (CircleFlag == 0 || CircleFlag == 1)
//    {
//        if (CircleFlag == 0)
//        {
//            CircleFlag = 1;
//
//            for (i = 115; i > 80; i--)
//            {
//                if (!imageBin[i][184])
//                {
//                    CircleFlag = 0;
//                    break;
//                }
//            }
//        }
//        else if (CircleFlag == 1)
//        {
//            CircleFlag = 2;
//            for (i = 115; i > 100; i--)
//            {
//                if (imageBin[i][184])
//                {
//                    CircleFlag = 1;
//                    break;
//                }
//            }
////            for (i = 100; i > 90; i--)
////            {
////                if (!imageBin[i][184])
////                {
////                    CircleFlag = 2;
////                    break;
////                }
////            }
//        }
//
//        if (!imgInfo.LeftLineSum)
//        {
//            for (i = 117; i > 10; i--)
//            {
//                if (!imageBin[i][3]) break;
//            }
//
//            for (j = 3; j < 160; j++)
//            {
//                if (!imageBin[i][j] && imageBin[i][j + 1] && imageBin[i][j + 2])
//                {
//                    LeftLine[0][0] = i;
//                    LeftLine[0][1] = j + 1;
//                    break;
//                }
//            }
//
//            NearNum = 2;
//            //左线邻域扫线
//            while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 250)
//            {
//                if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
//                else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);
//
//                Count++;
//
//                if (NearNum == 99) break;
//            }
//            imgInfo.LeftLineSum = Count;
//        }
//
//        for (i = 0; i < imgInfo.LeftLineSum; i++)
//        {
//            MiddleLine[i][0] = LeftLine[i][0];
//            MiddleLine[i][1] = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] / 2;
//        }
//        imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
//    }
//    if (CircleFlag == 2)
//    {
//        CircleFlag = 3;
//
//        for (i = 117; i > 70; i--)
//        {
//            if (!imageBin[i][3])
//            {
//                CircleFlag = 2;
//                break;
//            }
//        }
//
//        memset(turnpoint[0], 0, 6);
//
//        j1 = 187;
//        for (i = 0; i < imgInfo.RightLineSum; i++) if (j1 > RightLine[i][1]) j1 = RightLine[i][1];
//
//        if (imgInfo.LeftLineSum)
//        {
//            j2 = LeftLine[imgInfo.LeftLineSum - 10][1];
//            for (i = 80; i > 10; i--)
//            {
//                for (j = j2; j < j1-5; j++)
//                {
//                    if (imageBin[i][j] && imageBin[i][j + 1] && !imageBin[i][j + 2])
//                    {
//                        turnpoint[0][0] = i;
//                        turnpoint[0][1] = j+1;
//                        break;
//                    }
//                }
//                if (turnpoint[0][0]) break;
//            }
//        }
//
//        if (turnpoint[0][0]>20)
//        {
//            LeftLine[0][0] = 117;
//            if (RightLine[0][1] > 0x8E) LeftLine[0][1] = RightLine[0][1] - 0x8E;
//            else LeftLine[0][1] = 3;
//
//            imgInfo.LeftLineCount = repair(LeftLine[0], turnpoint[0]);
//
//            Count = imgInfo.LeftLineCount;
//            NearNum = 3;
//            while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 250)
//            {
//                if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
//                else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);
//
//                Count++;
//
//                if (NearNum == 99) break;
//            }
//
//            imgInfo.LeftLineSum = Count;
//
//            if (imgInfo.LeftLineSum <= imgInfo.RightLineSum)
//            {
//                for (i = 0; i < imgInfo.LeftLineSum; i++)
//                {
//                    MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
//                    MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
//                }
//                for (i = imgInfo.LeftLineSum; i < imgInfo.RightLineSum; i++)
//                {
//                    MiddleLine[i][0] = (LeftLine[imgInfo.LeftLineSum][0] + RightLine[i][0]) / 2;
//                    MiddleLine[i][1] = (LeftLine[imgInfo.LeftLineSum][1] + RightLine[i][1]) / 2;
//                }
//                imgInfo.MiddleLineCount = imgInfo.RightLineSum;
//                imgInfo.top = RightLine[imgInfo.RightLineSum][0];
//            }
//            else
//            {
//                for (i = 0; i < imgInfo.RightLineSum; i++)
//                {
//                    MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
//                    MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
//                }
//                for (i = imgInfo.RightLineSum; i < imgInfo.LeftLineSum; i++)
//                {
//                    MiddleLine[i][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineSum][0]) / 2;
//                    MiddleLine[i][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineSum][1]) / 2;
//                }
//                imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
//                imgInfo.top = LeftLine[imgInfo.LeftLineSum][0];
//            }
//        }
//        else
//        {
//            for (i = 0; i < imgInfo.LeftLineSum; i++)
//            {
//                MiddleLine[i][0] = LeftLine[i][0];
//                MiddleLine[i][1] = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] / 2;
//            }
//            imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
//        }
//
//    }
//    if (CircleFlag == 3)
//    {
//        CircleFlag = 4;
//        for (i = 60; i < 110; i++)
//        {
//            if (imageBin[i][3])
//            {
//                CircleFlag = 3;
//                break;
//            }
//        }
//
//        memset(turnpoint[0], 0, 6);
//
//        j1 = 187;
//        for (i = 0; i < imgInfo.RightLineSum; i++) if (j1 > RightLine[i][1]) j1 = RightLine[i][1];
//
//        if (!imgInfo.LeftLineSum)
//        {
//            for (i = 95; i > 10; i--)
//            {
//                for (j = 0; j < j1 - 5; j++)
//                {
//                    if (imageBin[i][j] && imageBin[i][j + 1] && !imageBin[i][j + 2])
//                    {
//                        turnpoint[0][0] = i;
//                        turnpoint[0][1] = j + 1;
//                        break;
//                    }
//                }
//                if (turnpoint[0][0]) break;
//            }
//
//            if (turnpoint[0][0])
//            {
//                LeftLine[0][0] = 117;
//                if (RightLine[0][1] > 0x8E) LeftLine[0][1] = RightLine[0][1] - 0x8E;
//                else LeftLine[0][1] = 3;
//
//                imgInfo.LeftLineCount = repair(LeftLine[0], turnpoint[0]);
//
//                Count = imgInfo.LeftLineCount;
//                NearNum = 3;
//                while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 250)
//                {
//                    if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
//                    else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);
//
//                    Count++;
//
//                    if (NearNum == 99) break;
//                }
//
//                imgInfo.LeftLineSum = Count;
//
//                if (imgInfo.LeftLineSum <= imgInfo.RightLineSum)
//                {
//                    for (i = 0; i < imgInfo.LeftLineSum; i++)
//                    {
//                        MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
//                        MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
//                    }
//                    for (i = imgInfo.LeftLineSum; i < imgInfo.RightLineSum; i++)
//                    {
//                        MiddleLine[i][0] = (LeftLine[imgInfo.LeftLineSum][0] + RightLine[i][0]) / 2;
//                        MiddleLine[i][1] = (LeftLine[imgInfo.LeftLineSum][1] + RightLine[i][1]) / 2;
//                    }
//                    imgInfo.MiddleLineCount = imgInfo.RightLineSum;
//                    imgInfo.top = RightLine[imgInfo.RightLineSum][0];
//                }
//                else
//                {
//                    for (i = 0; i < imgInfo.RightLineSum; i++)
//                    {
//                        MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
//                        MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
//                    }
//                    for (i = imgInfo.RightLineSum; i < imgInfo.LeftLineSum; i++)
//                    {
//                        MiddleLine[i][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineSum][0]) / 2;
//                        MiddleLine[i][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineSum][1]) / 2;
//                    }
//                    imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
//                    imgInfo.top = LeftLine[imgInfo.LeftLineSum][0];
//                }
//            }
//        }
//        else
//        {
//            TempLine1[0][0] = 117;
//            TempLine1[0][1] = 3;
//
//            for (j = 0; j < imgInfo.LeftLineSum; j++)
//            {
//                if (LeftLine[j][1] > 5) break;
//            }
//
//            j1 = repair(TempLine1[0], LeftLine[j]);
//
//            if (imgInfo.RightLineSum > j1)
//            {
//                for (i = 0; i < j1; i++)
//                {
//                    MiddleLine[i][0] = (TempLine1[i][0] + RightLine[i][0]) / 2;
//                    MiddleLine[i][1] = (TempLine1[i][1] + RightLine[i][1]) / 2;
//                }
//
//                for (i = j1; i < imgInfo.RightLineSum; i++)
//                {
//                    MiddleLine[i][0] = (LeftLine[j - j1 + i][0] + RightLine[i][0]) / 2;
//                    MiddleLine[i][1] = (LeftLine[j - j1 + i][1] + RightLine[i][1]) / 2;
//                }
//
//                for (i = imgInfo.RightLineSum; i < imgInfo.LeftLineSum; i++)
//                {
//                    MiddleLine[i][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineSum][0]) / 2;
//                    MiddleLine[i][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineSum][1]) / 2;
//                }
//                imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
//            }
//            else
//            {
//                for (i = 0; i < imgInfo.RightLineSum; i++)
//                {
//                    MiddleLine[i][0] = (TempLine1[i][0] + RightLine[i][0]) / 2;
//                    MiddleLine[i][1] = (TempLine1[i][1] + RightLine[i][1]) / 2;
//                }
//
//                for (i = imgInfo.RightLineSum; i < j1; i++)
//                {
//                    MiddleLine[i][0] = (TempLine1[i][0] + RightLine[imgInfo.RightLineSum][0]) / 2;
//                    MiddleLine[i][1] = (TempLine1[i][1] + RightLine[imgInfo.RightLineSum][1]) / 2;
//                }
//
//                for (i = imgInfo.RightLineSum; i < imgInfo.LeftLineSum; i++)
//                {
//                    MiddleLine[i][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineSum][0]) / 2;
//                    MiddleLine[i][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineSum][1]) / 2;
//                }
//                imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
//            }
//        }
//    }
//    if (CircleFlag == 4)
//    {
//        if (LeftTurn[0][1] < 175 && LeftTurn[0][1])
//        {
//            CircleFlag = 5;
//            j = LeftTurn[0][1];
//            for (i = 1; i < 5; i++)
//            {
//                if (!imageBin[LeftTurn[0][0] - i][j])
//                {
//                    CircleFlag = 4;
//                    break;
//                }
//            }
//        }
//
//        //        Count = 0;
//        //
//        //        for (i = 117; i > 60; i--)
//        //        {
//        //            if (imageBin[i][3] && imageBin[i][4])
//        //            {
//        //                LeftLine[Count][0] = i;
//        //                LeftLine[Count][1] = 3;
//        //                Count++;
//        //            }
//        //            else break;
//        //        }
//        //
//        //        for (j = 3; j < 160; j++)
//        //        {
//        //            if (!imageBin[i][j] && imageBin[i][j + 1] && imageBin[i][j + 2])
//        //            {
//        //                LeftLine[Count][0] = i;
//        //                LeftLine[Count][1] = j + 1;
//        //                break;
//        //            }
//        //        }
//        //
//        //        NearNum = 2;
//        //        while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 250)
//        //        {
//        //            if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
//        //            else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);
//        //
//        //            Count++;
//        //
//        //            if (NearNum == 99) break;
//        //        }
//        //
//        //        imgInfo.LeftLineSum = Count;
//        //
//        //
//        //
//        //        if (!imageBin[117][183] && !imageBin[117][182])
//        //        {
//        //            for (j = 183; j > 60; j--)
//        //            {
//        //                if (!imageBin[117][j] && imageBin[117][j - 1] && imageBin[117][j - 2])
//        //                {
//        //                    RightLine[0][0] = 117;
//        //                    RightLine[0][1] = j - 1;
//        //                    break;
//        //                }
//        //            }
//        //        }
//        //
//        //        if (RightLine[0][0])
//        //        {
//        //            Count = 0;
//        //            NearNum = 2;
//        //            while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 250)
//        //            {
//        //                NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);
//        //
//        //                Count++;
//        //
//        //                if (NearNum == 99) break;
//        //            }
//        //
//        //            imgInfo.RightLineSum = Count;
//        //        }
//        //        else
//        //        {
//        //            RightLine[0][0] = 117;
//        //            RightLine[0][1] = 183;
//        //        }
//        //
//        //
//        if (imgInfo.LeftLineSum <= imgInfo.RightLineSum)
//        {
//            for (i = 0; i < imgInfo.LeftLineSum; i++)
//            {
//                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
//                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
//            }
//            for (i = imgInfo.LeftLineSum; i < imgInfo.RightLineSum; i++)
//            {
//                MiddleLine[i][0] = (LeftLine[imgInfo.LeftLineSum][0] + RightLine[i][0]) / 2;
//                MiddleLine[i][1] = (LeftLine[imgInfo.LeftLineSum][1] + RightLine[i][1]) / 2;
//            }
//            imgInfo.MiddleLineCount = imgInfo.RightLineSum;
//            imgInfo.top = RightLine[imgInfo.RightLineSum][0];
//        }
//        else
//        {
//            for (i = 0; i < imgInfo.RightLineSum; i++)
//            {
//                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
//                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
//            }
//            for (i = imgInfo.RightLineSum; i < imgInfo.LeftLineSum; i++)
//            {
//                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineSum][0]) / 2;
//                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineSum][1]) / 2;
//            }
//            imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
//            imgInfo.top = LeftLine[imgInfo.LeftLineSum][0];
//        }
//    }
//    if (CircleFlag == 5)
//    {
//        //CircleFlag = 6;
//        //for (i = 119; i > 80; i--)
//        //{
//        //    if (!imageBin[i][3])
//        //    {
//        //        CircleFlag = 5;
//        //        break;
//        //    }
//        //}
//
//        //if (!imageBin[117][183] && !imageBin[117][182])
//        //{
//        //    for (j = 183; j > 60; j--)
//        //    {
//        //        if (!imageBin[117][j] && imageBin[117][j - 1] && imageBin[117][j - 2])
//        //        {
//        //            RightLine[0][0] = 117;
//        //            RightLine[0][1] = j - 1;
//        //            break;
//        //        }
//        //    }
//        //}
//
//        //Count = 0;
//        //NearNum = 2;
//        //while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 250)
//        //{
//        //    NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);
//
//        //    Count++;
//
//        //    if (NearNum == 99) break;
//        //}
//
//        //imgInfo.RightLineSum = Count;
//
//        //if (imgInfo.RightLineSum > 20)
//        //{
//        //    for (i = 0; i < Count; i++)
//        //    {
//        //        MiddleLine[i][0] = RightLine[i][0];
//        //        MiddleLine[i][1] = RightLine[i][1] - WidthBuffer[RightLine[i][0]] / 2;
//        //    }
//        //    imgInfo.MiddleLineCount = Count;
//        //    if (imgInfo.MiddleLineCount > 20)
//        //    {
//        //        imgInfo.MiddleLineCount += extend(MiddleLine[imgInfo.MiddleLineCount - 15], MiddleLine[imgInfo.MiddleLineCount - 2]);
//        //    }
//        //    else imgInfo.MiddleLineCount += extend(MiddleLine[0], MiddleLine[imgInfo.MiddleLineCount - 2]);
//        //}
//
//        if (imgInfo.LeftLineCount > 70)
//        {
//            imgInfo.LeftLineCount += extend(LeftLine[imgInfo.LeftLineCount - 10], LeftLine[imgInfo.LeftLineCount]);
//
//            if (!imgInfo.RightLineSum)
//            {
//                RightLine[0][0] = 117;
//                RightLine[0][1] = 184;
//            }
//
//            for (i = 0; i < imgInfo.RightLineSum; i++)
//            {
//                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
//                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
//            }
//            for (i = imgInfo.RightLineSum; i < imgInfo.LeftLineCount; i++)
//            {
//                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineSum][0]) / 2;
//                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineSum][1]) / 2;
//            }
//            imgInfo.MiddleLineCount = imgInfo.LeftLineCount;
//        }
//        else
//        {
//            CircleFlag = 6;
//        }
//    }
//    if (CircleFlag == 6)
//    {
//        CircleFlag = 7;
//        for (i = 117; i > 80; i--)
//        {
//            if (!imageBin[i][3])
//            {
//                CircleFlag = 6;
//                break;
//            }
//        }
//
//        //if (!imageBin[117][183] && !imageBin[117][182])
//        //{
//        //    for (j = 183; j > 60; j--)
//        //    {
//        //        if (!imageBin[117][j] && imageBin[117][j - 1] && imageBin[117][j - 2])
//        //        {
//        //            RightLine[0][0] = 117;
//        //            RightLine[0][1] = j - 1;
//        //            break;
//        //        }
//        //    }
//        //}
//
//        //Count = 0;
//        //NearNum = 2;
//        //while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 250)
//        //{
//        //    NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);
//
//        //    Count++;
//
//        //    if (NearNum == 99) break;
//        //}
//
//        //imgInfo.RightLineSum = Count;
//
//        //if (imgInfo.RightLineSum > 20)
//        //{
//        //    for (i = 0; i < Count; i++)
//        //    {
//        //        MiddleLine[i][0] = RightLine[i][0];
//        //        MiddleLine[i][1] = RightLine[i][1] - WidthBuffer[RightLine[i][0]] / 2;
//        //    }
//        //    imgInfo.MiddleLineCount = Count;
//        //    if (imgInfo.MiddleLineCount > 20)
//        //    {
//        //        imgInfo.MiddleLineCount += extend(MiddleLine[imgInfo.MiddleLineCount - 15], MiddleLine[imgInfo.MiddleLineCount - 2]);
//        //    }
//        //    else imgInfo.MiddleLineCount += extend(MiddleLine[0], MiddleLine[imgInfo.MiddleLineCount - 2]);
//        //}
//
//        //if (!LeftLine[0][0])
//        //{
//        //    Count = 0;
//        //    for (i = 117; i > 40; i--)
//        //    {
//        //        if (!imageBin[i][3]) break;
//        //    }
//        //    if (i > 45)
//        //    {
//        //        for (j = 3; j < 90; j++)
//        //        {
//        //            if (!imageBin[i][j] && imageBin[i][j + 1] && imageBin[i][j + 2])
//        //            {
//        //                LeftLine[Count][0] = i;
//        //                LeftLine[Count][1] = j + 1;
//        //                break;
//        //            }
//        //        }
//
//        //        //左线邻域扫线
//        //        while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 250)
//        //        {
//        //            if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
//        //            else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);
//
//        //            Count++;
//        //        }
//        //        imgInfo.LeftLineSum = Count;
//        //    }
//        //}
//
//        //if (LeftLine[imgInfo.LeftLineSum][1] > 20 && LeftLine[imgInfo.LeftLineSum][1] < 120) CircleFlag = 7;
//    }
//    if (CircleFlag == 7)
//    {
//        //LeftLine[0][0] = 0;
//        //for (i = 117; i > 30; i--)
//        //{
//        //    if (imageBin[i][4] && !imageBin[i - 1][4] && !imageBin[i - 2][4])
//        //    {
//        //        LeftLine[0][0] = i;
//        //        LeftLine[0][1] = 4;
//        //        break;
//        //    }
//        //}
//
//        //if (LeftLine[0][0])
//        //{
//        //    Count = 0;
//        //    NearNum = 2;
//        //    while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 250)
//        //    {
//        //        if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
//        //        else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);
//
//        //        Count++;
//        //    }
//
//        //    imgInfo.LeftLineCount = Count;
//
//        //    MiddleLine[0][0] = 119;
//        //    MiddleLine[0][1] = MiddleValue;
//
//        //    if (Count > 4) imgInfo.MiddleLineCount = repair(MiddleLine[0], LeftLine[Count - 3]);
//        //}
//        //else if (imgInfo.LeftLineCount > 4)
//        //{
//        //    MiddleLine[0][0] = 119;
//        //    MiddleLine[0][1] = LeftLine[0][1] / 2 + 94;
//
//        //    imgInfo.MiddleLineCount = repair(MiddleLine[0], LeftLine[imgInfo.LeftLineCount - 3]);
//        //}
//
//        if (imgInfo.LeftLineCount > 40 && LeftLine[imgInfo.LeftLineCount - 10][1] < 140) CircleFlag = 8;
//    }
//
//    if (CircleFlag == 8)
//    {
//        //！
//        CircleFlag = 0;
//        imgInfo.nextroadType = Straight;
//        RightTFlag = 1;
//        NoGarageFlag = 1;
//        for (i = 110; i > 85; i--)
//        {
//            if (imageBin[i][175])
//            {
//                CircleFlag = 8;
//                RightTFlag = 0;
//                imgInfo.nextroadType = RightCircle;
//                break;
//            }
//        }
//
//        if (!imgInfo.LeftLineSum)
//        {
//            for (i = 117; i > 30; i--)
//            {
//                if (!imageBin[i][3]) break;
//            }
//
//            for (j = 3; j < 120; j++)
//            {
//                if (!imageBin[i][j] && imageBin[i][j + 1] && imageBin[i][j + 2])
//                {
//                    LeftLine[0][0] = i;
//                    LeftLine[0][1] = j + 1;
//                    break;
//                }
//            }
//
//            Count = 0;
//            NearNum = 2;
//            while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 250)
//            {
//                if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
//                else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);
//
//                Count++;
//
//                if (NearNum == 99) break;
//            }
//
//            imgInfo.LeftLineSum = Count;
//        }
//
////        MiddleLine[0][0] = LeftLine[1][0];
////        MiddleLine[0][1] = LeftLine[1][1] + WidthBuffer[LeftLine[0][0]] / 2;
////
////        if (imgInfo.LeftLineSum > 4)
////        {
////            LeftLine[imgInfo.LeftLineSum - 3][1] += WidthBuffer[LeftLine[imgInfo.LeftLineSum - 3][0]] / 2;
////
////            imgInfo.MiddleLineCount = repair(MiddleLine[0], LeftLine[imgInfo.LeftLineSum - 3]);
////        }
//
//
//        for (i = 0; i < imgInfo.LeftLineSum && i<120; i++)
//        {
//            MiddleLine[i][0] = LeftLine[i][0];
//            MiddleLine[i][1] = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] / 2;
//        }
//        imgInfo.MiddleLineCount = i;
//    }
//}

//-------------------------------------------------------------------------------------------------------------------
// @brief       右环岛扫线
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void rightcircle1(void)
{
    uint8 i, j, Count = 0;
    uint8 j1 = 0, j2 = 0;
    uint8 NearNum = 2;

    if (CircleFlag == 0 || CircleFlag == 1)
    {
        CircleCount = 0;
        if (CircleFlag == 0)
        {
            if (!NoCircleFlag) CircleFlag = 1;
            else if (NoCircleFlag) CircleFlag = 8;

            for (i = 115; i > 80; i--)
            {
                if (!imageBin[i][184])
                {
                    CircleFlag = 0;
                    break;
                }
            }
        }

        if (CircleFlag == 1)
        {
            CircleFlag = 3;
            for (i = 117; i > 70; i--)
            {
                if (!imageBin[i][3])
                {
                    CircleFlag = 1;
                    break;
                }
            }
            if (NoCircleFlag) CircleFlag = 8;
        }

        if (!imgInfo.LeftLineSum)
        {
            for (i = 117; i > 10; i--)
            {
                if (!imageBin[i][3]) break;
            }

            for (j = 3; j < 160; j++)
            {
                if (!imageBin[i][j] && imageBin[i][j + 1] && imageBin[i][j + 2])
                {
                    LeftLine[0][0] = i;
                    LeftLine[0][1] = j + 1;
                    break;
                }
            }

            NearNum = 2;
            //左线邻域扫线
            imgInfo.LeftLineSum = ScanLine(LeftLine[0], 2, 0);
        }

        memset(TempLine1, 0, 100);
        Count = 0;
        for (i = 0; i < imgInfo.LeftLineSum; i++)
        {
            MiddleLine[i][0] = LeftLine[i][0];
            MiddleLine[i][1] = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] / 2;

            if (CircleFlag == 1 && LeftLine[i][0] < 100)
            {
                j1 = LeftLine[i][0];
                for (j2 = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] - 5; j2 < 175 && j2 < LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] + 10 && Count < 50; j2++)
                {
                    if (imageBin[j1][j2] && !imageBin[j1][j2 + 1] && !imageBin[j1][j2 + 2])
                    {
                        TempLine1[Count][0] = j1;
                        TempLine1[Count][1] = j2;
                        Count++;
                        break;
                    }
                }
            }
        }
        imgInfo.MiddleLineCount = imgInfo.LeftLineSum;

        if (Count)
        {
            memset(turnpoint, 0, 6);
            memset(RightTurn, 0, 6);
            RightTurn[0][0] = TempLine1[0][0];
            RightTurn[0][1] = TempLine1[0][1];

            for (i = 0; i < Count - 1; i++)
            {
                if (TempLine1[i][0] > TempLine1[i + 1][0] + 15)
                {
                    RightTurn[1][0] = TempLine1[i + 1][0];
                    RightTurn[1][1] = TempLine1[i + 1][1];
                    break;
                }
            }

            TempLine2[0][0] = RightTurn[0][0];
            TempLine2[0][1] = RightTurn[0][1];

            if (ScanLine(TempLine2[0], 2, 0) > 15)
            {
                if (TempLine2[0][0] - TempLine2[14][0] > -5)
                {
                    turnpoint[0][0] = TempLine2[0][0];
                    turnpoint[0][1] = TempLine2[0][1];
                }
            }

            if (!turnpoint[0][0])
            {
                TempLine2[0][0] = RightTurn[1][0];
                TempLine2[0][1] = RightTurn[1][1];
                if (ScanLine(TempLine2[0], 2, 0) > 15)
                {
                    if (TempLine2[0][0] - TempLine2[14][0] > -5)
                    {
                        turnpoint[0][0] = TempLine2[0][0];
                        turnpoint[0][1] = TempLine2[0][1];
                    }
                }
            }

            if (!NoCircleFlag && turnpoint[0][0] > RightCircleForesee)
            {
                LeftLine[0][0] = 117;
                if (RightLine[0][1] > 0x8E) LeftLine[0][1] = RightLine[0][1] - 0x8E;
                else LeftLine[0][1] = 3;

                imgInfo.LeftLineCount = repair(LeftLine[0], turnpoint[0]);

                imgInfo.LeftLineSum = imgInfo.LeftLineCount + ScanLine(LeftLine[imgInfo.LeftLineCount], 3, 0);

                FindMiddleLine(imgInfo.LeftLineSum, imgInfo.RightLineSum);
            }
        }
    }
    if (CircleFlag == 3)
    {
        CircleFlag = 4;
        for (i = 60; i < 110; i++)
        {
            if (imageBin[i][3])
            {
                CircleFlag = 3;
                break;
            }
        }

        memset(turnpoint[0], 0, 6);

        j1 = 187;
        for (i = 0; i < imgInfo.RightLineSum; i++) if (j1 > RightLine[i][1]) j1 = RightLine[i][1];

        if (!imgInfo.LeftLineSum || (LeftLine[imgInfo.LeftLineSum - 2][1] < 175))
        {
            for (i = 95; i > 10; i--)
            {
                for (j = 0; j < j1 - 5; j++)
                {
                    if (imageBin[i][j] && imageBin[i][j + 1] && !imageBin[i][j + 2])
                    {
                        turnpoint[0][0] = i;
                        turnpoint[0][1] = j + 1;
                        break;
                    }
                }
                if (turnpoint[0][0]) break;
            }

            if (turnpoint[0][0])
            {
                LeftLine[0][0] = 117;
                if (RightLine[0][1] > 0x8E) LeftLine[0][1] = RightLine[0][1] - 0x8E;
                else LeftLine[0][1] = 3;

                imgInfo.LeftLineCount = repair(LeftLine[0], turnpoint[0]);

                Count = imgInfo.LeftLineCount;
                NearNum = 3;
                while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 250)
                {
                    if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
                    else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);

                    Count++;

                    if (NearNum == 99) break;
                }

                imgInfo.LeftLineSum = Count;

                FindMiddleLine(imgInfo.LeftLineSum, imgInfo.RightLineSum);
            }
        }
        else
        {
            TempLine1[0][0] = 117;
            TempLine1[0][1] = 3;

            for (j = 0; j < imgInfo.LeftLineSum; j++)
            {
                if (LeftLine[j][1] > 5) break;
            }

            j1 = repair(TempLine1[0], LeftLine[j]);

            if (imgInfo.RightLineSum > j1)
            {
                for (i = 0; i < j1; i++)
                {
                    MiddleLine[i][0] = (TempLine1[i][0] + RightLine[i][0]) / 2;
                    MiddleLine[i][1] = (TempLine1[i][1] + RightLine[i][1]) / 2;
                }

                for (i = j1; i < imgInfo.RightLineSum; i++)
                {
                    MiddleLine[i][0] = (LeftLine[j - j1 + i][0] + RightLine[i][0]) / 2;
                    MiddleLine[i][1] = (LeftLine[j - j1 + i][1] + RightLine[i][1]) / 2;
                }

                for (i = 0; i < imgInfo.LeftLineSum; i++)
                {
                    MiddleLine[i + imgInfo.RightLineSum][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineSum][0]) / 2;
                    MiddleLine[i + imgInfo.RightLineSum][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineSum][1]) / 2;
                }
                imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
            }
            else
            {
                for (i = 0; i < imgInfo.RightLineSum; i++)
                {
                    MiddleLine[i][0] = (TempLine1[i][0] + RightLine[i][0]) / 2;
                    MiddleLine[i][1] = (TempLine1[i][1] + RightLine[i][1]) / 2;
                }

                for (i = imgInfo.RightLineSum; i < j1; i++)
                {
                    MiddleLine[i][0] = (TempLine1[i][0] + RightLine[imgInfo.RightLineSum][0]) / 2;
                    MiddleLine[i][1] = (TempLine1[i][1] + RightLine[imgInfo.RightLineSum][1]) / 2;
                }

                for (i = 0; i < imgInfo.LeftLineSum; i++)
                {
                    MiddleLine[i + j1][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineSum][0]) / 2;
                    MiddleLine[i + j1][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineSum][1]) / 2;
                }
                imgInfo.MiddleLineCount = imgInfo.LeftLineSum + j1;
            }

            if (LeftLine[1][1] < 10 && LeftLine[imgInfo.LeftLineCount][1] > 180) CircleFlag = 4;
        }
        CircleMeanCount = 19;
    }
    if (CircleFlag == 4)
    {
        if (CircleMeanCount < 19) CircleMeanCount++;
        else CircleMeanCount = 0;
        if (LeftTurn[0][1] < 175 && LeftTurn[0][1])
        {
            CircleFlag = 5;
            j = LeftTurn[0][1];
            for (i = 1; i < 5; i++)
            {
                if (!imageBin[LeftTurn[0][0] - i][j])
                {
                    CircleFlag = 4;
                    break;
                }
            }
        }

        FindMiddleLine(imgInfo.LeftLineSum, imgInfo.RightLineSum);
    }
    if (CircleFlag == 5)
    {
        if (imgInfo.LeftLineCount > 80)
        {
            imgInfo.LeftLineCount += extend(LeftLine[imgInfo.LeftLineCount - 10], LeftLine[imgInfo.LeftLineCount]);

            if (!imgInfo.RightLineSum)
            {
                RightLine[0][0] = 117;
                RightLine[0][1] = 184;
            }

            FindMiddleLine(imgInfo.LeftLineCount, imgInfo.RightLineSum);
        }
        else
        {
            CircleFlag = 6;
        }
    }
    if (CircleFlag == 6)
    {
        memset(turnpoint, 0, 8);

        CircleFlag = 7;
        for (i = 105; i > 75; i--)
        {
            if (!imageBin[i][15])
            {
                CircleFlag = 6;
                break;
            }
        }

        for (i = 117; i > 10; i--)
        {
            if (imageBin[i][183] && !imageBin[i - 1][183] && !imageBin[i - 2][183])
            {
                turnpoint[0][0] = i;
                turnpoint[0][1] = 183;

                for (i = turnpoint[0][0]; i < 117; i++)
                {
                    if (!imageBin[i][183])
                    {
                        turnpoint[1][0] = i;
                        turnpoint[1][1] = 183;
                        break;
                    }
                }
                break;
            }
        }

        if (turnpoint[1][0] && turnpoint[0][0])
        {
            LeftLine[0][1] = 1;
            RightLine[0][1] = 187;
            turnpoint[0][0] = (turnpoint[1][0] + turnpoint[0][0]) / 2;

            if (imgInfo.RightLineSum && RightLine[1][0] < 110) RightLine[0][1] = RightLine[1][1];
            if (imgInfo.LeftLineSum && LeftLine[1][0] < 110) LeftLine[0][1] = LeftLine[1][1];

            if (RightLine[1][1] && LeftLine[1][1])
            {
                MiddleLine[0][1] = (LeftLine[1][1] + RightLine[1][1]) / 2;
                turnpoint[0][0] = turnpoint[0][0] + MiddleLine[0][1] - 94;
            }
            else MiddleLine[0][1] = 94;
            MiddleLine[0][0] = 117;

            imgInfo.MiddleLineCount = repair(MiddleLine[0], turnpoint[0]);
        }
        else imgInfo.MiddleLineCount = 0;
    }
    if (CircleFlag == 7)
    {
        memset(turnpoint, 0, 8);

        if (!imgInfo.LeftLineSum)
        {
            LeftLine[0][0] = 0;
            for (i = 117; i > 10; i--)
            {
                if (!imageBin[i][4]) break;
            }

            if (i > 15)
            {
                for (j = 4; j < 120; j++)
                {
                    if (!imageBin[i][j] && imageBin[i][j + 1] && imageBin[i][j + 2])
                    {
                        LeftLine[0][0] = i;
                        LeftLine[0][1] = j + 1;
                        break;
                    }
                }

                if (!LeftLine[0][0])
                {
                    LeftLine[0][0] = i;
                    LeftLine[0][1] = 4;
                }
            }

            if (LeftLine[0][0]) imgInfo.LeftLineSum = ScanLine(LeftLine[0], 2, 0);
        }

        if (imgInfo.LeftLineSum > 80)
        {
            if (LeftLine[imgInfo.LeftLineSum - 3][0] > 3 && LeftLine[imgInfo.LeftLineSum - 3][1] > 175)
            {
                turnpoint[0][0] = LeftLine[imgInfo.LeftLineSum - 3][0];
                turnpoint[0][1] = 183;

                for (i = turnpoint[0][0] + 20; i < 117; i++)
                {
                    if (!imageBin[i][183])
                    {
                        turnpoint[1][0] = i;
                        turnpoint[1][1] = 183;
                        break;
                    }
                }

                if (!turnpoint[1][0]) turnpoint[1][0] = 117;
            }
            else if (LeftLine[imgInfo.LeftLineSum - 3][0] < 10 && LeftLine[imgInfo.LeftLineSum - 3][1] < 180)
            {
                turnpoint[0][0] = 5;
                turnpoint[0][1] = LeftLine[imgInfo.LeftLineSum - 3][1];

                turnpoint[1][0] = 5;
                turnpoint[1][1] = 183;
            }
            else
            {
                turnpoint[0][0] = 5;
                turnpoint[0][1] = 183;

                turnpoint[1][0] = 5;
                turnpoint[1][1] = 183;
            }
        }

        if (!turnpoint[1][0])
        {
            for (i = 117; i > 10; i--)
            {
                if (imageBin[i][183] && !imageBin[i - 1][183] && !imageBin[i - 2][183])
                {
                    turnpoint[0][0] = i;
                    turnpoint[0][1] = 183;

                    for (i = turnpoint[0][0]; i < 117; i++)
                    {
                        if (!imageBin[i][183])
                        {
                            turnpoint[1][0] = i;
                            turnpoint[1][1] = 183;
                            break;
                        }
                    }
                    break;
                }
            }
        }

        if (turnpoint[1][0] && turnpoint[0][0])
        {
            LeftLine[0][1] = 1;
            RightLine[0][1] = 187;
            turnpoint[0][0] = (turnpoint[1][0] + turnpoint[0][0]) / 2;

            if (imgInfo.RightLineSum && RightLine[1][0] < 110) RightLine[0][1] = RightLine[1][1];
            if (imgInfo.LeftLineSum && LeftLine[1][0] < 110) LeftLine[0][1] = LeftLine[1][1];
            if (RightLine[1][1] && LeftLine[1][1])
            {
                MiddleLine[0][1] = (LeftLine[1][1] + RightLine[1][1]) / 2;
                if (turnpoint[0][0] + MiddleLine[0][1] - 94 > 3) turnpoint[0][0] = turnpoint[0][0] + MiddleLine[0][1] - 94;
                else turnpoint[0][0] = 3;
            }
            else MiddleLine[0][1] = 94;
            MiddleLine[0][0] = 117;

            imgInfo.MiddleLineCount = repair(MiddleLine[0], turnpoint[0]);
        }
        else imgInfo.MiddleLineCount = 0;

        if (imgInfo.LeftLineSum > 80 && LeftLine[imgInfo.LeftLineSum - 10][1] < 140) CircleFlag = 8;
        CircleCount = 0;
    }

    if (CircleFlag == 8)
    {
        CircleCount++;
        if (!imgInfo.LeftLineSum)
        {
            for (i = 117; i > 30; i--)
            {
                if (!imageBin[i][3]) break;
            }

            for (j = 3; j < 120; j++)
            {
                if (!imageBin[i][j] && imageBin[i][j + 1] && imageBin[i][j + 2])
                {
                    LeftLine[0][0] = i;
                    LeftLine[0][1] = j + 1;
                    break;
                }
            }

            imgInfo.LeftLineSum = ScanLine(LeftLine[0], 2, 0);
        }
        else if (((!NoCircleFlag && CircleCount > 25) || (NoCircleFlag && CircleCount > 50)) && imgInfo.LeftLineSum > 30)
        {
            Count = 0;
            for (i = 0; i < 40; i++)
            {
                j1 = LeftLine[i][0];
                for (j2 = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] - 5; j2 < 183 && j2 < LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] + 10; j2++)
                {
                    if (imageBin[j1][j2] && !imageBin[j1][j2 + 1] && !imageBin[j1][j2 + 2])
                    {
                        TempLine1[Count][0] = j1;
                        TempLine1[Count][1] = j2;
                        Count++;
                        break;
                    }
                }
            }

            if (Count > 32)
            {
                CircleFlag = 0;
                imgInfo.nextroadType = Straight;
            }
        }

        for (i = 0; i < imgInfo.LeftLineSum && i < 120; i++)
        {
            MiddleLine[i][0] = LeftLine[i][0];
            MiddleLine[i][1] = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] / 2;
        }
        imgInfo.MiddleLineCount = i;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       左环岛扫线
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void leftcircle1(void)
{
    uint8 i, j, Count = 0;
    uint8 j1 = 0, j2 = 0;
    uint8 NearNum = 2;

    if (CircleFlag == 0 || CircleFlag == 11)
    {
        CircleCount = 0;
        if (CircleFlag == 0)
        {
            if (!NoCircleFlag) CircleFlag = 11;
            else if (NoCircleFlag) CircleFlag = 18;

            for (i = 115; i > 80; i--)
            {
                if (!imageBin[i][4])
                {
                    CircleFlag = 0;
                    break;
                }
            }
        }

        if (CircleFlag == 11)
        {
            CircleFlag = 13;
            for (i = 117; i > 70; i--)
            {
                if (!imageBin[i][183])
                {
                    CircleFlag = 11;
                    break;
                }
            }
            if (NoCircleFlag) CircleFlag = 18;
        }

        if (!imgInfo.RightLineSum)
        {
            for (i = 117; i > 10; i--)
            {
                if (!imageBin[i][184]) break;
            }

            for (j = 184; j > 20; j--)
            {
                if (!imageBin[i][j] && imageBin[i][j - 1] && imageBin[i][j - 2])
                {
                    RightLine[0][0] = i;
                    RightLine[0][1] = j - 1;
                    break;
                }
            }
            imgInfo.RightLineSum = ScanLine(RightLine[0], 2, 1);
        }

        memset(TempLine1, 0, 100);
        Count = 0;
        for (i = 0; i < imgInfo.RightLineSum; i++)
        {
            MiddleLine[i][0] = RightLine[i][0];
            MiddleLine[i][1] = RightLine[i][1] - WidthBuffer[RightLine[i][0]] / 2;

            if (CircleFlag == 11 && RightLine[i][0] < 100)
            {
                j1 = RightLine[i][0];
                for (j2 = RightLine[i][1] - WidthBuffer[RightLine[i][0]] + 5; j2 > 10 && j2 > RightLine[i][1] - WidthBuffer[RightLine[i][0]] - 10 && Count < 50; j2--)
                {
                    if (imageBin[j1][j2] && !imageBin[j1][j2 - 1] && !imageBin[j1][j2 - 2])
                    {
                        TempLine1[Count][0] = j1;
                        TempLine1[Count][1] = j2;
                        Count++;
                        break;
                    }
                }
            }
        }
        imgInfo.MiddleLineCount = imgInfo.RightLineSum;

        if (Count)
        {
            memset(turnpoint, 0, 6);
            memset(RightTurn, 0, 6);
            LeftTurn[0][0] = TempLine1[0][0];
            LeftTurn[0][1] = TempLine1[0][1];

            for (i = 0; i < Count - 1; i++)
            {
                if (TempLine1[i][0] > TempLine1[i + 1][0] + 15)
                {
                    LeftTurn[1][0] = TempLine1[i + 1][0];
                    LeftTurn[1][1] = TempLine1[i + 1][1];
                    break;
                }
            }

            TempLine2[0][0] = LeftTurn[0][0];
            TempLine2[0][1] = LeftTurn[0][1];

            if (ScanLine(TempLine2[0], 2, 1) > 15)
            {
                if (TempLine2[0][0] - TempLine2[14][0] > -5)
                {
                    turnpoint[0][0] = TempLine2[0][0];
                    turnpoint[0][1] = TempLine2[0][1];
                }
            }

            if (!turnpoint[0][0])
            {
                TempLine2[0][0] = LeftTurn[1][0];
                TempLine2[0][1] = LeftTurn[1][1];
                if (ScanLine(TempLine2[0], 2, 1) > 15)
                {
                    if (TempLine2[0][0] - TempLine2[14][0] > -5)
                    {
                        turnpoint[0][0] = TempLine2[0][0];
                        turnpoint[0][1] = TempLine2[0][1];
                    }
                }
            }

            if (!NoCircleFlag && turnpoint[0][0] > LeftCircleForesee)
            {
                RightLine[0][0] = 117;
                if (LeftLine[0][1] + 0x8E < 187) RightLine[0][1] = LeftLine[0][1] + 0x8E;
                else RightLine[0][1] = 187;
                imgInfo.RightLineCount = repair(RightLine[0], turnpoint[0]);

                imgInfo.RightLineSum = imgInfo.RightLineCount + ScanLine(RightLine[imgInfo.RightLineCount], 2, 1);

                FindMiddleLine(imgInfo.LeftLineSum, imgInfo.RightLineSum);
            }
        }
    }
    if (CircleFlag == 13)
    {
        CircleFlag = 14;
        for (i = 60; i < 110; i++)
        {
            if (imageBin[i][184])
            {
                CircleFlag = 13;
                break;
            }
        }

        memset(turnpoint, 0, 6);

        j1 = 3;
        for (i = 0; i < imgInfo.LeftLineSum; i++) if (j1 < LeftLine[i][1]) j1 = LeftLine[i][1];

        if (!imgInfo.RightLineSum || (RightLine[imgInfo.RightLineSum - 2][1] && RightLine[imgInfo.RightLineSum - 2][1] > 10))
        {
            for (i = 95; i > 10; i--)
            {
                for (j = 184; j > j1 + 5; j--)
                {
                    if (imageBin[i][j] && imageBin[i][j - 1] && !imageBin[i][j - 2])
                    {
                        turnpoint[0][0] = i;
                        turnpoint[0][1] = j - 1;
                        break;
                    }
                }
                if (turnpoint[0][0]) break;
            }

            if (turnpoint[0][0])
            {
                RightLine[0][0] = 117;
                if (LeftLine[0][1] + 0x8E < 187) RightLine[0][1] = LeftLine[0][1] + 0x8E;
                else RightLine[0][1] = 187;
                imgInfo.RightLineCount = repair(RightLine[0], turnpoint[0]);

                Count = imgInfo.RightLineCount;
                NearNum = 2;
                while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 200)
                {
                    NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

                    Count++;

                    if (NearNum == 99) break;
                }
                imgInfo.RightLineSum = Count;

                FindMiddleLine(imgInfo.LeftLineSum, imgInfo.RightLineSum);
            }
        }
        else
        {
            TempLine1[0][0] = 117;
            TempLine1[0][1] = 184;

            for (j = 0; j < imgInfo.RightLineSum; j++)
            {
                if (RightLine[j][1] < 180 && RightLine[j][1]) break;
            }

            j1 = repair(TempLine1[0], RightLine[j]);

            if (imgInfo.LeftLineSum > j1)
            {
                for (i = 0; i < j1; i++)
                {
                    MiddleLine[i][0] = (LeftLine[i][0] + TempLine1[i][0]) / 2;
                    MiddleLine[i][1] = (LeftLine[i][1] + TempLine1[i][1]) / 2;
                }

                for (i = j1; i < imgInfo.LeftLineSum; i++)
                {
                    MiddleLine[i][0] = (LeftLine[i][0] + RightLine[j - j1 + i][0]) / 2;
                    MiddleLine[i][1] = (LeftLine[i][1] + RightLine[j - j1 + i][1]) / 2;
                }

                for (i = 0; i < imgInfo.RightLineSum; i++)
                {
                    MiddleLine[i + imgInfo.LeftLineSum][0] = (LeftLine[imgInfo.LeftLineSum][0] + RightLine[i][0]) / 2;
                    MiddleLine[i + imgInfo.LeftLineSum][1] = (LeftLine[imgInfo.LeftLineSum][1] + RightLine[i][1]) / 2;
                }
                imgInfo.MiddleLineCount = imgInfo.RightLineSum;
            }
            else
            {
                for (i = 0; i < imgInfo.LeftLineSum; i++)
                {
                    MiddleLine[i][0] = (LeftLine[i][0] + TempLine1[i][0]) / 2;
                    MiddleLine[i][1] = (LeftLine[i][1] + TempLine1[i][1]) / 2;
                }
                for (i = imgInfo.LeftLineSum; i < j1; i++)
                {
                    MiddleLine[i][0] = (LeftLine[imgInfo.LeftLineSum][0] + TempLine1[i][0]) / 2;
                    MiddleLine[i][1] = (LeftLine[imgInfo.LeftLineSum][1] + TempLine1[i][1]) / 2;
                }
                for (i = 0; i < imgInfo.RightLineSum; i++)
                {
                    MiddleLine[i + j1][0] = (LeftLine[imgInfo.LeftLineSum][0] + RightLine[i][0]) / 2;
                    MiddleLine[i + j1][1] = (LeftLine[imgInfo.LeftLineSum][1] + RightLine[i][1]) / 2;
                }
                imgInfo.MiddleLineCount = imgInfo.RightLineSum + j1;
            }
            if (RightLine[1][1] > 180 && RightLine[imgInfo.RightLineCount - 1][1] < 10 && RightLine[imgInfo.RightLineCount - 1][1]) CircleFlag = 14;
        }
    }
    if (CircleFlag == 14)
    {
        if (CircleMeanCount < 19) CircleMeanCount++;
        else CircleMeanCount = 0;
        if (RightTurn[0][0] > 20)
        {
            CircleFlag = 15;
            j = RightTurn[0][1];
            for (i = 1; i < 5; i++)
            {
                if (!imageBin[RightTurn[0][0] - i][j])  //i行  j列
                {
                    CircleFlag = 14;
                    break;
                }
            }
        }

        FindMiddleLine(imgInfo.LeftLineSum, imgInfo.RightLineSum);
    }
    if (CircleFlag == 15)
    {
        if (imgInfo.RightLineCount > 80)
        {
            imgInfo.RightLineCount += extend(RightLine[imgInfo.RightLineCount - 10], RightLine[imgInfo.RightLineCount]);

            if (!imgInfo.LeftLineSum)
            {
                LeftLine[0][0] = 117;
                LeftLine[0][1] = 3;
            }

            FindMiddleLine(imgInfo.LeftLineCount, imgInfo.RightLineSum);
        }
        else
        {
            CircleFlag = 16;
        }
    }
    if (CircleFlag == 16)
    {
        memset(turnpoint, 0, 8);

        CircleFlag = 17;
        for (i = 117; i > 80; i--)
        {
            if (!imageBin[i][184])
            {
                CircleFlag = 16;
                break;
            }
        }
    }
    if (CircleFlag == 17)
    {
        if (!imgInfo.RightLineSum)
        {
            RightLine[0][0] = 0;
            for (i = 117; i > 10; i--)
            {
                if (!imageBin[i][183]) break;
            }

            if (i > 15)
            {
                for (j = 183; j > 60; j--)
                {
                    if (!imageBin[i][j] && imageBin[i][j - 1] && imageBin[i][j - 2])
                    {
                        RightLine[0][0] = i;
                        RightLine[0][1] = j - 1;
                        break;
                    }
                }

                if (!RightLine[0][0])
                {
                    RightLine[0][0] = i;
                    RightLine[0][1] = 183;
                }
            }

            if (RightLine[0][0]) imgInfo.RightLineSum = ScanLine(RightLine[0], 2, 1);
        }

        if (imgInfo.RightLineSum > 40 && RightLine[imgInfo.RightLineSum - 10][1] > 47) CircleFlag = 18;
        CircleCount = 0;
    }
    if (CircleFlag == 18)
    {
        CircleCount++;
        if (!imgInfo.RightLineSum)
        {
            for (i = 117; i > 30; i--)
            {
                if (!imageBin[i][184]) break;
            }

            for (j = 184; j > 60; j--)
            {
                if (!imageBin[i][j] && imageBin[i][j - 1] && imageBin[i][j - 2])
                {
                    RightLine[0][0] = i;
                    RightLine[0][1] = j - 1;
                    break;
                }
            }

            imgInfo.RightLineSum = ScanLine(RightLine[0], 2, 1);
        }
        else if (((!NoCircleFlag && CircleCount > 25) || (NoCircleFlag && CircleCount > 50)) && imgInfo.RightLineSum > 30)
        {
            Count = 0;
            for (i = 0; i < 40; i++)
            {
                j1 = RightLine[i][0];
                for (j2 = RightLine[i][1] - WidthBuffer[RightLine[i][0]] + 5; j2 > 4 && j2 > RightLine[i][1] - WidthBuffer[RightLine[i][0]] - 10; j2--)
                {
                    if (imageBin[j1][j2] && !imageBin[j1][j2 - 1] && !imageBin[j1][j2 - 2])
                    {
                        TempLine1[Count][0] = j1;
                        TempLine1[Count][1] = j2;
                        Count++;
                        break;
                    }
                }
            }

            if (Count > 32)
            {
                CircleFlag = 0;
                imgInfo.nextroadType = Straight;
            }
        }

        for (i = 0; i < imgInfo.RightLineSum && i < 120; i++)
        {
            MiddleLine[i][0] = RightLine[i][0];
            MiddleLine[i][1] = RightLine[i][1] - WidthBuffer[RightLine[i][0]] / 2;
        }
        imgInfo.MiddleLineCount = i;
    }
}


//-------------------------------------------------------------------------------------------------------------------
// @brief       S弯扫线
// @param       无
// @return      void
// Sample usage:            ErrorInit();//偏差权重数组初始化
//-------------------------------------------------------------------------------------------------------------------
void crooked(void)
{
    uint8 i, j, Count, NearNum;

    i = 117;
    Count = 0;

    while (imageBin[i][3] || imageBin[i][4])
    {
        LeftLine[Count][0] = i;
        LeftLine[Count][1] = 4;
        Count++;
        i--;
    }

    for (j = 3; j < 120; j++)
    {
        if (!imageBin[i][j] && imageBin[i][j + 1])
        {
            LeftLine[Count][0] = i;
            LeftLine[Count][1] = j + 1;
            break;
        }
    }

    NearNum = 0;
    while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 200)
    {
        if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
        else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);

        Count++;

        if (NearNum == 99) break;
    }
    imgInfo.LeftLineSum = Count;


    i = 117;
    Count = 0;

    while (imageBin[i][184] || imageBin[i][183])
    {
        RightLine[Count][0] = i;
        RightLine[Count][1] = 183;
        Count++;
        i--;
    }

    for (j = 184; j > 60; j--)
    {
        if (!imageBin[i][j] && imageBin[i][j - 1])
        {
            RightLine[Count][0] = i;
            RightLine[Count][1] = j - 1;
            break;
        }
    }

    NearNum = 4;
    while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 200)
    {
        NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

        Count++;

        if (NearNum == 99) break;
    }
    imgInfo.RightLineSum = Count;

    memset(LeftTurn, 0, 9);

    for (i = 20; i < imgInfo.LeftLineSum - 1; i++)
    {
        if (LeftLine[i][1] > LeftTurn[0][1])
        {
            LeftTurn[0][0] = LeftLine[i][0];
            LeftTurn[0][1] = LeftLine[i][1];
        }
        else if (LeftLine[i][1] < LeftTurn[0][1]) break;
    }

    //if (((int8)LeftTurn[0][1] - LeftLine[i - 10][1]) < 0 || (LeftLine[i + 10][0] && ((uint8)LeftTurn[0][1] - LeftLine[i + 10][1]) < 0))
    //{
    //    LeftTurn[0][0] = 0;
    //    LeftTurn[0][1] = 0;
    //}

    while (i < imgInfo.LeftLineSum - 1)
    {
        if (LeftLine[i][1] < LeftLine[i + 1][1]) break;
        else i++;
    }

    while (i < imgInfo.LeftLineSum - 1)
    {
        if (LeftLine[i][1] > LeftTurn[1][1])
        {
            LeftTurn[1][0] = LeftLine[i][0];
            LeftTurn[1][1] = LeftLine[i][1];
        }
        else if (LeftLine[i][1] < LeftTurn[1][1]) break;
        i++;
    }

    //if (LeftLine[i + 10][0])
    //{
    //    if (((int8)LeftTurn[1][1] - LeftLine[i - 10][1]) < 0 || ((uint8)LeftTurn[1][1] - LeftLine[i + 10][1]) < 0)
    //    {
    //        LeftTurn[1][0] = 0;
    //        LeftTurn[1][1] = 0;
    //    }
    //}
    //else
    //{
    //    if (((int8)LeftTurn[1][1] - LeftLine[i - 10][1]) < 0 || ((uint8)LeftTurn[1][1] - LeftLine[imgInfo.LeftLineSum][1]) < 0)
    //    {
    //        LeftTurn[1][0] = 0;
    //        LeftTurn[1][1] = 0;
    //    }
    //}
    //if (LeftLine[i + 10][0] && ((int8)LeftTurn[1][1] - LeftLine[i - 10][1]) < 2 || (LeftLine[i + 10][0] &&  ((uint8)LeftTurn[1][1] - LeftLine[i + 10][1]) < 2))
    //{
    //    LeftTurn[1][0] = 0;
    //    LeftTurn[1][1] = 0;
    //}

    memset(RightTurn, 0, 9);
    RightTurn[0][1] = 185;
    RightTurn[1][1] = 185;
    for (i = 20; i < imgInfo.RightLineSum - 1; i++)
    {
        if (RightLine[i][1] < RightTurn[0][1])
        {
            RightTurn[0][0] = RightLine[i][0];
            RightTurn[0][1] = RightLine[i][1];
        }
        else if (RightLine[i][1] > RightTurn[0][1]) break;
    }

    while (i < imgInfo.RightLineSum - 1)
    {
        if (RightLine[i][1] > RightLine[i + 1][1]) break;
        else i++;
    }

    while (i < imgInfo.RightLineSum - 1)
    {
        if (RightLine[i][1] < RightTurn[1][1])
        {
            RightTurn[1][0] = RightLine[i][0];
            RightTurn[1][1] = RightLine[i][1];
        }
        else if (RightLine[i][1] > RightTurn[1][1]) break;
        i++;
    }

    if ((LeftTurn[0][0] && LeftTurn[1][0] && RightTurn[0][0])
        || (RightTurn[0][0] && RightTurn[1][0] && LeftTurn[0][0]))
    {
        if (LeftTurn[1][1] > LeftTurn[0][1])
        {
            LeftTurn[3][0] = LeftTurn[1][0];
            LeftTurn[3][1] = LeftTurn[1][1];
        }
        else
        {
            LeftTurn[3][0] = LeftTurn[0][0];
            LeftTurn[3][1] = LeftTurn[0][1];
        }

        if (RightTurn[1][1] < RightTurn[0][1] && RightTurn[1][1]> LeftTurn[3][1])
        {
            RightTurn[3][0] = RightTurn[1][0];
            RightTurn[3][1] = RightTurn[1][1];
        }
        else if (RightTurn[0][1] < RightTurn[1][1] && RightTurn[0][1]> LeftTurn[3][1])
        {
            RightTurn[3][0] = RightTurn[0][0];
            RightTurn[3][1] = RightTurn[0][1];
        }

        for (i = 0; i < 15; i++)
        {
            MiddleLine[i][0] = 115 - i;
            MiddleLine[i][1] = (LeftTurn[3][1] + RightTurn[3][1]) / 2;
        }
        imgInfo.MiddleLineCount = 15;
    }
    else
    {
        //拟合有效直道中线
        if (imgInfo.LeftLineSum <= imgInfo.RightLineSum)
        {
            for (i = 0; i < imgInfo.LeftLineSum; i++)
            {
                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
            }
            for (i = imgInfo.LeftLineSum; i < imgInfo.RightLineSum; i++)
            {
                MiddleLine[i][0] = (LeftLine[imgInfo.LeftLineSum][0] + RightLine[i][0]) / 2;
                MiddleLine[i][1] = (LeftLine[imgInfo.LeftLineSum][1] + RightLine[i][1]) / 2;
            }
            imgInfo.MiddleLineCount = imgInfo.RightLineSum;
            imgInfo.top = RightLine[imgInfo.RightLineSum][0];
        }
        else
        {
            for (i = 0; i < imgInfo.RightLineSum; i++)
            {
                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
            }
            for (i = imgInfo.RightLineSum; i < imgInfo.LeftLineSum; i++)
            {
                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineSum][0]) / 2;
                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineSum][1]) / 2;
            }
            imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
            imgInfo.top = LeftLine[imgInfo.LeftLineSum][0];
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       斜入十字扫线
// @param       无
// @return      void
// Sample usage:            ErrorInit();//偏差权重数组初始化
//-------------------------------------------------------------------------------------------------------------------
//void badcross1(void)
//{
//    uint8 i, j, NearNum, Count;
//
//    if (!BadCrossFlag)
//    {
//        if (RepairFlag[0] == 1 && !RepairFlag[1]) BadCrossFlag = 1;
//        else if (RepairFlag[1] == 1 && !RepairFlag[0]) BadCrossFlag = 11;
//    }
//
//    if (RepairFlag[0] == 1 && RepairFlag[1] == 1)
//    {
//        if (imgInfo.LeftLineSum <= imgInfo.RightLineSum)
//        {
//            for (i = 0; i < imgInfo.LeftLineSum; i++)
//            {
//                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
//                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
//            }
//            for (i = imgInfo.LeftLineSum; i < imgInfo.RightLineSum; i++)
//            {
//                MiddleLine[i][0] = (LeftLine[imgInfo.LeftLineSum][0] + RightLine[i][0]) / 2;
//                MiddleLine[i][1] = (LeftLine[imgInfo.LeftLineSum][1] + RightLine[i][1]) / 2;
//            }
//            imgInfo.MiddleLineCount = imgInfo.RightLineSum;
//        }
//        else
//        {
//            for (i = 0; i < imgInfo.RightLineSum; i++)
//            {
//                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
//                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
//            }
//            for (i = imgInfo.RightLineSum; i < imgInfo.LeftLineSum; i++)
//            {
//                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineSum][0]) / 2;
//                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineSum][1]) / 2;
//            }
//            imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
//        }
//    }
//    else
//    {
//        if (BadCrossFlag == 1)
//        {
//            if (!LeftTurn[0][0] || LeftTurn[0][0] > 100) BadCrossFlag = 2;
//
//            if (RepairFlag[0] == 1)
//            {
//                for (i = imgInfo.LeftLineCount + 10; i < imgInfo.LeftLineSum - 10; i++)
//                {
//                    if (imageBin[LeftLine[i][0]][LeftLine[i][1] + 2] && !imageBin[LeftLine[i][0]][LeftLine[i][1] - 2] && !imageBin[LeftLine[i][0]][LeftLine[i][1] - 7])
//                    {
//                        turnpoint[0][0] = LeftLine[i][0];
//                        turnpoint[0][1] = LeftLine[i][1];
//                        break;
//                    }
//                }
//
//                if (LeftLine[i][0] > 35) Count = LeftLine[i][0] - 20;
//                else Count = LeftLine[i][0] - 10;
//
//                if (turnpoint[0][0] && !imageBin[Count][184])
//                {
//                    for (j = LeftLine[i][1]; j < 184; j++)
//                    {
//                        if (imageBin[Count][j] && !imageBin[Count][j + 1] && !imageBin[Count][j + 2])
//                        {
//                            TempLine2[0][0] = Count;
//                            TempLine2[0][1] = j;
//                            break;
//                        }
//                    }
//                }
//
//                if (TempLine2[0][0])
//                {
//                    Count = 0;
//                    NearNum = 4;
//                    while (TempLine2[Count][0] > 3 && TempLine2[Count][1] > 3 && TempLine2[Count][1] < 184 && Count < 200)
//                    {
//                        if (NearNum < 2) NearNum = FindNext(&TempLine2[Count][0], NearNum + 6, 0);
//                        else NearNum = FindNext(&TempLine2[Count][0], NearNum - 2, 0);
//
//                        Count++;
//                    }
//
//                    Count = extend(TempLine2[0], TempLine2[9]) + 5;
//
//                    for (i = 115; i > TempLine2[Count][0]; i--)
//                    {
//                        RightLine[115 - i][0] = i;
//                        RightLine[115 - i][1] = 184;
//                    }
//
//                    for (j = 0; j < Count; j++)
//                    {
//                        RightLine[115 - i + j][0] = TempLine2[Count - j][0];
//                        RightLine[115 - i + j][1] = TempLine2[Count - j][1];
//                    }
//
//                    imgInfo.RightLineSum = 115 - i + j;
//
//                    //拟合有效直道中线
//                    if (imgInfo.LeftLineSum <= imgInfo.RightLineSum)
//                    {
//                        for (i = 0; i < imgInfo.LeftLineSum; i++)
//                        {
//                            MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
//                            MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
//                        }
//
//                        imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
//                        imgInfo.top = RightLine[imgInfo.LeftLineSum][0];
//                    }
//                    else
//                    {
//                        for (i = 0; i < imgInfo.RightLineSum; i++)
//                        {
//                            MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
//                            MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
//                        }
//                        imgInfo.MiddleLineCount = imgInfo.RightLineSum;
//                        imgInfo.top = LeftLine[imgInfo.RightLineSum][0];
//                    }
//                }
//            }
//            else
//            {
//                for (i = 0; TempLine3[i][0]; i++)
//                {
//                    LeftLine[i + imgInfo.LeftLineCount][0] = TempLine3[i][0];
//                    LeftLine[i + imgInfo.LeftLineCount][1] = TempLine3[i][1];
//                }
//                imgInfo.LeftLineSum = i + imgInfo.LeftLineCount;
//
//                for (i = 0; i < imgInfo.LeftLineSum; i++)
//                {
//                    MiddleLine[i][0] = LeftLine[i][0];
//                    MiddleLine[i][1] = LeftLine[i][1] / 2 + 94;
//                }
//
//                imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
//            }
//        }
//        if (BadCrossFlag == 2)
//        {
//            uint8 j1, j2;
//            j1 = 20;
//            j2 = 167;
//
//            i = 115;
//            while (!imageBin[i][j2])
//            {
//                i--;
//                if (i < 25) break;
//            }
//            while (imageBin[i][j2])
//            {
//                i--;
//                if (i < 25) break;
//            }//找到十字前端
//
//            if (i > 25)//能找到
//            {
//                i -= 25;
//                for (j = j1; j < j2; j++)
//                {
//                    if (!imageBin[i][j] && imageBin[i][j + 1] && imageBin[i][j + 2])
//                    {
//                        TempLine1[0][0] = i;
//                        TempLine1[0][1] = j + 1;
//                        break;
//                    }
//                }
//                for (j = j2; j > j1; j--)
//                {
//                    if (!imageBin[i][j] && imageBin[i][j - 1] && imageBin[i][j - 2])
//                    {
//                        TempLine2[0][0] = i;
//                        TempLine2[0][1] = j - 1;
//                        break;
//                    }
//                }//找起始点
//
//                Count = 0;
//                NearNum = 0;
//                while (Count < 10)
//                {
//                    if (NearNum > 5) NearNum = FindNext(&TempLine1[Count][0], NearNum - 6, 1);
//                    else NearNum = FindNext(&TempLine1[Count][0], NearNum + 2, 1);
//
//                    Count++;
//                }
//
//                Count = 0;
//                NearNum = 4;
//                while (Count < 10)
//                {
//                    NearNum = FindNext(&TempLine2[Count][0], NearNum - 2, 0);
//
//                    Count++;
//                }
//
//                i = extend(TempLine1[0], TempLine1[9]) + 8;
//                j = extend(TempLine2[0], TempLine2[9]) + 8;
//
//                while (TempLine1[i][0] < 115)
//                {
//                    TempLine1[i + 1][0] = TempLine1[i][0] + 1;
//                    TempLine1[i + 1][1] = 3;
//                    i++;
//                }
//                while (TempLine2[j][0] < 115)
//                {
//                    TempLine2[j + 1][0] = TempLine2[j][0] + 1;
//                    TempLine2[j + 1][1] = 184;
//                    j++;
//                }
//
//                Count = 0;
//                while (i > 1 && j > 1)
//                {
//                    MiddleLine[Count][0] = (TempLine1[i][0] + TempLine2[j][0]) / 2;
//                    MiddleLine[Count][1] = (TempLine1[i][1] + TempLine2[j][1]) / 2;
//                    Count++;
//                    i--;
//                    j--;
//                }
//                imgInfo.MiddleLineCount = Count;
//
//                imgInfo.MiddleLineCount = Count;
//
//                LeftLine[0][0] = TempLine1[0][0];
//                LeftLine[0][1] = TempLine1[0][1];
//                i = 0;
//                while (LeftLine[i][0] > 3 && LeftLine[i][1] > 3 && LeftLine[i][1] < 184 && i < 200)
//                {
//                    if (NearNum < 2) NearNum = FindNext(&LeftLine[i][0], NearNum + 6, 0);
//                    else NearNum = FindNext(&LeftLine[i][0], NearNum - 2, 0);
//
//                    if (NearNum == 99) break;
//                    i++;
//                }
//                imgInfo.LeftLineSum = i;
//
//                RightLine[0][0] = TempLine2[0][0];
//                RightLine[0][1] = TempLine2[0][1];
//
//                i = 0;
//                while (RightLine[i][0] > 3 && RightLine[i][1] > 3 && RightLine[i][1] < 184 && i < 200)
//                {
//                    NearNum = FindNext(&RightLine[i][0], NearNum + 2, 1);
//
//                    if (NearNum == 99) break;
//                    i++;
//                }
//                imgInfo.RightLineSum = i;
//
//                if (imgInfo.LeftLineSum <= imgInfo.RightLineSum)
//                {
//                    for (i = 0; i < imgInfo.LeftLineSum; i++)
//                    {
//                        MiddleLine[i + Count][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
//                        MiddleLine[i + Count][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
//                        if (i + Count > 200)break;
//                    }
//                    for (i = imgInfo.LeftLineSum; i < imgInfo.RightLineSum; i++)
//                    {
//                        MiddleLine[i + Count][0] = (LeftLine[imgInfo.LeftLineSum][0] + RightLine[i][0]) / 2;
//                        MiddleLine[i + Count][1] = (LeftLine[imgInfo.LeftLineSum][1] + RightLine[i][1]) / 2;
//                        if (i + Count > 200)break;
//                    }
//                    if (imgInfo.RightLineSum + Count > 200) imgInfo.MiddleLineCount = 200;
//                    else imgInfo.MiddleLineCount = imgInfo.RightLineSum + Count;
//                }
//                else
//                {
//                    for (i = 0; i < imgInfo.RightLineSum; i++)
//                    {
//                        MiddleLine[i + Count][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
//                        MiddleLine[i + Count][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
//                        if (i + Count > 200)break;
//                    }
//                    for (i = imgInfo.RightLineSum; i < imgInfo.LeftLineSum; i++)
//                    {
//                        MiddleLine[i + Count][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineSum][0]) / 2;
//                        MiddleLine[i + Count][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineSum][1]) / 2;
//                        if (i + Count > 200)break;
//                    }
//                    if (imgInfo.LeftLineSum + Count > 200) imgInfo.MiddleLineCount = 200;
//                    else imgInfo.MiddleLineCount = imgInfo.LeftLineSum + Count;
//                }
//            }
//        }
//
//        if (BadCrossFlag == 11)
//        {
//            if (!RightTurn[0][0] || RightTurn[0][0] > 100) BadCrossFlag = 12;
//            if (RepairFlag[1] == 1)
//            {
//                for (i = imgInfo.RightLineCount + 10; i < imgInfo.RightLineSum - 10; i++)
//                {
//                    if (imageBin[RightLine[i][0]][LeftLine[i][1] - 2] && !imageBin[RightLine[i][0]][RightLine[i][1] + 2] && !imageBin[RightLine[i][0]][RightLine[i][1] + 7])
//                    {
//                        turnpoint[0][0] = RightLine[i][0];
//                        turnpoint[0][1] = RightLine[i][1];
//                        break;
//                    }
//                }
//
//                if (RightLine[i][0] > 35 && !imageBin[RightLine[i][0] - 20][RightLine[i][1]] && !imageBin[RightLine[i][0] - 30][RightLine[i][1]]) Count = RightLine[i][0] - 20;
//                else Count = RightLine[i][0] - 10;
//
//                if (turnpoint[0][0] && !imageBin[Count][3])
//                {
//                    for (j = RightLine[i][1]; j > 3; j--)
//                    {
//                        if (imageBin[Count][j] && !imageBin[Count][j - 1] && !imageBin[Count][j - 2])
//                        {
//                            TempLine1[0][0] = Count;
//                            TempLine1[0][1] = j;
//                            break;
//                        }
//                    }
//                }
//
//                if (TempLine1[0][0])
//                {
//                    Count = 0;
//                    NearNum = 0;
//                    //右线！
//                    while (TempLine1[Count][0] > 3 && TempLine1[Count][1] > 3 && TempLine1[Count][1] < 184 && Count < 200)
//                    {
//                        if (NearNum >= 6) NearNum = FindNext(&TempLine1[Count][0], NearNum - 6, 1);
//                        else NearNum = FindNext(&TempLine1[Count][0], NearNum + 2, 1);
//                        Count++;
//
//                        if (NearNum == 99) break;
//                    }
//
//                    Count = extend(TempLine1[0], TempLine1[9]) + 5;
//
//                    for (i = 115; i > TempLine1[Count][0]; i--)
//                    {
//                        LeftLine[115 - i][0] = i;
//                        LeftLine[115 - i][1] = 3;
//                    }
//
//                    for (j = 0; j < Count; j++)
//                    {
//                        LeftLine[115 - i + j][0] = TempLine1[Count - j][0];
//                        LeftLine[115 - i + j][1] = TempLine1[Count - j][1];
//                    }
//
//                    imgInfo.LeftLineSum = 115 - i + j;
//
//                    //拟合有效直道中线
//                    if (imgInfo.LeftLineSum <= imgInfo.RightLineSum)
//                    {
//                        for (i = 0; i < imgInfo.LeftLineSum; i++)
//                        {
//                            MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
//                            MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
//                        }
//
//                        imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
//                        imgInfo.top = RightLine[imgInfo.LeftLineSum][0];
//                    }
//                    else
//                    {
//                        for (i = 0; i < imgInfo.RightLineSum; i++)
//                        {
//                            MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
//                            MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
//                        }
//                        imgInfo.MiddleLineCount = imgInfo.RightLineSum;
//                        imgInfo.top = LeftLine[imgInfo.RightLineSum][0];
//                    }
//                }
//            }
//            else
//            {
//                for (i = 0; TempLine3[i][0]; i++)
//                {
//                    RightLine[i + imgInfo.RightLineCount][0] = TempLine3[i][0];
//                    RightLine[i + imgInfo.RightLineCount][1] = TempLine3[i][1];
//                }
//                imgInfo.RightLineSum = i + imgInfo.RightLineCount;
//
//                for (i = 0; i < imgInfo.RightLineSum; i++)
//                {
//                    MiddleLine[i][0] = RightLine[i][0];
//                    MiddleLine[i][1] = RightLine[i][1] / 2;
//                }
//
//                imgInfo.MiddleLineCount = imgInfo.RightLineSum;
//            }
//        }
//        if (BadCrossFlag == 12)
//        {
//            uint8 j1, j2;
//            j1 = 20;
//            j2 = 167;
//
//            i = 115;
//            while (!imageBin[i][j1])
//            {
//                i--;
//                if (i < 25) break;
//            }
//            while (imageBin[i][j1])
//            {
//                i--;
//                if (i < 25) break;
//            }//找到十字前端
//
//            if (i > 25)//能找到
//            {
//                i -= 25;
//                for (j = j1; j < j2; j++)
//                {
//                    if (!imageBin[i][j] && imageBin[i][j + 1] && imageBin[i][j + 2])
//                    {
//                        TempLine1[0][0] = i;
//                        TempLine1[0][1] = j + 1;
//                        break;
//                    }
//                }
//                for (j = j2; j > j1; j--)
//                {
//                    if (!imageBin[i][j] && imageBin[i][j - 1] && imageBin[i][j - 2])
//                    {
//                        TempLine2[0][0] = i;
//                        TempLine2[0][1] = j - 1;
//                        break;
//                    }
//                }//找起始点
//
//                Count = 0;
//                NearNum = 0;
//                while (Count < 10)
//                {
//                    if (NearNum > 5) NearNum = FindNext(&TempLine1[Count][0], NearNum - 6, 1);
//                    else NearNum = FindNext(&TempLine1[Count][0], NearNum + 2, 1);
//
//                    Count++;
//                }
//
//                Count = 0;
//                NearNum = 4;
//                while (Count < 10)
//                {
//                    NearNum = FindNext(&TempLine2[Count][0], NearNum - 2, 0);
//
//                    Count++;
//                }
//
//                i = extend(TempLine1[0], TempLine1[9]) + 8;
//                j = extend(TempLine2[0], TempLine2[9]) + 8;
//
//                while (TempLine1[i][0] < 115)
//                {
//                    TempLine1[i + 1][0] = TempLine1[i][0] + 1;
//                    TempLine1[i + 1][1] = 3;
//                    i++;
//                }
//                while (TempLine2[j][0] < 115)
//                {
//                    TempLine2[j + 1][0] = TempLine2[j][0] + 1;
//                    TempLine2[j + 1][1] = 184;
//                    j++;
//                }
//
//                Count = 0;
//                while (i > 1 && j > 1)
//                {
//                    MiddleLine[Count][0] = (TempLine1[i][0] + TempLine2[j][0]) / 2;
//                    MiddleLine[Count][1] = (TempLine1[i][1] + TempLine2[j][1]) / 2;
//                    Count++;
//                    i--;
//                    j--;
//                }
//                imgInfo.MiddleLineCount = Count;
//
//                LeftLine[0][0] = TempLine1[0][0];
//                LeftLine[0][1] = TempLine1[0][1];
//                i = 0;
//                while (LeftLine[i][0] > 3 && LeftLine[i][1] > 3 && LeftLine[i][1] < 184 && i < 200)
//                {
//                    if (NearNum < 2) NearNum = FindNext(&LeftLine[i][0], NearNum + 6, 0);
//                    else NearNum = FindNext(&LeftLine[i][0], NearNum - 2, 0);
//
//                    if (NearNum == 99) break;
//                    i++;
//                }
//                imgInfo.LeftLineSum = i;
//
//                RightLine[0][0] = TempLine2[0][0];
//                RightLine[0][1] = TempLine2[0][1];
//
//                i = 0;
//                while (RightLine[i][0] > 3 && RightLine[i][1] > 3 && RightLine[i][1] < 184 && i < 200)
//                {
//                    NearNum = FindNext(&RightLine[i][0], NearNum + 2, 1);
//
//                    if (NearNum == 99) break;
//                    i++;
//                }
//                imgInfo.RightLineSum = i;
//
//                if (imgInfo.LeftLineSum <= imgInfo.RightLineSum)
//                {
//                    for (i = 0; i < imgInfo.LeftLineSum; i++)
//                    {
//                        MiddleLine[i + Count][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
//                        MiddleLine[i + Count][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
//                        if (i + Count > 200)break;
//                    }
//                    for (i = imgInfo.LeftLineSum; i < imgInfo.RightLineSum; i++)
//                    {
//                        MiddleLine[i + Count][0] = (LeftLine[imgInfo.LeftLineSum][0] + RightLine[i][0]) / 2;
//                        MiddleLine[i + Count][1] = (LeftLine[imgInfo.LeftLineSum][1] + RightLine[i][1]) / 2;
//                        if (i + Count > 200)break;
//                    }
//                    if (imgInfo.RightLineSum + Count > 200) imgInfo.MiddleLineCount = 200;
//                    else imgInfo.MiddleLineCount = imgInfo.RightLineSum + Count;
//                }
//                else
//                {
//                    for (i = 0; i < imgInfo.RightLineSum; i++)
//                    {
//                        MiddleLine[i + Count][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
//                        MiddleLine[i + Count][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
//                        if (i + Count > 200)break;
//                    }
//                    for (i = imgInfo.RightLineSum; i < imgInfo.LeftLineSum; i++)
//                    {
//                        MiddleLine[i + Count][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineSum][0]) / 2;
//                        MiddleLine[i + Count][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineSum][1]) / 2;
//                        if (i + Count > 200)break;
//                    }
//                    if (imgInfo.LeftLineSum + Count > 200) imgInfo.MiddleLineCount = 200;
//                    else imgInfo.MiddleLineCount = imgInfo.LeftLineSum + Count;
//                }
//            }
//        }
//    }
//}

//-------------------------------------------------------------------------------------------------------------------
// @brief       斜入十字扫线
// @param       无
// @return      void
// Sample usage:            ErrorInit();//偏差权重数组初始化
//-------------------------------------------------------------------------------------------------------------------
void badcross(void)
{
    uint8 i, j, NearNum, Count;

    if (!BadCrossFlag)
    {
        if (RepairFlag[0] == 1 && !RepairFlag[1]) BadCrossFlag = 1;
        else if (RepairFlag[1] == 1 && !RepairFlag[0]) BadCrossFlag = 11;
    }

    if (RepairFlag[0] == 1 && RepairFlag[1] == 1)
    {
        if (imgInfo.LeftLineSum <= imgInfo.RightLineSum)
        {
            for (i = 0; i < imgInfo.LeftLineSum; i++)
            {
                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
            }
            for (i = imgInfo.LeftLineSum; i < imgInfo.RightLineSum; i++)
            {
                MiddleLine[i][0] = (LeftLine[imgInfo.LeftLineSum][0] + RightLine[i][0]) / 2;
                MiddleLine[i][1] = (LeftLine[imgInfo.LeftLineSum][1] + RightLine[i][1]) / 2;
            }
            imgInfo.MiddleLineCount = imgInfo.RightLineSum;
        }
        else
        {
            for (i = 0; i < imgInfo.RightLineSum; i++)
            {
                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
            }
            for (i = imgInfo.RightLineSum; i < imgInfo.LeftLineSum; i++)
            {
                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineSum][0]) / 2;
                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineSum][1]) / 2;
            }
            imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
        }
    }
    else
    {
        if (BadCrossFlag == 1)
        {
            if (!LeftTurn[0][0] || LeftTurn[0][0] > 100) BadCrossFlag = 2;

            imgInfo.LeftLineSum = imgInfo.LeftLineCount + extend(LeftLine[imgInfo.LeftLineCount - 15], LeftLine[imgInfo.LeftLineCount - 3]) - 3;

            for (i = 0; i < imgInfo.LeftLineSum; i++)
            {
                MiddleLine[i][0] = LeftLine[i][0];
                MiddleLine[i][1] = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] / 2;
            }

            imgInfo.MiddleLineCount = imgInfo.LeftLineSum;

            Count = 0;
            for (i = 0; i < imgInfo.MiddleLineCount; i++)
            {
                if (!imageBin[MiddleLine[i][0]][MiddleLine[i][1]]) Count++;
            }

            if (Count > 20)
            {
                BadCrossFlag = 0;
                RightTFlag = 4;
            }
        }
        if (BadCrossFlag == 2)
        {
            uint8 j1, j2;
            uint8 flag = 0;
            j1 = 0;
            j2 = 0;

            for (i = 100; i > 30; i--)
            {
                //寻找左起点
                for (j = 184; j > 60; j--)
                {
                    if (!imageBin[i][j] && imageBin[i][j - 1] && imageBin[i][j - 2])
                    {
                        j1 = j - 1;
                        for (j = j1; j > j1 - WidthBuffer[i] - 10; j--)
                        {
                            j2 = j1 - WidthBuffer[i] / 2 - 10;
                            if (!imageBin[i][j] && imageBin[i][j + 1] && imageBin[i][j + 2])
                            {
                                TempLine2[0][0] = i;
                                TempLine2[0][1] = j1;
                                TempLine1[0][0] = i;
                                TempLine1[0][1] = j + 1;
                                flag = 1;
                                break;
                            }
                        }
                        if (flag) break;
                    }
                }

                if (flag) break;
            }

            if (flag)
            {
                i = TempLine2[0][0] - 15;
                for (j = 184; j > 60; j--)
                {
                    if (!imageBin[i][j] && imageBin[i][j - 1] && imageBin[i][j - 2])
                    {
                        j1 = j - 1;
                        for (j = j1; j > j1 - WidthBuffer[i] - 10; j--)
                        {
                            j2 = j1 - WidthBuffer[i] / 2 - 10;
                            if (!imageBin[i][j] && imageBin[i][j + 1] && imageBin[i][j + 2])
                            {
                                TempLine2[0][0] = i;
                                TempLine2[0][1] = j1;
                                TempLine1[0][0] = i;
                                TempLine1[0][1] = j + 1;
                                flag = 2;
                                break;
                            }
                        }
                        if (flag == 2) break;
                    }
                }
            }

            if (flag == 2)//能找到
            {
                Count = 0;
                NearNum = 0;
                while (Count < 15)
                {
                    if (NearNum > 5) NearNum = FindNext(&TempLine1[Count][0], NearNum - 6, 1);
                    else NearNum = FindNext(&TempLine1[Count][0], NearNum + 2, 1);

                    Count++;
                }

                Count = 0;
                NearNum = 4;
                while (Count < 15)
                {
                    NearNum = FindNext(&TempLine2[Count][0], NearNum - 2, 0);

                    Count++;
                }

                i = extend(TempLine1[0], TempLine1[12]) + 11;
                j = extend(TempLine2[0], TempLine2[12]) + 11;

                while (TempLine1[i][0] && TempLine1[i][0] < 115)
                {
                    TempLine1[i + 1][0] = TempLine1[i][0] + 1;
                    TempLine1[i + 1][1] = 3;
                    i++;
                }
                while (TempLine2[j][0] && TempLine2[j][0] < 115)
                {
                    TempLine2[j + 1][0] = TempLine2[j][0] + 1;
                    TempLine2[j + 1][1] = 184;
                    j++;
                }

                Count = 0;
                while (i > 1 && j > 1)
                {
                    MiddleLine[Count][0] = (TempLine1[i][0] + TempLine2[j][0]) / 2;
                    MiddleLine[Count][1] = (TempLine1[i][1] + TempLine2[j][1]) / 2;
                    Count++;
                    i--;
                    j--;
                }
                imgInfo.MiddleLineCount = Count;

                imgInfo.MiddleLineCount = Count;

                LeftLine[0][0] = TempLine1[0][0];
                LeftLine[0][1] = TempLine1[0][1];
                i = 0;
                while (LeftLine[i][0] > 3 && LeftLine[i][1] > 3 && LeftLine[i][1] < 184 && i < 200)
                {
                    if (NearNum < 2) NearNum = FindNext(&LeftLine[i][0], NearNum + 6, 0);
                    else NearNum = FindNext(&LeftLine[i][0], NearNum - 2, 0);

                    if (NearNum == 99) break;
                    i++;
                }
                imgInfo.LeftLineSum = i;

                RightLine[0][0] = TempLine2[0][0];
                RightLine[0][1] = TempLine2[0][1];

                i = 0;
                while (RightLine[i][0] > 3 && RightLine[i][1] > 3 && RightLine[i][1] < 184 && i < 200)
                {
                    NearNum = FindNext(&RightLine[i][0], NearNum + 2, 1);

                    if (NearNum == 99) break;
                    i++;
                }
                imgInfo.RightLineSum = i;

                if (imgInfo.LeftLineSum <= imgInfo.RightLineSum)
                {
                    for (i = 0; i < imgInfo.LeftLineSum; i++)
                    {
                        MiddleLine[i + Count][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
                        MiddleLine[i + Count][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
                        if (i + Count > 200)break;
                    }
                    for (i = imgInfo.LeftLineSum; i < imgInfo.RightLineSum; i++)
                    {
                        MiddleLine[i + Count][0] = (LeftLine[imgInfo.LeftLineSum][0] + RightLine[i][0]) / 2;
                        MiddleLine[i + Count][1] = (LeftLine[imgInfo.LeftLineSum][1] + RightLine[i][1]) / 2;
                        if (i + Count > 200)break;
                    }
                    if (imgInfo.RightLineSum + Count > 200) imgInfo.MiddleLineCount = 200;
                    else imgInfo.MiddleLineCount = imgInfo.RightLineSum + Count;
                }
                else
                {
                    for (i = 0; i < imgInfo.RightLineSum; i++)
                    {
                        MiddleLine[i + Count][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
                        MiddleLine[i + Count][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
                        if (i + Count > 200)break;
                    }
                    for (i = imgInfo.RightLineSum; i < imgInfo.LeftLineSum; i++)
                    {
                        MiddleLine[i + Count][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineSum][0]) / 2;
                        MiddleLine[i + Count][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineSum][1]) / 2;
                        if (i + Count > 200)break;
                    }
                    if (imgInfo.LeftLineSum + Count > 200) imgInfo.MiddleLineCount = 200;
                    else imgInfo.MiddleLineCount = imgInfo.LeftLineSum + Count;
                }
            }
        }

        if (BadCrossFlag == 11)
        {
            if (!RightTurn[0][0] || RightTurn[0][0] > 100) BadCrossFlag = 12;

            imgInfo.RightLineSum = imgInfo.RightLineCount + extend(RightLine[imgInfo.RightLineCount - 15], RightLine[imgInfo.RightLineCount - 3]) - 3;

            for (i = 0; i < imgInfo.RightLineSum; i++)
            {
                MiddleLine[i][0] = RightLine[i][0];
                MiddleLine[i][1] = RightLine[i][1] - WidthBuffer[RightLine[i][0]] / 2;
            }

            imgInfo.MiddleLineCount = imgInfo.RightLineSum;

            Count = 0;
            for (i = 0; i < imgInfo.MiddleLineCount; i++)
            {
                if (!imageBin[MiddleLine[i][0]][MiddleLine[i][1]]) Count++;
            }

            if (Count > 20)
            {
                BadCrossFlag = 0;
                LeftTFlag = 4;
            }
        }
        if (BadCrossFlag == 12)
        {
            uint8 j1, j2;
            uint8 flag = 0;
            j1 = 0;
            j2 = 0;

            for (i = 100; i > 30; i--)
            {
                //寻找左起点
                for (j = 3; j < 130; j++)
                {
                    if (!imageBin[i][j] && imageBin[i][j + 1] && imageBin[i][j + 2])
                    {
                        j1 = j + 1;
                        for (j = j1; j < j1 + WidthBuffer[i] + 10; j++)
                        {
                            if (!imageBin[i][j] && imageBin[i][j - 1] && imageBin[i][j - 2])
                            {
                                TempLine1[0][0] = i;
                                TempLine1[0][1] = j1;
                                TempLine2[0][0] = i;
                                TempLine2[0][1] = j - 1;
                                flag = 1;
                                break;
                            }
                        }
                        if (flag) break;
                    }
                }

                if (flag) break;
            }

            if (flag)
            {
                i = TempLine1[0][0] - 15;
                //寻找左起点
                for (j = 3; j < 130; j++)
                {
                    if (!imageBin[i][j] && imageBin[i][j + 1] && imageBin[i][j + 2])
                    {
                        j1 = j + 1;
                        for (j = j1; j < j1 + WidthBuffer[i] + 10; j++)
                        {
                            if (!imageBin[i][j] && imageBin[i][j - 1] && imageBin[i][j - 2])
                            {
                                TempLine1[0][0] = i;
                                TempLine1[0][1] = j1;
                                TempLine2[0][0] = i;
                                TempLine2[0][1] = j - 1;
                                flag = 2;
                                break;
                            }
                        }
                        if (flag == 2) break;
                    }
                }
            }

            if (flag == 2)//能找到
            {
                Count = 0;
                NearNum = 0;
                while (Count < 10)
                {
                    if (NearNum > 5) NearNum = FindNext(&TempLine1[Count][0], NearNum - 6, 1);
                    else NearNum = FindNext(&TempLine1[Count][0], NearNum + 2, 1);

                    Count++;
                }

                Count = 0;
                NearNum = 4;
                while (Count < 10)
                {
                    NearNum = FindNext(&TempLine2[Count][0], NearNum - 2, 0);

                    Count++;
                }

                i = extend(TempLine1[0], TempLine1[9]) + 8;
                j = extend(TempLine2[0], TempLine2[9]) + 8;

                while (TempLine1[i][0] < 115)
                {
                    TempLine1[i + 1][0] = TempLine1[i][0] + 1;
                    TempLine1[i + 1][1] = 3;
                    i++;
                }
                while (TempLine2[j][0] < 115)
                {
                    TempLine2[j + 1][0] = TempLine2[j][0] + 1;
                    TempLine2[j + 1][1] = 184;
                    j++;
                }

                Count = 0;
                while (i > 1 && j > 1)
                {
                    MiddleLine[Count][0] = (TempLine1[i][0] + TempLine2[j][0]) / 2;
                    MiddleLine[Count][1] = (TempLine1[i][1] + TempLine2[j][1]) / 2;
                    Count++;
                    i--;
                    j--;
                }
                imgInfo.MiddleLineCount = Count;

                LeftLine[0][0] = TempLine1[0][0];
                LeftLine[0][1] = TempLine1[0][1];
                i = 0;
                while (LeftLine[i][0] > 3 && LeftLine[i][1] > 3 && LeftLine[i][1] < 184 && i < 200)
                {
                    if (NearNum < 2) NearNum = FindNext(&LeftLine[i][0], NearNum + 6, 0);
                    else NearNum = FindNext(&LeftLine[i][0], NearNum - 2, 0);

                    if (NearNum == 99) break;
                    i++;
                }
                imgInfo.LeftLineSum = i;

                RightLine[0][0] = TempLine2[0][0];
                RightLine[0][1] = TempLine2[0][1];

                i = 0;
                while (RightLine[i][0] > 3 && RightLine[i][1] > 3 && RightLine[i][1] < 184 && i < 200)
                {
                    NearNum = FindNext(&RightLine[i][0], NearNum + 2, 1);

                    if (NearNum == 99) break;
                    i++;
                }
                imgInfo.RightLineSum = i;

                if (imgInfo.LeftLineSum <= imgInfo.RightLineSum)
                {
                    for (i = 0; i < imgInfo.LeftLineSum; i++)
                    {
                        MiddleLine[i + Count][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
                        MiddleLine[i + Count][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
                        if (i + Count > 200)break;
                    }
                    for (i = imgInfo.LeftLineSum; i < imgInfo.RightLineSum; i++)
                    {
                        MiddleLine[i + Count][0] = (LeftLine[imgInfo.LeftLineSum][0] + RightLine[i][0]) / 2;
                        MiddleLine[i + Count][1] = (LeftLine[imgInfo.LeftLineSum][1] + RightLine[i][1]) / 2;
                        if (i + Count > 200)break;
                    }
                    if (imgInfo.RightLineSum + Count > 200) imgInfo.MiddleLineCount = 200;
                    else imgInfo.MiddleLineCount = imgInfo.RightLineSum + Count;
                }
                else
                {
                    for (i = 0; i < imgInfo.RightLineSum; i++)
                    {
                        MiddleLine[i + Count][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
                        MiddleLine[i + Count][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
                        if (i + Count > 200)break;
                    }
                    for (i = imgInfo.RightLineSum; i < imgInfo.LeftLineSum; i++)
                    {
                        MiddleLine[i + Count][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineSum][0]) / 2;
                        MiddleLine[i + Count][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineSum][1]) / 2;
                        if (i + Count > 200)break;
                    }
                    if (imgInfo.LeftLineSum + Count > 200) imgInfo.MiddleLineCount = 200;
                    else imgInfo.MiddleLineCount = imgInfo.LeftLineSum + Count;
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       偏差权重数组初始化
// @param       无
// @return      void
// Sample usage:            ErrorInit();//偏差权重数组初始化
//-------------------------------------------------------------------------------------------------------------------
void ErrorInit(void)
{
    if (imgInfo.nextroadType != NoHead)
    {
        if (ErrorBufferSum == 0)
        {
            uint8 i = 5;
            while (ErrorBuffer[i] != 0)
            {
                ErrorBufferSum += ErrorBuffer[i];
                i++;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       计算偏差
// @param       无
// @return      void
// Sample usage:           uint8 Error = ErrorSum();//计算偏差
//-------------------------------------------------------------------------------------------------------------------
uint8 ErrorSum(void)
{
    D0 = 0;
    if (LeftTFlag >= 15) return MiddleValue - 40;
    else if (RightTFlag >= 15) return MiddleValue + 40;
    else if (CircleFlag == 6 || CircleFlag == 7 || CircleFlag == 16 || CircleFlag == 17)
    {
        int32 Er = 0;

        for (uint8 i = 0; i < 20; i++)
        {
            Er += CircleMean[i];
        }
        Er /= 20;

        return MiddleValue - Er;
    }
    else if (imgInfo.nextroadType == ThreeFork && (ThreeForkFlag == 1 || (ThreeForkFlag == 0 && imgInfo.MiddleLineCount < 10)) && !imgInfo.ThreeForkDir) return MiddleValue - 55;
    else if (imgInfo.nextroadType == ThreeFork && (ThreeForkFlag == 1 || (ThreeForkFlag == 0 && imgInfo.MiddleLineCount < 10)) && imgInfo.ThreeForkDir) return MiddleValue + 55;
    else if (imgInfo.nextroadType == RightCircle && imgInfo.MiddleLineCount < 30 && CircleFlag) return MiddleValue + 50;
    else if (imgInfo.nextroadType == LeftCircle && imgInfo.MiddleLineCount < 30 && CircleFlag) return MiddleValue - 50;
    else if (imgInfo.nextroadType == LeftT && LeftTFlag >= 5 && imgInfo.MiddleLineCount < 20) return MiddleValue + 50;
    else if (imgInfo.nextroadType == RightT && RightTFlag >= 5 && imgInfo.MiddleLineCount < 20) return MiddleValue - 50;
    else if (imgInfo.nextroadType == NoHead && NoHeadFlag == 2 && !imgInfo.NoHeadDir) return MiddleValue - 45;
    else if (imgInfo.nextroadType == NoHead && NoHeadFlag == 2 && imgInfo.NoHeadDir) return MiddleValue + 50;
    else
    {
        D0 = 16;
        uint32 Sum = 0;
        uint32 ESum = 0;
        int32 Out = 0;
        int32 Er = 0;
        uint8 i = 0;
        while (i < imgInfo.MidLineCount)
        {
            if (MidLine[i][0])
            {
                Sum += ErrorBuffer[120 - MidLine[i][0]] * MidLine[i][1];
                ESum += ErrorBuffer[120 - MidLine[i][0]];
            }
            i++;
        }
        if (Sum)
        {
            Out = Sum / ESum;
            Er = MiddleValue - Out;
            CircleMean[CircleMeanCount] = Er;
            return Out;
        }
        else return MiddleValue;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       丢线保护
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void LoseLine(void)
{
    if ((imgInfo.MidLineCount < 45 || Thro < ThroLim) && imgInfo.nextroadType != NoHead && ThreeForkFlag != 1 && CircleFlag != 6 && CircleFlag != 7
        && CircleFlag != 16 && CircleFlag != 17 && LeftTFlag < 5 && RightTFlag < 5)
    {
        StopFlag++;
        if (StopFlag > 30)
        {
            imgInfo.StopFlag = 1;
            stop_time = 200;
        }
    }
    else
    {
        if (StopFlag > 1) StopFlag -= 1;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       坡道扫线
// @param       无
// @return      void
// Sample usage:            Ramp(void);
//-------------------------------------------------------------------------------------------------------------------
void ramp(void)
{
    turn();
    if (imgInfo.MiddleLineCount > 50)
    {
        imgInfo.MiddleLineCount = 45 + extend(MiddleLine[0], MiddleLine[45]);
    }
}

#pragma section all restore
