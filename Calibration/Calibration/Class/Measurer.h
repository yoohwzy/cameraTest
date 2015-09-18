#pragma once
#include "../globle.h"
#include "MicroDisplay\MicroDisplayInit.h";
#include "BlocksDetector.h"
#define RADIAN(a)  (a*CV_PI/180.0) //根据角度获得弧度
#define ANGLE(r)	(180.0*r/CV_PI)//根据弧度获得角度

class Measurer
{
private:
	MicroDisplayInit *mdi;
	BlocksDetector *bd_Standard;



	/**************************标准数据***********************/

	//标定砖 矫正后 实际的四个顶点
	cv::Point A_Standard;
	cv::Point B_Standard;
	cv::Point C_Standard;
	cv::Point D_Standard;

	//上边缘标准宽度
	double tileStandardWidth_AB_pix = 0;
	//下边缘标准宽度
	double tileStandardWidth_CD_pix = 0;
	//左边缘标准高度
	double tileStandardWidth_AD_pix = 0;
	//右边缘标准高度
	double tileStandardWidth_BC_pix = 0;
	//对角线1长（AC）
	double tileStandardDiagonalAC_pix = 0;
	//对角线2长（BD）
	double tileStandardDiagonalBD_pix = 0;


	//瓷砖标准宽度
	double tileStandardWidth_mm = 300;
	//瓷砖标准高度
	double tileStandardHeight_mm = 600;

	//一个像素表示多少mm
	double MilliMeterPerPix_Width = 0;
	double MilliMeterPerPix_Height = 0;
	double MilliMeterPerPix_Diagonal = 0;


	//矫正用数据

	//照片映射到实际 长度4096，表示照片X轴上，每个点经标定后应该位于哪个位置。
	vector<double> PincushionMap;
	//实际映射到照片（用以确定实际X位置处，像素的组成）
	vector<double> PincushionMap_Anti;
	//竖直标定的偏移值，在竖直方向上的点，实际y坐标应为y-RowAdjust[x]，或对于某点x,y，其像应在(x,y+RowAdjust[x])处寻找
	vector<int> RowAdjust;

	/**************************标准数据END***********************/

public:


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

	//视线标定 获得标准高度，宽度，1pix代表多少mm
	void ObserveCalibration();
	//根据标定数据
	void ObserveImgAdjust(cv::Mat& Img);
	//计算出瓷砖尺寸，四个角的角度
	void CaculteSize(BlocksDetector *bd);


	/**********************测量数据**********************/
	double AB_Len = 0;
	double CD_Len = 0;
	double AD_Len = 0;
	double BC_Len = 0;

	double& BA_Len = AB_Len;
	double& CB_Len = BC_Len;
	double& DA_Len = AD_Len;
	double& DC_Len = CD_Len;

	//对角线（pix）
	double AC_Len = 0;
	double BD_Len = 0;

	double& CA_Len = AC_Len;
	double& DB_Len = BD_Len;

	double AB_mm = 0;
	double CD_mm = 0;
	double AD_mm = 0;
	double BC_mm = 0;

	double& BA_mm = AB_mm;
	double& DC_mm = CD_mm;
	double& DA_mm = AD_mm;
	double& CB_mm = BC_mm;

	double angleA = 0;
	double angleB = 0;
	double angleC = 0;
	double angleD = 0;
};

