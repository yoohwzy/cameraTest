
// Spots.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "UI\SpotsMainDlg.h"
#include "Controller\Controller.h"

// CSpotsApp: 
// �йش����ʵ�֣������ Spots.cpp
//

class CSpotsApp : public CWinApp
{
public:
	CSpotsApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CSpotsApp theApp;