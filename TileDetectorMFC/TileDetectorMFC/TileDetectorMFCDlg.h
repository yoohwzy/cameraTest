
// TileDetectorMFCDlg.h : 头文件
//

#pragma once
#include "stdafx.h"
#include "globle_head.h"
#include "globle_debug.h"
#include "globle_var.h"
#include "CvvImage.h"
//#include "Controller/Contoller.h"
#include "Controller/TiggerWatcherAndGrabber.h"
#include "Controller/Consumer.h"

#include "SettingDlg.h"


#define WM_MSG_POSTMSG (WM_USER + 99)
#define WM_MSG_GRABBING_END (WM_USER + 100)
#define WM_MSG_GRABBINGCalibartion_END (WM_USER + 102)
#define WM_MSG_PROCESSING_END (WM_USER + 101)

// CTileDetectorMFCDlg 对话框
class CTileDetectorMFCDlg : public CDialogEx
{
// 构造
public:
	CTileDetectorMFCDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CTileDetectorMFCDlg()
	{
		if (twag != NULL)
			delete twag;
		if (consumer != NULL)
			delete consumer;
	};
// 对话框数据
	enum { IDD = IDD_TILEDETECTORMFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	TiggerWatcherAndGrabber *twag = NULL;
	Consumer *consumer = NULL;

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void BtnScan_OnBnClicked();
	afx_msg LRESULT OnMsgGrabbingEnd(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgGrabbingCalibrationEnd(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgProcessingEnd(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgPostMsg(WPARAM wParam, LPARAM lParam);
private:
	void DrawPicToHDC(cv::Mat& img, UINT ID);



	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedCbCanbetiggered();
	afx_msg void OnBnClickedBtnGrabSave();
	afx_msg void OnBnClickedBtnGrabLoad();
	afx_msg void OnBnClickedBtnTiggerSave();
	afx_msg void OnBnClickedBtnTiggerLoad();


	CString m_Info;
	CString m_VirtualCamera;
	afx_msg void OnEnChangeTbVirtualcamera();
	afx_msg void OnEnKillfocusTbVirtualcamera();
	afx_msg void OnBnClickedBtnCalibration();
	afx_msg void OnBnClickedBtnSavePic();
	afx_msg void OnBnClickedBtnSetting();


	afx_msg   void   PicOnMouseHover(WPARAM wParam, LPARAM lParam){ MessageBox(L"鼠标已进入 "); };
	afx_msg   void   PicOnMouseLeave(WPARAM wParam, LPARAM lParam){ MessageBox(L"鼠标已离开 "); };
	BOOL mouse_in_img = false;
	BOOL img_big_flag = false;
	CPoint mouse_point;
	int zoom = 1;//放大倍数
	cv::Mat img_on_show;//全尺寸图
	//缩放显示图片
	void ShowImgROI(CPoint point);
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};
