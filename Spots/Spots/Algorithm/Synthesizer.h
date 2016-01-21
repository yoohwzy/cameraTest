#pragma once

#include <opencv2\opencv.hpp>
//引入算法
#include <Algorithm/Base/Block.h>
#include <Algorithm/Base/Faults.h>

#include <Algorithm/BlocksDetector.h>
#include <Algorithm/EdgeDetector.h>
#include <Algorithm/Processor.h>
#include <Algorithm/EdgeDetector.h>
#include <Algorithm/EdgeInnerDetctor.h>
#include <Algorithm/Pretreatment.h>

#include <Class\Debug\MFCConsole.h>

using namespace std;
//算法综合器，用于统筹所有算法代码
class Synthesizer
{
public:
	enum Status
	{
		NoImage = 0,//无图
		NotFound = 1,//未找到瓷砖
		TypeA = 2,//A级
		TypeB = 3,//B级
		TypeC = 4,//C级
		Rejected = 5//不合格
	};
	Synthesizer(int _SN);
	~Synthesizer();
	//传入线阵相机所拍照片（乘以3倍后）
	Status Run(cv::Mat linerGrayImg);

	//编号
	int SN = 0;
	//二值化低阈值
	int ConsumerThreshodLow = 5;
	//二值化高阈值
	int ConsumerThreshodHight = 9;

	int ConsumerLedStartX = 0;
	int ConsumerLedEndX = 4095;
private:
	Block *p_block = NULL;
	Faults faults;

	//瓷砖定位，返回是否找到瓷砖
	bool positioning(cv::Mat grayImg);
	//边缘缺陷检测
	void detectEdge(cv::Mat grayImg);
	//内部缺陷检测
	void detectInner(cv::Mat grayImg);
};