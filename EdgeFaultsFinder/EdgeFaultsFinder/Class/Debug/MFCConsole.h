//@description	MFC�д򿪿���̨�������������Ϣ
//@author VShawn
//@last modify date 2016��1��21�� By VShawn


#pragma once

#include <iostream>
#include <string>
#include <sstream>



using namespace std;

class MFCConsole
{
public:
	MFCConsole();
	~MFCConsole();
	static void Init();
	static void Output(stringstream msg);
	static void Output(string msg);
	static void Output(const char *msg);
	static bool IsOpened;
private:
};


////��������̨�����������Ϣ
//static bool CONSOLE_OPEN = true;
//
////����Ϣ�����console
//static void printf_globle(const char *c)
//{
//	if (CONSOLE_OPEN != NULL && CONSOLE_OPEN == true)
//	{
//		AllocConsole();
//		//freopen("CON", "r", stdin);
//		freopen("CON", "w", stdout);
//		//freopen("CON", "w", stderr);
//		printf(c);
//	}
//	else
//	{
//		FreeConsole();
//	}
//}
////����Ϣ�����console
//static void printf_globle(string str)
//{
//	printf_globle(str.c_str());
//}
////����Ϣ�����console
//static void printf_globle(stringstream ss)
//{
//	printf_globle(ss.str().c_str());
//}
//



