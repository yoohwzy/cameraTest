#pragma once

#include <windows.h>
#include <string>
#include <iostream>
#include <sstream>
#include <tchar.h>
#include <winnt.h>

using namespace std;
class SettingHelper
{
public:
	enum Key
	{
		GRAB_MaxPics,
		GRAB_Width,
		GRAB_ColorType,
		TIGGER_WaitTime
	};



	SettingHelper();
	~SettingHelper();
	static void Init(Key key);

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

