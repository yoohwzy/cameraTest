#pragma once
#include <Algorithm\Base\Block.h>
class Measurer
{
public:
	Measurer();
	~Measurer();
	void CalculateDuiJiaoXian(Block b);
	
	//��֤��ֱ��һ���Ǵ�ֱʱ����
	void BiaoDing(Block b);

	double AB_mm = 300;
	double BC_mm = 600;
private:
	double duijiaoxian1, duijiaoxian2;
	double bc_mmperpix, ab_mmperpix;
};