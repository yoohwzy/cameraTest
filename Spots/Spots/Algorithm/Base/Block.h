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

	int Real_WidthMM = 600;//图像中瓷砖的横长
	int Real_LengthMM = 300;//图像中瓷砖的纵长
	double Axis_X_mmPerPix = 1;//x轴方向上每像素代表多少毫米
	double Axis_Y_mmPerPix = 1;//y轴方向上每像素代表多少毫米

	//根据四个边算出ABCD四个点 若四个点位置不对，返回false
	bool Lines2ABCD();
	//根据ABCD四个点算出四个边
	bool ABCD2Lines();

	void ABCDAdjust();

	cv::Point GetPonintByX(int x, Line *l);
	cv::Point GetPonintByY(int y, Line *l);

	//图像大小
	int imageWidth = 0;
	int imageHeight = 0;
};

