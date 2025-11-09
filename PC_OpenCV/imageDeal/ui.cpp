#include"ui.h"
#include"Track.h"
extern uint8_t LeftLine[256][2];
extern uint8_t RightLine[256][2];
extern uint8_t MiddleLine[256][2];
extern uint8_t MidLine[256][2];
extern uint8_t CloseFlag;
extern uint8_t LeftJudge[256][2];
extern uint8_t LeftTurn[4][3];
extern uint8_t RightJudge[256][2];
extern uint8_t RightTurn[4][3];
extern uint8_t MiddleTurn[4][3];
extern float LeftK[8];
extern float RightK[8];
extern uint8 RoadTypeSum[20];
extern uint8 RoadTypeCount;
extern uint8 TempLine1[256][2];
extern uint8 TempLine2[256][2];
extern uint8 TempLine3[256][2];
extern float NoHeadK;
extern uint8 turnpoint[3][2];
extern uint8 TempNum[2];
extern int8 Num[2];
extern float TransTemp[2];
extern float ThreeForkK;
extern float LeftCos,RightCos;
extern uint8 CircleFlag;
extern uint8 NoHeadFlag;
extern uint8 ThreeForkFlag;
extern uint8 GarageFlag;
extern uint8 BadCrossFlag;
extern uint8 LeftTFlag;
extern uint8 RightTFlag;
extern uint8 Mark[4];
extern uint8 CrossFlag;
//extern uint8 BadThreeForkFlag;
extern float TransBuffer[3][3];
extern float InTransBuffer[3][3];
extern float TransTemp[2];
extern uint8 InTransTemp[2];

uint8 Line30[180][2];

uint8 Line60[180][2];

uint8 Line90[180][2];



uint8_t outI;
extern struct imageInformation imgInfo;
using namespace cv;
std::vector<vector<int>> position;
//在对应的格子中绘图
void plot(Mat image, std::string name, int number, showtype type, bool isBin)
{
	Size dsize;
	switch (type)
	{
	case showtype::not_change:
		dsize = image.size();
		break;
	case showtype::keepRatio:
		if (image.size().height > image.size().width)
			dsize.width = GRID_H / image.size().height * image.size().width;
		else
			dsize.height = GRID_W / image.size().width * image.size().height;
		break;
	case showtype::whole_fill:
		dsize= Size(GRID_W, GRID_H);
		break;
	default:
		break;
	}
	if (isBin)
		image = image * 255;
	Mat back;
	resize(image, back, dsize);
	imshow(name, back);
	moveWindow(name, position[number][0], position[number][1]);
}

void plot(Mat image, std::string name, int number)
{
	plot(image, name, number, showtype::whole_fill, false);
}

void plot(Mat image, std::string name, int number, bool isBin)
{
	plot(image, name, number, showtype::whole_fill, isBin);
}
//实现类似Matlab中的subplot的功能
void subplot(int y, int x)
{
	position.clear();
	for (int i = 0; i < y; i++)
	{
		for (int j = 0; j < x; j++)
		{
			vector<int> tmp = { GRID_W * j ,GRID_H * i + 30 * i };
			position.push_back(tmp);
		}
	}
}
//二维数组转Mat格式
Mat array2mat(uint8* arr, uint8 h, uint8 w)
{
	Mat a = Mat(h, w, CV_8UC1, (uint8*)arr);
	Mat b;
	a.copyTo(b);
	return b;
}
//Mat格式转二维数组
void mat2array(Mat img)
{
	int h = img.size().height;
	int w = img.size().width;
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			mt9v30x_image[i][j] = img.at<uint8>(i, j);
		}
	}
}
const Vec3b WHITE = Vec3b(255, 255, 255);
const Vec3b BLACK = Vec3b(0, 0, 0);
const Vec3b RED = Vec3b(0, 0, 255);
const Vec3b GREEN = Vec3b(0, 255, 0);
const Vec3b BLUE = Vec3b(255, 0, 0);
const Vec3b YELLOW = Vec3b(0, 255, 255);
const Vec3b CYAN = Vec3b(255, 255, 0);
const Vec3b DARKYELLOW = Vec3b(0, 128, 128);
const Vec3b GRAY = Vec3b(128, 128, 128);
const Vec3b PINK = Vec3b(255, 105, 180);
const Vec3b Color1 = Vec3b(203, 192, 255);
const Vec3b Color2 = Vec3b(147, 20, 255);
const Vec3b Color3 = Vec3b(214, 112, 218);

