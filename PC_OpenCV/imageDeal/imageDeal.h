/*
 * @Description: 图像处理、搜线、巡线
 * @version:
 * @Author: wloves
 * @Date: 2020-05-03 20:14:59
 * @LastEditors: wloves
 * @LastEditTime: 2020-09-24 15:10:10
 */
#ifndef CODE_IMAGEDEAL_H_
#define CODE_IMAGEDEAL_H_
#include "mainInclude.h"


void imageDeal(void);
float leastSquares(void);
void searchLine(void);
void imgInfoInit(void);
void turnDeal(void);
void getDiffer(void);
void getLen(void);
uint8 myabs(int16 input);

//配置摄像头参数
#define MT9V03X_W               188             	//图像宽度 	范围1-188
#define MT9V03X_H               120             	//图像高度	范围1-120



#endif /* CODE_IMAGEDEAL_H_ */
