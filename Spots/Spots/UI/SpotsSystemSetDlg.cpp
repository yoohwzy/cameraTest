// E:\项目资料\cameraTest\Spots\Spots\UI\SpotsSystemSetDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Spots.h"
#include "SpotsSystemSetDlg.h"
#include "afxdialogex.h"
#include <Class/Setting/SettingHelper.h>


// SpotsSystemSetDlg 对话框

IMPLEMENT_DYNAMIC(SpotsSystemSetDlg, CDialogEx)

SpotsSystemSetDlg::SpotsSystemSetDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(SpotsSystemSetDlg::IDD, pParent)
{

}

SpotsSystemSetDlg::~SpotsSystemSetDlg()
{
}

BOOL SpotsSystemSetDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	string db_path;
	bool accConnFlag = false;
	if (SettingHelper::GetKeyString("DATABASE", "ACCDB_PATH", db_path))//读取数据库所在路径
	{
		GetDlgItem(IDC_TB_AccessPath)->SetWindowText(StringHelper::string2LPWSTR(db_path));
	}
	else
	{
		GetDlgItem(IDC_TB_AccessPath)->SetWindowText(L"src//..//瓷砖缺陷检测数据库.mdb");
	}
	int accEnable = 0;
	if (SettingHelper::GetKeyInt("DATABASE", "ACCDB_ENABLE", accEnable))//读取数据库所在路径
		((CButton*)GetDlgItem(IDC_CB_EnableAccess))->SetCheck(accEnable != 0);
	else
		((CButton*)GetDlgItem(IDC_CB_EnableAccess))->SetCheck(1);

	if (SettingHelper::GetKeyInt("SYS", "SAVE_IMG", accEnable))//读取数据库所在路径
		((CButton*)GetDlgItem(IDC_CB_SAVE_IMG))->SetCheck(accEnable != 0);
	else
		((CButton*)GetDlgItem(IDC_CB_SAVE_IMG))->SetCheck(0);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void SpotsSystemSetDlg::OnPaint()
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
HBRUSH SpotsSystemSetDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void SpotsSystemSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(SpotsSystemSetDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDOK, &SpotsSystemSetDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// SpotsSystemSetDlg 消息处理程序


void SpotsSystemSetDlg::OnBnClickedOk()
{
	//保存数据库设置
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CB_EnableAccess);
	SettingHelper::AddKey("DATABASE", "ACCDB_ENABLE", pBtn->GetCheck());
	CString cstr;
	GetDlgItem(IDC_TB_AccessPath)->GetWindowTextW(cstr);
	SettingHelper::AddKey("DATABASE", "ACCDB_PATH", StringHelper::CString2string(cstr));
	pBtn = (CButton*)GetDlgItem(IDC_CB_SAVE_IMG);
	SettingHelper::AddKey("SYS", "SAVE_IMG", pBtn->GetCheck());
	CDialogEx::OnOK();
}
