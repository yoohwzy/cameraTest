// D:\项目资料\cameraTest\Spots\Spots\UI\SpotsSurfaceParaSetDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Spots.h"
#include "SpotsSurfaceParaSetDlg.h"
#include "afxdialogex.h"
#include <Class/Setting/SettingHelper.h>


// SpotsSurfaceParaSetDlg 对话框

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
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE 
}

void SpotsSurfaceParaSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

void SpotsSurfaceParaSetDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文
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
	// TODO:  在此更改 DC 的任何特性
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	CString ClassName;
	GetClassName(pWnd->GetSafeHwnd(), ClassName.GetBuffer(255), 255);

	//if (ClassName.Find(_T("Static"), 0) >= 0 || ClassName.Find(_T("Static"), 0) >= 0 || pWnd->GetDlgCtrlID() == IDC_LB1)
	{
		//pDC->SetBkMode(TRANSPARENT);
		pDC->SetBkColor(RGB(255, 255, 255));
		//pDC->SetTextColor(RGB(0, 0, 0));
		//return (HBRUSH)GetStockObject(HOLLOW_BRUSH);//透明笔刷
		return (HBRUSH)GetStockObject(WHITE_BRUSH);
	}
	return hbr;
}

BEGIN_MESSAGE_MAP(SpotsSurfaceParaSetDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &SpotsSurfaceParaSetDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// SpotsSurfaceParaSetDlg 消息处理程序


void SpotsSurfaceParaSetDlg::OnBnClickedOk()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CB_EnablePretreatment);
	SettingHelper::AddKey("SURFACE", "Pretreatment_Enable", pBtn->GetCheck());
	// TODO:  在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}

void SpotsSurfaceParaSetDlg::saveParameter(int IDC, string SectionName, string key)
{
	//保存设置
	CString cstr;
	GetDlgItem(IDC)->GetWindowTextW(cstr);
	SettingHelper::AddKey(SectionName, key, StringHelper::CString2string(cstr));
}