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

	//if (A.x < 0 || A.x > (imageWidth - 1) || A.y < 0 || A.y > (imageHeight - 1))
	//	return false;
	//if (B.x < 0 || B.x > (imageWidth - 1) || B.y < 0 || B.y > (imageHeight - 1))
	//	return false;
	//if (C.x < 0 || C.x > (imageWidth - 1) || C.y < 0 || C.y > (imageHeight - 1))
	//	return false;
	//if (D.x < 0 || D.x > (imageWidth - 1) || D.y < 0 || D.y > (imageHeight - 1))
	//	return false;
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



















int Block::Standard_Width_mm = 600;//标准砖横长mm
int Block::Standard_Length_mm = 300;//标准砖纵长mm
double Block::X_mmPerPix = -1;
double Block::Y_mmPerPix = -1;



void Block::CalculateDuiJiaoXian(Block *p_b)
{
	p_b->ab_pix = sqrt((p_b->A.y - p_b->B.y)*(p_b->A.y - p_b->B.y) + (p_b->A.x - p_b->B.x)*(p_b->A.x - p_b->B.x));
	p_b->bc_pix = sqrt((p_b->C.y - p_b->B.y)*(p_b->C.y - p_b->B.y) + (p_b->C.x - p_b->B.x)*(p_b->C.x - p_b->B.x));
	p_b->cd_pix = sqrt((p_b->C.y - p_b->D.y)*(p_b->C.y - p_b->D.y) + (p_b->C.x - p_b->D.x)*(p_b->C.x - p_b->D.x));
	p_b->da_pix = sqrt((p_b->A.y - p_b->D.y)*(p_b->A.y - p_b->D.y) + (p_b->A.x - p_b->D.x)*(p_b->A.x - p_b->D.x));

	p_b->ac_pix = (sqrt(p_b->ab_pix*p_b->ab_pix + p_b->bc_pix*p_b->bc_pix) + sqrt(p_b->da_pix*p_b->da_pix + p_b->cd_pix*p_b->cd_pix)) / 2;
	p_b->bd_pix = (sqrt(p_b->cd_pix*p_b->cd_pix + p_b->bc_pix*p_b->bc_pix) + sqrt(p_b->ab_pix*p_b->ab_pix + p_b->da_pix*p_b->bc_pix)) / 2;
}


