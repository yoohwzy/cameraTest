#pragma once


#include <Class/Camera/e2v_EV71YC1CCL4005BA0/E2VBuffer.h>
#include <Class/IOCard/PCI1761/PCI1761.h>
#include <Class/Statistics/Statistics.h>

#include <View/SpotsMainView.h>

#include <opencv2/opencv.hpp>
#include <queue>
#include <thread>
#include <mutex>

#include <Model\Worker.h>
#include <Model\LogImgGenerator.h>


using namespace std;

class Controller
{
protected:
	SpotsMainView*  spotsMainView = NULL;

public:
	Controller(SpotsMainView* _spotsMainView) :	spotsMainView(_spotsMainView)
	{
		//ATLASSERT(spotsMainView);
		spotsMainView->AddController(this);

		//init();
	}
	~Controller()
	{
		exitFlag = true;
		Sleep(10);

		release();
	}
	bool IsRealModel = 1;

	void init();

	void StartWatch();
	void StopWatch();


	//处理结束后显示处理结果
	//结果图像
	//产品类型，1A 2B 3C 4Rejcet
	void ShowWorkResult(cv::Mat image,int type)
	{
		ui_lock.lock();
		switch (type)
		{
		case 1:Statistics::AddTodayA(); break;
		case 2:Statistics::AddTodayB(); break;
		case 3:Statistics::AddTodayC(); break;
		case 4:Statistics::AddTodayRejected(); break;
		default:break;
		}

		spotsMainView->ShowBigImg(image);
		logImg.AddItem(image,"test");
		spotsMainView->ShowLogImg(logImg.DrawingBoard);
		spotsMainView->UpdateStatistics();
		ui_lock.unlock();
	}
	void ImageGetCallBack(cv::Mat);//采图完成后回调，用于将图片保存至硬盘

	// 在设置中调整了系统参数后，调用本接口将参数更新至算法中。
	void ResetParameter();
	void LoadParameterFromIni();

	/*****************虚拟相机模式方法*****************/
	void VirtualSelectImg(cv::Mat);
	void VirtualWorkerStart();

	/*****************虚拟相机模式方法 End*****************/



	/**********************************/
	/*                                */
	/*                参数            */
	/*                                */
	/**********************************/

	int Real_WidthMM = 600;//图像中瓷砖的横长
	int Real_LengthMM = 300;//图像中瓷砖的纵长
	int Worker_WaitTimeMSIn = 100;
	int Worker_WaitTimeMSOut = 250;
	int Worker_FrameTimeOut = 2000;


	/******************边缘缺陷参数***************/
	double BlockLocalizer_THRESHOD = 10;			//边缘查找二值化阈值
	double BlockLocalizer_ContinuePointCount = 30;	//连续多少个点则判断为边缘

	int  BlockEdgeDetector_Enable = 1;
	double BlockEdgeDetector_DIFF_THRESHOLD = 0.4;	//两图之差超过这个阈值则认为可能有缺陷
	double BlockEdgeDetector_FAULTS_SPAN = 4;		//大于DIFF_THRESHOLD的点附近近相邻FAULTS_SPAN像素内
	double BlockEdgeDetector_FAULTS_COUNT = 5;		//连续几个点大于DIFF_THRESHOLD则判定为缺陷

	int BlockEdgeLineDetector_Enable = 1;			//
	int BlockEdgeLineDetector_BINARY_THRESHOD = 5;	//二值化阈值
	int BlockEdgeLineDetector_LENGTH_THRESHOD = 5;	//超过这个长度才认为是缺陷（pix）
	int BlockEdgeLineDetector_DEEP_THRESHOD = 5;	//超过这个深度才认为是缺陷（pix）

	bool SAVE_IMG = false;


	/**************分级参数*************/

	//边参数
	double Classify_EDGE_SINGLE_LENGTH_A = -1;
	double Classify_EDGE_SINGLE_LENGTH_B = -1;
	double Classify_EDGE_SINGLE_LENGTH_C = -1;
	double Classify_EDGE_SINGLE_LENGTH_ACCEPT = -1;

	double Classify_EDGE_TOTAL_LENGTH_A = -1;
	double Classify_EDGE_TOTAL_LENGTH_B = -1;
	double Classify_EDGE_TOTAL_LENGTH_C = -1;
	double Classify_EDGE_TOTAL_LENGTH_ACCEPT = -1;

	double Classify_EDGE_SINGLE_DEEP_A = -1;
	double Classify_EDGE_SINGLE_DEEP_B = -1;
	double Classify_EDGE_SINGLE_DEEP_C = -1;
	double Classify_EDGE_SINGLE_DEEP_ACCEPT = -1;

	double Classify_EDGE_TOTAL_DEEP_A = -1;
	double Classify_EDGE_TOTAL_DEEP_B = -1;
	double Classify_EDGE_TOTAL_DEEP_C = -1;
	double Classify_EDGE_TOTAL_DEEP_ACCEPT = -1;

	//凹坑参数
	double Classify_HOAL_DIAMETER_A = -1;
	double Classify_HOAL_DIAMETER_B = -1;
	double Classify_HOAL_DIAMETER_C = -1;
	double Classify_HOAL_DIAMETER_ACCEPT = -1;

	double Classify_HOAL_COUNT_A = -1;
	double Classify_HOAL_COUNT_B = -1;
	double Classify_HOAL_COUNT_C = -1;
	double Classify_HOAL_COUNT_ACCEPT = -1;


	//划痕
	double Classify_SCRATCH_SINGLE_LENGTH_A = -1;
	double Classify_SCRATCH_SINGLE_LENGTH_B = -1;
	double Classify_SCRATCH_SINGLE_LENGTH_C = -1;
	double Classify_SCRATCH_SINGLE_LENGTH_ACCEPT = -1;

	double Classify_SCRATCH_TOTAL_LENGTH_A = -1;
	double Classify_SCRATCH_TOTAL_LENGTH_B = -1;
	double Classify_SCRATCH_TOTAL_LENGTH_C = -1;
	double Classify_SCRATCH_TOTAL_LENGTH_ACCEPT = -1;


private:
	void release();
	//HWND handle_mainWindow;

	E2VBuffer *p_e2vbuffer = NULL;
	ImgScanner *p_imgscanner = NULL;

	PCI1761 pci1761;
	LogImgGenerator logImg;

	//同时只允许两个工人工作，即只能两张图（两块砖）

	//最后一个被召唤的工人
	Worker *lastestWorker = NULL;
	//工人1
	Worker *worker1 = NULL;
	//工人2
	Worker *worker2 = NULL;


	std::mutex ui_lock;
	std::mutex watcher_lock;
	std::mutex image_write_lock;

	bool exitFlag = false;//triggerWatcher 结束标志
	void triggerWatcher();
	//void jobWatcher();	
	//void workerCallBack();	//算法结束后回调，显示运算结果
	void imageSave(cv::Mat img);
};