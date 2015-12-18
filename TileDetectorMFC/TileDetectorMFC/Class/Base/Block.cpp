#include "Block.h"


Block::Block(int _imageWidth, int _imageHeight)
{
	imageWidth = _imageWidth;
	imageHeight = _imageHeight;
}


bool Block::Lines2ABCD()
{
	//A
	if (1 == 1)
	{
		double k1 = UpLine.k;
		double x1 = UpLine.x0;
		double y1 = UpLine.y0;
		double k2 = LeftLine.k;
		double x2 = LeftLine.x0;
		double y2 = LeftLine.y0;
		A.x = (y2 - y1 + k1*x1 - k2*x2) / (k1 - k2);
		A.y = (k2*y1 - k1*y2 + k1*k2*(x2 - x1)) / (k2 - k1);
	}
	//B
	if (1 == 1)
	{
		double k1 = UpLine.k;
		double x1 = UpLine.x0;
		double y1 = UpLine.y0;
		double k2 = RightLine.k;
		double x2 = RightLine.x0;
		double y2 = RightLine.y0;
		B.x = (y2 - y1 + k1*x1 - k2*x2) / (k1 - k2);
		B.y = (k2*y1 - k1*y2 + k1*k2*(x2 - x1)) / (k2 - k1);
	}
	//C
	if (1 == 1)
	{
		double k1 = DownLine.k;
		double x1 = DownLine.x0;
		double y1 = DownLine.y0;
		double k2 = RightLine.k;
		double x2 = RightLine.x0;
		double y2 = RightLine.y0;
		C.x = (y2 - y1 + k1*x1 - k2*x2) / (k1 - k2);
		C.y = (k2*y1 - k1*y2 + k1*k2*(x2 - x1)) / (k2 - k1);
	}
	//D
	if (1 == 1)
	{
		double k1 = DownLine.k;
		double x1 = DownLine.x0;
		double y1 = DownLine.y0;
		double k2 = LeftLine.k;
		double x2 = LeftLine.x0;
		double y2 = LeftLine.y0;
		D.x = (y2 - y1 + k1*x1 - k2*x2) / (k1 - k2);
		D.y = (k2*y1 - k1*y2 + k1*k2*(x2 - x1)) / (k2 - k1);
	}

	if (A.x <= 0 || A.x >= (imageWidth - 1) || A.y <= 0 || A.y >= (imageHeight - 1))
		return false;
	if (B.x <= 0 || B.x >= (imageWidth - 1) || B.y <= 0 || B.y >= (imageHeight - 1))
		return false;
	if (C.x <= 0 || C.x >= (imageWidth - 1) || C.y <= 0 || C.y >= (imageHeight - 1))
		return false;
	if (D.x <= 0 || D.x >= (imageWidth - 1) || D.y <= 0 || D.y >= (imageHeight - 1))
		return false;
	return true;
}
bool Block::ABCD2Lines()
{
	UpLine.k = (double)(B.y - A.y) / (double)(B.x - A.x);
	UpLine.x0 = A.x;
	UpLine.y0 = A.y;

	RightLine.k = (double)(C.y - B.y) / (double)(C.x - B.x);
	RightLine.x0 = B.x;
	RightLine.y0 = B.y;

	DownLine.k = (double)(D.y - C.y) / (double)(D.x - C.x);
	DownLine.x0 = C.x;
	DownLine.y0 = C.y;

	LeftLine.k = (double)(A.y - D.y) / (double)(A.x - D.x);
	LeftLine.x0 = D.x;
	LeftLine.y0 = D.y;

	return true;
}
cv::Point Block::GetPonintByX(int x, Line *l)
{
	cv::Point p;

	p.x = x;
	p.y = (int)(l->k*(x - l->x0)) + l->y0;

	l = NULL;
	return p;
}
cv::Point Block::GetPonintByY(int y, Line *l)
{
	cv::Point p;

	p.y = y;
	p.x = (double)(y - l->y0) / l->k + l->x0;

	l = NULL;
	return p;
}