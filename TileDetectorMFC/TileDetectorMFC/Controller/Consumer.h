#pragma once
//@description	本类为消费者类，用于对图像进行处理
//				需确保一个时间内只有一个处理在进行
//@author VShawn
//@last modify date 2015年10月31日 By VShawn
#include "../globle_head.h"
#include "../globle_debug.h"

#include "../Class/BlocksDetector.h"
#include "../Class/EdgeDetector.h"
#include "../Class/Processor.h"
#include "../Class/Measurer.h"
#include "../Class/EdgeDetector.h"

#include "../Class/EdgeInnerDetctor.h"

#include "../Class/Pretreatment.h"


class Consumer
{
public:
	Consumer(HWND _hwnd){
		hwnd = _hwnd;
	}
	~Consumer()
	{
		if (block == NULL)
			delete block;
	};
	Block *block;

	//传入照片，开始一场新的处理，若上一轮处理还未完成，返回false
	bool StartNewProces(cv::Mat img);
	int GrabbingIndex = 0;

	vector<cv::Point3f> EdgeFaults;
	cv::Mat originalImg;

	//定标
	bool IsCalibration = false;
	Measurer *m;
	void Process4Calibraion();
private:
	HWND hwnd;
	void processingThread();

	bool isProcessing = false;
};