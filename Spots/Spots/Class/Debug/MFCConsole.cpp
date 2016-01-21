#include "MFCConsole.h"

#ifndef AFX_DATA
#	include <afxwin.h>
#endif


MFCConsole::MFCConsole()
{
}

MFCConsole::~MFCConsole()
{
}
void MFCConsole::Init()
{
	AllocConsole();
	HANDLE cHandle = GetStdHandle(STD_OUTPUT_HANDLE);//获得控制台输出句柄
	IsOpened = true;
}
void MFCConsole::Output(stringstream msg)
{
	if (IsOpened)
		Output(msg.str());
}
void MFCConsole::Output(string msg)
{
	if (IsOpened)
		Output(msg.c_str());
}
void MFCConsole::Output(const char *msg)
{
	if (IsOpened)
	{
		//freopen("CON", "r", stdin);
		freopen("CON", "w", stdout);
		printf(msg);
	}
}


bool MFCConsole::IsOpened = false;

