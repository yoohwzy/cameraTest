///ȫ��ͷ�ļ�
#ifndef INCLUDE_ONCE
#define INCLUDE_ONCE
#pragma once

//ͷ�ļ�
#include <fgrab_struct.h>
#include <fgrab_prototyp.h>
#include <fgrab_define.h>
#include <SisoDisplay.h>


#include <string>
#include <iostream>
#include <fstream>

using namespace std;

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
//ͷ�ļ�END



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
		int dwLen = strlen(l.c_str()) + 1;
		int nwLen = MultiByteToWideChar(CP_ACP, 0, l.c_str(), dwLen, NULL, 0);//������ʵĳ���
		LPWSTR lpsz = new WCHAR[dwLen];
		MultiByteToWideChar(CP_ACP, 0, l.c_str(), dwLen, lpsz, nwLen);

		OutputDebugString(lpsz);
	}
#endif
}




//ȫ�ֺ���
static int ErrorMessage(Fg_Struct *fg)
{
	int error = Fg_getLastErrorNumber(fg);
	const char*	err_str = Fg_getLastErrorDescription(fg);
	fprintf(stderr, "Error: %d : %s\n", error, err_str);
	return error;
}
static int ErrorMessageWait(Fg_Struct *fg)
{
	int error = ErrorMessage(fg);
	printf(" ... press ENTER to continue\n");
	getchar();
	return error;
}









#endif