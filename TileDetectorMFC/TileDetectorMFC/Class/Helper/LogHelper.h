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

	//将信息写入记录
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
	static int hour;//文件记录时间，每小时换一个txt写入记录，以免log文件过大。




	static string getNowLogFileName()
	{
		SYSTEMTIME st;
		CString strTime;
		GetLocalTime(&st);
		strTime.Format(L"%02d_%02d_%02d.log", st.wHour, st.wMinute, st.wSecond);
		//LogFileName = StringHelper::CString2String(strTime);
		return StringHelper::CString2String(strTime);
	}
	//获得今天的日志应写入哪一文件夹
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
	//获得当前系统时间 XX:XX:XX
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
