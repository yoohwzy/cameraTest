#pragma once


// SpotsSystemSetDlg �Ի���

class SpotsSystemSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SpotsSystemSetDlg)

public:
	SpotsSystemSetDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~SpotsSystemSetDlg();

// �Ի�������
	enum { IDD = IDD_SYSTEMSET_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedOk();
};
