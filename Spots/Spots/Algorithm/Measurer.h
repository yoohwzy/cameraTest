#pragma once
#include <Algorithm\Base\Block.h>
class Measurer
{
public:
	Measurer();
	~Measurer();
	void CalculateDuiJiaoXian(Block b);
	
	//保证竖直边一定是垂直时调用
	void BiaoDing(Block b);

	double AB_mm = 300;
	double BC_mm = 600;
private:
	double duijiaoxian1, duijiaoxian2;
	double bc_mmperpix, ab_mmperpix;
};