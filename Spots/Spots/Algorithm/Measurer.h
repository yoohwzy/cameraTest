#pragma once
#include <Algorithm\Base\Block.h>
class Measurer
{
public:
	Measurer();
	~Measurer();

	//直接计算对角线的像素长度
	void CalculateDuiJiaoXian(Block b);
	//根据定标数据，求出瓷砖四边长度
	void Calculate(Block b);
	
	//保证竖直边一定是垂直时调用
	void BiaoDing(Block b);

	//定标时标准砖上边长
	double AB_standard_mm = 300;
	//定标时标准砖两边长
	double BC_standard_mm = 600;
private:
	double duijiaoxian1, duijiaoxian2;
	double bc_mmperpix, ab_mmperpix;
};