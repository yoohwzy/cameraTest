#include "globle_var.h"


globle_var::globle_var()
{
}


globle_var::~globle_var()
{
}
//将默认设置读取值globle_var
bool globle_var::InitSetting(bool isload)
{
	_mdi.width = 4096;
	_mdi.height = 1;
	_mdi.colorType = _mdi.RGB;
	_mdi.MaxPics = 10001;

	if (isload)
	{
		string str;
		if (
			SettingHelper::GetKeyString(SettingHelper::GRAB_ColorType, str) &&
			SettingHelper::GetKeyInt(SettingHelper::GRAB_MaxPics, _mdi.MaxPics) &&
			SettingHelper::GetKeyInt(SettingHelper::GRAB_Width, _mdi.width) &&
			SettingHelper::GetKeyInt(SettingHelper::TIGGER_WaitTime, TiggerWaitTimeMS)
			)
		{
			if (str == "RGB")
				_mdi.colorType = _mdi.RGB;
			else
				_mdi.colorType = _mdi.GRAY;
		}
		else
			return false;
	}

	s = BufferStorage(_mdi.MaxPics, _mdi.width);

	return true;
}
//将手动配置初始化globle_var
bool globle_var::SetGrabSetting(string colorType, int maxPics, int width)
{
	_mdi.colorType = colorType == "RGB" ? globle_var::_mdi.RGB : globle_var::_mdi.GRAY;
	_mdi.width = width;
	//_mdi.height = 1;
	//_mdi.colorType = _mdi.RGB;
	_mdi.MaxPics = maxPics;

	s = BufferStorage(_mdi.MaxPics, _mdi.width);

	return true;
}
void globle_var::SaveSetting()
{
	//写入ini文件
	SettingHelper::AddKey(SettingHelper::GRAB_MaxPics, _mdi.MaxPics);
	SettingHelper::AddKey(SettingHelper::GRAB_Width, _mdi.width);
	SettingHelper::AddKey(SettingHelper::GRAB_ColorType, _mdi.colorType == globle_var::_mdi.RGB ? "RGB" : "Gray");

	SettingHelper::AddKey(SettingHelper::TIGGER_WaitTime, TiggerWaitTimeMS);
}





/********************全局变量***********************/

MicroDisplayInit globle_var::_mdi;
BufferStorage globle_var::s = BufferStorage(_mdi.MaxPics, _mdi.width);
int globle_var::TiggerWaitTimeMS = 400;
string globle_var::VirtualCameraFileName = "";