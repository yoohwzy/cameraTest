// D:\��Ŀ����\cameraTest\Spots\Spots\UI\SpotsSurfaceParaSetDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Spots.h"
#include "SpotsSurfaceParaSetDlg.h"
#include "afxdialogex.h"
#include <Class/Setting/SettingHelper.h>


// SpotsSurfaceParaSetDlg �Ի���

IMPLEMENT_DYNAMIC(SpotsSurfaceParaSetDlg, CDialogEx)

SpotsSurfaceParaSetDlg::SpotsSurfaceParaSetDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(SpotsSurfaceParaSetDlg::IDD, pParent)
{

}

SpotsSurfaceParaSetDlg::~SpotsSurfaceParaSetDlg()
{
}

BOOL SpotsSurfaceParaSetDlg::OnInitDialog()
{
	int bedEnable = 1;
	if (SettingHelper::GetKeyInt("SURFACE", "Pretreatment_Enable", bedEnable))
		((CButton*)GetDlgItem(IDC_CB_EnablePretreatment))->SetCheck(bedEnable != 0);
	else
		((CButton*)GetDlgItem(IDC_CB_EnablePretreatment))->SetCheck(1);
	CDialogEx::OnInitDialog();
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE 
}

void SpotsSurfaceParaSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

void SpotsSurfaceParaSetDlg::OnPaint()
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
HBRUSH SpotsSurfaceParaSetDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

BEGIN_MESSAGE_MAP(SpotsSurfaceParaSetDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &SpotsSurfaceParaSetDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// SpotsSurfaceParaSetDlg ��Ϣ�������


void SpotsSurfaceParaSetDlg::OnBnClickedOk()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CB_EnablePretreatment);
	SettingHelper::AddKey("SURFACE", "Pretreatment_Enable", pBtn->GetCheck());
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnOK();
}

void SpotsSurfaceParaSetDlg::saveParameter(int IDC, string SectionName, string key)
{
	//��������
	CString cstr;
	GetDlgItem(IDC)->GetWindowTextW(cstr);
	SettingHelper::AddKey(SectionName, key, StringHelper::CString2string(cstr));
}