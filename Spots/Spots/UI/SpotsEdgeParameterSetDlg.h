#pragma once


// SpotsEdgeParameterSetDlg 对话框

class SpotsEdgeParameterSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SpotsEdgeParameterSetDlg)

public:
	SpotsEdgeParameterSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~SpotsEdgeParameterSetDlg();

// 对话框数据
	enum { IDD = IDD_EDGE_PARAMETER_SET_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
