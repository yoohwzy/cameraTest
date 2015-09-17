#pragma once
#include "../globle.h"
#include "MicroDisplay\MicroDisplayInit.h";
#include "BlocksDetector.h"

class Measurer
{
private:
	MicroDisplayInit *mdi;
	BlocksDetector *bd_Standard;

	//标定砖 矫正后 实际的四个顶点
	cv::Point A_Standard;
	cv::Point B_Standard;
	cv::Point C_Standard;
	cv::Point D_Standard;

public:
	//上边缘标准宽度
	double tileStandardWidthUp_pix = 0;
	//下边缘标准宽度
	double tileStandardWidthDown_pix = 0;
	//左边缘标准高度
	double tileStandardHeightLeft_pix = 0;
	//右边缘标准高度
	double tileStandardHeightRight_pix = 0;
	//对角线1长（AC）
	double tileStandardDiagonalAC_pix = 0;
	//对角线2长（BD）
	double tileStandardDiagonalBD_pix = 0;


	//瓷砖标准宽度
	double tileStandardWidth_mm = 0;
	//瓷砖标准高度
	double tileStandardHeight_mm = 0;



	//计算初始化 瓷砖宽度单位为mm
	Measurer(BlocksDetector *b, MicroDisplayInit *mdii, int TileStandardWidthmm, int TileStandardHeightmm);
	//读取数据初始化 后两参数表示一个像素表示多少毫米
	Measurer(BlocksDetector *b, MicroDisplayInit *mdii, double MilliMeterPerPix_Width, double MilliMeterPerPix_Height);
	~Measurer();


	/*//桶形失真
	void BarrelDistortion(cv::Mat calibrationImg);*/

	//枕形畸变标定，生成一行每个像素点所对应的实际坐标，输入为标定用的黑白格子图像。
	void PincushionCalibration(cv::Mat calibrationImg);
	void PincushionImgAdjust(cv::Mat& Img);
	//照片映射到实际 长度4096，表示照片X轴上，每个点经标定后应该位于哪个位置。
	vector<double> PincushionMap;
	//实际映射到照片（用以确定实际X位置处，像素的组成）
	vector<double> PincushionMap_Anti;


	//视线标定 获得标准高度，宽度，1pix代表多少mm
	void ObserveCalibration();
	//根据标定数据
	void ObserveImgAdjust(cv::Mat& Img);
	vector<int> RowAdjust;


	void CaculteSize(BlocksDetector *bd);


	double MilliMeterPerPix_Width = 0;
	double MilliMeterPerPix_Height = 0;
	double MilliMeterPerPix_Diagonal = 0;
};

