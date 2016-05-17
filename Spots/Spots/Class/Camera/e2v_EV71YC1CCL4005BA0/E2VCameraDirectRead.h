#pragma once

#include "E2VCameraModel.h"


using namespace std;


//E2V相机的操作类，为直接从相机读图版本
//@author VShawn
class E2VCameraDirectRead :public E2VCameraModel
{
public:
	//初始化采集参数
	//1.采集图像宽度
	//2.采集图像高度
	//3.色彩模式 枚举 RGB OR GRAY
	//4.采集卡编号Logical number of the board.（默认为0）
	//5.采集卡Port口号，PORT_A(默认) OR PORT_B
	E2VCameraDirectRead(int width, int height, int colorType = RGB,int boardID = 0, int Camport = PORT_A);
	~E2VCameraDirectRead();

	bool StartFlag = true;

	cv::Mat GetImage() override;
private:


	//开始循环采图
	bool freeRun();
};