#pragma once


// CSpotsClassifySet �Ի���

class CSpotsClassifySet : public CDialogEx
{
	DECLARE_DYNAMIC(CSpotsClassifySet)

public:
	CSpotsClassifySet(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSpotsClassifySet();

// �Ի�������
	enum { IDD = IDD_CLASSIFY_SET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
public:
	afx_msg void OnBnClickedOk();
};
