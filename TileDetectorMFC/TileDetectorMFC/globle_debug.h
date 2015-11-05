//@description	�˴�����ȫ�ֵ��������
//@author VShawn
//@last modify date 2015��10��31�� By VShawn



#ifndef INCLUDE_ONCE
#define INCLUDE_ONCE
#pragma once

//��������̨�����������Ϣ
static bool CONSOLE_OPEN = true;
static bool FILE_LOG = true;
static string FILE_NAME = "log.log";



#include <io.h>  


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


	if (FILE_LOG)
	{
		ofstream of(FILE_NAME,ios::app);
		string str(c);
		of << str;
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








#define OUTPUT_DEBUG_INFO 1

static void WriteImg(string path ,cv::Mat img)
{
	cv::imwrite(path, img);
}







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
