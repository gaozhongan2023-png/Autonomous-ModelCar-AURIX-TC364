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
uint8 StopFlag = 0;
uint8 BadCrossFlag = 0;
uint8 GoodCrossFlag = 0;
uint8 CircleFlag = 0;
uint8 GarageFlag = 0;
uint8 GarageCount = 0;
uint8 ThreeForkCount = 0;
uint16 ThreeForkTime = 0;
uint16 BadCrossTime = 0;
uint16 ChangeCount = 0;

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
uint8 LeftCloseCount = 0;
uint8 RightCloseCount = 0;
uint8 TurnFlag = 0;
uint8 LeftMax = 0;
uint8 RightMax = 0;

float LeftCos = 2, RightCos = 2;

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

uint8 WidthBuffer[120] =
{
    0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B,     //10
    0x1B, 0x1D, 0x1D, 0x1F, 0x21, 0x21, 0x23, 0x24, 0x25, 0x27,     //20
    0x27, 0x29, 0x29, 0x2B, 0x2B, 0x2D, 0x2E, 0x2F, 0x31, 0x31,     //30
    0x33, 0x34, 0x35, 0x36, 0x37, 0x39, 0x39, 0x3B, 0x3B, 0x3D,     //40
    0x3E, 0x3F, 0x41, 0x41, 0x43, 0x43, 0x45, 0x46, 0x47, 0x48,     //50
    0x49, 0x4A, 0x4B, 0x4D, 0x4D, 0x4F, 0x4F, 0x51, 0x51, 0x53,     //60
    0x54, 0x55, 0x56, 0x57, 0x59, 0x59, 0x5B, 0x5B, 0x5D, 0x5D,     //70
    0x5F, 0x60, 0x60, 0x62, 0x62, 0x64, 0x65, 0x66, 0x67, 0x68,     //80
    0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x71, 0x72, 0x73,     //90
    0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7B, 0x7B, 0x7D, 0x7D,     //100
    0x7F, 0x80, 0x81, 0x81, 0x83, 0x83, 0x84, 0x85, 0x86, 0x87,     //110
    0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8C, 0x8E, 0x8E, 0x8E, 0x8E,     //120
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

uint8 PlanNum = 0;

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
// @param       无
// @return      void
// Sample usage:            NearScan();//寻找有效直道
//-------------------------------------------------------------------------------------------------------------------
uint8 FindNext(uint8* LastPoint, uint8 NearNum, uint8 dir)
{
    uint8 i = NearNum;
    uint8 count = 0;
    if (!dir)
    {
        while (count < 8)
        {
            if (i == 0)
            {
                if (imageBin[*LastPoint + NearBuffer[0][0]][*(LastPoint + 1) + NearBuffer[0][1]] && !imageBin[*LastPoint + NearBuffer[7][0]][*(LastPoint + 1) + NearBuffer[7][1]])
                {
                    *(LastPoint + 2) = *LastPoint + NearBuffer[0][0];
                    *(LastPoint + 3) = *(LastPoint + 1) + NearBuffer[0][1];
                    return 0;
                }
                count++;
                i++;
            }
            else
            {
                if (imageBin[*LastPoint + NearBuffer[i][0]][*(LastPoint + 1) + NearBuffer[i][1]] && !imageBin[*LastPoint + NearBuffer[i - 1][0]][*(LastPoint + 1) + NearBuffer[i - 1][1]])
                {
                    *(LastPoint + 2) = *LastPoint + NearBuffer[i][0];
                    *(LastPoint + 3) = *(LastPoint + 1) + NearBuffer[i][1];
                    return i;
                }
                count++;
                i++;
                if (i == 8) i = 0;
            }
        }
    }
    else
    {
        while (count < 8)
        {
            if (i == 7)
            {
                if (imageBin[*LastPoint + NearBuffer[7][0]][*(LastPoint + 1) + NearBuffer[7][1]] && !imageBin[*LastPoint + NearBuffer[0][0]][*(LastPoint + 1) + NearBuffer[0][1]])
                {
                    *(LastPoint + 2) = *LastPoint + NearBuffer[7][0];
                    *(LastPoint + 3) = *(LastPoint + 1) + NearBuffer[7][1];
                    return 0;
                }
                count++;
                i--;
            }
            else
            {
                if (imageBin[*LastPoint + NearBuffer[i][0]][*(LastPoint + 1) + NearBuffer[i][1]] && !imageBin[*LastPoint + NearBuffer[i + 1][0]][*(LastPoint + 1) + NearBuffer[i + 1][1]])
                {
                    *(LastPoint + 2) = *LastPoint + NearBuffer[i][0];
                    *(LastPoint + 3) = *(LastPoint + 1) + NearBuffer[i][1];
                    return i;
                }
                count++;
                if (i == 0) i = 8;
                i--;
            }
        }
    }
    return 99;
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

    for (i = 115; i > 100; i--)
    {
        if (!imageBin[i][3]) break;
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

    for (i = 115; i > 100; i--)
    {
        if (!imageBin[i][184]) break;
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
        NearNum = 2;

        //右线邻域扫线
        while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 200)
        {
            NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

            Count++;
        }
        imgInfo.RightLineSum = Count - 1;
    }

    //左侧拐点补线
    Count = 0;
    if (flag1) Count = FindTurn2(LeftLine[0], 10, 4, 0);
    if (Count)
    {
        LeftTurn[0][0] = LeftLine[Count][0];
        LeftTurn[0][1] = LeftLine[Count][1];
        LeftTurn[0][2] = Count;
        imgInfo.LeftLineCount = Count;

        GetLeftCos();

        if (LeftCos > 0.6 && LeftCos < 1)
        {
            RepairFlag[0] = 2;
        }
        else
        {
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

        GetRightCos();

        if (RightCos > 0.6 && RightCos < 1)
        {
            RepairFlag[1] = 2;
        }
        else
        {
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

    if (imgInfo.MiddleLineCount > 30 && (imgInfo.nextroadType != NoHead || imgInfo.MiddleLineCount > 80))
    {
        imgInfo.roadType = Straight;
        imgInfo.nextroadType = Straight;
    }

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

        if (imgInfo.nextroadType == Crooked && LeftK[1] > -0.8 && RightK[1] > 0.2 && LeftK[1] < -0.2 && RightK[1] < 0.8) imgInfo.roadType = Crooked;
        else if (imgInfo.MiddleLineCount > 5) imgInfo.roadType = ToBeDone;
        else imgInfo.roadType = Stop;
        if (NoHeadFlag >= 2 && imgInfo.MiddleLineCount > 30)
        {
            NoHeadFlag = 0;
            RoadTypeSum[NoHead] = 0;
            imgInfo.nextroadType = Straight;
            RoadTypeCount = 0;
        }
        if (ThreeForkFlag == 2) ThreeForkFlag = 0;
    }
    else
    {
        if (ThreeForkTime > 20)
        {
            if (!ThreeForkCount) ThreeForkCount = 1;
            else ThreeForkCount = 0;
            //            if (ThreeForkCount >= 2)
            //            {
            //                ThreeForkCount = 0;
            //                if (imgInfo.ThreeForkDir) imgInfo.ThreeForkDir = 0;
            //                else if (imgInfo.ThreeForkDir != 1) imgInfo.ThreeForkDir = 1;
            //            }

            imgInfo.ThreeForkDir = 1;
        }

        ThreeForkTime = 0;
        BadCrossTime = 0;

        if (NoHeadFlag == 3) NoHeadFlag = 0;

        if (imgInfo.MiddleLineCount > 80)
        {
            NoHeadFlag = 0;
            //CircleFlag = 0;
            //RightTFlag = 0;
            CrossFlag = 0;
            BadCrossFlag = 0;
            ThreeForkFlag = 0;
            //LeftTFlag = 0;
            //imgInfo.roadType = Straight;
            //imgInfo.nextroadType = Straight;
        }
        //else if (imgInfo.nextroadType != Crooked)
        //{
        //    imgInfo.roadType = Straight;
        //    imgInfo.nextroadType = Straight;
        //}

//        if (CircleFlag == 8 || CircleFlag == 18)
//        {
//            CircleFlag = 0;
//        }

        if (RightTFlag == 6 || RightTFlag == 7) RightTFlag = 0;
        if (LeftTFlag == 6 || LeftTFlag == 7) LeftTFlag = 0;
        if (CrossFlag >= 1) CrossFlag = 0;
        if (BadCrossFlag >= 1) BadCrossFlag = 0;
        if (NoHeadFlag == 3) NoHeadFlag = 0;
        if (ThreeForkFlag == 2) ThreeForkFlag = 0;

        //        if (GarageFlag)
        //        {
        //            GarageFlag = 0;
        //            GarageCount++;
        //            //ThreeForkCount++;
        ////            if (imgInfo.ThreeForkDir) imgInfo.ThreeForkDir = 0;
        ////            else if (!imgInfo.ThreeForkDir) imgInfo.ThreeForkDir = 1;
        //        }

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
void RoadJudge(void)
{
    uint8 i = 0;
    uint8 NearNum = 0;
    uint8 Count = 0;
    uint8 flag1, j;

    if (RoadTypeCount > 200)
    {
        while (i < 20)
        {
            if (RoadTypeSum[i] > 10) RoadTypeSum[i] = 10;
            i++;
        }
        RoadTypeCount = 0;
    }

    //Croocked
    if ((LeftLine[imgInfo.LeftLineSum][0] < 5 && LeftLine[imgInfo.LeftLineSum][0] > 0 && RightLine[imgInfo.RightLineSum][0] < 5 && RightLine[imgInfo.RightLineSum][0]> 0 && imgInfo.MiddleLineCount < 100))
    {
        flag1 = 0;
        j = LeftLine[0][1];
        for (i = 1; i < 60; i++) if (imageBin[i][j]) flag1 = 1;
        j = RightLine[0][1];
        for (i = 1; i < 60; i++) if (imageBin[i][j]) flag1 = 1;

        if (!flag1)
        {
            flag1 = 1;

            int16 sum = 0;
            for (i = 0; i < imgInfo.LeftLineSum; i++)
            {
                sum += LeftLine[i][1];
            }
            sum /= i;
            Num[0] = (sum - LeftLine[0][1]) * 2 + LeftLine[0][1];

            if (Num[0] - LeftLine[imgInfo.LeftLineSum][1] < -20 || Num[0] - LeftLine[imgInfo.LeftLineSum][1]>20) flag1 = 0;

            for (i = 0; i < imgInfo.RightLineSum; i++)
            {
                sum += RightLine[i][1];
            }
            sum /= i;
            Num[1] = (sum - RightLine[0][1]) * 2 + RightLine[0][1];

            if (Num[1] - RightLine[imgInfo.RightLineSum][1] < -20 || Num[1] - RightLine[imgInfo.RightLineSum][1]>20) flag1 = 0;

            if (flag1)
            {
                RoadTypeSum[Crooked]++;
                RoadTypeCount++;
                imgInfo.nextroadType = Crooked;
                imgInfo.roadType = Crooked;
            }
        }
    }

    if (imgInfo.nextroadType == RightCircle || (CircleFlag && CircleFlag <= 10))
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
        if (RightTFlag < 2 && RightCos>0.65 && RightCos < 1)
        {
            RoadTypeSum[RightCircle]++;
            RoadTypeCount++;
            imgInfo.roadType = RightCircle;
            imgInfo.nextroadType = RightCircle;
            RightTFlag = 0;
        }
        else
        {
            RoadTypeSum[RightT]++;
            RoadTypeCount++;
            imgInfo.roadType = RightT;
            imgInfo.nextroadType = RightT;
            imgInfo.NoHeadDir = 1;
        }
    }
    else if (LeftTFlag || (GarageFlag && imgInfo.nextroadType == LeftT))
    {
        if (LeftTFlag < 2 && LeftCos>0.65 && LeftCos < 1)
        {
            RoadTypeSum[LeftCircle]++;
            RoadTypeCount++;
            imgInfo.roadType = LeftCircle;
            imgInfo.nextroadType = LeftCircle;
            LeftTFlag = 0;
        }
        else
        {
            RoadTypeSum[LeftT]++;
            RoadTypeCount++;
            imgInfo.roadType = LeftT;
            imgInfo.nextroadType = LeftT;
            imgInfo.NoHeadDir = 0;
        }
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
    else if (imgInfo.nextroadType == ThreeFork)
    {
        RoadTypeSum[ThreeFork]++;
        RoadTypeSum[TurnRight] = 0;
        RoadTypeSum[TurnLeft] = 0;
        RoadTypeCount++;
        //imgInfo.roadType = ThreeFork;
        imgInfo.nextroadType = ThreeFork;
    }
    else if (!ThreeForkCount)
    {
        if (LeftTurn[0][0] - RightTurn[0][0] > -15 && LeftTurn[0][0] - RightTurn[0][0] < 15 && ((RepairFlag[0] == 1 && RepairFlag[1] == 1) || (RightCos < 0.5 && RightCos > 0.25 && LeftCos < 0.55 && LeftCos > 0.25)))
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

        if (LeftCos > 0.6 && LeftCos < 0.95 && LeftTurn[0][1] < 94 && RightMax < 20 && RightMax)
        {
            RoadTypeSum[LeftCircle]++;
            RoadTypeCount++;
            imgInfo.roadType = LeftCircle;
            imgInfo.nextroadType = LeftCircle;
        }
        else if (RightCos > 0.6 && RightCos < 0.95 && RightTurn[0][1]>94 && LeftMax < 20 && LeftMax)
        {
            RoadTypeSum[RightCircle]++;
            RoadTypeCount++;
            imgInfo.roadType = RightCircle;
            imgInfo.nextroadType = RightCircle;
        }
        else if (LeftCos < 0.18 && LeftCos > -0.3 && RightCos < 0.18 && RightCos > -0.3)
        {
            RoadTypeSum[ThreeFork]++;
            RoadTypeCount++;
            imgInfo.roadType = ThreeFork;
            imgInfo.nextroadType = ThreeFork;
        }
        else if ((LeftCos < 0.18 && LeftCos > -0.3 && imgInfo.RightLineCount < 30 && imgInfo.LeftLineCount - imgInfo.RightLineCount > 20) || (RightCos < 0.18 && RightCos > -0.3 && imgInfo.LeftLineCount < 30 && imgInfo.RightLineCount - imgInfo.LeftLineCount > 20))
        {
            RoadTypeSum[ThreeFork]++;
            RoadTypeCount++;
            imgInfo.roadType = BadThreeFork;
            imgInfo.nextroadType = ThreeFork;
        }
        else
        {
            RoadTypeCount++;
            imgInfo.roadType = Turn;
        }
    }
    else
    {
        if (LeftCos < 0.18 && LeftCos > -0.3 && RightCos < 0.18 && RightCos > -0.3)
        {
            RoadTypeSum[ThreeFork]++;
            RoadTypeCount++;
            imgInfo.roadType = ThreeFork;
            imgInfo.nextroadType = ThreeFork;
        }
        else if ((LeftCos < 0.18 && LeftCos > -0.3 && imgInfo.RightLineCount < 30 && imgInfo.LeftLineCount - imgInfo.RightLineCount > 20) || (RightCos < 0.18 && RightCos > -0.3 && imgInfo.LeftLineCount < 30 && imgInfo.RightLineCount - imgInfo.LeftLineCount > 20))
        {
            RoadTypeSum[ThreeFork]++;
            RoadTypeCount++;
            imgInfo.roadType = BadThreeFork;
            imgInfo.nextroadType = ThreeFork;
        }
        else
        {
            RoadTypeCount++;
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
    if (imgInfo.nextroadType == TurnLeft)
    {
        turn();
    }
    else if (imgInfo.nextroadType == TurnRight)
    {
        turn();
    }
    else if (imgInfo.nextroadType == Straight)
    {
        turn();
        if (LeftLine[imgInfo.LeftLineSum][1] > 140 && RightLine[imgInfo.RightLineSum][1] > 140) imgInfo.nextroadType = TurnRight;
        if (LeftLine[imgInfo.LeftLineSum][1] < 50 && RightLine[imgInfo.RightLineSum][1] < 50) imgInfo.nextroadType = TurnLeft;
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
        rightcircle();
    }
    else if (imgInfo.nextroadType == LeftCircle)
    {
        leftcircle();
    }
    else if (imgInfo.nextroadType == Crooked)
    {
        //                gpio_set(P02_6, 0);
        crooked();
        //        turn();
    }
    //    else if (imgInfo.nextroadType == Straight && CloseFlag == 2)
    //    {
    //        //gpio_set(P02_6, 1);
    //        crooked();
    //    }
    else if (imgInfo.nextroadType == BadCross)
    {
        badcross();
        //        BadCrossTime++;
    }
    else
    {
        turn();
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       中线修正
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void MiddleRepair(void)
{
    uint8 i, Count;
    Count = 0;
    for (i = 0; i < imgInfo.MiddleLineCount - 1; i++)
    {
        if (MiddleLine[i][0] != MiddleLine[i + 1][0] || MiddleLine[i][1] != MiddleLine[i + 1][1])
        {
            MiddleLine[Count][0] = MiddleLine[i][0];
            MiddleLine[Count][1] = MiddleLine[i][1];
            Count++;
        }
    }
    imgInfo.MiddleLineCount = Count;

    Count = 0;
    for (i = 0; i < imgInfo.MiddleLineCount - 2; i++)
    {
        MiddleLine[Count][0] = MiddleLine[i][0];
        MiddleLine[Count][1] = MiddleLine[i][1];
        Count++;

        if ((MiddleLine[i][0] == MiddleLine[i + 2][0] + 1 || MiddleLine[i][0] == MiddleLine[i + 2][0] - 1) && (MiddleLine[i][1] == MiddleLine[i + 2][1] + 1 || MiddleLine[i][1] == MiddleLine[i + 2][1] - 1))
        {
            i++;
        }
    }
    imgInfo.MiddleLineCount = Count;
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       拟合固定纵坐标中线
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void GetMid(void)
{
    memset(MidLine, 0, 512);
    imgInfo.MidLineCount = 0;
    uint8 i, j, Count = 0;
    uint8 Tempnum = 0;
    uint8 gap = 1;
    uint8 col = MiddleLine[0][0];
    MidLine[0][0] = MiddleLine[0][0];
    MidLine[0][1] = MiddleLine[0][1];
    Count++;

    if (imgInfo.MiddleLineCount)
    {
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

        imgInfo.MidLineCount = Count - 1;
        if ((MiddleLine[imgInfo.MiddleLineCount - 1][0] - MidLine[imgInfo.MidLineCount][0] < 2) && (MiddleLine[imgInfo.MiddleLineCount - 1][0] - MidLine[imgInfo.MidLineCount][0] > -2))
        {
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
        }
    }

    MiddleTurn[0][0] = 0;
    MiddleTurn[0][1] = 0;

    i = FindTurn1(MidLine[0], 10, 2, 0);
    if (i)
    {
        MiddleTurn[0][0] = MidLine[i][0];
        MiddleTurn[0][1] = MidLine[i][1];
    }

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
// @brief       转向扫线
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void turnleft(void)
{
    uint8 i, j;
    uint8 Count = 0;//扫线序号
    uint8 NearNum = 2;//邻域序号
    imgInfo.NoHeadDir = 1;

    //for (i = 90; i > 60; i--)
    //{
    //    if (imageBin[i][183])
    //    {
    //        memset(RoadTypeSum, 0, 20);
    //        RoadTypeSum[TurnRight]++;
    //        RoadTypeCount++;
    //        imgInfo.nextroadType = TurnRight;
    //        turnright();
    //        break;
    //    }
    //}

    if (imgInfo.nextroadType == TurnLeft)
    {
        if (!imgInfo.LeftLineCount)
        {
            LeftLine[0][0] = 115;
            LeftLine[0][1] = 3;

            for (i = 115; i > 10; i--)
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

            //左线邻域扫线
            while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 200)
            {
                NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

                Count++;

                if (NearNum == 99) break;
            }

            i = FindTurn1(RightLine[0], 15, 10, 1);
            turnpoint[0][0] = RightLine[i][0];
            turnpoint[0][1] = RightLine[i][1];

            if (RightLine[Count][1] - RightLine[i][1] > 20)
            {
                imgInfo.RightLineCount = i;
            }
            else
            {
                imgInfo.RightLineCount = Count;
            }

            for (i = 0; i < imgInfo.RightLineCount; i++)
            {
                MiddleLine[i][0] = (LeftLine[0][0] + RightLine[i][0]) / 2;
                MiddleLine[i][1] = (LeftLine[0][1] + RightLine[i][1]) / 2;
            }
            imgInfo.MiddleLineCount = imgInfo.RightLineCount;
        }
        else
        {
            i = FindTurn1(RightLine[0], 15, 10, 1);
            turnpoint[0][0] = RightLine[i][0];
            turnpoint[0][1] = RightLine[i][1];

            if (RightLine[imgInfo.RightLineSum][1] - RightLine[i][1] > 20)
            {
                imgInfo.RightLineSum = i;
            }

            //拟合有效直道中线
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
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       转向扫线
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void turnright(void)
{
    uint8 i, j;
    uint8 Count = 0;//扫线序号
    uint8 NearNum = 2;//邻域序号
    imgInfo.NoHeadDir = 0;

    //for (i = 90; i > 60; i--)
    //{
    //    if (imageBin[i][3])
    //    {
    //        memset(RoadTypeSum, 0, 20);
    //        RoadTypeSum[TurnLeft]++;
    //        RoadTypeCount++;
    //        imgInfo.nextroadType = TurnLeft;
    //        break;
    //        turnleft();
    //    }
    //}

    if (imgInfo.nextroadType == TurnRight)
    {
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
            }

            i = FindTurn1(LeftLine[0], 15, 10, 1);
            turnpoint[0][0] = LeftLine[i][0];
            turnpoint[0][1] = LeftLine[i][1];

            if (LeftLine[Count][1] - LeftLine[i][1] < -20)
            {
                imgInfo.LeftLineCount = i;
            }
            else
            {
                imgInfo.LeftLineCount = Count;
            }

            for (i = 0; i < imgInfo.LeftLineCount; i++)
            {
                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[0][0]) / 2;
                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[0][1]) / 2;
            }
            imgInfo.MiddleLineCount = imgInfo.LeftLineCount;
        }
        else
        {
            i = FindTurn1(LeftLine[0], 15, 10, 1);
            turnpoint[0][0] = LeftLine[i][0];
            turnpoint[0][1] = LeftLine[i][1];

            if (LeftLine[imgInfo.LeftLineSum][1] - LeftLine[i][1] < -20)
            {
                imgInfo.LeftLineSum = i;
            }

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
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       转向扫线
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void turnleft1(void)
{
    uint8 i, j;
    uint8 Count = 0;//扫线序号
    uint8 NearNum = 2;//邻域序号

    if (!imgInfo.LeftLineCount)
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

        //右线邻域扫线
        while (RightLine[Count][0] > 3 && RightLine[Count][1] > 4 && RightLine[Count][1] < 184 && Count < 200)
        {
            NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

            Count++;

            if (NearNum == 99) break;
        }
        imgInfo.RightLineCount = Count;

        if (RightLine[0][0] > 100)
        {
            i = FindTurn1(RightLine[0], 15, 10, 1);
            turnpoint[0][0] = RightLine[i][0];
            turnpoint[0][1] = RightLine[i][1];
        }
        else i = 0;

        if ((RightLine[i + 10][1] - RightLine[i][1] > 5 || BadCrossFlag > 5) && i > 15)
        {
            BadCrossFlag++;
            j = extend(RightLine[i - 13], RightLine[i - 3]);
            imgInfo.RightLineCount = i - 3 + j;
            imgInfo.MiddleLineCount = i - 3 + j;
            for (i = 0; i < imgInfo.RightLineCount; i++)
            {
                MiddleLine[i][0] = RightLine[i][0];
                MiddleLine[i][1] = RightLine[i][1] / 2;
            }
        }
        else//正入十字
        {
            if (BadCrossFlag > 5)
            {
                i = 115;
                while (!imageBin[i][3] || !imageBin[i][184])
                {
                    i--;
                }
                while (imageBin[i][3] || imageBin[i][184])
                {
                    i--;
                }//找到十字前端

                if (i > 10)//能找到
                {
                    i -= 10;
                    for (j = 3; j < 184; j++)
                    {
                        if (!imageBin[i][j] && imageBin[i][j + 1] && imageBin[i][j + 2])
                        {
                            TempLine1[0][0] = i;
                            TempLine1[0][1] = j + 1;
                        }
                    }
                    for (j = 184; j > 3; j--)
                    {
                        if (!imageBin[i][j] && imageBin[i][j - 1] && imageBin[i][j - 2])
                        {
                            TempLine2[0][0] = i;
                            TempLine2[0][1] = j - 1;
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
                }
            }
            else
            {
                for (i = 0; i < imgInfo.RightLineCount; i++)
                {
                    MiddleLine[i][0] = (LeftLine[0][0] + RightLine[i][0]) / 2;
                    MiddleLine[i][1] = (LeftLine[0][1] + RightLine[i][1]) / 2;
                }
                imgInfo.MiddleLineCount = imgInfo.RightLineCount;
            }
        }
    }
    else//有左线
    {
        i = FindTurn1(RightLine[0], 10, 2, 1);
        turnpoint[0][0] = RightLine[i][0];
        turnpoint[0][1] = RightLine[i][1];

        if (RightLine[i + 10][1] - RightLine[i][1] > 5 || BadCrossFlag > 5)
        {
            if (i > 15)
            {
                BadCrossFlag++;
                j = extend(RightLine[i - 13], RightLine[i - 3]);
                imgInfo.RightLineCount = i - 3 + j;

                if (imgInfo.LeftLineCount > 15)
                {
                    GoodCrossFlag++;
                    j = extend(LeftLine[imgInfo.LeftLineCount - 13], LeftLine[imgInfo.LeftLineCount - 3]);
                    imgInfo.LeftLineCount = imgInfo.LeftLineCount - 3 + j;

                    //拟合有效直道中线
                    if (imgInfo.LeftLineCount <= imgInfo.RightLineCount)
                    {
                        for (i = 0; i < imgInfo.LeftLineCount; i++)
                        {
                            MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
                            MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
                        }
                        imgInfo.MiddleLineCount = imgInfo.LeftLineCount;
                    }
                    else
                    {
                        for (i = 0; i < imgInfo.RightLineCount; i++)
                        {
                            MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
                            MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
                        }
                        imgInfo.MiddleLineCount = imgInfo.RightLineCount;
                    }
                }
                else
                {
                    if (GoodCrossFlag < 5)
                    {
                        for (i = 0; i < imgInfo.RightLineCount; i++)
                        {
                            MiddleLine[i][0] = RightLine[i][0];
                            MiddleLine[i][1] = RightLine[i][1] / 2;
                        }
                        imgInfo.MiddleLineCount = imgInfo.RightLineCount;
                    }
                    else//经过斜入十字，现正入十字
                    {
                        //跳过十字左右岔路
                        for (i = turnpoint[0][0] - 10; i > 10; i--)
                        {
                            if (!imageBin[i][3] && !imageBin[i][184]) break;
                        }
                        if (i != 10)//能找到
                        {
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
                        }
                    }
                }
            }
        }
        else//找不到十字转折点
        {
            //拟合有效直道中线
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
            imgInfo.top = LeftLine[imgInfo.RightLineSum][0];
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       断头路扫线
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void nohead(void)
{
    uint8 i;
    uint8 NearNum = 0;
    TempLine1[0][1] = 74;

    if (NoHeadK < 0.5 || NoHeadK > 2)
    {
        for (i = 110; i > 0; i--)
        {
            if (!imageBin[i][74] && imageBin[i + 1][74]) TempLine1[0][0] = 1 + i;
        }//?

        if (TempLine1[0][0] > 40)
        {
            for (i = 0; i < 40; i++)
            {
                if (NearNum < 2) NearNum = FindNext(&TempLine1[i][0], NearNum + 6, 0);
                else NearNum = FindNext(&TempLine1[i][0], NearNum - 2, 0);
                if (NearNum == 99) break;
            }
            NoHeadK = (float)(TempLine1[39][0] - TempLine1[0][0]) / 40;
        }
        else
        {
            NoHeadK = 99;
        }
    }
    else
    {
        if (TempLine2[0][0] == 0) TempLine2[0][0] = TempLine1[0][0];

        for (i = 187; i > 1; i--)
        {
            if (!imageBin[TempLine2[0][0]][i] && imageBin[TempLine2[0][0]][i - 1]) TempLine2[0][1] = i - 1;
        }
        for (i = 0; i < 20; i++)
        {
            if (NearNum < 2) NearNum = FindNext(&TempLine2[i][0], NearNum + 6, 0);
            else NearNum = FindNext(&TempLine2[i][0], NearNum - 2, 0);
            if (NearNum == 99) break;
        }
        NoHeadK = (float)(TempLine2[19][0] - TempLine2[0][0]) / 20;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       断头路扫线
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void nohead1(void)
{
    uint8 i, j, Count;
    uint8 NearNum = 0;
    TempLine1[0][1] = 74;
    if (!NoHeadFlag) NoHeadFlag = 1;

    if (!imgInfo.NoHeadDir)
    {
        if (NoHeadFlag == 1)
        {
            for (i = 45; i < 115; i++)
            {
                if (!imageBin[i][60] && imageBin[i + 1][60])
                {
                    NoHeadFlag = 2;
                    break;
                }
            }
        }
        else if (NoHeadFlag == 2)
        {
            NoHeadFlag = 3;
            for (i = 110; i > 60; i--)
            {
                if (imageBin[i][180])
                {
                    NoHeadFlag = 2;
                    break;
                }
            }

            for (i = 20; i < 115; i++)
            {
                turnpoint[1][0] = 0;
                turnpoint[1][1] = 0;
                if (!imageBin[i][3] && imageBin[i + 1][3])
                {
                    turnpoint[1][0] = (i + 120) / 2;
                    turnpoint[1][1] = 3;
                    break;
                }
            }

            if (turnpoint[1][0] > 20)
            {
                turnpoint[0][0] = turnpoint[1][0];
                turnpoint[0][1] = turnpoint[1][1];
            }

            MiddleLine[0][0] = 115;
            MiddleLine[0][1] = MiddleValue;

            imgInfo.MiddleLineCount = repair(MiddleLine[0], turnpoint[0]);
        }
        else if (NoHeadFlag == 3)
        {
            //            NoHeadFlag = 0;
            //            for(i=117;i>80;i--)
            //            {
            //                if(imageBin[i][3]) NoHeadFlag = 3;
            //            }

            for (j = 184; j > 120; j--)
            {
                if (!imageBin[110][j] && imageBin[110][j - 1])
                {
                    RightLine[0][0] = 110;
                    RightLine[0][1] = j - 1;
                    break;
                }
            }

            Count = 0;
            NearNum = 2;

            //右线邻域扫线
            while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 200)
            {
                NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

                Count++;

                if (NearNum == 99) break;
            }

            imgInfo.RightLineCount = Count;

            for (i = 0; i < imgInfo.RightLineCount; i++)
            {
                MiddleLine[i][0] = RightLine[i][0] / 2 + 59;
                MiddleLine[i][1] = RightLine[i][1] / 2;
            }

            imgInfo.MiddleLineCount = imgInfo.RightLineCount;
        }
    }//if (!imgInfo.NoHeadDir)
    else
    {
        if (NoHeadFlag == 1)
        {
            for (i = 45; i < 115; i++)
            {
                if (!imageBin[i][120] && imageBin[i + 1][120])
                {
                    NoHeadFlag = 2;
                    break;
                }
            }
        }
        else if (NoHeadFlag == 2)
        {
            NoHeadFlag = 3;
            for (i = 110; i > 60; i--)
            {
                if (imageBin[i][7])
                {
                    NoHeadFlag = 2;
                    break;
                }
            }

            for (i = 20; i < 115; i++)
            {
                turnpoint[1][0] = 0;
                turnpoint[1][1] = 0;
                if (!imageBin[i][184] && imageBin[i + 1][184])
                {
                    turnpoint[1][0] = (i + 120) / 2;
                    turnpoint[1][1] = 184;
                    break;
                }
            }

            if (turnpoint[1][0] > 20)
            {
                turnpoint[0][0] = turnpoint[1][0];
                turnpoint[0][1] = turnpoint[1][1];
            }

            MiddleLine[0][0] = 115;
            MiddleLine[0][1] = MiddleValue;

            imgInfo.MiddleLineCount = repair(MiddleLine[0], turnpoint[0]);
        }
        else if (NoHeadFlag == 3)
        {
            //            NoHeadFlag = 0;
            //            for(i=117;i>80;i--)
            //            {
            //                if(imageBin[i][3]) NoHeadFlag = 3;
            //            }

            for (j = 3; j < 60; j++)
            {
                if (!imageBin[110][j] && imageBin[110][j + 1])
                {
                    LeftLine[0][0] = 110;
                    LeftLine[0][1] = j + 1;
                    break;
                }
            }

            Count = 0;
            NearNum = 2;

            //左线邻域扫线
            while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 200)
            {
                if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
                else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);

                Count++;

                if (NearNum == 99) break;
            }

            imgInfo.LeftLineCount = Count;

            for (i = 0; i < imgInfo.LeftLineCount; i++)
            {
                MiddleLine[i][0] = LeftLine[i][0] / 2 + 59;
                MiddleLine[i][1] = LeftLine[i][1] / 2 + 94;
            }

            imgInfo.MiddleLineCount = imgInfo.LeftLineCount;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       三叉路扫线
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void threefork(void)
{
    uint8 i, j, Count, NearNum = 2;

    if (ThreeForkFlag == 0)
    {
        ThreeForkFlag = 1;
        if (LeftTurn[0][0] && RightTurn[0][0] && LeftTurn[0][0] < 110 && RightTurn[0][0] < 110) ThreeForkFlag = 0;

        j = LeftTurn[0][1] + 15;
        for (i = 1; i < LeftTurn[0][0]; i++)//?
        {
            if (imageBin[i][j] && !imageBin[i - 1][j])
            {
                TempLine1[0][0] = i;
                TempLine1[0][1] = j;
            }
        }
        for (i = 0; i < 60; i++)
        {
            if (NearNum < 2) NearNum = FindNext(&TempLine1[i][0], NearNum + 6, 0);
            else NearNum = FindNext(&TempLine1[i][0], NearNum - 2, 0);
            if (NearNum == 99) break;

            if (TempLine1[i][0] > turnpoint[1][0])
            {
                turnpoint[1][0] = TempLine1[i][0];
                turnpoint[1][1] = TempLine1[i][1];
            }
        }

        j = RightTurn[0][1] - 15;
        for (i = 1; i < RightTurn[0][0]; i++)//?
        {
            if (imageBin[i][j] && !imageBin[i - 1][j])
            {
                TempLine2[0][0] = i;
                TempLine2[0][1] = j;
            }
        }
        for (i = 0; i < 60; i++)
        {
            NearNum = FindNext(&TempLine2[i][0], NearNum + 2, 1);
            if (NearNum == 99) break;

            if (TempLine2[i][0] > turnpoint[2][0])
            {
                turnpoint[2][0] = TempLine2[i][0];
                turnpoint[2][1] = TempLine2[i][1];
            }
        }

        if (turnpoint[2][0] > turnpoint[1][0])
        {
            turnpoint[0][0] = turnpoint[2][0];
            turnpoint[0][1] = turnpoint[2][1];
        }
        else
        {
            turnpoint[0][0] = turnpoint[1][0];
            turnpoint[0][1] = turnpoint[1][1];
        }

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
            for (i = 0; i < imgInfo.LeftLineCount; i++)
            {
                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
            }
            for (i = imgInfo.LeftLineCount; i < imgInfo.RightLineCount; i++)
            {
                MiddleLine[i][0] = (LeftLine[imgInfo.LeftLineCount - 1][0] + RightLine[i][0]) / 2;
                MiddleLine[i][1] = (LeftLine[imgInfo.LeftLineCount - 1][1] + RightLine[i][1]) / 2;
            }
            imgInfo.MiddleLineCount = imgInfo.RightLineCount;
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
                MiddleLine[i][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineCount - 1][0]) / 2;
                MiddleLine[i][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineCount - 1][1]) / 2;
            }
            imgInfo.MiddleLineCount = imgInfo.LeftLineCount;
        }
        memset(turnpoint, 0, 6);
    }
    else if (ThreeForkFlag == 1)
    {
        if (!imgInfo.ThreeForkDir)
        {
            uint8 EndCount = 0;
            ThreeForkFlag = 2;
            for (i = 90; i > 60; i--)
            {
                if (imageBin[i][184])
                {
                    ThreeForkFlag = 1;
                }
            }

            memset(TempLine1, 0, 200);

            for (i = 110; i > 30; i--)
            {
                if (imageBin[i][183] && !imageBin[i - 1][183] && !imageBin[i - 2][183])
                {
                    TempLine1[0][0] = i;
                    TempLine1[0][1] = 183;
                    break;
                }
            }

            Count = 0;
            NearNum = 2;
            while (TempLine1[Count][0] > 3 && TempLine1[Count][1] > 3 && TempLine1[Count][1] < 184 && Count < 200)
            {
                if (NearNum == 7 || NearNum == 6) NearNum = FindNext(&TempLine1[Count][0], NearNum - 6, 1);
                else NearNum = FindNext(&TempLine1[Count][0], NearNum + 2, 1);

                if (NearNum == 7) EndCount = 0;
                else if (NearNum == 1) EndCount++;
                else if (NearNum == 2) EndCount += 2;
                else if (NearNum == 99) break;

                Count++;
                if (EndCount > 5) break;
            }

            memset(turnpoint, 0, 6);
            for (i = 0; i < Count; i++)
            {
                if (TempLine1[i][0] >= turnpoint[0][0])
                {
                    turnpoint[0][0] = TempLine1[i][0];
                    turnpoint[0][1] = TempLine1[i][1];
                }
            }

            if (turnpoint[0][0])
            {
                RightLine[0][0] = 117;
                RightLine[0][1] = 183;
                imgInfo.RightLineCount = repair(RightLine[0], turnpoint[0]);

                Count = imgInfo.RightLineCount;
                NearNum = 2;
                while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 200)
                {
                    if (NearNum == 7 || NearNum == 6) NearNum = FindNext(&RightLine[Count][0], NearNum - 6, 1);
                    else NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

                    if (NearNum == 99) break;
                    Count++;
                }
                imgInfo.RightLineSum = Count;
            }

            LeftLine[0][0] = 0;
            LeftLine[0][1] = 0;

            for (i = 117; i > 100; i--)
            {
                if (!imageBin[i][3] && !imageBin[i][4])
                {
                    for (j = 3; j < 100; j++)
                    {
                        if (!imageBin[i][j] && imageBin[i][j + 1] && imageBin[i][j + 2])
                        {
                            LeftLine[0][0] = i;
                            LeftLine[0][1] = j + 1;
                            break;
                        }
                    }
                }
                if (LeftLine[0][0]) break;
            }

            if (LeftLine[0][0])
            {
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
            }
            else
            {
                LeftLine[0][0] = 117;
                LeftLine[0][1] = 4;
            }

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
        else//if (!imgInfo.ThreeForkDir)
        {
            uint8 EndCount = 0;
            ThreeForkFlag = 2;
            for (i = 90; i > 60; i--)
            {
                if (imageBin[i][3])
                {
                    ThreeForkFlag = 1;
                }
            }

            memset(TempLine1, 0, 200);

            for (i = 110; i > 30; i--)
            {
                if (imageBin[i][4] && !imageBin[i - 1][4] && !imageBin[i - 2][4])
                {
                    TempLine1[0][0] = i;
                    TempLine1[0][1] = 4;
                    break;
                }
            }

            Count = 0;
            NearNum = 2;
            while (TempLine1[Count][0] > 3 && TempLine1[Count][1] > 3 && TempLine1[Count][1] < 184 && Count < 200)
            {
                if (NearNum < 2) NearNum = FindNext(&TempLine1[Count][0], NearNum + 6, 0);
                else NearNum = FindNext(&TempLine1[Count][0], NearNum - 2, 0);

                if (NearNum == 5) EndCount = 0;
                else if (NearNum == 3) EndCount++;
                else if (NearNum == 2) EndCount += 2;
                else if (NearNum == 99) break;

                Count++;
                if (EndCount > 5) break;
            }

            memset(turnpoint, 0, 6);
            for (i = 0; i < Count; i++)
            {
                if (TempLine1[i][0] >= turnpoint[0][0])
                {
                    turnpoint[0][0] = TempLine1[i][0];
                    turnpoint[0][1] = TempLine1[i][1];
                }
            }

            if (turnpoint[0][0])
            {
                LeftLine[0][0] = 117;
                LeftLine[0][1] = 4;
                imgInfo.LeftLineCount = repair(LeftLine[0], turnpoint[0]);

                Count = imgInfo.LeftLineCount;
                NearNum = 2;
                while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 200)
                {
                    if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
                    else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);

                    if (NearNum == 99) break;
                    Count++;
                }
                imgInfo.LeftLineSum = Count;
            }

            RightLine[0][0] = 0;
            RightLine[0][1] = 0;

            for (i = 117; i > 100; i--)
            {
                if (!imageBin[i][184] && !imageBin[i][183])
                {
                    for (j = 184; j > 100; j--)
                    {
                        if (!imageBin[i][j] && imageBin[i][j - 1] && imageBin[i][j - 2])
                        {
                            RightLine[0][0] = i;
                            RightLine[0][1] = j - 1;
                            break;
                        }
                    }
                }
                if (RightLine[0][0]) break;
            }

            if (RightLine[0][0])
            {
                Count = 0;
                NearNum = 2;
                while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 200)
                {
                    NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);
                    if (NearNum == 99) break;
                    Count++;
                }
                imgInfo.RightLineSum = Count;
            }
            else
            {
                RightLine[0][0] = 117;
                RightLine[0][1] = 183;
            }

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
    else if (ThreeForkFlag == 2)
    {
        //        ThreeForkFlag = 3;
        //        for (i = 100; i > 80; i--)
        //        {
        //            if (imageBin[i][3] || imageBin[i][184])
        //            {
        //                ThreeForkFlag = 2;
        //                break;
        //            }
        //        }

        if (!imgInfo.ThreeForkDir)
        {
            memset(turnpoint, 0, 6);
            for (j = 183; j > 60; j--)
            {
                if (!imageBin[90][j] && imageBin[90][j - 1] && imageBin[90][j - 2])
                {
                    turnpoint[0][0] = 90;
                    turnpoint[0][1] = j - 1;
                    break;
                }
            }

            if (turnpoint[0][0])
            {
                RightLine[0][0] = 117;
                RightLine[0][1] = 183;
                imgInfo.RightLineCount = repair(RightLine[0], turnpoint[0]);

                Count = imgInfo.RightLineCount;
                NearNum = 2;
                while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 200)
                {
                    NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

                    if (NearNum == 99) break;
                    Count++;
                }
                imgInfo.RightLineSum = Count;

                for (i = 0; i < imgInfo.RightLineSum; i++)
                {
                    MiddleLine[i][0] = RightLine[i][0];
                    MiddleLine[i][1] = RightLine[i][1] / 2;
                    imgInfo.MiddleLineCount = Count;
                }
            }
        }
        else// if (!imgInfo.ThreeForkDir)
        {
            memset(turnpoint, 0, 6);
            for (j = 3; j < 160; j++)
            {
                if (!imageBin[90][j] && imageBin[90][j + 1] && imageBin[90][j + 2])
                {
                    turnpoint[0][0] = 90;
                    turnpoint[0][1] = j + 1;
                    break;
                }
            }

            if (turnpoint[0][0])
            {
                LeftLine[0][0] = 117;
                LeftLine[0][1] = 4;
                imgInfo.LeftLineCount = repair(LeftLine[0], turnpoint[0]);

                Count = imgInfo.LeftLineCount;
                NearNum = 2;
                while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 200)
                {
                    if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
                    else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);

                    if (NearNum == 99) break;
                    Count++;
                }
                imgInfo.LeftLineSum = Count;

                for (i = 0; i < imgInfo.LeftLineSum; i++)
                {
                    MiddleLine[i][0] = LeftLine[i][0];
                    MiddleLine[i][1] = LeftLine[i][1] / 2 + 94;
                    imgInfo.MiddleLineCount = Count;
                }
            }
        }
    }
    //    else if (ThreeForkFlag == 3)
    //    {
    //        for (i = 117; i > 80; i--)
    //        {
    //            if (!imageBin[i][3] && !imageBin[i][184])
    //            {
    //                ThreeForkFlag = 4;
    //                break;
    //            }
    //        }
    //    }
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
        //        ThreeForkTime = 0;
        if (imgInfo.roadType == BadThreeFork)
        {
            if (imgInfo.LeftLineCount > imgInfo.RightLineCount)
            {
                imgInfo.LeftLineCount += extend(LeftLine[imgInfo.LeftLineCount - 15], LeftLine[imgInfo.LeftLineCount - 2]);
                for (i = 0; i < imgInfo.LeftLineCount; i++)
                {
                    MiddleLine[i][0] = LeftLine[i][0];
                    MiddleLine[i][1] = LeftLine[i][1] / 2 + 94;
                }
                imgInfo.MiddleLineCount = imgInfo.LeftLineCount;
            }
            else
            {
                imgInfo.RightLineCount += extend(RightLine[imgInfo.RightLineCount - 15], RightLine[imgInfo.RightLineCount - 2]);
                for (i = 0; i < imgInfo.RightLineCount; i++)
                {
                    MiddleLine[i][0] = RightLine[i][0];
                    MiddleLine[i][1] = RightLine[i][1] / 2;
                }
                imgInfo.MiddleLineCount = imgInfo.RightLineCount;
            }
        }
        else if (LeftTurn[0][0] && RightTurn[0][0] && LeftTurn[0][0] < 110 && RightTurn[0][0] < 110)
        {
            memset(turnpoint, 0, 6);
            j = LeftTurn[0][1] + 15;
            for (i = 1; i < LeftTurn[0][0]; i++)//?
            {
                if (imageBin[i][j] && !imageBin[i - 1][j])
                {
                    TempLine1[0][0] = i;
                    TempLine1[0][1] = j;
                }
            }
            for (i = 0; i < 60; i++)
            {
                if (NearNum < 2) NearNum = FindNext(&TempLine1[i][0], NearNum + 6, 0);
                else NearNum = FindNext(&TempLine1[i][0], NearNum - 2, 0);
                if (NearNum == 99) break;

                if (TempLine1[i][0] > turnpoint[1][0])
                {
                    turnpoint[1][0] = TempLine1[i][0];
                    turnpoint[1][1] = TempLine1[i][1];
                }
            }

            j = RightTurn[0][1] - 15;
            for (i = 1; i < RightTurn[0][0]; i++)//?
            {
                if (imageBin[i][j] && !imageBin[i - 1][j])
                {
                    TempLine2[0][0] = i;
                    TempLine2[0][1] = j;
                }
            }
            for (i = 0; i < 60; i++)
            {
                NearNum = FindNext(&TempLine2[i][0], NearNum + 2, 1);
                if (NearNum == 99) break;

                if (TempLine2[i][0] > turnpoint[2][0])
                {
                    turnpoint[2][0] = TempLine2[i][0];
                    turnpoint[2][1] = TempLine2[i][1];
                }
            }

            if (turnpoint[2][0] > turnpoint[1][0])
            {
                turnpoint[0][0] = turnpoint[2][0];
                turnpoint[0][1] = turnpoint[2][1];
            }
            else
            {
                turnpoint[0][0] = turnpoint[1][0];
                turnpoint[0][1] = turnpoint[1][1];
            }

            if (turnpoint[0][0] > 40) ThreeForkFlag = 1;

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
                for (i = 0; i < imgInfo.LeftLineCount; i++)
                {
                    MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
                    MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
                }
                for (i = imgInfo.LeftLineCount; i < imgInfo.RightLineCount; i++)
                {
                    MiddleLine[i][0] = (LeftLine[imgInfo.LeftLineCount - 1][0] + RightLine[i][0]) / 2;
                    MiddleLine[i][1] = (LeftLine[imgInfo.LeftLineCount - 1][1] + RightLine[i][1]) / 2;
                }
                imgInfo.MiddleLineCount = imgInfo.RightLineCount;
                //            imgInfo.MiddleLineCount = imgInfo.LeftLineCount;
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
                    MiddleLine[i][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineCount - 1][0]) / 2;
                    MiddleLine[i][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineCount - 1][1]) / 2;
                }
                imgInfo.MiddleLineCount = imgInfo.LeftLineCount;
                //            imgInfo.MiddleLineCount = imgInfo.RightLineCount;
            }
        }
        else if ((!LeftTurn[0][0] && !RightTurn[0][0]) || (LeftTurn[0][0] > 110 && RightTurn[0][0] > 110))
        {
            ThreeForkFlag = 1;
            for (i = 100; i > 70; i--)
            {
                if (!imageBin[i][20] || !imageBin[i][163]) ThreeForkFlag = 0;
            }
        }
        else
        {
            if (!RightLine[0][0])
            {
                RightLine[0][0] = 117;
                RightLine[0][1] = 184;
            }
            else if (!LeftLine[0][0])
            {
                LeftLine[0][0] = 117;
                LeftLine[0][1] = 3;
            }

            if (!imgInfo.LeftLineCount && imgInfo.LeftLineSum) imgInfo.LeftLineCount = imgInfo.LeftLineSum;
            if (!imgInfo.RightLineCount && imgInfo.RightLineSum) imgInfo.RightLineCount = imgInfo.RightLineSum;

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
                imgInfo.MiddleLineCount = imgInfo.RightLineCount;
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
                imgInfo.MiddleLineCount = imgInfo.LeftLineCount;
                imgInfo.top = RightLine[imgInfo.RightLineCount][0];
            }
        }
    }
    else if (ThreeForkFlag == 1)
    {
        ThreeForkTime++;
        ThreeForkFlag = 2;
        if (!imgInfo.ThreeForkDir)
        {
            for (i = 117; i > 60; i--)
            {
                if (imageBin[i][180])
                {
                    ThreeForkFlag = 1;
                    break;
                }
            }
        }
        else
        {
            for (i = 117; i > 60; i--)
            {
                if (imageBin[i][10])
                {
                    ThreeForkFlag = 1;
                    break;
                }
            }
        }
    }
    else if (ThreeForkFlag == 2)
    {
        if (!imgInfo.ThreeForkDir)
        {
            for (j = 184; j > 10; j--)
            {
                if (!imageBin[115][j] && imageBin[115][j - 1])
                {
                    TempLine1[0][0] = 115;
                    TempLine1[0][1] = j - 1;
                    break;
                }
            }

            Count = 0;
            while (Count < 80)
            {
                NearNum = FindNext(&TempLine1[Count][0], NearNum + 2, 1);

                Count++;

                if (NearNum == 99) break;
            }

            imgInfo.RightLineCount = Count;
            imgInfo.MiddleLineCount = Count;

            for (i = 0; i < imgInfo.RightLineCount; i++)
            {
                MiddleLine[i][0] = TempLine1[i][0];
                MiddleLine[i][1] = TempLine1[i][1] - WidthBuffer[TempLine1[i][0]] / 2;
            }
        }
        else//if (!imgInfo.ThreeForkDir)
        {
            for (j = 10; j < 184; j++)
            {
                if (!imageBin[115][j] && imageBin[115][j + 1])
                {
                    TempLine1[0][0] = 115;
                    TempLine1[0][1] = j + 1;
                    break;
                }
            }

            Count = 0;
            while (Count < 80)
            {
                if (NearNum < 2) NearNum = FindNext(&TempLine1[Count][0], NearNum + 6, 0);
                else NearNum = FindNext(&TempLine1[Count][0], NearNum - 2, 0);

                Count++;

                if (NearNum == 99) break;
            }

            imgInfo.LeftLineCount = Count;
            imgInfo.MiddleLineCount = Count;

            for (i = 0; i < imgInfo.LeftLineCount; i++)
            {
                MiddleLine[i][0] = TempLine1[i][0];
                MiddleLine[i][1] = TempLine1[i][1] + WidthBuffer[TempLine1[i][0]] / 2;
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
    if (CrossFlag <= 1)
    {
        if (!CrossFlag)
        {
            for (i = 90; i > 70; i--)
            {
                if (imageBin[i][3] || imageBin[i][184])
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
void leftt(void)
{
    uint8 i, j, NearNum;
    int8 k;
    if (CloseFlag == 1)
    {
        j = LeftTurn[0][1] - 5;
        for (i = LeftTurn[0][0] - 5; i > 0; i--)
        {
            if (imageBin[i][j] && !imageBin[i - 1][j])
            {
                TempLine1[0][0] = i;
                TempLine1[0][1] = j;
                break;
            }
        }

        NearNum = 2;
        for (i = 0; i < 40; i++)
        {
            if (NearNum < 2) NearNum = FindNext(&TempLine1[i][0], NearNum + 6, 0);
            else NearNum = FindNext(&TempLine1[i][0], NearNum - 2, 0);
            if (NearNum == 99) break;
        }

        memset(turnpoint[0], 0, 2);
        k = FindTurn(TempLine1[0], 10, 2, 1) - 2;
        turnpoint[0][0] = TempLine1[k][0];
        turnpoint[0][1] = TempLine1[k][1];

        if (k != -2)
        {
            imgInfo.LeftLineCount += repair(LeftLine[imgInfo.LeftLineCount], TempLine1[k]);
        }

        //拟合有效直道中线
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
        TempLine2[0][0] = LeftLine[0][0];
        TempLine2[0][1] = LeftLine[0][1];
    }
    else
    {
        j = turnpoint[0][1] - 20;
        for (i = turnpoint[0][0] + 15; i > 0; i--)
        {
            if (imageBin[i][j] && !imageBin[i - 1][j])
            {
                TempLine1[0][0] = i;
                TempLine1[0][1] = j;
                break;
            }
        }

        NearNum = 2;
        for (i = 0; i < 40; i++)
        {
            if (NearNum < 2) NearNum = FindNext(&TempLine1[i][0], NearNum + 6, 0);
            else NearNum = FindNext(&TempLine1[i][0], NearNum - 2, 0);
            if (NearNum == 99) break;
        }

        memset(turnpoint[0], 0, 2);
        k = FindTurn(TempLine1[0], 10, 2, 1) - 2;
        if (k != -2)
        {
            turnpoint[0][0] = TempLine1[k][0];
            turnpoint[0][1] = TempLine1[k][1];
            LeftLine[0][0] = TempLine2[0][0];
            LeftLine[0][1] = TempLine2[0][1];
            imgInfo.LeftLineCount += repair(LeftLine[0], TempLine1[k]);
        }

        i = LeftLine[0][0];
        //寻找右起点
        for (j = 184; j > 90; j--)
        {
            if (!imageBin[i][j] && imageBin[i][j - 1] && imageBin[i][j - 2])
            {
                RightLine[0][0] = i;
                RightLine[0][1] = j - 1;
                break;
            }
        }

        i = 0;
        //右线邻域扫线
        while (RightLine[i][0] > 3 && RightLine[i][1] > 3 && RightLine[i][1] < 184 && i < 200)
        {
            NearNum = FindNext(&RightLine[i][0], NearNum + 2, 1);
            if (NearNum == 99) break;
            i++;
        }
        imgInfo.RightLineCount = i;

        //拟合有效直道中线
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
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       左岔路扫线
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void leftt1(void)
{
    uint8 i, j, NearNum, Count = 0;

    if (LeftTurn[0][0] && LeftTurn[0][0] < 110 && LeftTFlag < 2)
    {
        for (i = LeftTurn[0][0] - 5; i > LeftTurn[0][0] - 30; i--)
        {
            NearNum = Count;
            Count = 0;
            for (j = LeftTurn[0][1] + 5; j < LeftTurn[0][1] + WidthBuffer[i] - 5; j++)
            {
                if (imageBin[i][j] && !imageBin[i][j + 1])
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

            if (NearNum > 4)
            {
                LeftTFlag = 10;
                break;
            }
        }
    }

    if (LeftTFlag == 0)
    {
        for (i = 100; i > 50; i--) if (!imageBin[i][4]) LeftTFlag = 1;

        for (i = 0; i < imgInfo.RightLineCount; i++)
        {
            MiddleLine[i][0] = RightLine[i][0];
            MiddleLine[i][1] = RightLine[i][1] - WidthBuffer[RightLine[i][0]] / 2;
        }
        imgInfo.MiddleLineCount = imgInfo.RightLineCount;
    }
    if (LeftTFlag == 1)
    {
        LeftTFlag = 2;
        for (i = 110; i > 80; i--) if (!imageBin[i][4]) LeftTFlag = 1;

        for (i = 0; i < imgInfo.RightLineCount; i++)
        {
            MiddleLine[i][0] = RightLine[i][0];
            MiddleLine[i][1] = RightLine[i][1] - WidthBuffer[RightLine[i][0]] / 2;
        }
        imgInfo.MiddleLineCount = imgInfo.RightLineCount;
    }
    if (LeftTFlag == 2)
    {
        LeftTFlag = 3;
        for (i = 110; i > 80; i--) if (imageBin[i][3]) LeftTFlag = 2;

        NearNum = 3;
        Count = 0;

        for (j = 184; j > 60; j--)
        {
            if (!imageBin[115][j] && imageBin[115][j - 1] && imageBin[115][j - 2])
            {
                RightLine[0][0] = 115;
                RightLine[0][1] = j - 1;
                break;
            }
        }

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
        ChangeCount = 0;
    }
    if (LeftTFlag == 3)
    {
        ChangeCount++;
        //LeftLine[0][0] = 115;
        //LeftLine[0][1] = 3;

        //for (i = 115; i > 10; i--)
        //{
        //    if (!imageBin[i][184]) break;
        //}

        //for (j = 184; j > 60; j--)
        //{
        //    if (!imageBin[i][j] && imageBin[i][j - 1] && imageBin[i][j - 2])
        //    {
        //        RightLine[0][0] = i;
        //        RightLine[0][1] = j - 1;
        //        break;
        //    }
        //}

        //Count = 0;
        //NearNum = 3;

        ////右线邻域扫线
        //while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 200)
        //{
        //    NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

        //    Count++;

        //    if (NearNum == 99) break;
        //}
        //imgInfo.RightLineSum = Count;


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

        turnpoint[0][0] = 0;
        turnpoint[0][1] = 0;

        if (RightLine[imgInfo.RightLineSum - 1][0] && RightLine[imgInfo.RightLineSum - 1][0] < 10 && RightLine[imgInfo.RightLineSum - 1][1] > 40)
        {
            i = RightLine[imgInfo.RightLineSum - 1][0];
            for (j = RightLine[imgInfo.RightLineSum - 1][1]; j > RightLine[imgInfo.RightLineSum - 1][1] - WidthBuffer[i] - 20; j--)
            {
                if (imageBin[i][j] && !imageBin[i][j - 1] && !imageBin[i][j - 2])
                {
                    turnpoint[0][0] = i;
                    turnpoint[0][1] = j;
                    break;
                }
            }
        }

        if (turnpoint[0][0] && ChangeCount < 150)
        {
            LeftTFlag = 0;
            CircleFlag = 12;
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
            MiddleLine[i][0] = RightLine[i][0] / 2 + 60;
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

            if (turnpoint[0][0] && turnpoint[0][0] > 20 && turnpoint[0][1] > 45) LeftTFlag = 5;
        }
    }
    if (LeftTFlag == 5 || LeftTFlag == 6)
    {
        //LeftTFlag = 6;
        //for (i = 115; i > 40; i--)
        //{
        //    if (imageBin[i][184])
        //    {
        //        LeftTFlag = 5;
        //        break;
        //    }
        //}

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

        if (turnpoint[1][0] > 20)
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
        for (j = 184; j > 60; j--)
        {
            if (!imageBin[115][j] && imageBin[115][j - 1] && imageBin[115][j - 2])
            {
                RightLine[0][0] = 115;
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
        imgInfo.RightLineCount = Count;

        for (i = 0; i < imgInfo.RightLineCount; i++)
        {
            MiddleLine[i][0] = RightLine[i][0];
            MiddleLine[i][1] = RightLine[i][1] - WidthBuffer[RightLine[i][0]] / 2;
        }
        imgInfo.MiddleLineCount = imgInfo.RightLineSum;
    }

    if (LeftTFlag == 10 || LeftTFlag == 11)
    {
        if (LeftTFlag == 10)
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

        if (LeftTFlag == 11)
        {
            LeftTFlag = 0;
            for (i = 115; i > 90; i--)
            {
                if (imageBin[i][3])
                {
                    LeftTFlag = 11;
                    break;
                }
            }
        }

        for (j = 184; j > 60; j--)
        {
            if (!imageBin[115][j] && imageBin[115][j - 1] && imageBin[115][j - 2])
            {
                RightLine[0][0] = 115;
                RightLine[0][1] = j - 1;
                break;
            }
        }

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
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       右岔路扫线
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void rightt(void)
{
    uint8 i, j, NearNum;
    int8 k;
    if (CloseFlag == 1)
    {
        j = RightTurn[0][1] + 5;
        for (i = RightTurn[0][0] - 5; i > 0; i--)
        {
            if (imageBin[i][j] && !imageBin[i - 1][j])
            {
                TempLine1[0][0] = i;
                TempLine1[0][1] = j;
                break;
            }
        }

        NearNum = 2;
        for (i = 0; i < 40; i++)
        {
            NearNum = FindNext(&TempLine1[i][0], NearNum + 2, 1);
            if (NearNum == 99) break;
        }

        memset(turnpoint[0], 0, 2);
        k = FindTurn(TempLine1[0], 10, 2, 1) - 2;
        turnpoint[0][0] = TempLine1[k][0];
        turnpoint[0][1] = TempLine1[k][1];

        if (k != -2)
        {
            imgInfo.RightLineCount += repair(RightLine[imgInfo.RightLineCount], TempLine1[k]);
        }

        //拟合有效直道中线
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
        TempLine2[0][0] = RightLine[0][0];
        TempLine2[0][1] = RightLine[0][1];
    }
    else
    {
        j = turnpoint[0][1] + 20;
        for (i = turnpoint[0][0] + 15; i > 0; i--)
        {
            if (imageBin[i][j] && !imageBin[i - 1][j])
            {
                TempLine1[0][0] = i;
                TempLine1[0][1] = j;
                break;
            }
        }

        NearNum = 2;
        for (i = 0; i < 40; i++)
        {
            NearNum = FindNext(&TempLine1[i][0], NearNum + 2, 1);
            if (NearNum == 99) break;
        }

        memset(turnpoint[0], 0, 2);
        k = FindTurn(TempLine1[0], 10, 2, 1) - 2;
        if (k != -2)
        {
            turnpoint[0][0] = TempLine1[k][0];
            turnpoint[0][1] = TempLine1[k][1];
            RightLine[0][0] = TempLine2[0][0];
            RightLine[0][1] = TempLine2[0][1];
            imgInfo.RightLineCount += repair(RightLine[0], TempLine1[k]);
        }

        i = RightLine[0][0];
        //寻找左起点
        for (j = 0; j < 90; j++)
        {
            if (!imageBin[i][j] && imageBin[i][j + 1] && imageBin[i][j + 2])
            {
                LeftLine[0][0] = i;
                LeftLine[0][1] = j + 1;
                break;
            }
        }

        i = 0;
        //左线邻域扫线
        while (LeftLine[i][0] > 3 && LeftLine[i][1] > 3 && LeftLine[i][1] < 184 && i < 200)
        {
            if (NearNum < 2) NearNum = FindNext(&LeftLine[i][0], NearNum + 6, 0);
            else NearNum = FindNext(&LeftLine[i][0], NearNum - 2, 0);
            if (NearNum == 99) break;
            i++;
        }
        imgInfo.LeftLineCount = i;

        //拟合有效直道中线
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
}

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

    if (RightTurn[0][0] && RightTurn[0][0] < 110 && RightTFlag < 2)
    {
        for (i = RightTurn[0][0] - 5; i > RightTurn[0][0] - 30; i--)
        {
            NearNum = Count;
            Count = 0;
            for (j = RightTurn[0][1] - 5; j > RightTurn[0][1] - WidthBuffer[i] + 5; j--)
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
                RightTFlag = 10;
                break;
            }
        }
    }

    if (RightTFlag == 0)
    {
        for (i = 100; i > 50; i--) if (!imageBin[i][184]) RightTFlag = 1;

        for (i = 0; i < imgInfo.LeftLineCount; i++)
        {
            MiddleLine[i][0] = LeftLine[i][0];
            MiddleLine[i][1] = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] / 2;
        }
        imgInfo.MiddleLineCount = imgInfo.LeftLineCount;
    }
    if (RightTFlag == 1)
    {
        RightTFlag = 2;
        for (i = 110; i > 80; i--) if (!imageBin[i][184]) RightTFlag = 1;

        for (i = 0; i < imgInfo.LeftLineCount; i++)
        {
            MiddleLine[i][0] = LeftLine[i][0];
            MiddleLine[i][1] = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] / 2;
        }
        imgInfo.MiddleLineCount = imgInfo.LeftLineCount;
    }
    if (RightTFlag == 2)
    {
        RightTFlag = 3;
        for (i = 110; i > 80; i--) if (imageBin[i][184]) RightTFlag = 2;

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

        ChangeCount = 0;
    }
    if (RightTFlag == 3)
    {
        ChangeCount++;
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
            }

            imgInfo.LeftLineSum = Count;

        }

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
        if (j && LeftLine[imgInfo.LeftLineSum - 1][1] < 180)
        {
            turnpoint[0][0] = LeftLine[i + j][0];
            turnpoint[0][1] = LeftLine[i + j][1] - 1;

            RightTFlag = 4;
            for (i = 0; i < 5; i++) if (!imageBin[turnpoint[0][0] - i][turnpoint[0][1]]) RightTFlag = 3;
        }

        turnpoint[0][0] = 0;
        turnpoint[0][1] = 0;

        if (LeftLine[imgInfo.LeftLineSum - 1][0] && LeftLine[imgInfo.LeftLineSum - 1][0] < 10 && LeftLine[imgInfo.LeftLineSum - 1][1] < 140)
        {
            i = LeftLine[imgInfo.LeftLineSum - 1][0];
            for (j = LeftLine[imgInfo.LeftLineSum - 1][1]; j < LeftLine[imgInfo.LeftLineSum - 1][1] + WidthBuffer[i] + 10; j++)
            {
                if (imageBin[i][j] && !imageBin[i][j + 1] && !imageBin[i][j + 2])
                {
                    turnpoint[0][0] = i;
                    turnpoint[0][1] = j;
                    break;
                }
            }
        }

        if (turnpoint[0][0] && ChangeCount < 150)
        {
            RightTFlag = 0;
            CircleFlag = 2;
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
            if (0 && LeftLine[i][0] > 60 && LeftLine[i][1] < 94) RightTFlag = 5;
            else
            {
                imgInfo.LeftLineSum = extend(LeftLine[i - 20], LeftLine[i - 5]) + i;
                imgInfo.LeftLineSum -= 5;
            }
        }

        for (i = 0; i < imgInfo.LeftLineSum; i++)
        {
            MiddleLine[i][0] = LeftLine[i][0] / 2 + 60;
            MiddleLine[i][1] = LeftLine[i][1] / 2 + 94;
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

            if (turnpoint[0][0] && turnpoint[0][0] > 20 && turnpoint[0][1] < 135) RightTFlag = 5;
        }

    }
    if (RightTFlag == 5 || RightTFlag == 6)
    {
        //        RightTFlag = 6;
        //        for (i = 115; i > 40; i--)
        //        {
        //            if (imageBin[i][3])
        //            {
        //                RightTFlag = 5;
        //                break;
        //            }
        //        }

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




        //            for (i = 20; i < 115; i++)
        //            {
        //                turnpoint[1][0] = 0;
        //                turnpoint[1][1] = 0;
        //                if (!imageBin[i][MiddleValue] && imageBin[i + 1][MiddleValue])
        //                {
        //                    turnpoint[1][0] = (i + 120) / 2;
        //                    turnpoint[1][1] = 3;
        //                    break;
        //                }
        //            }
        //
        //            if (turnpoint[1][0] > 20)
        //            {
        //                turnpoint[0][0] = turnpoint[1][0];
        //                turnpoint[0][1] = turnpoint[1][1];
        //            }
        //
        //            MiddleLine[0][0] = 115;
        //            MiddleLine[0][1] = MiddleValue;
        //
        //            imgInfo.MiddleLineCount = repair(MiddleLine[0], turnpoint[0]);
        //            turn();
    }
    if (RightTFlag == 7)
    {
        for (j = 3; j < 60; j++)
        {
            if (!imageBin[115][j] && imageBin[115][j + 1])
            {
                LeftLine[0][0] = 115;
                LeftLine[0][1] = j + 1;
                break;
            }
        }

        Count = 0;
        NearNum = 2;

        //左线邻域扫线
        while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 200)
        {
            if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
            else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);

            Count++;

            if (NearNum == 99) break;
        }

        imgInfo.LeftLineCount = Count;

        for (i = 0; i < imgInfo.LeftLineCount; i++)
        {
            MiddleLine[i][0] = LeftLine[i][0];
            MiddleLine[i][1] = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] / 2;
        }

        imgInfo.MiddleLineCount = imgInfo.LeftLineCount;
    }


    if (RightTFlag == 10 || RightTFlag == 11)
    {
        if (RightTFlag == 10)
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

        if (RightTFlag == 11)
        {
            RightTFlag = 0;
            for (i = 115; i > 90; i--)
            {
                if (imageBin[i][184])
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
    }

}

//-------------------------------------------------------------------------------------------------------------------
// @brief       右环岛扫线
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void rightcircle(void)
{
    uint8 i, j, Count = 0;
    uint8 NearNum = 2;

    if (CircleFlag == 0 || CircleFlag == 1)
    {
        if (CircleFlag == 0)
        {
            CircleFlag = 1;

            for (i = 115; i > 80; i--)
            {
                if (!imageBin[i][184])
                {
                    CircleFlag = 0;
                    break;
                }
            }
        }
        else if (CircleFlag == 1)
        {
            CircleFlag = 2;
            for (i = 115; i > 60; i--)
            {
                if (imageBin[i][184])
                {
                    CircleFlag = 1;
                    break;
                }
            }
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
            while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 250)
            {
                if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
                else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);

                Count++;

                if (NearNum == 99) break;
            }
            imgInfo.LeftLineSum = Count;
        }

        for (i = 0; i < imgInfo.LeftLineSum; i++)
        {
            MiddleLine[i][0] = LeftLine[i][0];
            MiddleLine[i][1] = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] / 2;
        }
        imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
    }
    if (CircleFlag == 2)
    {
        CircleFlag = 3;

        for (i = 117; i > 70; i--)
        {
            if (!imageBin[i][3])
            {
                CircleFlag = 2;
                break;
            }
        }

        memset(turnpoint[0], 0, 6);

        if (!imgInfo.RightLineSum)
        {
            if (!imageBin[117][183] && !imageBin[117][182])
            {
                for (j = 183; j > 60; j--)
                {
                    if (!imageBin[117][j] && imageBin[117][j - 1] && imageBin[117][j - 2])
                    {
                        RightLine[0][0] = 117;
                        RightLine[0][1] = j - 1;
                        break;
                    }
                }
            }

            if (RightLine[0][0])
            {
                Count = 0;
                NearNum = 2;
                while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 250)
                {
                    NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

                    Count++;

                    if (NearNum == 99) break;
                }

                imgInfo.RightLineSum = Count;
            }
        }

        j = 187;
        for (i = 0; i < imgInfo.RightLineSum; i++) if (j > RightLine[i][1]) j = RightLine[i][1];

        if (imgInfo.RightLineSum && j < 175)
        {
            if (imgInfo.LeftLineSum)
            {
                for (i = 0; i < imgInfo.RightLineSum; i++)
                {
                    if (RightLine[i][1] < RightLine[i + 1][1]) break;
                }

                while (i > 10)
                {
                    if (RightLine[i][1] < RightLine[i - 1][1])
                    {
                        turnpoint[0][0] = RightLine[i][0];
                        turnpoint[0][1] = RightLine[i][1];
                        break;
                    }
                    i--;
                }

                if (turnpoint[0][0])
                {
                    for (Count = 0; Count < imgInfo.LeftLineSum; Count++)
                    {
                        if (LeftLine[Count][0] == turnpoint[0][0])
                        {
                            imgInfo.LeftLineCount = Count;
                            break;
                        }
                    }
                }
                else
                {
                    imgInfo.LeftLineCount = 0;
                    Count = 0;
                }

                while (Count < imgInfo.LeftLineSum)
                {
                    if (turnpoint[0][1])
                    {
                        for (j = LeftLine[Count][1] + 3; j < turnpoint[0][1] - 3; j++)
                        {
                            i = LeftLine[Count][0];
                            if (imageBin[i][j] && !imageBin[i][j + 1])
                            {
                                turnpoint[1][0] = i;
                                turnpoint[1][1] = j;
                                break;
                            }
                        }
                    }
                    else
                    {
                        for (j = LeftLine[Count][1] + 3; j < 130; j++)
                        {
                            i = LeftLine[Count][0];
                            if (imageBin[i][j] && !imageBin[i][j + 1])
                            {
                                turnpoint[1][0] = i;
                                turnpoint[1][1] = j;
                                break;
                            }
                        }
                    }

                    if (turnpoint[1][0]) break;
                    Count++;
                }

                if (turnpoint[1][0] != 0)
                {
                    LeftLine[0][0] = 117;
                    if (RightLine[0][1] > 0x8E) LeftLine[0][1] = RightLine[0][1] - 0x8E;
                    else LeftLine[0][1] = 3;
                    imgInfo.LeftLineCount = repair(LeftLine[0], turnpoint[1]);

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
                }//if(imgInfo.RightLineSum&& j < 175)
                else
                {
                    for (i = 0; i < imgInfo.RightLineSum; i++)
                    {
                        MiddleLine[i][0] = RightLine[i][0];
                        MiddleLine[i][1] = RightLine[i][1] - WidthBuffer[RightLine[i][0]] / 2;
                    }
                    imgInfo.MiddleLineCount = imgInfo.RightLineSum;
                }
            }//if (imgInfo.LeftLineSum)
            else
            {
                for (i = 110; i > 10; i--)
                {
                    if (RightLine[0][1])
                    {
                        for (j = 30; j < RightLine[0][1] - 30; j++)
                        {
                            if (imageBin[i][j] && !imageBin[i][j + 1])
                            {
                                turnpoint[1][0] = i;
                                turnpoint[1][1] = j;
                                break;
                            }
                        }
                    }
                    else
                    {
                        for (j = 30; j < 140; j++)
                        {
                            if (imageBin[i][j] && !imageBin[i][j + 1])
                            {
                                turnpoint[1][0] = i;
                                turnpoint[1][1] = j;
                                break;
                            }
                        }
                    }
                    if (turnpoint[1][0]) break;
                }

                if (turnpoint[1][0])
                {
                    LeftLine[0][0] = 117;
                    if (RightLine[0][1] > 0x8E) LeftLine[0][1] = RightLine[0][1] - 0x8E;
                    else LeftLine[0][1] = 3;
                    imgInfo.LeftLineCount = repair(LeftLine[0], turnpoint[1]);

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
                }//if (turnpoint[1][0])
                else
                {
                    for (i = 0; i < imgInfo.RightLineSum; i++)
                    {
                        MiddleLine[i][0] = RightLine[i][0];
                        MiddleLine[i][1] = RightLine[i][1] - WidthBuffer[RightLine[i][0]] / 2;
                    }
                    imgInfo.MiddleLineCount = imgInfo.RightLineSum;
                }
            }//if (!imgInfo.LeftLineSum)
        }//if (imgInfo.RightLineSum)
        else
        {
            if (!imageBin[117][3] && !imageBin[117][4])
            {
                for (j = 3; j < 160; j++)
                {
                    if (!imageBin[117][j] && imageBin[117][j + 1] && imageBin[117][j + 2])
                    {
                        LeftLine[0][0] = 117;
                        LeftLine[0][1] = j + 1;
                        break;
                    }
                }

                Count = 0;
                NearNum = 2;
                while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 250)
                {
                    if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
                    else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);

                    Count++;

                    if (NearNum == 99) break;
                }
                imgInfo.LeftLineCount = Count;
            }
            else
            {
                LeftLine[0][0] = 117;
                if (RightLine[0][1] > 0x8E) LeftLine[0][1] = RightLine[0][1] - 0x8E;
                else LeftLine[0][1] = 3;
                imgInfo.LeftLineCount = 0;
            }

            if (imgInfo.LeftLineCount)
            {
                for (Count = 10; Count < imgInfo.LeftLineCount; Count++)
                {
                    i = LeftLine[Count][0];
                    for (j = LeftLine[i][1] + 3; j < 155; j++)
                    {
                        if (imageBin[i][j] && !imageBin[i][j + 1])
                        {
                            turnpoint[1][0] = i;
                            turnpoint[1][1] = j;
                            break;
                        }
                    }
                    if (turnpoint[1][0]) break;
                }
            }
            else
            {
                for (i = 110; i > 10; i--)
                {
                    for (j = 30; j < 155; j++)
                    {
                        if (imageBin[i][j] && !imageBin[i][j + 1])
                        {
                            turnpoint[1][0] = i;
                            turnpoint[1][1] = j;
                            break;
                        }
                    }
                    if (turnpoint[1][0]) break;
                }
            }

            if (turnpoint[1][0])
            {
                LeftLine[0][0] = 117;
                if (RightLine[0][1] > 0x8E) LeftLine[0][1] = RightLine[0][1] - 0x8E;
                else LeftLine[0][1] = 3;
                imgInfo.LeftLineCount = repair(LeftLine[0], turnpoint[1]);

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

                for (i = 0; i < imgInfo.LeftLineSum; i++)
                {
                    MiddleLine[i][0] = LeftLine[i][0] / 2 + 58;
                    MiddleLine[i][1] = LeftLine[i][1] / 2 + 92;
                }
                imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
                imgInfo.top = LeftLine[imgInfo.LeftLineSum][0];
            }//if (turnpoint[1][0])
        }//if (!imgInfo.RightLineSum)
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
        if (!imgInfo.RightLineSum)
        {
            if (!imageBin[117][183] && !imageBin[117][182])
            {
                for (j = 183; j > 60; j--)
                {
                    if (!imageBin[117][j] && imageBin[117][j - 1] && imageBin[117][j - 2])
                    {
                        RightLine[0][0] = 117;
                        RightLine[0][1] = j - 1;
                        break;
                    }
                }
            }

            if (RightLine[0][0])
            {
                Count = 0;
                NearNum = 2;
                while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 250)
                {
                    NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

                    Count++;

                    if (NearNum == 99) break;
                }

                imgInfo.RightLineSum = Count;
            }
        }

        if (imgInfo.RightLineSum)
        {
            for (i = 0; i < imgInfo.RightLineSum; i++)
            {
                if (RightLine[i][1] < RightLine[i + 1][1]) break;
            }

            while (i > 10)
            {
                if (RightLine[i][1] < RightLine[i - 1][1])
                {
                    turnpoint[0][0] = RightLine[i][0];
                    turnpoint[0][1] = RightLine[i][1];
                    break;
                }
                i--;
            }

            if (RightLine[i][0] < 110) Count = i - 5;
            else Count = 0;

            while (Count < imgInfo.RightLineSum - 1)
            {
                for (j = RightLine[Count][1] - WidthBuffer[RightLine[Count][0]] / 2; j > 3; j--)
                {
                    i = RightLine[Count][0];
                    if (imageBin[i][j] && !imageBin[i][j - 1])
                    {
                        turnpoint[1][0] = i;
                        turnpoint[1][1] = j;
                        break;
                    }
                }
                if (turnpoint[1][0]) break;
                Count++;
            }

            if (!turnpoint[1][0])
            {
                for (i = RightLine[Count][0]; i > 10; i--)
                {
                    for (j = 160; j > 3; j--)
                    {
                        if (imageBin[i][j] && !imageBin[i][j - 1])
                        {
                            turnpoint[1][0] = i;
                            turnpoint[1][1] = j;
                            break;
                        }
                    }
                    if (turnpoint[1][0]) break;
                }
            }

            if (!turnpoint[1][0])
            {
                for (i = 0; i < imgInfo.RightLineSum; i++)
                {
                    MiddleLine[i][0] = RightLine[i][0];
                    MiddleLine[i][1] = RightLine[i][1] - WidthBuffer[RightLine[i][0]] / 2;
                }

                imgInfo.MiddleLineCount = imgInfo.RightLineSum;
            }
            else
            {
                LeftLine[0][0] = 117;
                if (RightLine[0][1] > 0x8E) LeftLine[0][1] = RightLine[0][1] - 0x8E;
                else LeftLine[0][1] = 3;

                imgInfo.LeftLineCount = repair(LeftLine[0], turnpoint[1]);

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
        else
        {
            for (i = 110; i > 10; i--)
            {
                for (j = 30; j < 140; j++)
                {
                    if (imageBin[i][j] && !imageBin[i][j + 1])
                    {
                        turnpoint[1][0] = i;
                        turnpoint[1][1] = j;
                        break;
                    }
                }
                if (turnpoint[1][0]) break;
            }

            if (turnpoint[1][0] != 0)
            {
                LeftLine[0][0] = 117;
                if (RightLine[0][1] > 0x8E) LeftLine[0][1] = RightLine[0][1] - 0x8E;
                else LeftLine[0][1] = 3;
                imgInfo.LeftLineCount = repair(LeftLine[0], turnpoint[1]);

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

                for (i = 0; i < imgInfo.LeftLineSum; i++)
                {
                    MiddleLine[i][0] = LeftLine[i][0] / 2 + 58;
                    MiddleLine[i][1] = LeftLine[i][1] / 2 + 92;
                }
                imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
                imgInfo.top = LeftLine[imgInfo.LeftLineSum][0];
            }
        }
    }
    if (CircleFlag == 4)
    {
        if (LeftTurn[0][0] > 20)
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

        Count = 0;

        for (i = 117; i > 60; i--)
        {
            if (imageBin[i][3] && imageBin[i][4])
            {
                LeftLine[Count][0] = i;
                LeftLine[Count][1] = 3;
                Count++;
            }
            else break;
        }

        for (j = 3; j < 160; j++)
        {
            if (!imageBin[i][j] && imageBin[i][j + 1] && imageBin[i][j + 2])
            {
                LeftLine[Count][0] = i;
                LeftLine[Count][1] = j + 1;
                break;
            }
        }

        NearNum = 2;
        while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 250)
        {
            if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
            else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);

            Count++;

            if (NearNum == 99) break;
        }

        imgInfo.LeftLineSum = Count;



        if (!imageBin[117][183] && !imageBin[117][182])
        {
            for (j = 183; j > 60; j--)
            {
                if (!imageBin[117][j] && imageBin[117][j - 1] && imageBin[117][j - 2])
                {
                    RightLine[0][0] = 117;
                    RightLine[0][1] = j - 1;
                    break;
                }
            }
        }

        if (RightLine[0][0])
        {
            Count = 0;
            NearNum = 2;
            while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 250)
            {
                NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

                Count++;

                if (NearNum == 99) break;
            }

            imgInfo.RightLineSum = Count;
        }
        else
        {
            RightLine[0][0] = 117;
            RightLine[0][1] = 183;
        }


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
    if (CircleFlag == 5)
    {
        CircleFlag = 6;
        for (i = 119; i > 80; i--)
        {
            if (!imageBin[i][3])
            {
                CircleFlag = 5;
                break;
            }
        }

        if (!imageBin[117][183] && !imageBin[117][182])
        {
            for (j = 183; j > 60; j--)
            {
                if (!imageBin[117][j] && imageBin[117][j - 1] && imageBin[117][j - 2])
                {
                    RightLine[0][0] = 117;
                    RightLine[0][1] = j - 1;
                    break;
                }
            }
        }

        Count = 0;
        NearNum = 2;
        while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 250)
        {
            NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

            Count++;

            if (NearNum == 99) break;
        }

        imgInfo.RightLineSum = Count;

        if (imgInfo.RightLineSum > 20)
        {
            for (i = 0; i < Count; i++)
            {
                MiddleLine[i][0] = RightLine[i][0];
                MiddleLine[i][1] = RightLine[i][1] - WidthBuffer[RightLine[i][0]] / 2;
            }
            imgInfo.MiddleLineCount = Count;
            if (imgInfo.MiddleLineCount > 20)
            {
                imgInfo.MiddleLineCount += extend(MiddleLine[imgInfo.MiddleLineCount - 15], MiddleLine[imgInfo.MiddleLineCount - 2]);
            }
            else imgInfo.MiddleLineCount += extend(MiddleLine[0], MiddleLine[imgInfo.MiddleLineCount - 2]);
        }

        //imgInfo.LeftLineCount += extend(LeftLine[imgInfo.LeftLineCount - 10], LeftLine[imgInfo.LeftLineCount]);

        //if (!imgInfo.RightLineSum)
        //{
        //    RightLine[0][0] = 117;
        //    RightLine[0][1] = 184;
        //}

        //for (i = 0; i < imgInfo.RightLineSum; i++)
        //{
        //    MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
        //    MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
        //}
        //for (i = imgInfo.RightLineSum; i < imgInfo.LeftLineCount; i++)
        //{
        //    MiddleLine[i][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineSum][0]) / 2;
        //    MiddleLine[i][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineSum][1]) / 2;
        //}
        //imgInfo.MiddleLineCount = imgInfo.LeftLineCount;

    }
    if (CircleFlag == 6)
    {
        if (!imageBin[117][183] && !imageBin[117][182])
        {
            for (j = 183; j > 60; j--)
            {
                if (!imageBin[117][j] && imageBin[117][j - 1] && imageBin[117][j - 2])
                {
                    RightLine[0][0] = 117;
                    RightLine[0][1] = j - 1;
                    break;
                }
            }
        }

        Count = 0;
        NearNum = 2;
        while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 250)
        {
            NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

            Count++;

            if (NearNum == 99) break;
        }

        imgInfo.RightLineSum = Count;

        if (imgInfo.RightLineSum > 20)
        {
            for (i = 0; i < Count; i++)
            {
                MiddleLine[i][0] = RightLine[i][0];
                MiddleLine[i][1] = RightLine[i][1] - WidthBuffer[RightLine[i][0]] / 2;
            }
            imgInfo.MiddleLineCount = Count;
            if (imgInfo.MiddleLineCount > 20)
            {
                imgInfo.MiddleLineCount += extend(MiddleLine[imgInfo.MiddleLineCount - 15], MiddleLine[imgInfo.MiddleLineCount - 2]);
            }
            else imgInfo.MiddleLineCount += extend(MiddleLine[0], MiddleLine[imgInfo.MiddleLineCount - 2]);
        }

        if (!LeftLine[0][0])
        {
            Count = 0;
            for (i = 117; i > 40; i--)
            {
                if (!imageBin[i][3]) break;
            }
            if (i > 45)
            {
                for (j = 3; j < 90; j++)
                {
                    if (!imageBin[i][j] && imageBin[i][j + 1] && imageBin[i][j + 2])
                    {
                        LeftLine[Count][0] = i;
                        LeftLine[Count][1] = j + 1;
                        break;
                    }
                }

                //左线邻域扫线
                while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 250)
                {
                    if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
                    else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);

                    Count++;
                }
                imgInfo.LeftLineSum = Count;
            }
        }

        if (LeftLine[imgInfo.LeftLineSum][1] > 20 && LeftLine[imgInfo.LeftLineSum][1] < 120) CircleFlag = 7;
    }
    if (CircleFlag == 7 || CircleFlag == 8)
    {
        if (CircleFlag == 7)
        {
            CircleFlag = 8;
            for (i = 110; i > 75; i--)
            {
                if (!imageBin[i][175])
                {
                    CircleFlag = 7;
                    break;
                }
            }
        }
        else
        {
            CircleFlag = 0;
            for (i = 110; i > 65; i--)
            {
                if (imageBin[i][175])
                {
                    CircleFlag = 8;
                    break;
                }
            }
        }

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

            Count = 0;
            NearNum = 2;
            while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 250)
            {
                if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
                else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);

                Count++;

                if (NearNum == 99) break;
            }

            imgInfo.LeftLineSum = Count;
        }

        for (i = 0; i < imgInfo.LeftLineSum; i++)
        {
            MiddleLine[i][0] = LeftLine[i][0];
            MiddleLine[i][1] = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] / 2;
        }
        imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
    }
}




