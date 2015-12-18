#pragma once

#include "globle_head.h"
#include "globle_debug.h"
#include "globle_var.h"
#include "CvvImage.h"
//#include "Controller/Contoller.h"
#include "Controller/TiggerWatcherAndGrabber.h"
#include "Controller/Consumer.h"

// SettingDlg 对话框

class SettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SettingDlg)

public:
	SettingDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~SettingDlg();

// 对话框数据
	enum { IDD = IDD_Setting };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	int frameTimeUS = 0;
	void calculateFrameTime()
	{
		UpdateData(true);
		frameTimeUS = set_grabTime * 1000 / set_grabFrameCount;
		CString c;
		c.Format(L"%d", frameTimeUS);
		SetDlgItemText(IDC_Lable_FrameTime, c);
	}
public:
	afx_msg void OnBnClickedSave();
	// 采集参数，总采集行数
	int set_grabFrameCount;
	// 采集参数，一行的采集宽度
	int set_grabWidth;
	// 采图时颜色类型 "RGB" or "Gray" 区分大小写
	CString set_grabRGBType;
	// 触发器触发后，等待多少毫秒再开始采集
	int set_TiggerWaitTimeMS;
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	int set_grabTime;
	afx_msg void OnEnKillfocusTbGrabtime();
	afx_msg void OnEnChangeTbGrabframecount();
	afx_msg void OnEnChangeTbGrabtime();
};
