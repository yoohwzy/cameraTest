//@description	此处声明全局调试相关类
//@author VShawn
//@last modify date 2015年10月31日 By VShawn


#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

//开启控制台，用于输出信息
static bool CONSOLE_OPEN = true;


#include <fgrab_struct.h>
#include <fgrab_prototyp.h>
#include <fgrab_define.h>
#include <SisoDisplay.h>


//将信息输出到console
static void printf_globle(const char *c)
{
	if (CONSOLE_OPEN != NULL && CONSOLE_OPEN == true)
	{
		AllocConsole();
		//freopen("CON", "r", stdin);
		freopen("CON", "w", stdout);
		//freopen("CON", "w", stderr);
		printf(c);
	}
	else
	{
		FreeConsole();
	}
}
//将信息输出到console
static void printf_globle(string str)
{
	printf_globle(str.c_str());
}
//将信息输出到console
static void printf_globle(stringstream ss)
{
	printf_globle(ss.str().c_str());
}
















#include <afxwin.h>
//#include <windows.h>
//当发生不可逆转的错误时，调用本程序报错，退出。
static void ExitWithError(std::string errorInfo,int errorCode = 1){
	printf_globle(errorInfo);


	int dwLen = strlen(errorInfo.c_str()) + 1;
	int nwLen = MultiByteToWideChar(CP_ACP, 0, errorInfo.c_str(), dwLen, NULL, 0);//算出合适的长度
	LPWSTR lpwstr = new WCHAR[dwLen];
	MultiByteToWideChar(CP_ACP, 0, errorInfo.c_str(), dwLen, lpwstr, nwLen);

	MessageBox(NULL, lpwstr, L"Error", 0);
	exit(errorCode);
}

#define OUTPUT_TO_CONSOLE