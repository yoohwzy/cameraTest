#pragma once

#include <afxwin.h>

using namespace std;


class TimeHelper
{
public:
	//获取当前时间，并加上一个毫秒数
	static SYSTEMTIME GetTimeNow(int AddMilliseconds = 0)
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		if (AddMilliseconds == 0)
			return st;

		CTime ct = CTime(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

		if (AddMilliseconds >= 1000)
		{
			int addSecond = AddMilliseconds / 1000;
			AddMilliseconds = AddMilliseconds % 1000;
			ct = ct + CTimeSpan(0, 0, 0, addSecond);
		}
		if (AddMilliseconds <= -1000)
		{
			int decSecond = AddMilliseconds / 1000;
			AddMilliseconds = AddMilliseconds % 1000;
			ct = ct - CTimeSpan(0, 0, 0, decSecond);
		}




		int milliseconds = st.wMilliseconds;
		milliseconds += AddMilliseconds;
		if (milliseconds >= 1000)
		{
			milliseconds -= 1000;
			ct = ct + CTimeSpan(0, 0, 0, 1);
		}
		else if (milliseconds < 0)
		{
			milliseconds += 1000;
			ct = ct - CTimeSpan(0, 0, 0, 1);
		}


		st.wYear = ct.GetYear();
		st.wMonth = ct.GetMonth();
		st.wDay = ct.GetDay();
		st.wHour = ct.GetHour();
		st.wMinute = ct.GetMinute();
		st.wSecond = ct.GetSecond();
		st.wMilliseconds = milliseconds;
		
		return st;
	};
	//相当于st1-st2，计算时间间隔
	static int GetTimeSpanMilliseconds(SYSTEMTIME st1, SYSTEMTIME st2)
	{
		CTime ct1 = SYSTEMTIME2CTime(st1);
		CTime ct2 = SYSTEMTIME2CTime(st2);
		CTimeSpan cts = ct1 - ct2;
		int milliseconds = cts.GetTotalSeconds() * 1000;
		int diff = st1.wMilliseconds - st2.wMilliseconds;
		return (milliseconds + diff);
	}
private:
	static CTime SYSTEMTIME2CTime(SYSTEMTIME st)
	{
		CTime ct = CTime(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		return ct;
	}

};