//将获取到的图像信息标注到图片上
Mat paintLine(Mat imgBin)
{	
	Mat imgGray;
	imgBin.copyTo(imgGray);
	imgGray = imgGray * 255;
	Mat imgColor;
	cvtColor(imgGray, imgColor, COLOR_GRAY2BGR);
	//中线及左右边线
	for (int i = imgInfo.top; i < imgInfo.bottom; i++)
	{
		imgColor.at<Vec3b>(i, RLB[i]) = RED;
		imgColor.at<Vec3b>(i, LLB[i]) = RED;
		imgColor.at<Vec3b>(i, MLB[i]) = GREEN;

	}
	//有效区域标记
	for (int j = 0; j < COLUMN; j++)
	{
		imgColor.at<Vec3b>(imgInfo.bottom, j) = YELLOW;
		imgColor.at<Vec3b>(imgInfo.top, j) = YELLOW;
	}
	String guide="";
	if (imgInfo.differ > 8)
		guide = ">";
	if (imgInfo.differ > 16)
		guide = ">>";
	if (imgInfo.differ > 24)
		guide = ">>>";
	if (imgInfo.differ < -8)
		guide = "<";
	if (imgInfo.differ < -16)
		guide = "<<";
	if (imgInfo.differ < -24)
		guide = "<<<";
	if(imgInfo.differ>0)
		cvui::printf(imgColor,138,40,0.4, 0xdc143c, guide.c_str());
	else
		cvui::printf(imgColor,50,40,0.4, 0xdc143c, guide.c_str());
	return imgColor;
}

