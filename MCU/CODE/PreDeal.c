/*********************************************************************************************************************
* @file             Camera.c
* @author           Andreas WH
* @brief            摄像头显示及预处理
* @Target core      TC364
* @board            TC364_MainBoard_V1.1
* @date             2021-12-29
********************************************************************************************************************/

#include "PreDeal.h"
#include "headfile.h"
#include "common.h"
#include "SEEKFREE_MT9V03X.h"

#pragma section all "cpu0_dsram"
uint8 imageBin[MT9V03X_H][MT9V03X_W];

//-------------------------------------------------------------------------------------------------------------------
// @brief		二值化
// @param		*picture_buffer		被转换的数组地址
// @param		value				转换阈值
// @return		void
// Sample usage:				Binarization(Img[0], 128)
//-------------------------------------------------------------------------------------------------------------------
void Binarization(uint8 thro)
{
    uint32 i,j;

    uint16 temp = 0;

    for(j=0;j<MT9V03X_H;j++)
    {
        for(i=0;i<MT9V03X_W;i++)
        {
            temp = *(mt9v03x_image[0]+j*MT9V03X_W+i);//读取像素点
            if(temp>=thro) *(imageBin[0]+j*MT9V03X_W+i)=1;
            else *(imageBin[0]+j*MT9V03X_W+i)=0;
        }
    }
}


uint8 OSTU(void)
{
uint8 HistGram[256] = {0,};
uint16 x,y;
int16 Y;
uint32 Amount = 0;
uint32 PixelBack = 0;
uint32 PixelIntegralBack = 0;
uint32 PixelIntegral = 0;
int32 PixelIntegralFore = 0;
int32 PixelFore = 0;
double OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma; // 类间方差;
int16 MinValue, MaxValue;
uint8 Threshold = 0;

	for (y = 0; y < 256; y++)
	{
		HistGram[y] = 0; //初始化灰度直方图
	}
	for (y = 0; y < MT9V03X_H; y++)
	{
		for (x = 0; x < MT9V03X_W; x++)
		{
			HistGram[mt9v03x_image[y][x]]++; //统计每个灰度值的个数信息
		}
	}


for (MinValue = 0; MinValue < 256 && HistGram[MinValue] == 0; MinValue++) ;        //获取最小灰度的值
for (MaxValue = 255; MaxValue > MinValue && HistGram[MinValue] == 0; MaxValue--) ; //获取最大灰度的值

if (MaxValue == MinValue) 
{
    return MaxValue;          // 图像中只有一个颜色    
}
if (MinValue + 1 == MaxValue) 
{
    return MinValue;      // 图像中只有二个颜色
}

for (Y = MinValue; Y <= MaxValue; Y++)
{
    Amount += HistGram[Y];        //  像素总数
}

PixelIntegral = 0;
for (Y = MinValue; Y <= MaxValue; Y++)
{
    PixelIntegral += HistGram[Y] * Y;//灰度值总数
}
SigmaB = -1;
for (Y = MinValue; Y < MaxValue; Y++)
{
    PixelBack = PixelBack + HistGram[Y];    //前景像素点数
    PixelFore = Amount - PixelBack;         //背景像素点数
    OmegaBack = (double)PixelBack / Amount;//前景像素百分比
    OmegaFore = (double)PixelFore / Amount;//背景像素百分比
    PixelIntegralBack += HistGram[Y] * Y;  //前景灰度值
    PixelIntegralFore = PixelIntegral - PixelIntegralBack;//背景灰度值
    MicroBack = (double)PixelIntegralBack / PixelBack;//前景灰度百分比
    MicroFore = (double)PixelIntegralFore / PixelFore;//背景灰度百分比
    Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);//g
    if (Sigma > SigmaB)//遍历最大的类间方差g
    {
        SigmaB = Sigma;
        Threshold = Y;
    }
    if (Sigma < SigmaB)//遍历最大的类间方差g
    {
        break;
    }
}
return Threshold;
}

uint8 ostu(void)   //注意计算阈值的一定要是原图像
{
   #define GrayScale 256
    static uint8 lasthreshold=0;
    uint16 width = MT9V03X_W;
    uint16 height = MT9V03X_H;
    int pixelCount[GrayScale];
    float pixelPro[GrayScale];
    int i, j, pixelSum = width * height/4;
    uint8 threshold = 0;
    uint8* data = mt9v03x_image[0];  //指向像素数据的指针
    for (i = 0; i < GrayScale; i++)
    {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }

    uint32 gray_sum=0;
    //统计灰度级中每个像素在整幅图像中的个数
    for (i = 0; i < height; i+=2)
    {
        for (j = 0; j < width; j+=2)
        {
            pixelCount[(int)data[i * width + j]]++;  //将当前的点的像素值作为计数数组的下标
            gray_sum+=(int)data[i * width + j];       //灰度值总和
        }
    }

    //计算每个像素值的点在整幅图像中的比例

    for (i = 0; i < GrayScale; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;

    }

    //遍历灰度级[0,255]
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;


        w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
        if( lasthreshold-50>0)
    j= lasthreshold-50;
        else
    j=0;
        for (; j < GrayScale; j++)
        {

                w0 += pixelPro[j];  //背景部分每个灰度值的像素点所占比例之和   即背景部分的比例
                u0tmp += j * pixelPro[j];  //背景部分 每个灰度值的点的比例 *灰度值

               w1=1-w0;
               u1tmp=gray_sum/pixelSum-u0tmp;

                u0 = u0tmp / w0;              //背景平均灰度
                u1 = u1tmp / w1;              //前景平均灰度
                u = u0tmp + u1tmp;            //全局平均灰度
                deltaTmp = w0 * (u0 - u) * (u0 - u) + w1 * (u1 - u) * (u1 - u);
                if (deltaTmp > deltaMax)
                {
                    deltaMax = deltaTmp;
                    threshold = j;
                }
                if (deltaTmp < deltaMax)
                {
                break;
                }

         }

    return threshold;
}

#pragma section all restore
