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

	//����һ�����õ�ini�ļ�
	static void AddKey(Key key, int value);
	//����һ�����õ�ini�ļ�
	static void AddKey(Key key, float value);
	//����һ�����õ�ini�ļ�
	static void AddKey(Key key, double value);
	//����һ�����õ�ini�ļ�
	static void AddKey(Key key, string value);
	//����һ�����õ�ini�ļ�
	static void AddKey(Key key, LPWSTR value);

	//��ini�ļ��а����Ͷ�ȡĳ������
	static bool GetKeyInt(Key key, int& value);
	//��ini�ļ��а����Ͷ�ȡĳ������
	static bool GetKeyFloat(Key key, float& value);
	//��ini�ļ��а����Ͷ�ȡĳ������
	static bool GetKeyDouble(Key key, double& value);
	//��ini�ļ��а����Ͷ�ȡĳ������
	static bool GetKeyString(Key key, string& value);

	//�����ļ��Ƿ����
	//static bool IsINIExisted();

private:
	static string getSectionName(Key key);
	static string getKeyName(Key key);
	const static LPWSTR path;
};