Mat paintLine1(Mat imgBin)
{
	Mat imgGray;
	imgBin.copyTo(imgGray);
	imgGray = imgGray * 255;
	Mat imgColor;
	cvtColor(imgGray, imgColor, COLOR_GRAY2BGR);

	for (int i = 1; LeftLine[i][0] != 0; i++)
	{
		if (LeftLine[i][1] > 187 || i > 200) break;
		imgColor.at<Vec3b>(LeftLine[i][0], LeftLine[i][1]) = GREEN;
		//Trans2cam(LeftLine[i][0], LeftLine[i][1]);
		imgColor.at<Vec3b>(InTransTemp[0], InTransTemp[1]) = RED;
	}
	for (int i = 1; RightLine[i][0] != 0; i++)
	{
		if (RightLine[i][1] > 187 || i>200) break;
		imgColor.at<Vec3b>(RightLine[i][0], RightLine[i][1]) = GREEN;
		//Trans2cam(RightLine[i][0], RightLine[i][1]);
		imgColor.at<Vec3b>(InTransTemp[0], InTransTemp[1]) = RED;
	}

	for (int i = 1; i < imgInfo.LeftLineCount; i++)
	{
		if (LeftLine[i][1] > 187 || i > 200) break;
		imgColor.at<Vec3b>(LeftLine[i][0], LeftLine[i][1]) = RED;
	}
	for (int i = 1; i < imgInfo.RightLineCount; i++)
	{
		if (RightLine[i][1] > 187 || i > 200) break;
		imgColor.at<Vec3b>(RightLine[i][0], RightLine[i][1]) = RED;
	}

	//for (int i = 0; i < imgInfo.MiddleLineCount; i++)
	//{
	//	if (MiddleLine[i][1] > 187 || i > 200) break;
	//	//Trans2cam(MiddleLine[i][0], MiddleLine[i][1]);
	//	//imgColor.at<Vec3b>(InTransTemp[0], InTransTemp[1]) = GREEN;
	//	imgColor.at<Vec3b>(MiddleLine[i][0], MiddleLine[i][1]) = BLUE;
	//}

	for (int i = 0; i < imgInfo.MidLineCount; i++)
	{
		if (MidLine[i][1] > 187 || i > 200) break;
		imgColor.at<Vec3b>(MidLine[i][0], MidLine[i][1]) = RED;
		//Trans2cam(MidLine[i][0], MidLine[i][1]);
		//imgColor.at<Vec3b>(InTransTemp[0], InTransTemp[1]) = GREEN;
	}

	for (int i = 0; TempLine1[i][0] != 0; i++)
	{
		if (TempLine1[i][1] > 187 || i > 200) break;
		imgColor.at<Vec3b>(TempLine1[i][0], TempLine1[i][1]) = Color3;
	}
	memset(TempLine1, 0, sizeof(TempLine1));

	for (int i = 0; TempLine2[i][0] != 0; i++)
	{
		if (TempLine2[i][1] > 187 || i > 200) break;
		imgColor.at<Vec3b>(TempLine2[i][0], TempLine2[i][1]) = Color2;
	}
	memset(TempLine2, 0, sizeof(TempLine2));

	if (LeftTurn[1][0] != 0)
	{
		imgColor.at<Vec3b>(LeftTurn[1][0], LeftTurn[1][1]) = PINK;
		imgColor.at<Vec3b>(LeftTurn[1][0]-1, LeftTurn[1][1]-1) = PINK;
		imgColor.at<Vec3b>(LeftTurn[1][0]-1, LeftTurn[1][1]) = PINK;
		imgColor.at<Vec3b>(LeftTurn[1][0]-1, LeftTurn[1][1]+1) = PINK;
		imgColor.at<Vec3b>(LeftTurn[1][0], LeftTurn[1][1]-1) = PINK;
		imgColor.at<Vec3b>(LeftTurn[1][0], LeftTurn[1][1]+1) = PINK;
		imgColor.at<Vec3b>(LeftTurn[1][0]+1, LeftTurn[1][1]-1) = PINK;
		imgColor.at<Vec3b>(LeftTurn[1][0]+1, LeftTurn[1][1]) = PINK;
		imgColor.at<Vec3b>(LeftTurn[1][0]+1, LeftTurn[1][1]+1) = PINK;
	}

	if (RightTurn[1][0] != 0)
	{
		imgColor.at<Vec3b>(RightTurn[1][0], RightTurn[1][1]) = PINK;
		imgColor.at<Vec3b>(RightTurn[1][0] - 1, RightTurn[1][1] - 1) = PINK;
		imgColor.at<Vec3b>(RightTurn[1][0] - 1, RightTurn[1][1]) = PINK;
		imgColor.at<Vec3b>(RightTurn[1][0] - 1, RightTurn[1][1] + 1) = PINK;
		imgColor.at<Vec3b>(RightTurn[1][0], RightTurn[1][1] - 1) = PINK;
		imgColor.at<Vec3b>(RightTurn[1][0], RightTurn[1][1] + 1) = PINK;
		imgColor.at<Vec3b>(RightTurn[1][0] + 1, RightTurn[1][1] - 1) = PINK;
		imgColor.at<Vec3b>(RightTurn[1][0] + 1, RightTurn[1][1]) = PINK;
		imgColor.at<Vec3b>(RightTurn[1][0] + 1, RightTurn[1][1] + 1) = PINK;
	}

	if (LeftTurn[2][0] != 0)
	{
		imgColor.at<Vec3b>(LeftTurn[2][0], LeftTurn[2][1]) = BLUE;
		imgColor.at<Vec3b>(LeftTurn[2][0], LeftTurn[2][1] - 1) = BLUE;
		imgColor.at<Vec3b>(LeftTurn[2][0], LeftTurn[2][1] + 1) = BLUE;
		imgColor.at<Vec3b>(LeftTurn[2][0] - 1, LeftTurn[2][1] - 1) = BLUE;
		imgColor.at<Vec3b>(LeftTurn[2][0] - 1, LeftTurn[2][1]) = BLUE;
		imgColor.at<Vec3b>(LeftTurn[2][0] - 1, LeftTurn[2][1] + 1) = BLUE;
		imgColor.at<Vec3b>(LeftTurn[2][0] + 1, LeftTurn[2][1] - 1) = BLUE;
		imgColor.at<Vec3b>(LeftTurn[2][0] + 1, LeftTurn[2][1]) = BLUE;
		imgColor.at<Vec3b>(LeftTurn[2][0] + 1, LeftTurn[2][1] + 1) = BLUE;
	}

	if (RightTurn[2][0] != 0)
	{
		imgColor.at<Vec3b>(RightTurn[2][0], RightTurn[2][1]) = BLUE;
		imgColor.at<Vec3b>(RightTurn[2][0], RightTurn[2][1] - 1) = BLUE;
		imgColor.at<Vec3b>(RightTurn[2][0], RightTurn[2][1] + 1) = BLUE;
		imgColor.at<Vec3b>(RightTurn[2][0] - 1, RightTurn[2][1] - 1) = BLUE;
		imgColor.at<Vec3b>(RightTurn[2][0] - 1, RightTurn[2][1]) = BLUE;
		imgColor.at<Vec3b>(RightTurn[2][0] - 1, RightTurn[2][1] + 1) = BLUE;
		imgColor.at<Vec3b>(RightTurn[2][0] + 1, RightTurn[2][1] - 1) = BLUE;
		imgColor.at<Vec3b>(RightTurn[2][0] + 1, RightTurn[2][1]) = BLUE;
		imgColor.at<Vec3b>(RightTurn[2][0] + 1, RightTurn[2][1] + 1) = BLUE;
	}

	if (LeftTurn[0][0] != 0)
	{
		imgColor.at<Vec3b>(LeftTurn[0][0], LeftTurn[0][1]) = RED;
		imgColor.at<Vec3b>(LeftTurn[0][0], LeftTurn[0][1] - 1) = RED;
		imgColor.at<Vec3b>(LeftTurn[0][0], LeftTurn[0][1] + 1) = RED;
		imgColor.at<Vec3b>(LeftTurn[0][0] - 1, LeftTurn[0][1] - 1) = RED;
		imgColor.at<Vec3b>(LeftTurn[0][0] - 1, LeftTurn[0][1]) = RED;
		imgColor.at<Vec3b>(LeftTurn[0][0] - 1, LeftTurn[0][1] + 1) = RED;
		imgColor.at<Vec3b>(LeftTurn[0][0] + 1, LeftTurn[0][1] - 1) = RED;
		imgColor.at<Vec3b>(LeftTurn[0][0] + 1, LeftTurn[0][1]) = RED;
		imgColor.at<Vec3b>(LeftTurn[0][0] + 1, LeftTurn[0][1] + 1) = RED;
	}

	if (RightTurn[1][0] != 0)
	{
		imgColor.at<Vec3b>(RightTurn[1][0], RightTurn[1][1]) = PINK;
		imgColor.at<Vec3b>(RightTurn[1][0] - 1, RightTurn[1][1] - 1) = PINK;
		imgColor.at<Vec3b>(RightTurn[1][0] - 1, RightTurn[1][1]) = PINK;
		imgColor.at<Vec3b>(RightTurn[1][0] - 1, RightTurn[1][1] + 1) = PINK;
		imgColor.at<Vec3b>(RightTurn[1][0], RightTurn[1][1] - 1) = PINK;
		imgColor.at<Vec3b>(RightTurn[1][0], RightTurn[1][1] + 1) = PINK;
		imgColor.at<Vec3b>(RightTurn[1][0] + 1, RightTurn[1][1] - 1) = PINK;
		imgColor.at<Vec3b>(RightTurn[1][0] + 1, RightTurn[1][1]) = PINK;
		imgColor.at<Vec3b>(RightTurn[1][0] + 1, RightTurn[1][1] + 1) = PINK;
	}

	if (LeftTurn[2][0] != 0)
	{
		imgColor.at<Vec3b>(LeftTurn[2][0], LeftTurn[2][1]) = BLUE;
		imgColor.at<Vec3b>(LeftTurn[2][0], LeftTurn[2][1] - 1) = BLUE;
		imgColor.at<Vec3b>(LeftTurn[2][0], LeftTurn[2][1] + 1) = BLUE;
		imgColor.at<Vec3b>(LeftTurn[2][0] - 1, LeftTurn[2][1] - 1) = BLUE;
		imgColor.at<Vec3b>(LeftTurn[2][0] - 1, LeftTurn[2][1]) = BLUE;
		imgColor.at<Vec3b>(LeftTurn[2][0] - 1, LeftTurn[2][1] + 1) = BLUE;
		imgColor.at<Vec3b>(LeftTurn[2][0] + 1, LeftTurn[2][1] - 1) = BLUE;
		imgColor.at<Vec3b>(LeftTurn[2][0] + 1, LeftTurn[2][1]) = BLUE;
		imgColor.at<Vec3b>(LeftTurn[2][0] + 1, LeftTurn[2][1] + 1) = BLUE;
	}

	if (RightTurn[0][0] != 0)
	{
		imgColor.at<Vec3b>(RightTurn[0][0], RightTurn[0][1]) = RED;
		imgColor.at<Vec3b>(RightTurn[0][0], RightTurn[0][1] - 1) = RED;
		imgColor.at<Vec3b>(RightTurn[0][0], RightTurn[0][1] + 1) = RED;
		imgColor.at<Vec3b>(RightTurn[0][0] - 1, RightTurn[0][1] - 1) = RED;
		imgColor.at<Vec3b>(RightTurn[0][0] - 1, RightTurn[0][1]) = RED;
		imgColor.at<Vec3b>(RightTurn[0][0] - 1, RightTurn[0][1] + 1) = RED;
		imgColor.at<Vec3b>(RightTurn[0][0] + 1, RightTurn[0][1] - 1) = RED;
		imgColor.at<Vec3b>(RightTurn[0][0] + 1, RightTurn[0][1]) = RED;
		imgColor.at<Vec3b>(RightTurn[0][0] + 1, RightTurn[0][1] + 1) = RED;
	}

	if (turnpoint[0][0] != 0)
	{
		imgColor.at<Vec3b>(turnpoint[0][0], turnpoint[0][1]) = Color1;
		imgColor.at<Vec3b>(turnpoint[0][0], turnpoint[0][1] - 1) = Color1;
		imgColor.at<Vec3b>(turnpoint[0][0], turnpoint[0][1] + 1) = Color1;
		imgColor.at<Vec3b>(turnpoint[0][0] - 1, turnpoint[0][1] - 1) = Color1;
		imgColor.at<Vec3b>(turnpoint[0][0] - 1, turnpoint[0][1]) = Color1;
		imgColor.at<Vec3b>(turnpoint[0][0] - 1, turnpoint[0][1] + 1) = Color1;
		imgColor.at<Vec3b>(turnpoint[0][0] + 1, turnpoint[0][1] - 1) = Color1;
		imgColor.at<Vec3b>(turnpoint[0][0] + 1, turnpoint[0][1]) = Color1;
		imgColor.at<Vec3b>(turnpoint[0][0] + 1, turnpoint[0][1] + 1) = Color1;
	}

	if (turnpoint[1][0] != 0)
	{
		imgColor.at<Vec3b>(turnpoint[1][0], turnpoint[1][1]) = Color1;
		imgColor.at<Vec3b>(turnpoint[1][0], turnpoint[1][1] - 1) = Color1;
		imgColor.at<Vec3b>(turnpoint[1][0], turnpoint[1][1] + 1) = Color1;
		imgColor.at<Vec3b>(turnpoint[1][0] - 1, turnpoint[1][1] - 1) = Color1;
		imgColor.at<Vec3b>(turnpoint[1][0] - 1, turnpoint[1][1]) = Color1;
		imgColor.at<Vec3b>(turnpoint[1][0] - 1, turnpoint[1][1] + 1) = Color1;
		imgColor.at<Vec3b>(turnpoint[1][0] + 1, turnpoint[1][1] - 1) = Color1;
		imgColor.at<Vec3b>(turnpoint[1][0] + 1, turnpoint[1][1]) = Color1;
		imgColor.at<Vec3b>(turnpoint[1][0] + 1, turnpoint[1][1] + 1) = Color1;
	}

	if (LeftTurn[2][2] != 0)
	{
		imgColor.at<Vec3b>(LeftTurn[2][0], LeftTurn[2][1]) = BLUE;
		imgColor.at<Vec3b>(LeftTurn[2][0], LeftTurn[2][1] - 1) = BLUE;
		imgColor.at<Vec3b>(LeftTurn[2][0], LeftTurn[2][1] + 1) = BLUE;
		imgColor.at<Vec3b>(LeftTurn[2][0] - 1, LeftTurn[2][1] - 1) = BLUE;
		imgColor.at<Vec3b>(LeftTurn[2][0] - 1, LeftTurn[2][1]) = BLUE;
		imgColor.at<Vec3b>(LeftTurn[2][0] - 1, LeftTurn[2][1] + 1) = BLUE;
		imgColor.at<Vec3b>(LeftTurn[2][0] + 1, LeftTurn[2][1] - 1) = BLUE;
		imgColor.at<Vec3b>(LeftTurn[2][0] + 1, LeftTurn[2][1]) = BLUE;
		imgColor.at<Vec3b>(LeftTurn[2][0] + 1, LeftTurn[2][1] + 1) = BLUE;
	}

	if (RightTurn[2][2] != 0)
	{
		imgColor.at<Vec3b>(RightTurn[2][0], RightTurn[2][1]) = BLUE;
		imgColor.at<Vec3b>(RightTurn[2][0], RightTurn[2][1] - 1) = BLUE;
		imgColor.at<Vec3b>(RightTurn[2][0], RightTurn[2][1] + 1) = BLUE;
		imgColor.at<Vec3b>(RightTurn[2][0] - 1, RightTurn[2][1] - 1) = BLUE;
		imgColor.at<Vec3b>(RightTurn[2][0] - 1, RightTurn[2][1]) = BLUE;
		imgColor.at<Vec3b>(RightTurn[2][0] - 1, RightTurn[2][1] + 1) = BLUE;
		imgColor.at<Vec3b>(RightTurn[2][0] + 1, RightTurn[2][1] - 1) = BLUE;
		imgColor.at<Vec3b>(RightTurn[2][0] + 1, RightTurn[2][1]) = BLUE;
		imgColor.at<Vec3b>(RightTurn[2][0] + 1, RightTurn[2][1] + 1) = BLUE;
	}

	if (MiddleTurn[0][0] != 0)
	{
		imgColor.at<Vec3b>(MiddleTurn[0][0], MiddleTurn[0][1]) = RED;
		imgColor.at<Vec3b>(MiddleTurn[0][0], MiddleTurn[0][1] - 1) = RED;
		imgColor.at<Vec3b>(MiddleTurn[0][0], MiddleTurn[0][1] + 1) = RED;
		imgColor.at<Vec3b>(MiddleTurn[0][0] - 1, MiddleTurn[0][1] - 1) = RED;
		imgColor.at<Vec3b>(MiddleTurn[0][0] - 1, MiddleTurn[0][1]) = RED;
		imgColor.at<Vec3b>(MiddleTurn[0][0] - 1, MiddleTurn[0][1] + 1) = RED;
		imgColor.at<Vec3b>(MiddleTurn[0][0] + 1, MiddleTurn[0][1] - 1) = RED;
		imgColor.at<Vec3b>(MiddleTurn[0][0] + 1, MiddleTurn[0][1]) = RED;
		imgColor.at<Vec3b>(MiddleTurn[0][0] + 1, MiddleTurn[0][1] + 1) = RED;
	}

	if (MiddleTurn[1][0] != 0)
	{
		imgColor.at<Vec3b>(MiddleTurn[1][0], MiddleTurn[1][1]) = PINK;
		imgColor.at<Vec3b>(MiddleTurn[1][0] - 1, MiddleTurn[1][1] - 1) = PINK;
		imgColor.at<Vec3b>(MiddleTurn[1][0] - 1, MiddleTurn[1][1]) = PINK;
		imgColor.at<Vec3b>(MiddleTurn[1][0] - 1, MiddleTurn[1][1] + 1) = PINK;
		imgColor.at<Vec3b>(MiddleTurn[1][0], MiddleTurn[1][1] - 1) = PINK;
		imgColor.at<Vec3b>(MiddleTurn[1][0], MiddleTurn[1][1] + 1) = PINK;
		imgColor.at<Vec3b>(MiddleTurn[1][0] + 1, MiddleTurn[1][1] - 1) = PINK;
		imgColor.at<Vec3b>(MiddleTurn[1][0] + 1, MiddleTurn[1][1]) = PINK;
		imgColor.at<Vec3b>(MiddleTurn[1][0] + 1, MiddleTurn[1][1] + 1) = PINK;
	}

	if (MiddleTurn[2][0] != 0)
	{
		imgColor.at<Vec3b>(MiddleTurn[2][0], MiddleTurn[2][1]) = BLUE;
		imgColor.at<Vec3b>(MiddleTurn[2][0], MiddleTurn[2][1] - 1) = BLUE;
		imgColor.at<Vec3b>(MiddleTurn[2][0], MiddleTurn[2][1] + 1) = BLUE;
		imgColor.at<Vec3b>(MiddleTurn[2][0] - 1, MiddleTurn[2][1] - 1) = BLUE;
		imgColor.at<Vec3b>(MiddleTurn[2][0] - 1, MiddleTurn[2][1]) = BLUE;
		imgColor.at<Vec3b>(MiddleTurn[2][0] - 1, MiddleTurn[2][1] + 1) = BLUE;
		imgColor.at<Vec3b>(MiddleTurn[2][0] + 1, MiddleTurn[2][1] - 1) = BLUE;
		imgColor.at<Vec3b>(MiddleTurn[2][0] + 1, MiddleTurn[2][1]) = BLUE;
		imgColor.at<Vec3b>(MiddleTurn[2][0] + 1, MiddleTurn[2][1] + 1) = BLUE;
	}

	return imgColor;
}

