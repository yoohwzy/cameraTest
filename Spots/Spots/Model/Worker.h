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



	//需要设置的参数
	int Real_WidthMM = 600;//图像中瓷砖的横长
	int Real_LengthMM = 300;//图像中瓷砖的纵长
	int WaitTimeMSIn = 50;//等待瓷砖进入拍摄区的时间
	int WaitTimeMSOut = 50;//等待瓷砖离开拍摄区的时间
	int FrameTimeOut = 2000;//拍摄超时时间
	// 瓷砖序列号
	int SN = 0;

	//edge 参数

	double BlockLocalizer_THRESHOD = 10;			//边缘查找二值化阈值
	double BlockLocalizer_ContinuePointCount = 30;	//连续多少个点则判断为边缘

	int  BlockEdgeDetector_Enable = 1;
	double BlockEdgeDetector_DIFF_THRESHOLD = 7;	//两图之差超过这个阈值则认为可能有缺陷
	double BlockEdgeDetector_FAULTS_SPAN = 4;		//大于DIFF_THRESHOLD的点附近近相邻FAULTS_SPAN像素内
	double BlockEdgeDetector_FAULTS_COUNT = 5;		//连续几个点大于DIFF_THRESHOLD则判定为缺陷

	int BlockEdgeLineDetector_Enable = 1;

	/**************分级参数*************/

	//边参数
	double Classify_EDGE_SINGLE_LENGTH_A = 0;
	double Classify_EDGE_SINGLE_LENGTH_B = 0;
	double Classify_EDGE_SINGLE_LENGTH_C = 0;
	double Classify_EDGE_SINGLE_LENGTH_ACCEPT = 0;

	double Classify_EDGE_TOTAL_LENGTH_A = 0;
	double Classify_EDGE_TOTAL_LENGTH_B = 0;
	double Classify_EDGE_TOTAL_LENGTH_C = 0;
	double Classify_EDGE_TOTAL_LENGTH_ACCEPT = 0;

	double Classify_EDGE_SINGLE_DEEP_A = 0;
	double Classify_EDGE_SINGLE_DEEP_B = 0;
	double Classify_EDGE_SINGLE_DEEP_C = 0;
	double Classify_EDGE_SINGLE_DEEP_ACCEPT = 0;

	double Classify_EDGE_TOTAL_DEEP_A = 0;
	double Classify_EDGE_TOTAL_DEEP_B = 0;
	double Classify_EDGE_TOTAL_DEEP_C = 0;
	double Classify_EDGE_TOTAL_DEEP_ACCEPT = 0;

	//凹坑参数
	double Classify_HOAL_DIAMETER_A = 0;
	double Classify_HOAL_DIAMETER_B = 0;
	double Classify_HOAL_DIAMETER_C = 0;
	double Classify_HOAL_DIAMETER_ACCEPT = 0;

	double Classify_HOAL_COUNT_A = 0;
	double Classify_HOAL_COUNT_B = 0;
	double Classify_HOAL_COUNT_C = 0;
	double Classify_HOAL_COUNT_ACCEPT = 0;


	//划痕
	double Classify_SCRATCH_SINGLE_LENGTH_A = 0;
	double Classify_SCRATCH_SINGLE_LENGTH_B = 0;
	double Classify_SCRATCH_SINGLE_LENGTH_C = 0;
	double Classify_SCRATCH_SINGLE_LENGTH_ACCEPT = 0;

	double Classify_SCRATCH_TOTAL_LENGTH_A = 0;
	double Classify_SCRATCH_TOTAL_LENGTH_B = 0;
	double Classify_SCRATCH_TOTAL_LENGTH_C = 0;
	double Classify_SCRATCH_TOTAL_LENGTH_ACCEPT = 0;


private:
	//double axis_x_mmPerPix = 1;//x轴方向上每像素代表多少毫米
	//double axis_y_mmPerPix = 1;//y轴方向上每像素代表多少毫米

	E2VBuffer *p_e2vbuffer = NULL;
	// 直接读取N张图片
	void work();
	// 从循环缓存中取出图片
	// 开始采集的行，采集行数，为0时等待下降沿或采集行数超过frameCountsOut
	cv::Mat getPhoto(int startFrame, int length = 0);

	// 绘制缺陷
	void drawFaults(cv::Mat&, Faults& faults);

	void frameIndexAdd(int& oldFrame, int add)
	{
		oldFrame += add;
		if (oldFrame >= E2VBuffer::BufferLength)//exp:5600+400=6000 >= 6000 -> 6000 - 6000 =0
			oldFrame -= E2VBuffer::BufferLength;
	}
};