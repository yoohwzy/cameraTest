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
	if (SettingHelper::GetKeyString("DATABASE", "ACCDB_PATH", db_path))//读取数据库所在路径
		GetDlgItem(IDC_TB_AccessPath)->SetWindowText(StringHelper::string2LPWSTR(db_path));
	else
		GetDlgItem(IDC_TB_AccessPath)->SetWindowText(L"src//..//瓷砖缺陷检测数据库.mdb");

	int accEnable = 0;
	if (SettingHelper::GetKeyInt("DATABASE", "ACCDB_ENABLE", accEnable))
		((CButton*)GetDlgItem(IDC_CB_EnableAccess))->SetCheck(accEnable != 0);
	else
		((CButton*)GetDlgItem(IDC_CB_EnableAccess))->SetCheck(0);

	/********************************采图参数**********************************/
	int saveImgFlag = 0;
	if (SettingHelper::GetKeyInt("SYS_IMG_CAPTURE", "SAVE_IMG", saveImgFlag))
		((CButton*)GetDlgItem(IDC_CB_SAVE_IMG))->SetCheck(saveImgFlag != 0);
	else
		((CButton*)GetDlgItem(IDC_CB_SAVE_IMG))->SetCheck(0);

	int Worker_WaitTimeMSIn = 0;
	if (SettingHelper::GetKeyInt("SYS_IMG_CAPTURE", "Worker_WaitTimeMSIn", Worker_WaitTimeMSIn))
		GetDlgItem(IDC_TB_Worker_WaitTimeMSIn)->SetWindowText(StringHelper::int2CString(Worker_WaitTimeMSIn));
	else
		GetDlgItem(IDC_TB_Worker_WaitTimeMSIn)->SetWindowText(L"100");

	int Worker_WaitTimeMSOut = 0;
	if (SettingHelper::GetKeyInt("SYS_IMG_CAPTURE", "Worker_WaitTimeMSOut", Worker_WaitTimeMSOut))
		GetDlgItem(IDC_TB_Worker_WaitTimeMSOut)->SetWindowText(StringHelper::int2CString(Worker_WaitTimeMSOut));
	else
		GetDlgItem(IDC_TB_Worker_WaitTimeMSOut)->SetWindowText(L"250");

	int Worker_FrameTimeOut = 0;
	if (SettingHelper::GetKeyInt("SYS_IMG_CAPTURE", "Worker_FrameTimeOut", Worker_FrameTimeOut))
		GetDlgItem(IDC_TB_Worker_FrameTimeOut)->SetWindowText(StringHelper::int2CString(Worker_FrameTimeOut));
	else
		GetDlgItem(IDC_TB_Worker_FrameTimeOut)->SetWindowText(L"2000");

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
	saveParameter(IDC_TB_AccessPath, "DATABASE", "ACCDB_PATH");
	//采图设置
	pBtn = (CButton*)GetDlgItem(IDC_CB_SAVE_IMG);
	SettingHelper::AddKey("SYS_IMG_CAPTURE", "SAVE_IMG", pBtn->GetCheck());
	saveParameter(IDC_TB_Worker_WaitTimeMSIn, "SYS_IMG_CAPTURE", "Worker_WaitTimeMSIn");
	saveParameter(IDC_TB_Worker_WaitTimeMSOut, "SYS_IMG_CAPTURE", "Worker_WaitTimeMSOut");
	saveParameter(IDC_TB_Worker_FrameTimeOut, "SYS_IMG_CAPTURE", "Worker_FrameTimeOut");




	CDialogEx::OnOK();
}
void SpotsSystemSetDlg::saveParameter(int IDC, string SectionName, string key)
{
	//保存设置
	CString cstr;
	GetDlgItem(IDC)->GetWindowTextW(cstr);
	SettingHelper::AddKey(SectionName, key, StringHelper::CString2string(cstr));
}