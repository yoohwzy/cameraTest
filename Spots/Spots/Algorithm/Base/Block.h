#pragma once

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"


//用于存储瓷砖在图像中的位置信息
class Block
{
public:
	Block(int _imageWidth, int _imageHeight);
	~Block()
	{
		
	};
	struct Line
	{
		int x0;
		int y0;
		double k;
		double dx;
		double dy;
		bool isAllOutOfImg = false;//是否是位于图片外面
	};

	cv::Point A, B, C, D;

	Line LeftLine;
	Line RightLine;
	Line UpLine;
	Line DownLine;

	//根据四个边算出ABCD四个点 若四个点位置不对，返回false
	bool Lines2ABCD();
	//根据ABCD四个点算出四个边
	bool ABCD2Lines();

	cv::Point GetPonintByX(int x, Line *l);
	cv::Point GetPonintByY(int y, Line *l);

	double ab_mm;
	double bc_mm;
	double cd_mm;
	double da_mm;
	double ac_mm;
	double bd_mm;

	double ab_pix;
	double bc_pix;
	double cd_pix;
	double da_pix;
	double ac_pix;
	double bd_pix;
private:
	//图像大小
	int imageWidth = 0;
	int imageHeight = 0;
public:
	static int Standard_Width_mm;//标准砖横长mm
	static int Standard_Length_mm;//标准砖纵长mm
	static double X_mmPerPix;//x轴方向上每像素代表多少毫米
	static double Y_mmPerPix;//y轴方向上每像素代表多少毫米

	//直接计算对角线的像素长度
	static void CalculateDuiJiaoXian(Block *p_b);
	//根据定标数据，求出瓷砖四边长度
	static void Calculate(Block *p_b);
	//保证竖直边一定是垂直时调用
	static void BiaoDing(Block *p_b);
};

