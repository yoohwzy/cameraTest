//@description	�˴�����ȫ�ֵ��������
//@author VShawn
//@last modify date 2015��10��31�� By VShawn


#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

//��������̨�����������Ϣ
static bool CONSOLE_OPEN = true;


#include <fgrab_struct.h>
#include <fgrab_prototyp.h>
#include <fgrab_define.h>
#include <SisoDisplay.h>


//����Ϣ�����console
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
//����Ϣ�����console
static void printf_globle(string str)
{
	printf_globle(str.c_str());
}
//����Ϣ�����console
static void printf_globle(stringstream ss)
{
	printf_globle(ss.str().c_str());
}
















#include <afxwin.h>
//#include <windows.h>
//������������ת�Ĵ���ʱ�����ñ����򱨴��˳���
static void ExitWithError(std::string errorInfo,int errorCode = 1){
	printf_globle(errorInfo);


	int dwLen = strlen(errorInfo.c_str()) + 1;
	int nwLen = MultiByteToWideChar(CP_ACP, 0, errorInfo.c_str(), dwLen, NULL, 0);//������ʵĳ���
	LPWSTR lpwstr = new WCHAR[dwLen];
	MultiByteToWideChar(CP_ACP, 0, errorInfo.c_str(), dwLen, lpwstr, nwLen);

	MessageBox(NULL, lpwstr, L"Error", 0);
	exit(errorCode);
}

#define OUTPUT_TO_CONSOLE