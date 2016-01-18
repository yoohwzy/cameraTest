
// SpotsMainDlg.h : 头文件
//
#include "Class\Helper\StringHelper.h"
#include "globle_debug.h"

#include "../View/SpotsMainView.h"
#include "CvvImage.h"

#pragma once

// CSpotsMainDlg 对话框
class CSpotsMainDlg : public CDialogEx
	, public SpotsMainView
{
// 构造
public:
	CSpotsMainDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CSpotsMainDlg()
	{
		ReleaseDC(p_DC);
	}
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
	CMenu menu;
public:
	afx_msg void OnBnClickedOk();
	void DrawPicToHDC(cv::Mat& img, UINT ID);
	HDC hDC = NULL;
	CDC *p_DC;

	// Override
	void ShowBigImg(cv::Mat);
};
