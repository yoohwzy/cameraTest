
// TileDetectorMFCDlg.h : ͷ�ļ�
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


#define WM_MSG_POSTMSG (WM_USER + 99)
#define WM_MSG_GRABBING_END (WM_USER + 100)
#define WM_MSG_GRABBINGCalibartion_END (WM_USER + 102)
#define WM_MSG_PROCESSING_END (WM_USER + 101)

// CTileDetectorMFCDlg �Ի���
class CTileDetectorMFCDlg : public CDialogEx
{
// ����
public:
	CTileDetectorMFCDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CTileDetectorMFCDlg()
	{
		if (twag != NULL)
			delete twag;
		if (consumer != NULL)
			delete consumer;
	};
// �Ի�������
	enum { IDD = IDD_TILEDETECTORMFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

	TiggerWatcherAndGrabber *twag = NULL;
	Consumer *consumer = NULL;

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
public:
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedCbCanbetiggered();
	afx_msg void OnBnClickedBtnGrabSave();
	afx_msg void OnBnClickedBtnGrabLoad();
	afx_msg void OnBnClickedBtnTiggerSave();
	afx_msg void OnBnClickedBtnTiggerLoad();

	// �趨�ɼ�����������
	int set_grabMaxPics;
	// �ɼ����������
	int set_grabWidth;
	// ��ɫģʽ
	CString set_grabRGBType;
	// ������ȴ�ʱ��
	int set_TiggerWaitTimeMS;
	CString m_Info;
	CString m_VirtualCamera;
	afx_msg void OnEnChangeTbVirtualcamera();
	afx_msg void OnEnKillfocusTbVirtualcamera();
	afx_msg void OnBnClickedBtnCalibration();
};
