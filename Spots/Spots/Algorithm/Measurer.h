#pragma once
#include <Algorithm\Base\Block.h>
class Measurer
{
public:
	Measurer();
	~Measurer();

	//ֱ�Ӽ���Խ��ߵ����س���
	static void CalculateDuiJiaoXian(Block b);
	//���ݶ������ݣ������ש�ı߳���
	static void Calculate(Block b);
	
	//��֤��ֱ��һ���Ǵ�ֱʱ����
	static void BiaoDing(Block b);
private:
};
