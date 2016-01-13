
// SpotsMainDlg.h : 头文件
//
#include "Class\Helper\StringHelper.h"

#include "Controller\ImgScanner.h"
#include "Controller\Worker.h"

#pragma once


// CSpotsMainDlg 对话框
class CSpotsMainDlg : public CDialogEx
{
// 构造
public:
	CSpotsMainDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SPOTS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	E2VBuffer *p_e2vbuffer = NULL;
	ImgScanner *p_imgscanner;
public:
	afx_msg void OnBnClickedOk();
};
