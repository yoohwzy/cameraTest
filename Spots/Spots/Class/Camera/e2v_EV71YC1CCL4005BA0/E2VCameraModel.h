#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include <fgrab_struct.h>
#include <fgrab_prototyp.h>
#include <fgrab_define.h>
#include <SisoDisplay.h>

#include <opencv2/opencv.hpp>


using namespace std;


//E2V相机的基类，以此派生出三种采集模式
//@author VShawn
class E2VCameraModel
{
public:
	enum ColorType
	{
		GRAY = CV_8U,
		RGB = CV_8UC3
	};

	//初始化采集参数
	//1.采集图像宽度
	//2.采集图像高度
	//3.色彩模式 枚举 RGB OR GRAY
	//4.采集卡编号Logical number of the board.（默认为0）
	//5.采集卡Port口号，PORT_A(默认) OR PORT_B
	E2VCameraModel(int width, int height = 0, int colorType = RGB, int boardID = 0, int Camport = PORT_A);
	~E2VCameraModel();

	//虚函数，待子类实现
	virtual cv::Mat GetImage(){ return cv::Mat(); };

	bool ReInitFg();//尝试重新初始化
	bool HasBeenInited = false;		//是否已完成初始化

protected:
	Fg_Struct *fg = NULL;			//frame grabber
	dma_mem *memHandle = NULL;		//Memory buffer




	const char *dllNameGRAY = "DualLineGray16.dll";
	const char *dllNameRGB = "DualLineRGB30.dll";

	int _width = 0;					//图像宽度
	int _height = 0;				//图像高度
	const int _frameHeight = 1;		//每帧高度
	int _nBoard = 0;				//采集卡编号
	int _camPort = PORT_A;			//采集卡Port
	int _colorType = 1;				//采图颜色模式

	//检测MD采集卡是否有错误
	//报错->退出
	void errorMessageWait();

private:
	//初始化fg，返回ErrCode，0为无错误
	bool init_fg();

	// 在控制台显示出采集卡信息
	int getNrOfBoards();
	// get board and camera-link serial port information
	// using silicon-software runtime library fglib5
	int getBoardInfo();

	//创建显示窗口，返回窗口id;
	int createDiplayBuffer();

	//分配内存，返回ErrCode
	bool memoryAllocation();

	//释放内存
	void release();
};