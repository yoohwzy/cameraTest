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
		GRAB_TimeMS,//����ʱ�������룩
		grab_frameTimeUS,//һ֡������ʱ����΢�룩ֻ��
		TIGGER_CaptureWaitTime,//��������ʱ���ٺ��ʼ����
		TIGGER_HoldTime, //������Ӧ�ڣ���ɲ����󣬶��ٺ������޷��ٴδ�������
		TIGGER_ActionWaitTime //�������Ҫ��е�۶����󣬵ȴ����ٺ����е�۲�����
	};



	SettingHelper();
	~SettingHelper();
	//��ʼ��INI�ļ�����ɾ��ԭ����
	static void Init();

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