//获取灰度直方图
Mat getHistImg(Mat img, int th)
{
	//把图像分割成三个通道BRG

	//创建有256个子区间的直方图
	//值的可能数量为【0.255】
	int numbins = 256;

	//设置范围（BGR）最后一个值不包含
	float range[] = { 0, 256 };
	const float* histRange = { range };

	Mat hist;

	calcHist(&img, 1, 0, Mat(), hist, 1, &numbins, &histRange);

	//绘制直方图
	//将为每个图像通道绘制
	int width = IMAGE_W;
	int height = IMAGE_H;

	//以灰色为基底创建图像
	Mat histImage(height, width, CV_8UC3, Scalar(255, 255, 255));

	//从0到图像的高度归一化直方图
	normalize(hist, hist, 0, height, NORM_MINMAX);

	int binStep = cvRound((float)width / (float)numbins);
	for (int i = 1; i < width; i++)
	{
		if ((i - 1) * numbins / width == th)
			line(histImage,
				Point(i - 1, height),
				Point(i - 1, 0),
				Scalar(0, 0, 255));
		else
			line(histImage,
				Point(i - 1, height),
				Point(i - 1, height - cvRound(hist.at<float>((i - 1) * numbins / width))),
				Scalar(128, 128, 128));
	}
	return histImage;
}
//将图片的灰度值显示出来
Mat getTextImg(Mat img)
{
	int scalex = 3;
	int scaley = 9;
	Mat tmp = Mat(60 * scaley, 188 * scalex, CV_8UC1);
	tmp = cv::Scalar(0);
	for (int i = 0; i < 60; i += 2)
	{
		for (int j = 0; j < 188; j += 8)
		{
			cvui::printf(tmp, j * scalex, i * scaley, 0.4, 2 * img.at<uint8>(i,j), "%3d", img.at<uint8>(i, j));
		}
	}
	return tmp;
}
//刷新UI框架并获取用户输入
void UI_refresh(Mat& frame,int &direction,bool &autoplay)
{
	frame = cv::Scalar(40, 40, 40);
	int line = 0;
	cvui::printf(frame, 0, 150 + 50 * line++, 1, 0xFFFAFA, "imgNum:%d", number_image);
	cvui::printf(frame, 0, 150 + 50 * line++, 1, 0xFFFAFA, "differ: %d ", imgInfo.differ);
	cvui::printf(frame, 0, 150 + 50 * line++, 1, 0xFFFAFA, "th:%d ", threshold_image);
	cvui::printf(frame, 0, 150 + 50 * line++, 1, 0xFFFAFA, "MidCnt:%d ", imgInfo.MidLineCount);
	cvui::printf(frame, 0, 150 + 50 * line++, 1, 0xFFFAFA, "RodCnt:%d ", RoadTypeCount);

	line = 0;
	cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "close:%d", CloseFlag);

	if (imgInfo.roadType == Straight) cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "Straight");
	else if (imgInfo.roadType == NoHead) cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "NoHead");
	else if (imgInfo.roadType == Cross) cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "Cross");
	else if (imgInfo.roadType == ThreeFork) cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "ThreeFork");
	else if (imgInfo.roadType == RightCircle) cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "RightCircle");
	else if (imgInfo.roadType == LeftCircle) cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "LeftCircle");
	else if (imgInfo.roadType == Ramp) cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "Ramp");
	else if (imgInfo.roadType == TurnLeft) cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "TurnLeft");
	else if (imgInfo.roadType == TurnRight) cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "TurnRight");
	else if (imgInfo.roadType == LeftT) cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "LeftT");
	else if (imgInfo.roadType == RightT) cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "RightT");
	else if (imgInfo.roadType == ToBeDone) cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "ToBeDone");
	else if (imgInfo.roadType == Crooked) cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "Crooked");
	else if (imgInfo.roadType == BadCross) cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "BadCross");
	else if (imgInfo.roadType == BadThreeFork) cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "BadThreeFork");

	if (imgInfo.nextroadType == Straight) cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "Straight");
	else if (imgInfo.nextroadType == NoHead) cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "NoHead");
	else if (imgInfo.nextroadType == Cross) cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "Cross");
	else if (imgInfo.nextroadType == ThreeFork) cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "ThreeFork");
	else if (imgInfo.nextroadType == RightCircle) cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "RightCircle");
	else if (imgInfo.nextroadType == LeftCircle) cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "LeftCircle");
	else if (imgInfo.nextroadType == Ramp) cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "Ramp");
	else if (imgInfo.nextroadType == TurnLeft) cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "TurnLeft");
	else if (imgInfo.nextroadType == TurnRight) cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "TurnRight");
	else if (imgInfo.nextroadType == LeftT) cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "LeftT");
	else if (imgInfo.nextroadType == RightT) cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "RightT");
	else if (imgInfo.nextroadType == ToBeDone) cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "ToBeDone");
	else if (imgInfo.nextroadType == Crooked) cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "Crooked");
	else if (imgInfo.nextroadType == BadCross) cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "BadCross");
	else if (imgInfo.nextroadType == BadThreeFork) cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "BadThreeFork");

	cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "Lk0=%f", LeftK[0]);
	cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "Rk0=%f", RightK[0]);
	//cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "turn:%d", outI);

	cvui::printf(frame, 0, 150 + 50 * ++line, 1, 0xAFEEEE, "Straig:%d ", RoadTypeSum[Straight]);
	cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xAFEEEE, "NoHead:%d", RoadTypeSum[NoHead]);
	cvui::printf(frame, 0, 150 + 50 * line, 1, 0xAFEEEE, "Cross:%d ", RoadTypeSum[Cross]);
	cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xAFEEEE, "3-Fork:%d", RoadTypeSum[ThreeFork]);
	cvui::printf(frame, 0, 150 + 50 * line, 1, 0xAFEEEE, "LeftT:%d ", RoadTypeSum[LeftT]);
	cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xAFEEEE, "RightT:%d", RoadTypeSum[RightT]);
	cvui::printf(frame, 0, 150 + 50 * line, 1, 0xAFEEEE, "TurnLe:%d", RoadTypeSum[TurnLeft]);
	cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xAFEEEE, "TurnRi:%d ", RoadTypeSum[TurnRight]);
	cvui::printf(frame, 0, 150 + 50 * line, 1, 0xAFEEEE, "ToBeDn:%d", RoadTypeSum[ToBeDone]);
	cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xAFEEEE, "RightCir:%d ", RoadTypeSum[RightCircle]);
	cvui::printf(frame, 0, 150 + 50 * line, 1, 0xAFEEEE, "LeftCir:%d", RoadTypeSum[LeftCircle]);

	//cvui::printf(frame, 0, 150 + 700, 1, 0xFFFAFA, "Num0:%d", CircleFlag);

	cvui::printf(frame, 0, 150 + 700, 1, 0xFFFAFA, "Num0:%d", Mark[2]);
	//cvui::printf(frame, 250, 150 + 700, 1, 0xFFFAFA, "Num1:%d", Mark[3]);
	cvui::printf(frame, 250, 150 + 700, 1, 0xFFFAFA, "Num1:%d", LeftTFlag+RightTFlag+CircleFlag+NoHeadFlag+ThreeForkFlag);

	cvui::printf(frame, 0, 200+ 700, 1, 0xFFFAFA, "LK1=%.4f", RightK[2]);
	cvui::printf(frame, 250, 200 + 700, 1, 0xFFFAFA, "RK1:%.4f", RightK[3]);
	cvui::printf(frame, 0, 200 + 750, 1, 0xFFFAFA, "LCos=%.4f", LeftCos);
	cvui::printf(frame, 250, 200 + 750, 1, 0xFFFAFA, "RCos:%.4f", RightCos);

	cvui::printf(frame, 200, 150 + 50 * line++, 1, 0xFFFAFA, "%f",NoHeadK);
	cvui::printf(frame, 0, 150 + 50 * line, 1, 0xAFEEEE, "TurnP:%d", turnpoint[1][0]);

	if (!autoplay)direction = 0;
	if (cvui::button(frame, 10, 20, 80, 50, "Previous"))
		direction = -1;
	if (cvui::button(frame, 100, 20, 80, 50, "Next"))
		direction = 1;
	cvui::checkbox(frame, 200, 40, "Auto Play", &autoplay, 0x00ff7f);
	if (cvui::button(frame, 60, 80, 80, 50, "End"))
		exit(0);
	cvui::update();
	cv::imshow("UI_block", frame);
}
