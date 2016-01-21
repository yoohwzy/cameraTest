#pragma once

#ifndef AFX_DATA
#	include <afxwin.h>
#endif
#include <string>
#include <iostream>
#include <sstream>
#include <tchar.h>
#include <winnt.h>

#include "StringHelper.h"
using namespace std;
class SettingHelper
{
public:
	enum Key
	{
		GRAB_MaxPics,
		GRAB_Width,
		GRAB_ColorType,
		GRAB_TimeMS,//拍摄时长（毫秒）
		grab_frameTimeUS,//一帧的拍摄时长（微秒）只读
		TIGGER_CaptureWaitTime,//触发后，延时多少毫妙开始采样
		TIGGER_HoldTime, //触发不应期，完成采样后，多少毫秒内无法再次触发采样
		TIGGER_ActionWaitTime //计算出需要机械臂动作后，等待多少毫秒机械臂才启动
	};



	SettingHelper();
	~SettingHelper();
	//初始化INI文件，即删除原来的
	static void Init();

	//保存一个设置到ini文件
	static void AddKey(Key key, int value);
	//保存一个设置到ini文件
	static void AddKey(Key key, float value);
	//保存一个设置到ini文件
	static void AddKey(Key key, double value);
	//保存一个设置到ini文件
	static void AddKey(Key key, string value);
	//保存一个设置到ini文件
	static void AddKey(Key key, LPWSTR value);

	//从ini文件中按类型读取某个参数
	static bool GetKeyInt(Key key, int& value);
	//从ini文件中按类型读取某个参数
	static bool GetKeyFloat(Key key, float& value);
	//从ini文件中按类型读取某个参数
	static bool GetKeyDouble(Key key, double& value);
	//从ini文件中按类型读取某个参数
	static bool GetKeyString(Key key, string& value);

	//配置文件是否存在
	//static bool IsINIExisted();

private:
	static string getSectionName(Key key);
	static string getKeyName(Key key);
	const static LPWSTR path;
};

