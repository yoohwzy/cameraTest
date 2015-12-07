#pragma once
//@description	����Ϊ�������࣬���ڶ�ͼ����д���
//				��ȷ��һ��ʱ����ֻ��һ�������ڽ���
//@author VShawn
//@last modify date 2015��10��31�� By VShawn
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
	Consumer(HWND _hwnd){
		hwnd = _hwnd;
	}
	~Consumer()
	{
		if (block == NULL)
			delete block;
	};
	Block *block;


	int GrabbingIndex = 0;

	Faults faults;
	//vector<cv::Point3f> EdgeFaults;
	//vector<cv::Point3f> EIDFaults;
	//vector<cv::Point3f> InnerFaults;
	cv::Mat originalImg;
	cv::Mat grayImg;

	//�����־
	bool IsCalibration = false;
	//�Ƿ����ڴ���
	bool IsProcessing = false;

	Measurer *m = NULL;

	//������Ƭ����ʼһ���µĴ�������һ�ִ���δ��ɣ�����false
	bool StartNewProces(cv::Mat img);
	//Ϊ���괦��һ��
	bool StartNewProces4Calibraion(cv::Mat img);
private:
	HWND hwnd;
	void processingThread();

};