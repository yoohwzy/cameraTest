#pragma once


#include <Class/Camera/e2v_EV71YC1CCL4005BA0/E2VBuffer.h>
#include <Class/IOCard/PCI1761/PCI1761.h>
#include <Class/Statistics/Statistics.h>
#include <Class/Debug/MFCConsole.h>

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


	/*****************虚拟相机模式方法*****************/
	void VirtualSelectImg(cv::Mat);
	void VirtualWorkerStart();

	/*****************虚拟相机模式方法 End*****************/


	/*****************参数*****************/
	int BlockLocalizer_THRESHOD = 10;			//边缘查找二值化阈值
	int BlockLocalizer_ContinuePointCount = 30;	//连续多少个点则判断为边缘

	int BlockEdgeDetector_DIFF_THRESHOLD = 7;	//两图之差超过这个阈值则认为可能有缺陷
	int BlockEdgeDetector_FAULTS_SPAN = 4;		//大于DIFF_THRESHOLD的点附近近相邻FAULTS_SPAN像素内
	int BlockEdgeDetector_FAULTS_COUNT = 5;		//连续几个点大于DIFF_THRESHOLD则判定为缺陷

	bool SAVE_IMG = false;

private:
	void release();
	//HWND handle_mainWindow;
	bool isRealModel = 1;

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