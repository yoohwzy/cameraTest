// E:\��Ŀ����\cameraTest\Spots\Spots\UI\SpotsEdgeParameterSetDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Spots.h"
#include "SpotsEdgeParameterSetDlg.h"
#include "afxdialogex.h"


// SpotsEdgeParameterSetDlg �Ի���

IMPLEMENT_DYNAMIC(SpotsEdgeParameterSetDlg, CDialogEx)

SpotsEdgeParameterSetDlg::SpotsEdgeParameterSetDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(SpotsEdgeParameterSetDlg::IDD, pParent)
{

}

SpotsEdgeParameterSetDlg::~SpotsEdgeParameterSetDlg()
{
}
void SpotsEdgeParameterSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL SpotsEdgeParameterSetDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	return TRUE;
}
void SpotsEdgeParameterSetDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
	}
	else
	{
		CRect rect;
		CPaintDC dc(this);
		GetClientRect(rect);
		dc.FillSolidRect(rect, RGB(255, 255, 255));
		dc.FillPath();

		CDialogEx::OnPaint();
	}
}
HBRUSH SpotsEdgeParameterSetDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);
	// TODO:  �ڴ˸��� DC ���κ�����
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	CString ClassName;
	GetClassName(pWnd->GetSafeHwnd(), ClassName.GetBuffer(255), 255);

	//if (ClassName.Find(_T("Static"), 0) >= 0 || ClassName.Find(_T("Static"), 0) >= 0 || pWnd->GetDlgCtrlID() == IDC_LB1)
	{
		//pDC->SetBkMode(TRANSPARENT);
		pDC->SetBkColor(RGB(255, 255, 255));
		//pDC->SetTextColor(RGB(0, 0, 0));
		//return (HBRUSH)GetStockObject(HOLLOW_BRUSH);//͸����ˢ
		return (HBRUSH)GetStockObject(WHITE_BRUSH);
	}
	return hbr;
}


BEGIN_MESSAGE_MAP(SpotsEdgeParameterSetDlg, CDialogEx)
END_MESSAGE_MAP()


// SpotsEdgeParameterSetDlg ��Ϣ�������
