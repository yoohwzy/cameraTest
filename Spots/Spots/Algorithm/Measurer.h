#pragma once
#include <Algorithm\Base\Block.h>
class Measurer
{
public:
	Measurer();
	~Measurer();

	//ֱ�Ӽ���Խ��ߵ����س���
	void CalculateDuiJiaoXian(Block b);
	//���ݶ������ݣ������ש�ı߳���
	void Calculate(Block b);
	
	//��֤��ֱ��һ���Ǵ�ֱʱ����
	void BiaoDing(Block b);

	//����ʱ��׼ש�ϱ߳�
	double AB_standard_mm = 300;
	//����ʱ��׼ש���߳�
	double BC_standard_mm = 600;
private:
	double duijiaoxian1, duijiaoxian2;
	double bc_mmperpix, ab_mmperpix;
};