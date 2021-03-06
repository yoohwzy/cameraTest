#pragma once

#include "Class\Helper\SettingHelper.h"

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"

//@description	此处声明全局变量，全局类，以及相关函数
//@author VShawn
//@last modify date 2015年10月31日 By VShawn
class globle_var
{
public:
	globle_var();
	~globle_var();
	//初始化系统参数
	//输入true:从ini文件读取参数，false:使用默认参数
	static bool InitSetting(bool isload = false);
	//配置采集参数，修改后相机应重新初始化
	static bool SetGrabSetting(string colorType, int maxPics, int width);
	//将参数信息保存至inf
	static void SaveSetting();



	//触发器触发后 延时多少毫秒开始采集
	static int TiggerCaptureWaitTimeMS;
	//机械臂触发延时
	static int TiggerActionWaitTimeMS;
	//虚拟相机底片文件名，为""时使用真实相机
	static string VirtualCameraFileName;

	static int Width;					//每帧宽度
	static const int FrameHeight = 1;		//每帧高度
	static int FrameCount;				//总共帧数
	static int FrameTimeUS;				//拍摄单帧时长
	static int GrabTimeMS;				//采集一块砖用时（毫秒）
	static int ColorType;				//采图颜色模式










	enum ColorType
	{
		GRAY = 0,
		RGB = 1
	};
private:
};

