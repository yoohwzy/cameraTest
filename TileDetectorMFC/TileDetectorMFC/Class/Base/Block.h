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

	int imageWidth = 0;
	int imageHeight = 0;
};

