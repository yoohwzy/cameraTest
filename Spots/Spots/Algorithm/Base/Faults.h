#pragma once

#ifndef FAULTS
#define FAULTS

#ifndef CV_BGR2GRAY
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#endif


using namespace std;

//用于存放检测到的缺陷，包含了所有类型
class Faults
{
public:
	//Faults();
	//~Faults(){
	//	Clear();
	//};

	//缺边
	struct BrokenEdge
	{
		cv::Point position;//缺角位置
		double deep;//进边深度（pix）
		double length;//缺边长（pix）
		double deep_mm;//进边深度（mm）
		double length_mm;//缺边长（mm）
	};
	//缺角
	struct BrokenCorner
	{
		cv::Point position;//缺角位置
		double deep;//进边深度（pix）
		double length;//缺角长y轴（pix）
		double width;//缺角宽x轴（pix）

		double deep_mm;//进边深度（mm）
		double length_mm;//缺角长y轴（mm）
		double width_mm;//缺角宽x轴（mm）
	};
	//釉裂
	struct Crazing
	{
		cv::Point position;//釉裂位置
		double diameter;//直径（pix）
		double diameter_mm;//直径（mm）
	};
	//凹点
	struct Hole
	{
		cv::Point position;//凹点位置
		double diameter;//直径（pix）
		double diameter_mm;//直径（mm）
	};
	//划痕
	struct Scratch
	{
		cv::Point position;//位置
		double length;//划痕长度（pix）
		double length_mm;//划痕长度（pix）
		double angel;//角度°
	};
	////巨大缺陷，一遇到就判断为不合格品
	//struct SomethingBig
	//{
	//	cv::Point position;//位置
	//	double diameter;//直径（pix）
	//};
	//人工标记
	struct MarkPen
	{
		cv::Rect markposition;//位置
	};




	/************************方法************************/

	//清空上一轮的错误信息
	void Clear(){
		BrokenEdges.clear();//缺边
		BrokenCorners.clear();//缺角
		Crazings.clear();//釉裂
		Holes.clear();//凹点
		Scratchs.clear();//划痕
		//SomethingBigs.clear();
		MarkPens.clear();
	};

	//设置参数，输入：x轴每像素对应多少pix，y轴每像素对应多少pix，对角线每像素对应多少pix
	void SetParameter(double MilliMeterPermm_X, double MilliMeterPermm_Y, double MilliMeterPermm_Diagonal)
	{
		milliMeterPermm_X = MilliMeterPermm_X;
		milliMeterPermm_Y = MilliMeterPermm_Y;
		milliMeterPermm_Diagonal = MilliMeterPermm_Diagonal;
	};

	//获取x轴每像素对应多少pix
	double GetMilliMeterPermm_X(){ return milliMeterPermm_X; };
	//获取y轴每像素对应多少pix
	double GetMilliMeterPermm_Y(){ return milliMeterPermm_Y; };
	//获取对角线每像素对应多少pix
	double GetMilliMeterPermm_Diagonal(){ return milliMeterPermm_Diagonal; };





	/**************************属性********************************/
	//对角线相差pix数
	double DiagonalDiff_mm;

	vector<BrokenEdge> BrokenEdges;//缺边
	vector<BrokenCorner> BrokenCorners;//缺角
	vector<Crazing> Crazings;//釉裂
	vector<Hole> Holes;//凹点
	vector<Scratch> Scratchs;//划痕
	//vector<SomethingBig> SomethingBigs;//巨大缺陷，一遇到就判断为不合格品
	vector<MarkPen> MarkPens;//人工标记
private:
	double milliMeterPermm_X = 0;//x轴每像素对应多少pix
	double milliMeterPermm_Y = 0;//y轴每像素对应多少pix
	double milliMeterPermm_Diagonal = 0;//对角线每像素对应多少pix
};

#endif