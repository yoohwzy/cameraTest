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

	double ac = duijiaoxian1 = (sqrt(ab*ab + bc*bc) + sqrt(da*da + cd*cd)) / 2;
	double bd = duijiaoxian2 = (sqrt(cd*cd + bc*bc) + sqrt(ab*ab + da*bc)) / 2;
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
		double y_offsetMM = abs(b.B.y - b.A.y) * bc_mmperpix;//A B两点间高度落差mm
		if (y_offsetMM > 7)//为什么是7？假设上边长300mm，投影到水平方向后为299.9mm，则AB点高度差应为300平方-299.9平方≈7.7平方
		{
			double d1 = abs(b.B.x - b.A.x)*ab_mmperpix;
			ab_mm = sqrt(d1*d1 + y_offsetMM*y_offsetMM);
		}
		else
			ab_mm = abs(b.B.x - b.A.x)*ab_mmperpix;
	}
	//CD边
	if (1 == 1)
	{
		double y_offsetMM = abs(b.C.y - b.D.y) * bc_mmperpix;
		if (y_offsetMM > 7)//为什么是7？假设上边长300mm，投影到水平方向后为299.9mm，则AB点高度差应为300平方-299.9平方≈7.7平方
		{
			double d1 = abs(b.C.x - b.D.x)*ab_mmperpix;
			cd_mm = sqrt(d1*d1 + y_offsetMM*y_offsetMM);
		}
		else
			cd_mm = abs(b.C.x - b.D.x)*ab_mmperpix;
	}
	//BD
	if (1 == 1)
	{
		double x_offsetMM = abs(b.B.x - b.C.x) * ab_mmperpix;
		if (x_offsetMM > 7)
		{
			double d1 = abs(b.C.y - b.B.y)*bc_mmperpix;
			cd_mm = sqrt(d1*d1 + x_offsetMM*x_offsetMM);
		}
		else
			bc_mm = abs(b.C.y - b.B.y)*bc_mmperpix;
	}
	//AD
	if (1 == 1)
	{
		double x_offsetMM = abs(b.A.x - b.D.x) * ab_mmperpix;
		if (x_offsetMM > 7)
		{
			double d1 = abs(b.A.y - b.D.y)*bc_mmperpix;
			cd_mm = sqrt(d1*d1 + x_offsetMM*x_offsetMM);
		}
		else
			bc_mm = abs(b.A.y - b.D.y)*bc_mmperpix;
	}

	ac_mm = (sqrt(ab_mm*ab_mm + bc_mm*bc_mm) + sqrt(da_mm*da_mm + cd_mm*cd_mm)) / 2;
	bd_mm = (sqrt(cd_mm*cd_mm + bc_mm*bc_mm) + sqrt(ab_mm*ab_mm + da_mm*bc_mm)) / 2;
}

//保证竖直边一定是垂直时调用
void Measurer::BiaoDing(Block b)
{
	//计算竖直方向一个像素代表多少毫米
	double bc_pix = (b.C.y - b.B.y + b.D.y - b.A.y) / 2;
	bc_mmperpix = BC_standard_mm / bc_pix;

	//计算水平方向一个像素代表多少毫米
	double y_offsetMM = abs(b.B.y - b.A.y) * bc_mmperpix;//A B两点间高度落差mm
	if (y_offsetMM > 7)//为什么是7？假设上边长300mm，投影到水平方向后为299.9mm，则AB点高度差应为300平方-299.9平方≈7.7平方
	{
		double x_lengthMM = sqrt(AB_standard_mm*AB_standard_mm - y_offsetMM*y_offsetMM);
		ab_mmperpix = x_lengthMM / (double)(b.B.x - b.A.x);
	}
	else
	{
		ab_mmperpix = AB_standard_mm / (double)(b.B.x - b.A.x);
	}
}
