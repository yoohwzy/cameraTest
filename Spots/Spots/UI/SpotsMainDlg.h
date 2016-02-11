#pragma once

// SpotsMainDlg.h : ͷ�ļ�
//

#include <View/SpotsMainView.h>
#include <CvvImage.h>

#include <Class\Helper\StringHelper.h>
#include <Class\Log\Recorder.h>
#include <Class\Debug\MFCConsole.h>

#include <Controller\Controller.h>
#include <Controller\StatisticsController.h>

#import "c:\program files\common files\system\ado\msado15.dll" no_namespace rename("EOF","adoEOF")

// CSpotsMainDlg �Ի���
class CSpotsMainDlg : public CDialogEx
	, public SpotsMainView
{
// ����
public:
	CSpotsMainDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CSpotsMainDlg()
	{
		ReleaseDC(p_DC);
	}
// �Ի�������
	enum { IDD = IDD_SPOTS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CMenu menu;
	bool isRunning = true;
public:
	afx_msg void OnBnClickedOk();
	void DrawPicToHDC(cv::Mat& img, UINT ID);
	HDC hDC = NULL;
	CDC *p_DC;

	// ��Controller���� //Override

	void ShowBigImg(cv::Mat);
	// ����UI����ʾ�ļ�¼����
	void UpdateRecord();
	//����true�л�UI���������ģʽ��false�л�UI����ʵ���ģʽ
	void SwitchModel2Virtual(bool switchToV);

	// ��Controller���� END


	afx_msg void OnBnClickedBtnRun();
	afx_msg void OnBnClickedBtnSelectvirtualimg();
	afx_msg void OnBnClickedBtnvirtualtigger();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
