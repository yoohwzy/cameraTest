#pragma once


// SpotsSurfaceParaSetDlg 对话框

class SpotsSurfaceParaSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SpotsSurfaceParaSetDlg)

public:
	SpotsSurfaceParaSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~SpotsSurfaceParaSetDlg();

// 对话框数据
	enum { IDD = IDD_SURFACE_PARAMETER_SET_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()
	void saveParameter(int IDC, string SectionName, string key);
public:
	afx_msg void OnBnClickedOk();
};
