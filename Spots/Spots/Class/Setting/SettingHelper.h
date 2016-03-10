#pragma once

#include <afxwin.h>
//#include <windows.h>
#include <string>
#include <iostream>
#include <sstream>
#include <tchar.h>
#include <winnt.h>

using namespace std;
class SettingHelper
{
public:
	SettingHelper();
	~SettingHelper();
	//初始化INI文件，即删除原来的
	static void Init();

	//保存一个设置到ini文件
	static void AddKey(string SectionName, string Key, int value);
	//保存一个设置到ini文件
	static void AddKey(string SectionName, string Key, float value);
	//保存一个设置到ini文件
	static void AddKey(string SectionName, string Key, double value);
	//保存一个设置到ini文件
	static void AddKey(string SectionName, string Key, string value);
	//保存一个设置到ini文件
	static void AddKey(string SectionName, string Key, LPWSTR value);

	//从ini文件中按类型读取某个参数
	static bool GetKeyInt(string SectionName, string Key, int& value);
	//从ini文件中按类型读取某个参数
	static bool GetKeyFloat(string SectionName, string Key, float& value);
	//从ini文件中按类型读取某个参数
	static bool GetKeyDouble(string SectionName, string Key, double& value);
	//从ini文件中按类型读取某个参数
	static bool GetKeyString(string SectionName, string Key, string& value);

	//配置文件是否存在
	//static bool IsINIExisted();
private:
	const static LPWSTR path;
};

