
// TileDetectorMFC.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������





// CTileDetectorMFCApp: 
// �йش����ʵ�֣������ TileDetectorMFC.cpp
//

class CTileDetectorMFCApp : public CWinApp
{
public:
	CTileDetectorMFCApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTileDetectorMFCApp theApp;