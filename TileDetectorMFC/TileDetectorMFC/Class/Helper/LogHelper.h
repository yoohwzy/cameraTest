#pragma once



#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

#include "StringHelper.h"

//FOR PathIsDirectory
#include "shlwapi.h"
#pragma comment(lib,"shlwapi.lib")
class LogHelper
{
public:
	LogHelper();
	~LogHelper();

	//����Ϣд���¼
	static void Log(string logFileName, string content)
	{
		string time = getNowTime();
		stringstream ss;
		ss << getTodayDirectory();
		ss << "\\";
		ss << logFileName;

		if (!Enable)
			return;

		ofstream of(ss.str(), ios::app);
		of << time;
		of << content;
	};
	static void Log(string logFileName, CString ccontent)
	{
		Log(logFileName, StringHelper::CString2String(ccontent));
	};
	static void Log(string content)
	{
		Log(LogFileName, content);
	};
	static void Log(CString ccontent)
	{
		Log(LogFileName, StringHelper::CString2String(ccontent));
	};



	static string LogFileName;
	static bool Enable;
private:
	static string LogNowDir;
	static int hour;//�ļ���¼ʱ�䣬ÿСʱ��һ��txtд���¼������log�ļ�����




	static string getNowLogFileName()
	{
		SYSTEMTIME st;
		CString strTime;
		GetLocalTime(&st);
		strTime.Format(L"%02d_%02d_%02d.log", st.wHour, st.wMinute, st.wSecond);
		//LogFileName = StringHelper::CString2String(strTime);
		return StringHelper::CString2String(strTime);
	}
	//��ý������־Ӧд����һ�ļ���
	static string getTodayDirectory()
	{
		//if (LogNowDir != "")
		//	return LogNowDir;
		SYSTEMTIME st;
		CString m_strFolderPath;// , strTime;
		GetLocalTime(&st);
		m_strFolderPath.Format(L"LOG\\%04d-%02d-%02d", st.wYear, st.wMonth, st.wDay);
		//strTime.Format(L"%2d:%2d:%2d", st.wHour, st.wMinute, st.wSecond);

		LogHelper::Enable = true;
		if (!PathIsDirectory(m_strFolderPath))
		{
			if (!CreateDirectory(m_strFolderPath, NULL))
			{
				LogHelper::Enable = false;
			}
		}
		LogNowDir = StringHelper::CString2String(m_strFolderPath);
		return LogNowDir;
	};
	//��õ�ǰϵͳʱ�� XX:XX:XX
	static string getNowTime()
	{
		SYSTEMTIME st;
		CString strTime;
		GetLocalTime(&st);
		strTime.Format(L"%02d:%02d:%02d  ", st.wHour, st.wMinute, st.wSecond);
		int mhour = st.wHour;
		if (mhour != hour)
		{
			hour = mhour;
			LogFileName = getNowLogFileName();
		}
		return StringHelper::CString2String(strTime);
	};
};
