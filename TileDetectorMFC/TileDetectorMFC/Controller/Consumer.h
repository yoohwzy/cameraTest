#pragma once
//@description	����Ϊ�������࣬���ڶ�ͼ����д���
//				��ȷ��һ��ʱ����ֻ��һ�������ڽ���
//@author VShawn
//@last modify date 2015��10��31�� By VShawn
#include "../globle_head.h"
#include "../globle_debug.h"

#include "../Class/BlocksDetector.h"
#include "../Class/EdgeDetector.h"
#include "../Class/Processor.h"
#include "../Class/Measurer.h"
#include "../Class/EdgeDetector.h"
class Consumer
{
public:
	Consumer(HWND _hwnd){
		hwnd = _hwnd;
	}
	~Consumer(){};

	//������Ƭ����ʼһ���µĴ�������һ�ִ���δ��ɣ�����false
	bool StartNewProces(cv::Mat img);
	int GrabbingIndex = 0;

	vector<cv::Point3f> EdgeFaults;
	cv::Mat originalImg;


private:
	HWND hwnd;
	void processingThread();

	bool isProcessing = false;
};