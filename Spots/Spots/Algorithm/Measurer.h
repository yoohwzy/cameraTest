#pragma once
#include <Algorithm\Base\Block.h>
class Measurer
{
public:
	Measurer();
	~Measurer();

	//直接计算对角线的像素长度
	static void CalculateDuiJiaoXian(Block b);
	//根据定标数据，求出瓷砖四边长度
	static void Calculate(Block b);
	
	//保证竖直边一定是垂直时调用
	static void BiaoDing(Block b);
private:
};
