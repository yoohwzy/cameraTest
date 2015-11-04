//@description	�˴�����ȫ�ֵ��������
//@author VShawn
//@last modify date 2015��10��31�� By VShawn



#ifndef INCLUDE_ONCE
#define INCLUDE_ONCE
#pragma once

//��������̨�����������Ϣ
static bool CONSOLE_OPEN = true;
#define CREATE_CONSOLE
#ifdef CREATE_CONSOLE

#include <io.h>  
#include <fcntl.h>  

#endif

//����Ϣ�����console
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









/*
//�������
//�Ƿ����������Ϣ
#define OUTPUT_DEBUG_INFO 1
#include <Windows.h>
// ���궨������Ϊ���
// ���ڿ���̨���������Ϣ
// ����λ�� global.h
static void OutPutDebugInfo(string info){
#ifdef OUTPUT_DEBUG_INFO
	if (OUTPUT_DEBUG_INFO)
	{
		string l = "--------------------------------\n" + info + "\n--------------------------------\n";
		cout << info << endl;
		int dwLen = strlen(l.c_str()) + 1;
		int nwLen = MultiByteToWideChar(CP_ACP, 0, l.c_str(), dwLen, NULL, 0);//������ʵĳ���
		LPWSTR lpsz = new WCHAR[dwLen];
		MultiByteToWideChar(CP_ACP, 0, l.c_str(), dwLen, lpsz, nwLen);

		OutputDebugString(lpsz);
	}
#endif
}
*/






#include <windows.h>
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

//���MD�ɼ����Ƿ��д���
//����->�˳�
static void MD_ErrorMessageWait(Fg_Struct *fg)
{
	int error = Fg_getLastErrorNumber(fg);
	const char*	err_str = Fg_getLastErrorDescription(fg);
	fprintf(stderr, "Error: %d : %s\n", error, err_str);
	ExitWithError(err_str);
}









#endif