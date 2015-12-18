#pragma once

#include "globle_head.h"
#include "globle_debug.h"
#include "globle_var.h"
#include "CvvImage.h"
//#include "Controller/Contoller.h"
#include "Controller/TiggerWatcherAndGrabber.h"
#include "Controller/Consumer.h"

// SettingDlg �Ի���

class SettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SettingDlg)

public:
	SettingDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~SettingDlg();

// �Ի�������
	enum { IDD = IDD_Setting };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

private:
	int frameTimeUS = 0;
	void calculateFrameTime()
	{
		UpdateData(true);
		frameTimeUS = set_grabTime * 1000 / set_grabFrameCount;
		CString c;
		c.Format(L"%d", frameTimeUS);
		SetDlgItemText(IDC_Lable_FrameTime, c);
	}
public:
	afx_msg void OnBnClickedSave();
	// �ɼ��������ܲɼ�����
	int set_grabFrameCount;
	// �ɼ�������һ�еĲɼ����
	int set_grabWidth;
	// ��ͼʱ��ɫ���� "RGB" or "Gray" ���ִ�Сд
	CString set_grabRGBType;
	// �����������󣬵ȴ����ٺ����ٿ�ʼ�ɼ�
	int set_TiggerWaitTimeMS;
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	int set_grabTime;
	afx_msg void OnEnKillfocusTbGrabtime();
	afx_msg void OnEnChangeTbGrabframecount();
	afx_msg void OnEnChangeTbGrabtime();
};
