#pragma once


// SpotsSystemSetDlg 对话框

class SpotsSystemSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SpotsSystemSetDlg)

public:
	SpotsSystemSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~SpotsSystemSetDlg();

// 对话框数据
	enum { IDD = IDD_SYSTEMSET_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedOk();
};
