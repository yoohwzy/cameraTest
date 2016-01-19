
// SpotsMainDlg.h : 头文件
//
#include "Class\Helper\StringHelper.h"
#include <globle_debug.h>

#include <View/SpotsMainView.h>
#include <CvvImage.h>
#include <Class\Log\Recorder.h>

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
	bool isRunning = true;
public:
	afx_msg void OnBnClickedOk();
	void DrawPicToHDC(cv::Mat& img, UINT ID);
	HDC hDC = NULL;
	CDC *p_DC;

	// 供Controller调用 //Override

	void ShowBigImg(cv::Mat);
	// 更新UI上显示的记录数据
	void UpdateRecord();
	//传入true切换UI到虚拟相机模式，false切换UI到真实相机模式
	void SwitchModel2Virtual(bool switchToV);

	// 供Controller调用 END


	afx_msg void OnBnClickedBtnRun();
	afx_msg void OnBnClickedBtnSelectvirtualimg();
	afx_msg void OnBnClickedBtnvirtualtigger();
};
