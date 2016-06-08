#pragma once

#include <Class\Camera\MVCAM\MVCAM.h>

// SpotAreaCamSetDlg 对话框

class SpotAreaCamSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SpotAreaCamSetDlg)

public:
	SpotAreaCamSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~SpotAreaCamSetDlg();

// 对话框数据
	enum { IDD = IDD_AreaCamSet_Dialog };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	void saveParameter(int IDC, string SectionName, string key);
	MVCAM *p_mvcam = NULL;
	bool biaoDingExitFlag = true;
	void biaodingDispThread();//
	bool biaodingDispThreadEndFlag = true;//biaodingDispThread已结束标志，为true时表示biaodingDispThread线程是停止的。
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedAreacamsetdlgBtnDingbiao();
};
