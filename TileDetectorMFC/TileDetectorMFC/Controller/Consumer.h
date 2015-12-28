#pragma once
//@description	本类为消费者类，用于对图像进行处理
//				需确保一个时间内只有一个处理在进行
//@author VShawn
//@last modify date 2015年10月31日 By VShawn
#include "../globle_head.h"
#include "../globle_debug.h"

#include "../Class/Base/Faults.h"

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
	Consumer(HWND _hwnd);
	~Consumer()
	{
		if (p_block == NULL)
		{
			delete p_block;
			p_block = NULL;
		}
		if (p_measurer != NULL)
		{
			delete p_measurer;
			p_measurer = NULL;
		}
		hwnd = NULL;
	};


public:
	int GrabbingIndex = 0;
	int ConsumerThreshodLow = 5;
	int ConsumerThreshodHight = 9;
	int ConsumerLedStartX = 0;
	int ConsumerLedEndX = 4095;
	Faults faults;
	//vector<cv::Point3f> EdgeFaults;
	//vector<cv::Point3f> EIDFaults;
	//vector<cv::Point3f> InnerFaults;
	cv::Mat originalImg;
	cv::Mat grayImg;

	//定标标志
	bool IsCalibration = false;
	//是否正在处理
	bool IsProcessing = false;


	//传入照片，开始一场新的处理，若上一轮处理还未完成，返回false
	bool StartNewProces(cv::Mat img);
	//为定标处理一次
	bool StartNewProces4Calibraion(cv::Mat img);


	static string GetErrorDescription(int subtype)
	{
		switch (subtype)
		{
		case 1:return "未检测到瓷砖";
		case 2:return "瓷砖摆放姿态错误";
		default:
			return "";
		}
	}

	Block *p_block = NULL;

private:
	Measurer *p_measurer = NULL;
	HWND hwnd = NULL;//父窗口句柄
	void processingThread();
	void sendMsg(int type, int subtype);
};