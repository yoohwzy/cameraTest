#pragma once

#include <afxwin.h>

using namespace std;


class TimeHelper
{
public:
	//��ȡʱ���
	static unsigned int GetTimestamp()
	{
		CTime ct = CTime::GetCurrentTime();
		return ct.GetTime();
	}

};