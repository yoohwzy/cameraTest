#pragma once

#include <afxwin.h>

using namespace std;


class TimeHelper
{
public:
	//获取时间戳
	static unsigned int GetTimestamp()
	{
		CTime ct = CTime::GetCurrentTime();
		return ct.GetTime();
	}

};