#pragma once
#include "globle_var.h"

using namespace std;

globle_var::globle_var()
{
}


globle_var::~globle_var()
{
}
//将默认设置读取值globle_var
bool globle_var::InitSetting(bool isload)
{
	Width = 4096;
	ColorType = RGB;
	FrameCount = 10000;
	FrameTimeUS = 100;
	GrabTimeMS = FrameTimeUS * FrameCount / 1000;

	if (isload)
	{
		string str;
		if (
			SettingHelper::GetKeyString(SettingHelper::GRAB_ColorType, str) &&
			SettingHelper::GetKeyInt(SettingHelper::GRAB_MaxPics, FrameCount) &&
			SettingHelper::GetKeyInt(SettingHelper::GRAB_Width, Width) &&
			SettingHelper::GetKeyInt(SettingHelper::TIGGER_WaitTime, TiggerWaitTimeMS)
			)
		{
			if (str == "RGB")
				ColorType = RGB;
			else
				ColorType = GRAY;
		}
		else
			return false;
	}

	//s = BufferStorage(FrameCount, Width);

	return true;
}
//将手动配置初始化globle_var
bool globle_var::SetGrabSetting(string colorType, int frameCount, int width)
{
	ColorType = colorType == "RGB" ? globle_var::RGB : globle_var::GRAY;
	Width = width;
	FrameCount = frameCount;

	//s = BufferStorage(FrameCount, Width);

	return true;
}
void globle_var::SaveSetting()
{
	//写入ini文件
	SettingHelper::AddKey(SettingHelper::GRAB_MaxPics, FrameCount);
	SettingHelper::AddKey(SettingHelper::GRAB_Width, Width);
	SettingHelper::AddKey(SettingHelper::GRAB_ColorType, ColorType == globle_var::RGB ? "RGB" : "Gray");

	SettingHelper::AddKey(SettingHelper::TIGGER_WaitTime, TiggerWaitTimeMS);
}





/********************全局变量***********************/


//BufferStorage globle_var::s = BufferStorage(FrameCount, Width);
int globle_var::TiggerWaitTimeMS = 400;
string globle_var::VirtualCameraFileName = "";

int globle_var::Width = 0;					//每帧宽度
int globle_var::FrameCount = 0;				//总共帧数
int globle_var::ColorType = globle_var::RGB;				//采图颜色模式
int globle_var::FrameTimeUS = 10;
int globle_var::GrabTimeMS = 10;
