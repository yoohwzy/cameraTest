
#pragma once
#include "../stdafx.h"

#ifndef CV_BGR2GRAY
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#endif

//用于存放检测到的缺陷，包含了所有类型
class Faults
{
	//缺边
	struct BrokenEdge
	{
		cv::Point position;//缺角位置
		double deep;//进边深度（毫米）
		double length;//缺边长（毫米）
	};
	//缺角
	struct BrokenCorner
	{
		cv::Point position;//缺角位置
		double deep;//进边深度（毫米）
		double length;//缺角长y轴（毫米）
		double width;//缺角宽x轴（毫米）
	};
	//釉裂
	struct Crazing
	{
		cv::Point position;//缺角位置
		double diameter;//直径（毫米）
	};
	//凹点
	struct Hole
	{
		cv::Point position;//凹点位置
		double diameter;//直径（毫米）
	};
	//划痕
	struct Scratch
	{
		cv::Point position;//位置
		double length;//缺角长y轴（毫米）
		double angel;//直径（毫米）
	};



public:


	/************************方法************************/

	//清空上一轮的错误信息
	void Clear(){
		BrokenEdges.clear();//缺边
		BrokenCorners.clear();//缺角
		Crazings.clear();//釉裂
		Holes.clear();//凹点
		Scratchs.clear();//划痕
	};

	//设置参数，输入：x轴每像素对应多少毫米，y轴每像素对应多少毫米，对角线每像素对应多少毫米
	void SetParameter(double MilliMeterPerPix_X, double MilliMeterPerPix_Y, double MilliMeterPerPix_Diagonal)
	{
		milliMeterPerPix_X = MilliMeterPerPix_X;
		milliMeterPerPix_Y = MilliMeterPerPix_Y;
		milliMeterPerPix_Diagonal = MilliMeterPerPix_Diagonal;
	};

	//获取x轴每像素对应多少毫米
	double GetMilliMeterPerPix_X(){ return milliMeterPerPix_X; };
	//获取y轴每像素对应多少毫米
	double GetMilliMeterPerPix_Y(){ return milliMeterPerPix_Y; };
	//获取对角线每像素对应多少毫米
	double GetMilliMeterPerPix_Diagonal(){ return milliMeterPerPix_Diagonal; };





	/**************************属性********************************/
	//对角线相差毫米数
	double DiagonalDiff_mm;

	vector<BrokenEdge> BrokenEdges;//缺边
	vector<BrokenCorner> BrokenCorners;//缺角
	vector<Crazing> Crazings;//釉裂
	vector<Hole> Holes;//凹点
	vector<Scratch> Scratchs;//划痕
private:
	double milliMeterPerPix_X = 0;//x轴每像素对应多少毫米
	double milliMeterPerPix_Y = 0;//y轴每像素对应多少毫米
	double milliMeterPerPix_Diagonal = 0;//对角线每像素对应多少毫米
};