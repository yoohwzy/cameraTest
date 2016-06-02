#include "Measurer.h"
#include <math.h>

Measurer::Measurer()
{
}


Measurer::~Measurer()
{
}

void Measurer::CalculateDuiJiaoXian(Block b)
{
	double ab = sqrt((b.A.y - b.B.y)*(b.A.y - b.B.y) + (b.A.x - b.B.x)*(b.A.x - b.B.x));
	double bc = sqrt((b.C.y - b.B.y)*(b.C.y - b.B.y) + (b.C.x - b.B.x)*(b.C.x - b.B.x));
	double cd = sqrt((b.C.y - b.D.y)*(b.C.y - b.D.y) + (b.C.x - b.D.x)*(b.C.x - b.D.x));
	double da = sqrt((b.A.y - b.D.y)*(b.A.y - b.D.y) + (b.A.x - b.D.x)*(b.A.x - b.D.x));

	double ac = (sqrt(ab*ab + bc*bc) + sqrt(da*da + cd*cd)) / 2;
	double bd = (sqrt(cd*cd + bc*bc) + sqrt(ab*ab + da*bc)) / 2;
}


void Measurer::Calculate(Block b)
{
	double ab_mm;
	double bc_mm;
	double cd_mm;
	double da_mm;
	double ac_mm;
	double bd_mm;
	//AB边
	if (1 == 1)
	{
		double y_offsetMM = abs(b.B.y - b.A.y) * Block::Y_mmPerPix;//A B两点间高度落差mm
		if (y_offsetMM > 7)//为什么是7？假设上边长300mm，投影到水平方向后为299.9mm，则AB点高度差应为300平方-299.9平方≈7.7平方
		{
			double d1 = abs(b.B.x - b.A.x)*Block::X_mmPerPix;
			ab_mm = sqrt(d1*d1 + y_offsetMM*y_offsetMM);
		}
		else
			ab_mm = abs(b.B.x - b.A.x)*Block::X_mmPerPix;
	}
	//CD边
	if (1 == 1)
	{
		double y_offsetMM = abs(b.C.y - b.D.y) * Block::Y_mmPerPix;
		if (y_offsetMM > 7)//为什么是7？假设上边长300mm，投影到水平方向后为299.9mm，则AB点高度差应为300平方-299.9平方≈7.7平方
		{
			double d1 = abs(b.C.x - b.D.x)*Block::X_mmPerPix;
			cd_mm = sqrt(d1*d1 + y_offsetMM*y_offsetMM);
		}
		else
			cd_mm = abs(b.C.x - b.D.x)*Block::X_mmPerPix;
	}
	//BD
	if (1 == 1)
	{
		double x_offsetMM = abs(b.B.x - b.C.x) * Block::X_mmPerPix;
		if (x_offsetMM > 7)
		{
			double d1 = abs(b.C.y - b.B.y)*Block::Y_mmPerPix;
			cd_mm = sqrt(d1*d1 + x_offsetMM*x_offsetMM);
		}
		else
			bc_mm = abs(b.C.y - b.B.y)*Block::Y_mmPerPix;
	}
	//AD
	if (1 == 1)
	{
		double x_offsetMM = abs(b.A.x - b.D.x) * Block::X_mmPerPix;
		if (x_offsetMM > 7)
		{
			double d1 = abs(b.A.y - b.D.y)*Block::Y_mmPerPix;
			cd_mm = sqrt(d1*d1 + x_offsetMM*x_offsetMM);
		}
		else
			bc_mm = abs(b.A.y - b.D.y)*Block::Y_mmPerPix;
	}

	ac_mm = (sqrt(ab_mm*ab_mm + bc_mm*bc_mm) + sqrt(da_mm*da_mm + cd_mm*cd_mm)) / 2;
	bd_mm = (sqrt(cd_mm*cd_mm + bc_mm*bc_mm) + sqrt(ab_mm*ab_mm + da_mm*bc_mm)) / 2;
}

//保证竖直边一定是垂直时调用
void Measurer::BiaoDing(Block b)
{
	//计算竖直方向一个像素代表多少毫米
	double bc_pix = (b.C.y - b.B.y + b.D.y - b.A.y) / 2;
	Block::Y_mmPerPix = Block::Standard_Length_mm / bc_pix;

	//计算水平方向一个像素代表多少毫米
	double y_offsetMM = abs(b.B.y - b.A.y) * Block::Y_mmPerPix;//A B两点间高度落差mm
	if (y_offsetMM > 7)//为什么是7？假设上边长300mm，投影到水平方向后为299.9mm，则AB点高度差应为300平方-299.9平方≈7.7平方
	{
		double x_lengthMM = sqrt(Block::Standard_Width_mm*Block::Standard_Width_mm - y_offsetMM*y_offsetMM);
		Block::X_mmPerPix = x_lengthMM / (double)(b.B.x - b.A.x);
	}
	else
	{
		Block::X_mmPerPix = Block::Standard_Width_mm / (double)(b.B.x - b.A.x);
	}
}
