#pragma once


// SpotsEdgeParameterSetDlg �Ի���

class SpotsEdgeParameterSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SpotsEdgeParameterSetDlg)

public:
	SpotsEdgeParameterSetDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~SpotsEdgeParameterSetDlg();

// �Ի�������
	enum { IDD = IDD_EDGE_PARAMETER_SET_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
