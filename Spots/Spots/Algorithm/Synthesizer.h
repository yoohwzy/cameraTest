#pragma once

//#include "../globle_debug.h"
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

using namespace std;
//算法综合器，用于统筹所有算法代码
class Synthesizer
{
public:
	Synthesizer();
	~Synthesizer();
	//传入线阵相机所拍照片（乘以3倍后）
	void Run(cv::Mat linerGrayImg);

	//编号
	int SN = 0;
	//二值化低阈值
	int ConsumerThreshodLow = 5;
	//二值化高阈值
	int ConsumerThreshodHight = 9;

	int ConsumerLedStartX = 0;
	int ConsumerLedEndX = 4095;
private:
	Block *block = NULL;
	Faults faults;

	//瓷砖定位
	void positioning();
	//边缘缺陷检测
	void detectEdge();
	//内部缺陷检测
	void detectInner();
};