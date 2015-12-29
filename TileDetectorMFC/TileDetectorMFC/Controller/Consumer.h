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

	//�����־
	bool IsCalibration = false;
	//�Ƿ����ڴ���
	bool IsProcessing = false;


	//������Ƭ����ʼһ���µĴ�������һ�ִ���δ��ɣ�����false
	bool StartNewProces(cv::Mat img);
	//Ϊ���괦��һ��
	bool StartNewProces4Calibraion(cv::Mat img);


	static string GetErrorDescription(int subtype)
	{
		switch (subtype)
		{
		case 1:return "δ��⵽��ש";
		case 2:return "��ש�ڷ���̬����";
		default:
			return "";
		}
	}

	Block *p_block = NULL;

private:
	Measurer *p_measurer = NULL;
	HWND hwnd = NULL;//�����ھ��
	void processingThread();
	void sendMsg(int type, int subtype);
};