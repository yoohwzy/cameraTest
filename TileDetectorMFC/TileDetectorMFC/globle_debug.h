//@description	此处声明全局调试相关类
//@author VShawn
//@last modify date 2015年10月31日 By VShawn



#ifndef INCLUDE_ONCE
#define INCLUDE_ONCE
#pragma once

//开启控制台，用于输出信息
static bool CONSOLE_OPEN = true;
#define CREATE_CONSOLE
#ifdef CREATE_CONSOLE

#include <io.h>  
#include <fcntl.h>  

#endif

//将信息输出到console
static void printf_globle(const char *c)
{
	if (CONSOLE_OPEN)
	{
		AllocConsole();
		freopen("CON", "r", stdin);
		freopen("CON", "w", stdout);
		freopen("CON", "w", stderr);
		printf(c);
	}
	//else
	//{
	//	FreeConsole();
	//}
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









/*
//调试相关
//是否输出调试信息
#define OUTPUT_DEBUG_INFO 1
#include <Windows.h>
// 若宏定义配置为输出
// 则在控制台输出调试信息
// 函数位于 global.h
static void OutPutDebugInfo(string info){
#ifdef OUTPUT_DEBUG_INFO
	if (OUTPUT_DEBUG_INFO)
	{
		string l = "--------------------------------\n" + info + "\n--------------------------------\n";
		cout << info << endl;
		int dwLen = strlen(l.c_str()) + 1;
		int nwLen = MultiByteToWideChar(CP_ACP, 0, l.c_str(), dwLen, NULL, 0);//算出合适的长度
		LPWSTR lpsz = new WCHAR[dwLen];
		MultiByteToWideChar(CP_ACP, 0, l.c_str(), dwLen, lpsz, nwLen);

		OutputDebugString(lpsz);
	}
#endif
}
*/






#include <windows.h>
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

//检测MD采集卡是否有错误
//报错->退出
static void MD_ErrorMessageWait(Fg_Struct *fg)
{
	int error = Fg_getLastErrorNumber(fg);
	const char*	err_str = Fg_getLastErrorDescription(fg);
	fprintf(stderr, "Error: %d : %s\n", error, err_str);
	ExitWithError(err_str);
}









#endif