void Block::Calculate(Block *p_b)
{
	p_b->ab_pix = sqrt((p_b->A.y - p_b->B.y)*(p_b->A.y - p_b->B.y) + (p_b->A.x - p_b->B.x)*(p_b->A.x - p_b->B.x));
	p_b->bc_pix = sqrt((p_b->C.y - p_b->B.y)*(p_b->C.y - p_b->B.y) + (p_b->C.x - p_b->B.x)*(p_b->C.x - p_b->B.x));
	p_b->cd_pix = sqrt((p_b->C.y - p_b->D.y)*(p_b->C.y - p_b->D.y) + (p_b->C.x - p_b->D.x)*(p_b->C.x - p_b->D.x));
	p_b->da_pix = sqrt((p_b->A.y - p_b->D.y)*(p_b->A.y - p_b->D.y) + (p_b->A.x - p_b->D.x)*(p_b->A.x - p_b->D.x));

	p_b->ac_pix = (sqrt(p_b->ab_pix*p_b->ab_pix + p_b->bc_pix*p_b->bc_pix) + sqrt(p_b->da_pix*p_b->da_pix + p_b->cd_pix*p_b->cd_pix)) / 2;
	p_b->bd_pix = (sqrt(p_b->cd_pix*p_b->cd_pix + p_b->bc_pix*p_b->bc_pix) + sqrt(p_b->ab_pix*p_b->ab_pix + p_b->da_pix*p_b->bc_pix)) / 2;


	//AB边
	if (1 == 1)
	{
		double y_offsetMM = abs(p_b->B.y - p_b->A.y) * Block::Y_mmPerPix;//A B两点间高度落差mm
		if (y_offsetMM > 7)//为什么是7？假设上边长300mm，投影到水平方向后为299.9mm，则AB点高度差应为300平方-299.9平方≈7.7平方
		{
			double d1 = abs(p_b->B.x - p_b->A.x)*Block::X_mmPerPix;
			p_b->ab_mm = sqrt(d1*d1 + y_offsetMM*y_offsetMM);
		}
		else
			p_b->ab_mm = abs(p_b->B.x - p_b->A.x)*Block::X_mmPerPix;
	}
	//CD边
	if (1 == 1)
	{
		double y_offsetMM = abs(p_b->C.y - p_b->D.y) * Block::Y_mmPerPix;
		if (y_offsetMM > 7)//为什么是7？假设上边长300mm，投影到水平方向后为299.9mm，则AB点高度差应为300平方-299.9平方≈7.7平方
		{
			double d1 = abs(p_b->C.x - p_b->D.x)*Block::X_mmPerPix;
			p_b->cd_mm = sqrt(d1*d1 + y_offsetMM*y_offsetMM);
		}
		else
			p_b->cd_mm = abs(p_b->C.x - p_b->D.x)*Block::X_mmPerPix;
	}
	//BC
	if (1 == 1)
	{
		double x_offsetMM = abs(p_b->B.x - p_b->C.x) * Block::X_mmPerPix;
		if (x_offsetMM > 7)
		{
			double d1 = abs(p_b->C.y - p_b->B.y)*Block::Y_mmPerPix;
			p_b->bc_mm = sqrt(d1*d1 + x_offsetMM*x_offsetMM);
		}
		else
			p_b->bc_mm = abs(p_b->C.y - p_b->B.y)*Block::Y_mmPerPix;
	}
	//AD
	if (1 == 1)
	{
		double x_offsetMM = abs(p_b->A.x - p_b->D.x) * Block::X_mmPerPix;
		if (x_offsetMM > 7)
		{
			double d1 = abs(p_b->A.y - p_b->D.y)*Block::Y_mmPerPix;
			p_b->da_mm = sqrt(d1*d1 + x_offsetMM*x_offsetMM);
		}
		else
			p_b->da_mm = abs(p_b->A.y - p_b->D.y)*Block::Y_mmPerPix;
	}

	p_b->ac_mm = (sqrt(p_b->ab_mm*p_b->ab_mm + p_b->bc_mm*p_b->bc_mm) + sqrt(p_b->da_mm*p_b->da_mm + p_b->cd_mm*p_b->cd_mm)) / 2;
	p_b->bd_mm = (sqrt(p_b->cd_mm*p_b->cd_mm + p_b->bc_mm*p_b->bc_mm) + sqrt(p_b->ab_mm*p_b->ab_mm + p_b->da_mm*p_b->bc_mm)) / 2;
}

//保证竖直边一定是垂直时调用
void Block::BiaoDing(Block *p_b)
{
	//计算竖直方向一个像素代表多少毫米
	double bc_pix = (p_b->C.y - p_b->B.y + p_b->D.y - p_b->A.y) / 2;
	Block::Y_mmPerPix = Block::Standard_Length_mm / bc_pix;

	//计算水平方向一个像素代表多少毫米
	double y_offsetMM = abs(p_b->B.y - p_b->A.y) * Block::Y_mmPerPix;//A B两点间高度落差mm
	if (y_offsetMM > 7)//为什么是7？假设上边长300mm，投影到水平方向后为299.9mm，则AB点高度差应为300平方-299.9平方≈7.7平方
	{
		double x_lengthMM = sqrt(Block::Standard_Width_mm*Block::Standard_Width_mm - y_offsetMM*y_offsetMM);
		Block::X_mmPerPix = x_lengthMM / (double)(p_b->B.x - p_b->A.x);
	}
	else
	{
		Block::X_mmPerPix = Block::Standard_Width_mm / (double)(p_b->B.x - p_b->A.x);
	}
}