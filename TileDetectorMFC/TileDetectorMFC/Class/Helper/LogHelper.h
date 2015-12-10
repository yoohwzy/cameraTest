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
		stringstream ss;
		ss << GetTodayDirectory();
		ss << "\\";
		ss << logFileName;
		ofstream of(ss.str(), ios::app);
		string str(content);
		of << str;
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

	static string GetNowLogFileName()
	{
		SYSTEMTIME st;
		CString strTime;
		GetLocalTime(&st);
		strTime.Format(L"%2d_%2d_%2d.log", st.wHour, st.wMinute, st.wSecond);
		return StringHelper::CString2String(strTime);
	}


	static string LogFileName;
	static bool Enable;
private:

	static string GetTodayDirectory()
	{
		SYSTEMTIME st;
		CString m_strFolderPath, strTime;
		GetLocalTime(&st);
		m_strFolderPath.Format(L"LOG\\%4d-%2d-%2d", st.wYear, st.wMonth, st.wDay);
		strTime.Format(L"%2d:%2d:%2d", st.wHour, st.wMinute, st.wSecond);

		LogHelper::Enable = true;
		if (!PathIsDirectory(m_strFolderPath))
		{
			if (!CreateDirectory(m_strFolderPath, NULL))
			{
				LogHelper::Enable = false;
			}
		}
		return StringHelper::CString2String(m_strFolderPath);
	};
};
