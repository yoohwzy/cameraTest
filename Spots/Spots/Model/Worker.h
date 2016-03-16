#pragma once
#include "ImgScanner.h"
#include <Class/Camera/e2v_EV71YC1CCL4005BA0/E2VCamera.h>
#include <thread>

//引入算法综合器
#include <Algorithm\Synthesizer.h>
#include <Class\Debug\MFCConsole.h>

extern class Controller;
class Worker
{
public:
	//需要设置的参数
	int WaitTimeMS = 50;//等待瓷砖进入拍摄区的时间

	// 瓷砖序列号
	int SN = 0;


	//初始化时若传入指针为NULL，则表示使用虚拟相机。
	Worker(E2VBuffer *_e2vbuffer = NULL);
	~Worker();

	enum WorkerStatus{
		Free,
		Busy,
		Done
	};
	void StartWork();
	
	int MyStatus = WorkerStatus::Free;

	// 为true时采集图像，转为false时结束采集
	// 仅当length = 0时有效，用于确定采集何时结束
	bool GetPhotoOn = false;
	cv::Mat image;

	Controller *P_Controller = NULL;

	//edge 参数
	int BlockLocalizer_THRESHOD = 10;			//边缘查找二值化阈值
	int BlockLocalizer_ContinuePointCount = 30;	//连续多少个点则判断为边缘

	int BlockEdgeDetector_DIFF_THRESHOLD = 7;	//两图之差超过这个阈值则认为可能有缺陷
	int BlockEdgeDetector_FAULTS_SPAN = 4;		//大于DIFF_THRESHOLD的点附近近相邻FAULTS_SPAN像素内
	int BlockEdgeDetector_FAULTS_COUNT = 5;		//连续几个点大于DIFF_THRESHOLD则判定为缺陷

private:
	E2VBuffer *p_e2vbuffer = NULL;
	// 直接读取N张图片
	void work();
	// 从循环缓存中取出图片
	// 开始采集的行，采集行数，为0时等待下降沿或采集行数超过frameCountsOut
	cv::Mat getPhoto(int startFrame, int length = 0);

	// 绘制缺陷
	void drawFaults(cv::Mat&, Faults& faults);

	// 超过该长度直接退出
	static const int frameCountsOut = 12000;
	
	void frameIndexAdd(int& oldFrame, int add)
	{
		oldFrame += add;
		if (oldFrame >= E2VBuffer::BufferLength)
			oldFrame -= E2VBuffer::BufferLength;
	}
};