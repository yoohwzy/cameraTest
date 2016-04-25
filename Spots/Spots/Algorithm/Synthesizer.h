#pragma once

#include <opencv2\opencv.hpp>
//引入算法
#include <Algorithm/Base/Block.h>
#include <Algorithm/Base/Faults.h>


//#include <Algorithm/Pretreatment.h>

#include <Class\Debug\MFCConsole.h>

using namespace std;
//算法综合器，用于统筹所有算法代码
class Synthesizer
{
public:
	enum Status
	{
		NoImage = 0,//无图
		NotFound = 1,//未找到瓷砖
		TypeA = 2,//A级
		TypeB = 3,//B级
		TypeC = 4,//C级
		Rejected = 5,//不合格
	};
	Synthesizer(int _SN, int Real_WidthMM, int Real_LengthMM);
	~Synthesizer();
	//传入线阵相机所拍照片（乘以3倍后）
	Status Run(cv::Mat TileImg);


	//编号
	int SN = 0;
	Faults faults;
	Block *p_block = NULL;

	//parameter
	int Real_WidthMM = 600;//图像中瓷砖的横长
	int Real_LengthMM = 300;//图像中瓷砖的纵长
	double BlockLocalizer_THRESHOD = 10;			//边缘查找二值化阈值
	double BlockLocalizer_ContinuePointCount = 30;	//连续多少个点则判断为边缘

	int  BlockEdgeDetector_Enable = 1;
	double BlockEdgeDetector_DIFF_THRESHOLD = 7;	//两图之差超过这个阈值则认为可能有缺陷
	double BlockEdgeDetector_FAULTS_SPAN = 4;		//大于DIFF_THRESHOLD的点附近近相邻FAULTS_SPAN像素内
	double BlockEdgeDetector_FAULTS_COUNT = 5;		//连续几个点大于DIFF_THRESHOLD则判定为缺陷

	int BlockEdgeLineDetector_Enable = 1;
private:


	enum _Status
	{
		_NotFound,
		_Edge_Broken,//崩边
		_NEXT//崩边
	};


	//瓷砖定位，返回是否找到瓷砖
	_Status positioning(cv::Mat grayImg);
	//边缘缺陷检测
	_Status detectEdge(cv::Mat grayImg);
	//内部缺陷检测
	_Status detectInner(cv::Mat grayImg);
};