//-------------------------------------------------------------------------------------------------------------------
// @brief       右环岛扫线
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void rightcircle1(void)
{
    uint8 i, j, Count = 0;
    uint8 NearNum = 2;

    if (CircleFlag == 0 || CircleFlag == 1)
    {
        if (CircleFlag == 0)
        {
            CircleFlag = 1;

            for (i = 115; i > 80; i--)
            {
                if (!imageBin[i][184])
                {
                    CircleFlag = 0;
                    break;
                }
            }
        }
        else if (CircleFlag == 1)
        {
            CircleFlag = 2;
            for (i = 115; i > 60; i--)
            {
                if (imageBin[i][184])
                {
                    CircleFlag = 1;
                    break;
                }
            }
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
            while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 200)
            {
                if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
                else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);

                Count++;

                if (NearNum == 99) break;
            }
            imgInfo.LeftLineSum = Count;
        }

        for (i = 0; i < imgInfo.LeftLineSum; i++)
        {
            MiddleLine[i][0] = LeftLine[i][0];
            MiddleLine[i][1] = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] / 2;
        }
        imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
    }
    else if (CircleFlag == 2)
    {
        CircleFlag = 3;

        for (i = 117; i > 70; i--)
        {
            if (!imageBin[i][3])
            {
                CircleFlag = 2;
                break;
            }
        }

        if (!imgInfo.RightLineSum)
        {
            for (j = 183; j > 60; j--)
            {
                if (!imageBin[117][j] && imageBin[117][j - 1] && imageBin[117][j - 2])
                {
                    RightLine[0][0] = 117;
                    RightLine[0][1] = j - 1;
                    break;
                }
            }

            Count = 0;
            NearNum = 2;
            while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 200)
            {
                NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

                Count++;

                if (NearNum == 99) break;
            }

            imgInfo.RightLineSum = Count;
        }

        for (i = 0; i < imgInfo.RightLineSum; i++)
        {
            MiddleLine[i][0] = RightLine[i][0];
            MiddleLine[i][1] = RightLine[i][1] - WidthBuffer[RightLine[i][0]] / 2;
        }
        imgInfo.MiddleLineCount = imgInfo.RightLineSum;
    }
    else if (CircleFlag == 3)
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

        if (!imgInfo.RightLineSum)
        {
            if (!imageBin[117][183] && !imageBin[117][182])
            {
                for (j = 183; j > 60; j--)
                {
                    if (!imageBin[117][j] && imageBin[117][j - 1] && imageBin[117][j - 2])
                    {
                        RightLine[0][0] = 117;
                        RightLine[0][1] = j - 1;
                        break;
                    }
                }
            }

            if (RightLine[0][0])
            {
                Count = 0;
                NearNum = 2;
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

            }
        }
    }
    else if (CircleFlag == 4)
    {
        if (LeftTurn[0][0] > 20)
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

        Count = 0;

        for (i = 117; i > 60; i--)
        {
            if (imageBin[i][3] && imageBin[i][4])
            {
                LeftLine[Count][0] = i;
                LeftLine[Count][1] = 3;
                Count++;
            }
            else break;
        }

        for (j = 3; j < 160; j++)
        {
            if (!imageBin[i][j] && imageBin[i][j + 1] && imageBin[i][j + 2])
            {
                LeftLine[Count][0] = i;
                LeftLine[Count][1] = j + 1;
                break;
            }
        }

        NearNum = 2;
        while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 200)
        {
            if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
            else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);

            Count++;

            if (NearNum == 99) break;
        }

        imgInfo.LeftLineSum = Count;



        if (!imageBin[117][183] && !imageBin[117][182])
        {
            for (j = 183; j > 60; j--)
            {
                if (!imageBin[117][j] && imageBin[117][j - 1] && imageBin[117][j - 2])
                {
                    RightLine[0][0] = 117;
                    RightLine[0][1] = j - 1;
                    break;
                }
            }
        }

        if (RightLine[0][0])
        {
            Count = 0;
            NearNum = 2;
            while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 200)
            {
                NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

                Count++;

                if (NearNum == 99) break;
            }

            imgInfo.RightLineSum = Count;
        }
        else
        {
            RightLine[0][0] = 117;
            RightLine[0][1] = 183;
        }


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
    else if (CircleFlag == 5)
    {
        //        if (imgInfo.LeftLineCount > 10 && CircleFlag == 4)
        //        {
        //            imgInfo.LeftLineCount += extend2border(LeftLine[imgInfo.LeftLineCount - 10], LeftLine[imgInfo.LeftLineCount]);
        //
        //            if (imgInfo.LeftLineCount <= imgInfo.RightLineSum)
        //            {
        //                for (i = 0; i < imgInfo.LeftLineCount - 1; i++)
        //                {
        //                    MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
        //                    MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
        //                }
        //                for (i = imgInfo.LeftLineCount - 1; i < imgInfo.RightLineSum; i++)
        //                {
        //                    MiddleLine[i][0] = (LeftLine[imgInfo.LeftLineCount - 1][0] + RightLine[i][0]) / 2;
        //                    MiddleLine[i][1] = (LeftLine[imgInfo.LeftLineCount - 1][1] + RightLine[i][1]) / 2;
        //                }
        //                imgInfo.MiddleLineCount = imgInfo.RightLineSum;
        //                imgInfo.top = RightLine[imgInfo.RightLineSum][0];
        //            }
        //            else
        //            {
        //                for (i = 0; i < imgInfo.RightLineSum - 1; i++)
        //                {
        //                    MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
        //                    MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
        //                }
        //                for (i = imgInfo.RightLineSum - 1; i < imgInfo.LeftLineCount; i++)
        //                {
        //                    MiddleLine[i][0] = (LeftLine[i][0] + RightLine[imgInfo.RightLineSum - 1][0]) / 2;
        //                    MiddleLine[i][1] = (LeftLine[i][1] + RightLine[imgInfo.RightLineSum - 1][1]) / 2;
        //                }
        //                imgInfo.MiddleLineCount = imgInfo.LeftLineCount;
        //                imgInfo.top = LeftLine[imgInfo.LeftLineCount][0];
        //            }
        //        }
        //        else CircleFlag = 5;

        CircleFlag = 6;
        for (i = 115; i > 80; i--)
        {
            if (!imageBin[i][3])
            {
                CircleFlag = 5;
                break;
            }
        }

        if (!imgInfo.RightLineSum)
        {
            if (!imageBin[117][183] && !imageBin[117][182])
            {
                for (j = 183; j > 60; j--)
                {
                    if (!imageBin[117][j] && imageBin[117][j - 1] && imageBin[117][j - 2])
                    {
                        RightLine[0][0] = 117;
                        RightLine[0][1] = j - 1;
                        break;
                    }
                }
            }

            Count = 0;
            NearNum = 2;
            while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 200)
            {
                NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

                Count++;

                if (NearNum == 99) break;
            }

            imgInfo.RightLineSum = Count;

        }

        if (imgInfo.RightLineSum)
        {
            for (i = 0; i < imgInfo.RightLineSum; i++)
            {
                MiddleLine[i][0] = RightLine[i][0];
                MiddleLine[i][1] = RightLine[i][1] - WidthBuffer[RightLine[i][0]] / 2 - 5;
            }
            imgInfo.MiddleLineCount = imgInfo.RightLineSum;
        }
        else
        {
            for (i = 0; i < 10; i++)
            {
                MiddleLine[0][0] = 115 - i;
                MiddleLine[0][1] = MiddleValue - 40;
            }
            imgInfo.MiddleLineCount = 5;
        }

    }
    else if (CircleFlag == 6)
    {
        CircleFlag = 7;
        for (i = 100; i > 60; i--)
        {
            if (imageBin[i][3])
            {
                CircleFlag = 6;
                break;
            }
        }

        if (!imageBin[117][183] && !imageBin[117][182])
        {
            for (j = 183; j > 60; j--)
            {
                if (!imageBin[117][j] && imageBin[117][j - 1] && imageBin[117][j - 2])
                {
                    RightLine[0][0] = 117;
                    RightLine[0][1] = j - 1;
                    break;
                }
            }
        }

        Count = 0;
        NearNum = 2;
        while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 200)
        {
            NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

            Count++;

            if (NearNum == 99) break;
        }

        imgInfo.RightLineSum = Count;

        if (imgInfo.RightLineSum)
        {
            for (i = 0; i < Count; i++)
            {
                MiddleLine[i][0] = RightLine[i][0];
                MiddleLine[i][1] = RightLine[i][1] - WidthBuffer[RightLine[i][0]] / 2;
            }
            imgInfo.MiddleLineCount = Count;
            CirCleOutFlag = 0;
        }
        else
        {
            CirCleOutFlag = 1;
        }
    }
    else if (CircleFlag == 7)
    {
        if (!imgInfo.LeftLineSum)
        {
            for (j = 3; j < 120; j++)
            {
                if (!imageBin[115][j] && imageBin[115][j + 1] && imageBin[115][j + 2])
                {
                    LeftLine[0][0] = 115;
                    LeftLine[0][1] = j + 1;
                    break;
                }
            }

            Count = 0;
            NearNum = 2;
            while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 200)
            {
                if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
                else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);

                Count++;

                if (NearNum == 99) break;
            }

            imgInfo.LeftLineSum = Count;
        }

        for (i = 0; i < imgInfo.LeftLineSum; i++)
        {
            MiddleLine[i][0] = LeftLine[i][0];
            MiddleLine[i][1] = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] / 2;
        }
        imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       左环岛扫线
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void leftcircle(void)
{
    uint8 i, j, Count = 0;
    uint8 NearNum = 2;

    if (CircleFlag == 0 || CircleFlag == 11)
    {
        if (CircleFlag == 0)
        {
            CircleFlag = 11;

            for (i = 115; i > 80; i--)
            {
                if (!imageBin[i][3])
                {
                    CircleFlag = 0;
                    break;
                }
            }
        }
        else if (CircleFlag == 11)
        {
            CircleFlag = 12;
            for (i = 115; i > 60; i--)
            {
                if (imageBin[i][3])
                {
                    CircleFlag = 11;
                    break;
                }
            }
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

            NearNum = 2;
            //左线邻域扫线
            while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 250)
            {
                NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

                Count++;

                if (NearNum == 99) break;
            }
            imgInfo.RightLineSum = Count;
        }

        for (i = 0; i < imgInfo.RightLineSum; i++)
        {
            MiddleLine[i][0] = RightLine[i][0];
            MiddleLine[i][1] = RightLine[i][1] - WidthBuffer[RightLine[i][0]] / 2;
        }
        imgInfo.MiddleLineCount = imgInfo.RightLineSum;
    }
    if (CircleFlag == 12)
    {
        CircleFlag = 13;

        for (i = 117; i > 70; i--)
        {
            if (!imageBin[i][184])
            {
                CircleFlag = 12;
                break;
            }
        }

        memset(turnpoint[0], 0, 6);//不懂

        if (!imgInfo.LeftLineSum)
        {
            if (!imageBin[117][4] && !imageBin[117][5])
            {
                for (j = 4; j < 120; j++)
                {
                    if (!imageBin[117][j] && imageBin[117][j + 1] && imageBin[117][j + 2])
                    {
                        LeftLine[0][0] = 117;
                        LeftLine[0][1] = j + 1;
                        break;
                    }
                }
            }

            if (LeftLine[0][0])
            {
                Count = 0;
                NearNum = 2;
                while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 250)
                {
                    if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
                    else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);

                    Count++;

                    if (NearNum == 99) break;
                }

                imgInfo.LeftLineSum = Count;
            }
        }

        j = 0;
        for (i = 0; i < imgInfo.LeftLineSum; i++)
        {
            if (j < LeftLine[i][1])
            {
                j = LeftLine[i][1];
            }
        }

        if (imgInfo.LeftLineSum && j > 22)
        {
            if (imgInfo.RightLineSum)
            {
                for (i = 0; i < imgInfo.LeftLineSum; i++)
                {
                    if (LeftLine[i][1] > LeftLine[i + 1][1]) break;
                }

                while (i > 10)
                {
                    if (LeftLine[i][1] > LeftLine[i - 1][1])
                    {
                        turnpoint[0][0] = LeftLine[i][0];
                        turnpoint[0][1] = LeftLine[i][1];
                        break;
                    }
                    i--;
                }

                if (turnpoint[0][0])
                {
                    for (Count = 0; Count < imgInfo.RightLineSum; Count++)
                    {
                        if (RightLine[Count][0] == turnpoint[0][0])
                        {
                            imgInfo.RightLineCount = Count;
                            break;
                        }
                    }
                }
                else
                {
                    imgInfo.RightLineCount = 0;
                    Count = 0;
                }

                while (Count < imgInfo.RightLineSum)
                {
                    if (turnpoint[0][1])
                    {
                        for (j = RightLine[Count][1] - 3; j > turnpoint[0][1] + 3; j--)
                        {
                            i = RightLine[Count][0];
                            if (imageBin[i][j] && !imageBin[i][j - 1])
                            {
                                turnpoint[1][0] = i;
                                turnpoint[1][1] = j;
                                break;
                            }
                        }
                    }
                    else
                    {
                        for (j = RightLine[Count][1] - 3; j > 50; j--)
                        {
                            i = RightLine[Count][0];
                            if (imageBin[i][j] && !imageBin[i][j - 1])
                            {
                                turnpoint[1][0] = i;
                                turnpoint[1][1] = j;
                                break;
                            }
                        }
                    }

                    if (turnpoint[1][0]) break;
                    Count++;
                }

                if (turnpoint[1][0] != 0)
                {
                    RightLine[0][0] = 117;
                    if (LeftLine[0][1] + 0x8E < 187) RightLine[0][1] = LeftLine[0][1] + 0x8E;
                    else RightLine[0][1] = 187;
                    imgInfo.RightLineCount = repair(RightLine[0], turnpoint[1]);

                    Count = imgInfo.RightLineCount;
                    NearNum = 2;
                    while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 200)
                    {
                        //                        if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
                        //                        else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);
                        NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

                        Count++;

                        if (NearNum == 99) break;
                    }
                    imgInfo.RightLineSum = Count;

                    if (imgInfo.RightLineSum <= imgInfo.LeftLineSum)
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
                    else
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
                }
                else
                {
                    for (i = 0; i < imgInfo.LeftLineSum; i++)
                    {
                        MiddleLine[i][0] = LeftLine[i][0];
                        MiddleLine[i][1] = LeftLine[i][1] + WidthBuffer[RightLine[i][0]] / 2;
                    }
                    imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
                }
            }
            else
            {
                for (i = 110; i > 10; i--)
                {
                    if (LeftLine[0][1])
                    {
                        for (j = 150; j > LeftLine[0][1] + 30; j--)
                        {
                            if (imageBin[i][j] && !imageBin[i][j - 1])
                            {
                                turnpoint[1][0] = i;
                                turnpoint[1][1] = j;
                                break;
                            }
                        }
                    }
                    else
                    {
                        for (j = 150; j > 40; j--)
                        {
                            if (imageBin[i][j] && !imageBin[i][j - 1])
                            {
                                turnpoint[1][0] = i;
                                turnpoint[1][1] = j;
                                break;
                            }
                        }
                    }
                    if (turnpoint[1][0]) break;
                }
                if (turnpoint[1][0])
                {
                    RightLine[0][0] = 117;
                    if (LeftLine[0][1] + 0x8E < 187) RightLine[0][1] = LeftLine[0][1] + 0x8E;
                    else RightLine[0][1] = 187;
                    imgInfo.RightLineCount = repair(RightLine[0], turnpoint[1]);

                    Count = imgInfo.RightLineCount;
                    NearNum = 2;
                    while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 200)
                    {
                        //                        if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
                        //                        else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);
                        NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

                        Count++;

                        if (NearNum == 99) break;
                    }
                    imgInfo.RightLineSum = Count;

                    if (imgInfo.RightLineSum <= imgInfo.LeftLineSum)
                    {
                        for (i = 0; i < imgInfo.LeftLineSum; i++)
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
                    else
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
                }
                else
                {
                    for (i = 0; i < imgInfo.LeftLineSum; i++)
                    {
                        MiddleLine[i][0] = LeftLine[i][0];
                        MiddleLine[i][1] = LeftLine[i][1] + WidthBuffer[RightLine[i][0]] / 2;
                    }
                    imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
                }
            }//if (!imgInfo.LeftLineSum)
        }//if (imgInfo.RightLineSum)
        else
        {
            if (!imageBin[117][184] && !imageBin[117][183])
            {
                for (j = 184; j > 20; j--)
                {
                    if (!imageBin[117][j] && imageBin[117][j - 1] && imageBin[117][j - 2])
                    {
                        LeftLine[0][0] = 117;
                        LeftLine[0][1] = j - 1;
                        break;
                    }
                }

                Count = 0;
                NearNum = 2;
                while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 200)
                {
                    //                    if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
                    //                    else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);
                    NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

                    Count++;

                    if (NearNum == 99) break;
                }
                imgInfo.RightLineCount = Count;
            }
            else
            {
                RightLine[0][0] = 117;
                if (LeftLine[0][1] + 0x8E < 187) RightLine[0][1] = LeftLine[0][1] + 0x8E;
                else RightLine[0][1] = 187;
                imgInfo.RightLineCount = 0;
            }

            if (imgInfo.RightLineCount)
            {
                for (Count = 10; Count < imgInfo.RightLineCount; Count++)
                {
                    i = RightLine[Count][0];
                    for (j = RightLine[i][1] - 3; j > 32; j--)
                    {
                        if (imageBin[i][j] && !imageBin[i][j - 1])
                        {
                            turnpoint[1][0] = i;
                            turnpoint[1][1] = j;
                            break;
                        }
                    }
                    if (turnpoint[1][0]) break;
                }
            }
            else
            {
                for (i = 110; i > 10; i--)
                {
                    for (j = 150; j > 32; j--)
                    {
                        if (imageBin[i][j] && !imageBin[i][j - 1])
                        {
                            turnpoint[1][0] = i;
                            turnpoint[1][1] = j;
                            break;
                        }
                    }
                    if (turnpoint[1][0]) break;
                }
            }

            if (turnpoint[1][0])
            {
                RightLine[0][0] = 117;
                if (LeftLine[0][1] + 0x8E < 187) RightLine[0][1] = LeftLine[0][1] + 0x8E;
                else RightLine[0][1] = 187;
                imgInfo.RightLineCount = repair(RightLine[0], turnpoint[1]);

                Count = imgInfo.RightLineCount;
                NearNum = 3;
                while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 200)
                {
                    //                    if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
                    //                    else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);
                    NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

                    Count++;

                    if (NearNum == 99) break;
                }
                imgInfo.RightLineSum = Count;

                for (i = 0; i < imgInfo.RightLineSum; i++)
                {
                    MiddleLine[i][0] = RightLine[i][0] / 2 + 58;
                    MiddleLine[i][1] = RightLine[i][1] / 2;
                }
                imgInfo.MiddleLineCount = imgInfo.RightLineSum;
                imgInfo.top = RightLine[imgInfo.RightLineSum][0];
            }//if (turnpoint[1][0])
        }//if (!imgInfo.RightLineSum)
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

        memset(turnpoint[0], 0, 6);
        if (!imgInfo.LeftLineSum)
        {
            if (!imageBin[117][4] && !imageBin[117][5])
            {
                for (j = 4; j < 120; j++)
                {
                    if (!imageBin[117][j] && imageBin[117][j + 1] && imageBin[117][j + 2])
                    {
                        LeftLine[0][0] = 117;
                        LeftLine[0][1] = j + 1;
                        break;
                    }
                }
            }

            if (LeftLine[0][0])
            {
                Count = 0;
                NearNum = 2;
                while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 250)
                {
                    if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
                    else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);

                    Count++;

                    if (NearNum == 99) break;
                }

                imgInfo.LeftLineSum = Count;
            }
        }

        if (imgInfo.LeftLineSum)
        {
            for (i = 0; i < imgInfo.LeftLineSum; i++)
            {
                if (LeftLine[i][1] > LeftLine[i + 1][1]) break;
            }

            while (i > 10)
            {
                if (LeftLine[i][1] > LeftLine[i - 1][1])
                {
                    turnpoint[0][0] = LeftLine[i][0];
                    turnpoint[0][1] = LeftLine[i][1];
                    break;
                }
                i--;
            }

            if (LeftLine[i][0] < 110)  Count = i - 5;
            else Count = 0;

            while (Count < imgInfo.LeftLineSum - 1)
            {
                for (j = LeftLine[Count][1] + WidthBuffer[LeftLine[Count][0]] / 2; j < 184; j++)
                {
                    i = LeftLine[Count][0];
                    if (imageBin[i][j] && !imageBin[i][j + 1])
                    {
                        turnpoint[1][0] = i;
                        turnpoint[1][1] = j;
                        break;
                    }
                }
                if (turnpoint[1][0]) break;
                Count++;
            }

            if (!turnpoint[1][0])
            {
                for (i = LeftLine[Count][0]; i > 10; i--)
                {
                    for (j = 20; j < 184; j++)
                    {
                        if (imageBin[i][j] && !imageBin[i][j + 1])
                        {
                            turnpoint[1][0] = i;
                            turnpoint[1][1] = j;
                            break;
                        }
                    }
                    if (turnpoint[1][0]) break;
                }
            }

            if (!turnpoint[1][0])
            {
                for (i = 0; i < imgInfo.LeftLineSum; i++)
                {
                    MiddleLine[i][0] = LeftLine[i][0];
                    MiddleLine[i][1] = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] / 2;
                }

                imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
            }
            else
            {
                RightLine[0][0] = 117;
                if (LeftLine[0][1] + 0x8E < 187) RightLine[0][1] = LeftLine[0][1] + 0x8E;
                else RightLine[0][1] = 187;

                imgInfo.RightLineCount = repair(RightLine[0], turnpoint[1]);

                Count = imgInfo.RightLineCount;
                NearNum = 3;
                while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 250)
                {
                    NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

                    Count++;

                    if (NearNum == 99) break;
                }

                imgInfo.RightLineSum = Count;

                if (imgInfo.RightLineSum <= imgInfo.LeftLineSum)
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
                else
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
            }
        }
        else
        {
            for (i = 110; i > 10; i--)
            {
                for (j = 150; j > 40; j--)
                {
                    if (imageBin[i][j] && !imageBin[i][j - 1])
                    {
                        turnpoint[1][0] = i;
                        turnpoint[1][1] = j;
                        break;
                    }
                }
                if (turnpoint[1][0]) break;
            }

            if (turnpoint[1][0] != 0)
            {
                RightLine[0][0] = 117;
                if (LeftLine[0][1] + 0x8E < 187) RightLine[0][1] = LeftLine[0][1] + 0x8E;
                else RightLine[0][1] = 187;
                imgInfo.RightLineCount = repair(RightLine[0], turnpoint[1]);

                Count = imgInfo.RightLineCount;
                NearNum = 2;
                while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 250)
                {
                    NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

                    Count++;

                    if (NearNum == 99) break;
                }

                imgInfo.RightLineSum = Count;

                for (i = 0; i < imgInfo.RightLineSum; i++)
                {
                    MiddleLine[i][0] = RightLine[i][0] / 2 + 58;
                    MiddleLine[i][1] = RightLine[i][1] / 2;
                }
                imgInfo.MiddleLineCount = imgInfo.RightLineSum;
                imgInfo.top = RightLine[imgInfo.RightLineSum][0];
            }
        }
    }
    if (CircleFlag == 14)
    {
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

        Count = 0;

        for (i = 117; i > 60; i--)//行 从下向上遍历
        {
            if (imageBin[i][184] && imageBin[i][183])   //共187列
            {
                RightLine[Count][0] = i;  //右边起始行和列
                RightLine[Count][1] = 184;
                Count++;
            }
            else break;
        }

        for (j = 184; j > 30; j--)//从右向中间扫
        {
            if (!imageBin[i][j] && imageBin[i][j - 1] && imageBin[i][j - 2])
            {
                RightLine[Count][0] = i;
                RightLine[Count][1] = j - 1; //右边线
                break;
            }
        }

        NearNum = 2;
        while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 200)
        {

            NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

            Count++;

            if (NearNum == 99) break;
        }

        imgInfo.RightLineSum = Count;


        if (!imageBin[117][4] && !imageBin[117][5])
        {
            for (j = 4; j < 120; j++)
            {
                if (!imageBin[117][j] && imageBin[117][j + 1] && imageBin[117][j + 2])
                {
                    LeftLine[0][0] = 117;
                    LeftLine[0][1] = j + 1;
                    break;
                }
            }
        }

        if (LeftLine[0][0])
        {
            Count = 0;
            NearNum = 2;
            while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 200)
            {
                if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
                else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);

                Count++;

                if (NearNum == 99) break;
            }

            imgInfo.LeftLineSum = Count;
        }
        else
        {
            LeftLine[0][0] = 117;
            LeftLine[0][1] = 4;
        }

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
    if (CircleFlag == 15)
    {
        CircleFlag = 16;
        for (i = 115; i > 80; i--)
        {
            if (!imageBin[i][184])
            {
                CircleFlag = 15;
                break;
            }
        }

        Count = 0;
        NearNum = 2;
        while (LeftLine[Count][0] > 10 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 200)
        {
            if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
            else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);

            Count++;

            if (NearNum == 99) break;
        }

        imgInfo.LeftLineSum = Count;

        if (imgInfo.LeftLineSum > 20)
        {
            for (i = 0; i < Count; i++)
            {
                MiddleLine[i][0] = LeftLine[i][0];
                MiddleLine[i][1] = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] / 2;
            }
            imgInfo.MiddleLineCount = Count;
            if (imgInfo.MiddleLineCount > 20)
            {
                imgInfo.MiddleLineCount += extend(MiddleLine[imgInfo.MiddleLineCount - 15], MiddleLine[imgInfo.MiddleLineCount - 2]);
            }
            else imgInfo.MiddleLineCount += extend(MiddleLine[0], MiddleLine[imgInfo.MiddleLineCount - 2]);
        }

        //imgInfo.RightLineCount += extend(RightLine[imgInfo.RightLineCount - 10], RightLine[imgInfo.RightLineCount]);

        //if (!imgInfo.LeftLineSum)
        //{
        //    LeftLine[0][0] = 117;
        //    LeftLine[0][1] = 3;
        //}

        //for (i = 0; i < imgInfo.LeftLineSum; i++)
        //{
        //    MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
        //    MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
        //}
        //for (i = imgInfo.LeftLineSum; i < imgInfo.RightLineCount; i++)
        //{
        //    MiddleLine[i][0] = (LeftLine[imgInfo.LeftLineSum][0] + RightLine[i][0]) / 2;
        //    MiddleLine[i][1] = (LeftLine[imgInfo.LeftLineSum][1] + RightLine[i][1]) / 2;
        //}
        //imgInfo.MiddleLineCount = imgInfo.RightLineCount;
    }
    if (CircleFlag == 16)
    {
        if (!imageBin[117][4] && !imageBin[117][5])
        {
            for (j = 4; j < 120; j++)
            {
                if (!imageBin[117][j] && imageBin[117][j + 1] && imageBin[117][j + 2])
                {
                    LeftLine[0][0] = 117;
                    LeftLine[0][1] = j + 1;
                    break;
                }
            }
        }

        Count = 0;
        NearNum = 2;
        while (LeftLine[Count][0] > 10 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 200)
        {
            if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
            else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);

            Count++;

            if (NearNum == 99) break;
        }

        imgInfo.LeftLineSum = Count;

        if (imgInfo.LeftLineSum > 20)
        {
            for (i = 0; i < Count; i++)
            {
                MiddleLine[i][0] = LeftLine[i][0];
                MiddleLine[i][1] = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] / 2;
            }
            imgInfo.MiddleLineCount = Count;
            if (imgInfo.MiddleLineCount > 20)
            {
                imgInfo.MiddleLineCount += extend(MiddleLine[imgInfo.MiddleLineCount - 15], MiddleLine[imgInfo.MiddleLineCount - 2]);
            }
            else imgInfo.MiddleLineCount += extend(MiddleLine[0], MiddleLine[imgInfo.MiddleLineCount - 2]);
        }

        if (!RightLine[0][0])
        {
            Count = 0;
            for (i = 117; i > 40; i--)
            {
                if (!imageBin[i][184]) break;
            }
            if (i > 45)
            {
                for (i = 184; i > 90; i--)
                {
                    if (!imageBin[j][i] && imageBin[j][i - 1] && imageBin[j][i - 2])
                    {
                        RightLine[0][0] = j;
                        RightLine[0][1] = i - 1;
                        break;
                    }
                }

                //右线邻域扫线
                while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 200)
                {
                    NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

                    if (NearNum == 99) break;
                    Count++;
                }
                imgInfo.RightLineSum = Count;
            }
        }

        if (RightLine[imgInfo.RightLineSum][1] > 60 && RightLine[imgInfo.RightLineSum][1] < 160) CircleFlag = 17;
    }
    if (CircleFlag == 17 || CircleFlag == 18)
    {
        if (CircleFlag == 17)
        {
            CircleFlag = 18;
            for (i = 110; i > 75; i--)
            {
                if (!imageBin[i][3])
                {
                    CircleFlag = 17;
                    break;
                }
            }
        }
        else
        {
            CircleFlag = 0;
            for (i = 115; i > 75; i--)
            {
                if (imageBin[i][3])
                {
                    CircleFlag = 18;
                    break;
                }
            }
        }

        if (!imgInfo.RightLineSum)
        {
            for (i = 117; i > 30; i--)
            {
                if (!imageBin[i][184]) break;
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

            Count = 0;
            NearNum = 2;
            while (RightLine[Count][0] > 10 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 200)
            {
                NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

                Count++;

                if (NearNum == 99) break;
            }

            imgInfo.RightLineSum = Count;
        }

        for (i = 0; i < imgInfo.RightLineSum; i++)
        {
            MiddleLine[i][0] = RightLine[i][0];
            MiddleLine[i][1] = RightLine[i][1] - WidthBuffer[RightLine[i][0]] / 2;
        }
        imgInfo.MiddleLineCount = imgInfo.RightLineSum;
    }
}


