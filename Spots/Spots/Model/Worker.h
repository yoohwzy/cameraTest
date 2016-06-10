#pragma once
#include <thread>

//引入算法综合器
#include <Class\Debug\MFCConsole.h>

#include <Algorithm/Base/Block.h>
#include <Algorithm/Base/Faults.h>
#include <Algorithm/Pretreatment.h>//一定要放在这 放到cpp中后会opencv gpu.hpp报错

//#define WORKER_DEBUG

class ControllerModel;// 解决类互相包含


//工人类
//负责当线阵相机采集图像后，进行算法处理的调度
//以及当处理完成后的绘图
class Worker
{
public:
	enum Status
	{
		SizeError = -1,//对角线超标
		NoImage = 0,//无图
		NotFound = 1,//未找到瓷砖
		TypeA = 2,//A级
		TypeB = 3,//B级
		TypeC = 4,//C级
		Rejected = 5,//不合格
	};

	//初始化时若传入指针为NULL，则表示使用虚拟相机。
	Worker(string WorkerInfo = "");
	~Worker();

	enum WorkerStatus{
		Free,
		InProcessing,
	};
	void StartWork();
	
	int MyStatus = WorkerStatus::Free;

	ControllerModel *P_Controller = NULL;
	Faults *P_Faults = NULL;
	Block *P_Block = NULL;
	cv::Mat image;

	// 瓷砖序列号
	int SN = 0;

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
	void drawFaults(cv::Mat&);
	Status algorithmSynthesize(cv::Mat);
	//瓷砖定位，返回是否找到瓷砖
	//1 找到
	//0 未找到
	//-1 有缺陷
	int positioning(cv::Mat grayImg);
	//边缘缺陷检测
	//1 找到
	//-1 有缺陷
	int detectEdge(cv::Mat grayImg);
	//内部缺陷检测
	//1 找到
	//-1 有缺陷
	int detectInner(cv::Mat grayImg);
};