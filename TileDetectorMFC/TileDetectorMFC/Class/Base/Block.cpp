#include "Block.h"


Block::Block(int _imageWidth, int _imageHeight)
{
	imageWidth = _imageWidth;
	imageHeight = _imageHeight;
}


Block::~Block()
{
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
	return false;
}