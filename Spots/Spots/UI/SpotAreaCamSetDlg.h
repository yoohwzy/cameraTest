#pragma once


// SpotAreaCamSetDlg �Ի���

class SpotAreaCamSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SpotAreaCamSetDlg)

public:
	SpotAreaCamSetDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~SpotAreaCamSetDlg();

// �Ի�������
	enum { IDD = IDD_AreaCamSet_Dialog };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	void saveParameter(int IDC, string SectionName, string key);
public:
	afx_msg void OnBnClickedOk();
};
