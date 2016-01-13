
// SpotsMainDlg.h : ͷ�ļ�
//
#include "Class\Helper\StringHelper.h"

#include "Controller\ImgScanner.h"
#include "Controller\Worker.h"

#pragma once


// CSpotsMainDlg �Ի���
class CSpotsMainDlg : public CDialogEx
{
// ����
public:
	CSpotsMainDlg(CWnd* pParent = NULL);	// ��׼���캯��

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
	E2VBuffer *p_e2vbuffer = NULL;
	ImgScanner *p_imgscanner;
public:
	afx_msg void OnBnClickedOk();
};
