#pragma once

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"

struct Line
{
	int x0;
	int y0;
	double k;
	double dx;
	double dy;
};
class Block
{
public:
	Block(int _imageWidth, int _imageHeight);
	~Block();

	cv::Point A, B, C, D;

	Line LeftLine;
	Line RightLine;
	Line UpLine;
	Line DownLine;

	//�����ĸ������ABCD�ĸ��� ���ĸ���λ�ò��ԣ�����false
	bool Lines2ABCD();
	//����ABCD�ĸ�������ĸ���
	bool ABCD2Lines();

	int imageWidth = 0;
	int imageHeight = 0;
};