//-------------------------------------------------------------------------------------------------------------------
// @brief       右环岛扫线
// @param       无
// @return      void
// Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void leftcircle1(void)
{
    uint8 i, j, Count = 0;
    uint8 NearNum = 2;

    if (CircleFlag == 0 || CircleFlag == 11)
    {
        if (CircleFlag == 0)
        {
            CircleFlag = 11;

            for (i = 115; i > 80; i--)
            {
                if (!imageBin[i][3])
                {
                    CircleFlag = 0;
                    break;
                }
            }
        }
        else if (CircleFlag == 11)
        {
            CircleFlag = 12;
            for (i = 115; i > 60; i--)
            {
                if (imageBin[i][3])
                {
                    CircleFlag = 11;
                    break;
                }
            }
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

            NearNum = 2;
            //左线邻域扫线
            while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 200)
            {
                NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

                Count++;

                if (NearNum == 99) break;
            }
            imgInfo.RightLineSum = Count;
        }

        for (i = 0; i < imgInfo.RightLineSum; i++)
        {
            MiddleLine[i][0] = RightLine[i][0];
            MiddleLine[i][1] = RightLine[i][1] - WidthBuffer[RightLine[i][0]] / 2;
        }
        imgInfo.MiddleLineCount = imgInfo.RightLineSum;
    }
    else if (CircleFlag == 12)
    {
        CircleFlag = 13;

        for (i = 117; i > 70; i--)
        {
            if (!imageBin[i][184])
            {
                CircleFlag = 12;
                break;
            }
        }

        if (!imgInfo.LeftLineSum)
        {
            for (j = 3; j < 160; j++)
            {
                if (!imageBin[117][j] && imageBin[117][j + 1] && imageBin[117][j + 2])
                {
                    LeftLine[0][0] = 117;
                    LeftLine[0][1] = j + 1;
                    break;
                }
            }

            Count = 0;
            NearNum = 2;
            while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 200)
            {
                if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
                else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);

                Count++;

                if (NearNum == 99) break;
            }

            imgInfo.LeftLineSum = Count;
        }

        for (i = 0; i < imgInfo.LeftLineSum; i++)
        {
            MiddleLine[i][0] = LeftLine[i][0];
            MiddleLine[i][1] = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] / 2;
        }
        imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
    }
    else if (CircleFlag == 13)
    {
        if (RightTurn[0][0] > 20)
        {
            CircleFlag = 14;
            j = RightTurn[0][1];
            for (i = 1; i < 5; i++)
            {
                if (!imageBin[RightTurn[0][0] - i][j])
                {
                    CircleFlag = 13;
                    break;
                }
            }
        }


        if (!imageBin[115][4] && !imageBin[115][5])
        {
            LeftLine[0][0] = 0;
            LeftLine[0][1] = 0;
            for (j = 4; j < 120; j++)
            {
                if (!imageBin[115][j] && imageBin[115][j + 1] && imageBin[115][j + 2])
                {
                    LeftLine[0][0] = 115;
                    LeftLine[0][1] = j + 1;
                    break;
                }
            }
        }

        if (LeftLine[0][0])
        {
            Count = 0;
            NearNum = 2;
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
                MiddleLine[i][1] = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] / 2 + 3;
            }

            imgInfo.MiddleLineCount = imgInfo.LeftLineSum;

        }
        else
        {
            for (j = 184; j > 60; j--)
            {
                if (!imageBin[115][j] && imageBin[115][j - 1] && imageBin[115][j - 2])
                {
                    RightLine[0][0] = 115;
                    RightLine[0][1] = j - 1;
                    break;
                }
            }

            Count = 0;
            NearNum = 2;
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
        }

    }
    else if (CircleFlag == 14)
    {
        CircleFlag = 15;
        for (i = 115; i > 80; i--)
        {
            if (!imageBin[i][184])
            {
                CircleFlag = 14;
                break;
            }
        }

        if (!imageBin[115][4] && !imageBin[115][5])
        {
            LeftLine[0][0] = 0;
            LeftLine[0][1] = 0;
            for (j = 4; j < 120; j++)
            {
                if (!imageBin[115][j] && imageBin[115][j + 1] && imageBin[115][j + 2])
                {
                    LeftLine[0][0] = 115;
                    LeftLine[0][1] = j + 1;
                    break;
                }
            }
        }

        if (LeftLine[0][0])
        {
            Count = 0;
            NearNum = 2;
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

        }
    }
    else if (CircleFlag == 15)
    {
        CircleFlag = 16;
        for (i = 100; i > 50; i--)
        {
            if (imageBin[i][184])
            {
                CircleFlag = 15;
                break;
            }
        }

        if (!imageBin[117][4] && !imageBin[117][5])
        {
            for (j = 4; j < 120; j++)
            {
                if (!imageBin[117][j] && imageBin[117][j + 1] && imageBin[117][j + 2])
                {
                    LeftLine[0][0] = 117;
                    LeftLine[0][1] = j + 1;
                    break;
                }
            }
        }

        Count = 0;
        NearNum = 2;
        while (LeftLine[Count][0] > 3 && LeftLine[Count][1] > 3 && LeftLine[Count][1] < 184 && Count < 200)
        {
            if (NearNum < 2) NearNum = FindNext(&LeftLine[Count][0], NearNum + 6, 0);
            else NearNum = FindNext(&LeftLine[Count][0], NearNum - 2, 0);

            Count++;

            if (NearNum == 99) break;
        }

        imgInfo.LeftLineSum = Count;

        if (imgInfo.LeftLineSum)
        {
            for (i = 0; i < Count; i++)
            {
                MiddleLine[i][0] = LeftLine[i][0];
                MiddleLine[i][1] = LeftLine[i][1] + WidthBuffer[LeftLine[i][0]] / 2;
            }
            imgInfo.MiddleLineCount = Count;
            CirCleOutFlag = 0;
        }
        else
        {
            CirCleOutFlag = 1;
        }
    }
    else if (CircleFlag == 16)
    {
        if (!imgInfo.RightLineSum)
        {
            for (j = 184; j > 20; j--)
            {
                if (!imageBin[115][j] && imageBin[115][j - 1] && imageBin[115][j - 2])
                {
                    RightLine[0][0] = 115;
                    RightLine[0][1] = j - 1;
                    break;
                }
            }

            Count = 0;
            NearNum = 2;
            while (RightLine[Count][0] > 3 && RightLine[Count][1] > 3 && RightLine[Count][1] < 184 && Count < 200)
            {
                NearNum = FindNext(&RightLine[Count][0], NearNum + 2, 1);

                Count++;

                if (NearNum == 99) break;
            }

            imgInfo.RightLineSum = Count;
        }

        for (i = 0; i < imgInfo.RightLineSum; i++)
        {
            MiddleLine[i][0] = RightLine[i][0];
            if (RightLine[i][1] - WidthBuffer[RightLine[i][0]] / 2 < MiddleValue) MiddleLine[i][1] = RightLine[i][1] - WidthBuffer[RightLine[i][0]] / 2;
            else MiddleLine[i][1] = MiddleValue;
        }
        imgInfo.MiddleLineCount = imgInfo.RightLineSum;
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
void badcross1(void)
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

            if (RepairFlag[0] == 1)
            {
                for (i = imgInfo.LeftLineCount + 10; i < imgInfo.LeftLineSum - 10; i++)
                {
                    if (imageBin[LeftLine[i][0]][LeftLine[i][1] + 2] && !imageBin[LeftLine[i][0]][LeftLine[i][1] - 2] && !imageBin[LeftLine[i][0]][LeftLine[i][1] - 7])
                    {
                        turnpoint[0][0] = LeftLine[i][0];
                        turnpoint[0][1] = LeftLine[i][1];
                        break;
                    }
                }

                if (LeftLine[i][0] > 35) Count = LeftLine[i][0] - 20;
                else Count = LeftLine[i][0] - 10;

                if (turnpoint[0][0] && !imageBin[Count][184])
                {
                    for (j = LeftLine[i][1]; j < 184; j++)
                    {
                        if (imageBin[Count][j] && !imageBin[Count][j + 1] && !imageBin[Count][j + 2])
                        {
                            TempLine2[0][0] = Count;
                            TempLine2[0][1] = j;
                            break;
                        }
                    }
                }

                if (TempLine2[0][0])
                {
                    Count = 0;
                    NearNum = 4;
                    while (TempLine2[Count][0] > 3 && TempLine2[Count][1] > 3 && TempLine2[Count][1] < 184 && Count < 200)
                    {
                        if (NearNum < 2) NearNum = FindNext(&TempLine2[Count][0], NearNum + 6, 0);
                        else NearNum = FindNext(&TempLine2[Count][0], NearNum - 2, 0);

                        Count++;
                    }

                    Count = extend(TempLine2[0], TempLine2[9]) + 5;

                    for (i = 115; i > TempLine2[Count][0]; i--)
                    {
                        RightLine[115 - i][0] = i;
                        RightLine[115 - i][1] = 184;
                    }

                    for (j = 0; j < Count; j++)
                    {
                        RightLine[115 - i + j][0] = TempLine2[Count - j][0];
                        RightLine[115 - i + j][1] = TempLine2[Count - j][1];
                    }

                    imgInfo.RightLineSum = 115 - i + j;

                    //拟合有效直道中线
                    if (imgInfo.LeftLineSum <= imgInfo.RightLineSum)
                    {
                        for (i = 0; i < imgInfo.LeftLineSum; i++)
                        {
                            MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
                            MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
                        }

                        imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
                        imgInfo.top = RightLine[imgInfo.LeftLineSum][0];
                    }
                    else
                    {
                        for (i = 0; i < imgInfo.RightLineSum; i++)
                        {
                            MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
                            MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
                        }
                        imgInfo.MiddleLineCount = imgInfo.RightLineSum;
                        imgInfo.top = LeftLine[imgInfo.RightLineSum][0];
                    }
                }
            }
            else
            {
                for (i = 0; TempLine3[i][0]; i++)
                {
                    LeftLine[i + imgInfo.LeftLineCount][0] = TempLine3[i][0];
                    LeftLine[i + imgInfo.LeftLineCount][1] = TempLine3[i][1];
                }
                imgInfo.LeftLineSum = i + imgInfo.LeftLineCount;

                for (i = 0; i < imgInfo.LeftLineSum; i++)
                {
                    MiddleLine[i][0] = LeftLine[i][0];
                    MiddleLine[i][1] = LeftLine[i][1] / 2 + 94;
                }

                imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
            }
        }
        if (BadCrossFlag == 2)
        {
            uint8 j1, j2;
            j1 = 20;
            j2 = 167;

            i = 115;
            while (!imageBin[i][j2])
            {
                i--;
                if (i < 25) break;
            }
            while (imageBin[i][j2])
            {
                i--;
                if (i < 25) break;
            }//找到十字前端

            if (i > 25)//能找到
            {
                i -= 25;
                for (j = j1; j < j2; j++)
                {
                    if (!imageBin[i][j] && imageBin[i][j + 1] && imageBin[i][j + 2])
                    {
                        TempLine1[0][0] = i;
                        TempLine1[0][1] = j + 1;
                        break;
                    }
                }
                for (j = j2; j > j1; j--)
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
            if (RepairFlag[1] == 1)
            {
                for (i = imgInfo.RightLineCount + 10; i < imgInfo.RightLineSum - 10; i++)
                {
                    if (imageBin[RightLine[i][0]][LeftLine[i][1] - 2] && !imageBin[RightLine[i][0]][RightLine[i][1] + 2] && !imageBin[RightLine[i][0]][RightLine[i][1] + 7])
                    {
                        turnpoint[0][0] = RightLine[i][0];
                        turnpoint[0][1] = RightLine[i][1];
                        break;
                    }
                }

                if (RightLine[i][0] > 35 && !imageBin[RightLine[i][0] - 20][RightLine[i][1]] && !imageBin[RightLine[i][0] - 30][RightLine[i][1]]) Count = RightLine[i][0] - 20;
                else Count = RightLine[i][0] - 10;

                if (turnpoint[0][0] && !imageBin[Count][3])
                {
                    for (j = RightLine[i][1]; j > 3; j--)
                    {
                        if (imageBin[Count][j] && !imageBin[Count][j - 1] && !imageBin[Count][j - 2])
                        {
                            TempLine1[0][0] = Count;
                            TempLine1[0][1] = j;
                            break;
                        }
                    }
                }

                if (TempLine1[0][0])
                {
                    Count = 0;
                    NearNum = 0;
                    //右线！
                    while (TempLine1[Count][0] > 3 && TempLine1[Count][1] > 3 && TempLine1[Count][1] < 184 && Count < 200)
                    {
                        if (NearNum >= 6) NearNum = FindNext(&TempLine1[Count][0], NearNum - 6, 1);
                        else NearNum = FindNext(&TempLine1[Count][0], NearNum + 2, 1);
                        Count++;

                        if (NearNum == 99) break;
                    }

                    Count = extend(TempLine1[0], TempLine1[9]) + 5;

                    for (i = 115; i > TempLine1[Count][0]; i--)
                    {
                        LeftLine[115 - i][0] = i;
                        LeftLine[115 - i][1] = 3;
                    }

                    for (j = 0; j < Count; j++)
                    {
                        LeftLine[115 - i + j][0] = TempLine1[Count - j][0];
                        LeftLine[115 - i + j][1] = TempLine1[Count - j][1];
                    }

                    imgInfo.LeftLineSum = 115 - i + j;

                    //拟合有效直道中线
                    if (imgInfo.LeftLineSum <= imgInfo.RightLineSum)
                    {
                        for (i = 0; i < imgInfo.LeftLineSum; i++)
                        {
                            MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
                            MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
                        }

                        imgInfo.MiddleLineCount = imgInfo.LeftLineSum;
                        imgInfo.top = RightLine[imgInfo.LeftLineSum][0];
                    }
                    else
                    {
                        for (i = 0; i < imgInfo.RightLineSum; i++)
                        {
                            MiddleLine[i][0] = (LeftLine[i][0] + RightLine[i][0]) / 2;
                            MiddleLine[i][1] = (LeftLine[i][1] + RightLine[i][1]) / 2;
                        }
                        imgInfo.MiddleLineCount = imgInfo.RightLineSum;
                        imgInfo.top = LeftLine[imgInfo.RightLineSum][0];
                    }
                }
            }
            else
            {
                for (i = 0; TempLine3[i][0]; i++)
                {
                    RightLine[i + imgInfo.RightLineCount][0] = TempLine3[i][0];
                    RightLine[i + imgInfo.RightLineCount][1] = TempLine3[i][1];
                }
                imgInfo.RightLineSum = i + imgInfo.RightLineCount;

                for (i = 0; i < imgInfo.RightLineSum; i++)
                {
                    MiddleLine[i][0] = RightLine[i][0];
                    MiddleLine[i][1] = RightLine[i][1] / 2;
                }

                imgInfo.MiddleLineCount = imgInfo.RightLineSum;
            }
        }
        if (BadCrossFlag == 12)
        {
            uint8 j1, j2;
            j1 = 20;
            j2 = 167;

            i = 115;
            while (!imageBin[i][j1])
            {
                i--;
                if (i < 25) break;
            }
            while (imageBin[i][j1])
            {
                i--;
                if (i < 25) break;
            }//找到十字前端

            if (i > 25)//能找到
            {
                i -= 25;
                for (j = j1; j < j2; j++)
                {
                    if (!imageBin[i][j] && imageBin[i][j + 1] && imageBin[i][j + 2])
                    {
                        TempLine1[0][0] = i;
                        TempLine1[0][1] = j + 1;
                        break;
                    }
                }
                for (j = j2; j > j1; j--)
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
    if (imgInfo.MiddleLineCount < 5)
    {
        StopFlag++;
    }
    else
    {
        StopFlag = 0;
    }

    if (imgInfo.nextroadType == NoHead)
    {
        //if (NoHeadK == 99)
        //{
        //    return MiddleValue;
        //}
        //else
        //{
        //    if (!imgInfo.NoHeadDir)
        //    {
        //        if (NoHeadK < 0.5) return MiddleValue - 40;
        //        else if (NoHeadK > 2) return MiddleValue;
        //        else return MiddleValue - (20 / NoHeadK);
        //    }
        //    else
        //    {
        //        if (NoHeadK > -0.5) return MiddleValue + 40;
        //        else if (NoHeadK < -2) return MiddleValue;
        //        else return MiddleValue - (20 / NoHeadK);
        //    }
        //}

        if (NoHeadFlag == 1) return MiddleValue;
        else
        {
            uint16 Sum = 0;
            uint8 i = 5;
            while (i < imgInfo.MiddleLineCount)
            {
                Sum += MiddleLine[i][1];
                i++;
            }
            if (Sum) return Sum / (i - 5);
            else return MiddleValue;
        }
    }
    else if (imgInfo.nextroadType == ThreeFork && (ThreeForkFlag == 1 || (ThreeForkFlag == 0 && imgInfo.MiddleLineCount < 10)) && !imgInfo.ThreeForkDir) return MiddleValue - 55;
    else if (imgInfo.nextroadType == ThreeFork && (ThreeForkFlag == 1 || (ThreeForkFlag == 0 && imgInfo.MiddleLineCount < 10)) && imgInfo.ThreeForkDir) return MiddleValue + 55;
    else if (imgInfo.nextroadType == RightCircle && imgInfo.MiddleLineCount < 30) return MiddleValue + 50;
    else if (imgInfo.nextroadType == LeftCircle && imgInfo.MiddleLineCount < 30) return MiddleValue - 50;
    else if (imgInfo.nextroadType == LeftT && LeftTFlag >= 5 && imgInfo.MiddleLineCount < 20) return MiddleValue + 50;
    else if (imgInfo.nextroadType == RightT && RightTFlag >= 5 && imgInfo.MiddleLineCount < 20) return MiddleValue - 50;
    else if (imgInfo.nextroadType == NoHead && NoHeadFlag == 2 && !imgInfo.NoHeadDir) return MiddleValue - 50;
    else if (imgInfo.nextroadType == NoHead && NoHeadFlag == 2 && imgInfo.NoHeadDir) return MiddleValue + 50;
    //    else if (!imgInfo.NoHeadDir && !LeftTurn[0][0] && GarageFlag == 1) return MiddleValue - 40;
    //    else if (imgInfo.NoHeadDir && !RightTurn[0][0] && GarageFlag == 1) return MiddleValue + 40;
    else if (imgInfo.nextroadType == Crooked)
    {
        uint16 Sum = 0;
        int16 Out = 0;
        int16 Er = 0;
        uint8 i = 5;
        while (MiddleLine[i][0] != 0 && i < 75)
        {
            Sum += MiddleLine[i][1];
            i++;
        }

        Out = Sum / (i - 5);
        Er = MiddleValue - Out;

        if (Er < 20 && Er > -20) return MiddleValue - Er / 2;
        else if (Er >= 20) return MiddleValue - ((Er - 20) * 3 / 2 + 10);
        else if (Er <= -20) return MiddleValue - ((Er + 20) * 3 / 2 - 10);
        else return Out;
    }
    else
    {
        uint32 Sum = 0;
        uint32 ESum = 0;
        int32 Out = 0;
        int32 Er = 0;
        uint8 i = 0;
        //        while (MiddleLine[i][0] != 0 && i < 75)
        //        {
        //            Sum += ErrorBuffer[i] * MiddleLine[i][1];
        //            ESum += ErrorBuffer[i];
        //            i++;
        //        }
        //while (i < imgInfo.MiddleLineCount)
        //{
        //    if (MiddleLine[i][0])
        //    {
        //        Sum += ErrorBuffer[120 - MiddleLine[i][0]] * MiddleLine[i][1];
        //        ESum += ErrorBuffer[120 - MiddleLine[i][0]];
        //    }
        //    i++;
        //}
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

            return Out;

            //            if (CircleFlag == 2 || CircleFlag == 3) return MiddleValue - Er / 2 * 3;
        }
        else return MiddleValue;

        //        uint32 Sum = 0;
        //        uint32 ESum = 0;
        //        int32 Out = 0;
        ////        int32 Er = 0;
        //        uint8 i = 5;
        //        for (i = 0; i < 120; i++)
        //        {
        //            if (MidLine[i][0])
        //            {
        ////                Sum += ErrorBuffer[i] * MidLine[i][1];
        //                Sum += ErrorBuffer[120-MidLine[i][0]] * MidLine[i][1];
        //                ESum += ErrorBuffer[120-MidLine[i][0]];
        //            }
        //        }
        //        if (Sum)
        //        {
        //            Out = Sum / ESum;
        //            return Out;
        //        }
        //        else return MiddleValue;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       计算偏差
// @param       无
// @return      void
// Sample usage:           uint8 Error = ErrorSum();//计算偏差
//-------------------------------------------------------------------------------------------------------------------
uint8 CloseErrorSum(void)
{
    uint16 Sum = 0;
    uint8 i = 5;
    uint8 Count = 0;

    Sum = 0;
    for (uint8 i = 0; i < imgInfo.MiddleLineCount; i++)
    {
        if (MiddleLine[i][0] > 105)
        {
            Sum += MiddleLine[i][1];
            Count++;
        }
        else break;
    }
    Sum = Sum / Count;

    if (Sum)
    {
        return Sum;
    }
    else return MiddleValue;
}

#pragma section all restore
