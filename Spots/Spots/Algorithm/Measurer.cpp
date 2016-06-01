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
//保证竖直边一定是垂直时调用
void Measurer::BiaoDing(Block b)
{
	//计算竖直方向一个像素代表多少毫米
	double bc_pix = (b.C.y - b.B.y + b.D.y - b.A.y) / 2;
	bc_mmperpix = BC_mm / bc_pix;

	//计算水平方向一个像素代表多少毫米
	if (abs(b.B.y - b.A.y) * bc_mmperpix > 24)//为什么是24？假设上边长300mm，投影到水平方向后为299mm，则AB点高度差应为300平方-299平方≈24平方
	{
		 
	}
	else
	{
		ab_mmperpix = AB_mm / (double)(b.B.x - b.A.x);
	}
}
