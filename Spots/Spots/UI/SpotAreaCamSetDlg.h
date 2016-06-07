#pragma once


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
public:
	afx_msg void OnBnClickedOk();
};
