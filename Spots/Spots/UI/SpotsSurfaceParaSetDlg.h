#pragma once


// SpotsSurfaceParaSetDlg �Ի���

class SpotsSurfaceParaSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SpotsSurfaceParaSetDlg)

public:
	SpotsSurfaceParaSetDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~SpotsSurfaceParaSetDlg();

// �Ի�������
	enum { IDD = IDD_SURFACE_PARAMETER_SET_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()
	void saveParameter(int IDC, string SectionName, string key);
public:
	afx_msg void OnBnClickedOk();
};
