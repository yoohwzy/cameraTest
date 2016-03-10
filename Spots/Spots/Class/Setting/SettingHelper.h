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
	//��ʼ��INI�ļ�����ɾ��ԭ����
	static void Init();

	//����һ�����õ�ini�ļ�
	static void AddKey(string SectionName, string Key, int value);
	//����һ�����õ�ini�ļ�
	static void AddKey(string SectionName, string Key, float value);
	//����һ�����õ�ini�ļ�
	static void AddKey(string SectionName, string Key, double value);
	//����һ�����õ�ini�ļ�
	static void AddKey(string SectionName, string Key, string value);
	//����һ�����õ�ini�ļ�
	static void AddKey(string SectionName, string Key, LPWSTR value);

	//��ini�ļ��а����Ͷ�ȡĳ������
	static bool GetKeyInt(string SectionName, string Key, int& value);
	//��ini�ļ��а����Ͷ�ȡĳ������
	static bool GetKeyFloat(string SectionName, string Key, float& value);
	//��ini�ļ��а����Ͷ�ȡĳ������
	static bool GetKeyDouble(string SectionName, string Key, double& value);
	//��ini�ļ��а����Ͷ�ȡĳ������
	static bool GetKeyString(string SectionName, string Key, string& value);

	//�����ļ��Ƿ����
	//static bool IsINIExisted();
private:
	const static LPWSTR path;
};

