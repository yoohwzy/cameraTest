#pragma once


// CSpotsClassifySet 对话框

class CSpotsClassifySet : public CDialogEx
{
	DECLARE_DYNAMIC(CSpotsClassifySet)

public:
	CSpotsClassifySet(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSpotsClassifySet();

// 对话框数据
	enum { IDD = IDD_CLASSIFY_SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
public:
	afx_msg void OnBnClickedOk();
};
