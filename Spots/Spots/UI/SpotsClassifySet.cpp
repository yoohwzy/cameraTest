// E:\��Ŀ����\cameraTest\Spots\Spots\UI\SpotsClassifySet.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Spots.h"
#include "SpotsClassifySet.h"
#include "afxdialogex.h"


// CSpotsClassifySet �Ի���

IMPLEMENT_DYNAMIC(CSpotsClassifySet, CDialogEx)

CSpotsClassifySet::CSpotsClassifySet(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSpotsClassifySet::IDD, pParent)
{

}

CSpotsClassifySet::~CSpotsClassifySet()
{
}
BOOL CSpotsClassifySet::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	return TRUE;
}
void CSpotsClassifySet::OnPaint()
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
HBRUSH CSpotsClassifySet::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void CSpotsClassifySet::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSpotsClassifySet, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSpotsClassifySet::OnBnClickedOk)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CSpotsClassifySet ��Ϣ�������


void CSpotsClassifySet::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnOK();
}
