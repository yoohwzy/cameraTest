#pragma once
#include <thread>

//引入算法综合器
#include <Algorithm\Synthesizer.h>
#include <Class\Debug\MFCConsole.h>


//#define WORKER_DEBUG

class ControllerModel;// 解决类互相包含


//工人类
//负责当线阵相机采集图像后，进行算法处理的调度
//以及当处理完成后的绘图
class Worker
{
public:
	//初始化时若传入指针为NULL，则表示使用虚拟相机。
	Worker(string WorkerInfo = "");
	~Worker();

	enum WorkerStatus{
		Free,
		InProcessing,
	};
	void StartWork();
	
	int MyStatus = WorkerStatus::Free;
	cv::Mat image;

	ControllerModel *P_Controller = NULL;


	// 瓷砖序列号
	int SN = 0;

	//edge 参数

	//double BlockLocalizer_THRESHOD = 10;			//边缘查找二值化阈值
	//double BlockLocalizer_ContinuePointCount = 30;	//连续多少个点则判断为边缘

	//int BlockEdgeLineDetector_Enable = 1;			//
	//int BlockEdgeLineDetector_BINARY_THRESHOD = 5;	//二值化阈值
	//int BlockEdgeLineDetector_LENGTH_THRESHOD = 5;	//超过这个长度才认为是缺陷（pix）
	//int BlockEdgeLineDetector_DEEP_THRESHOD = 5;	//超过这个深度才认为是缺陷（pix）

	//int  BlockEdgeDetector_Enable = 1;
	//double BlockEdgeDetector_DIFF_THRESHOLD = 0.4;	//两点差值百分比超过这个阈值则认为可能有缺陷
	//double BlockEdgeDetector_FAULTS_SPAN = 4;		//大于DIFF_THRESHOLD的点附近近相邻FAULTS_SPAN像素内
	//double BlockEdgeDetector_FAULTS_COUNT = 5;		//连续几个点大于DIFF_THRESHOLD则判定为缺陷

	/**************内部参数****************/
	int Pretreatment_Enable = 1;

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
	string workerInfo = "";
	//double axis_x_mmPerPix = 1;//x轴方向上每像素代表多少毫米
	//double axis_y_mmPerPix = 1;//y轴方向上每像素代表多少毫米

	// 直接读取N张图片
	void work();
	// 从循环缓存中取出图片
	// 开始采集的行，采集行数，为0时等待下降沿或采集行数超过frameCountsOut
	cv::Mat getPhoto(int startFrame, int length = 0);

	// 绘制缺陷
	void drawFaults(cv::Mat&, Faults& faults);
};