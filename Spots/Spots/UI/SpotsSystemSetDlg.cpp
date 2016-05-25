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

	int VirtualCamEnable = 0;
	SettingHelper::GetKeyInt("E2V", "Virtual_Cam_Enable", VirtualCamEnable);
	((CButton*)GetDlgItem(IDC_CB_VirtualCam))->SetCheck(VirtualCamEnable != 0);

	int COLOR_TYPE_IS_GRAY = 1;
	SettingHelper::GetKeyInt("E2V", "COLOR_TYPE_IS_GRAY", COLOR_TYPE_IS_GRAY);
	((CButton*)GetDlgItem(IDC_CB_CAM_GRAY))->SetCheck(COLOR_TYPE_IS_GRAY != 0);
	((CButton*)GetDlgItem(IDC_CB_CAM_BGR))->SetCheck(COLOR_TYPE_IS_GRAY == 0);


	int Cam_FrameCount_PerSecond = 0;
	if (SettingHelper::GetKeyInt("E2V", "Cam_FrameCount_PerSecond", Cam_FrameCount_PerSecond))
		GetDlgItem(IDC_TB_Cam_FrameCount_PerSecond)->SetWindowText(StringHelper::int2CString(Cam_FrameCount_PerSecond));
	else
		GetDlgItem(IDC_TB_Cam_FrameCount_PerSecond)->SetWindowText(L"5000");




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

	int WaitTimeMSIn = 0;
	if (SettingHelper::GetKeyInt("SYS_IMG_CAPTURE", "WaitTimeMSIn", WaitTimeMSIn))
		GetDlgItem(IDC_TB_WaitTimeMSIn)->SetWindowText(StringHelper::int2CString(WaitTimeMSIn));
	else
		GetDlgItem(IDC_TB_WaitTimeMSIn)->SetWindowText(L"100");

	int WaitTimeMSOut = 0;
	if (SettingHelper::GetKeyInt("SYS_IMG_CAPTURE", "WaitTimeMSOut", WaitTimeMSOut))
		GetDlgItem(IDC_TB_WaitTimeMSOut)->SetWindowText(StringHelper::int2CString(WaitTimeMSOut));
	else
		GetDlgItem(IDC_TB_WaitTimeMSOut)->SetWindowText(L"250");

	int FrameTimeOut = 0;
	if (SettingHelper::GetKeyInt("SYS_IMG_CAPTURE", "FrameTimeOut", FrameTimeOut))
		GetDlgItem(IDC_TB_FrameTimeOut)->SetWindowText(StringHelper::int2CString(FrameTimeOut));
	else
		GetDlgItem(IDC_TB_FrameTimeOut)->SetWindowText(L"2000");



	int Real_WidthMM = 0;
	if (SettingHelper::GetKeyInt("SYS_IMG_CAPTURE", "Real_WidthMM", Real_WidthMM))
		GetDlgItem(IDC_TB_Real_WidthMM)->SetWindowText(StringHelper::int2CString(Real_WidthMM));
	else
		GetDlgItem(IDC_TB_Real_WidthMM)->SetWindowText(L"600");

	int Real_LengthMM = 0;
	if (SettingHelper::GetKeyInt("SYS_IMG_CAPTURE", "Real_LengthMM", Real_LengthMM))
		GetDlgItem(IDC_TB_Real_LengthMM)->SetWindowText(StringHelper::int2CString(Real_LengthMM));
	else
		GetDlgItem(IDC_TB_Real_LengthMM)->SetWindowText(L"300");


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
	ON_BN_CLICKED(IDC_CB_CAM_GRAY, &SpotsSystemSetDlg::OnBnClickedCbCamGray)
	ON_BN_CLICKED(IDC_CB_CAM_BGR, &SpotsSystemSetDlg::OnBnClickedCbCamBgr)
END_MESSAGE_MAP()


// SpotsSystemSetDlg 消息处理程序


void SpotsSystemSetDlg::OnBnClickedOk()
{
	//E2V
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CB_CAM_GRAY);
	SettingHelper::AddKey("E2V", "COLOR_TYPE_IS_GRAY", pBtn->GetCheck());
	saveParameter(IDC_TB_Cam_FrameCount_PerSecond, "E2V", "Cam_FrameCount_PerSecond");
	//虚拟相机
	pBtn = (CButton*)GetDlgItem(IDC_CB_VirtualCam);
	SettingHelper::AddKey("E2V", "Virtual_Cam_Enable", pBtn->GetCheck());
	

	//保存数据库设置
	pBtn = (CButton*)GetDlgItem(IDC_CB_EnableAccess);
	SettingHelper::AddKey("DATABASE", "ACCDB_ENABLE", pBtn->GetCheck());
	saveParameter(IDC_TB_AccessPath, "DATABASE", "ACCDB_PATH");
	//采图设置
	pBtn = (CButton*)GetDlgItem(IDC_CB_SAVE_IMG);
	SettingHelper::AddKey("SYS_IMG_CAPTURE", "SAVE_IMG", pBtn->GetCheck());
	saveParameter(IDC_TB_WaitTimeMSIn, "SYS_IMG_CAPTURE", "WaitTimeMSIn");
	saveParameter(IDC_TB_WaitTimeMSOut, "SYS_IMG_CAPTURE", "WaitTimeMSOut");
	saveParameter(IDC_TB_FrameTimeOut, "SYS_IMG_CAPTURE", "FrameTimeOut");
	saveParameter(IDC_TB_Real_WidthMM, "SYS_IMG_CAPTURE", "Real_WidthMM");
	saveParameter(IDC_TB_Real_LengthMM, "SYS_IMG_CAPTURE", "Real_LengthMM");


	CDialogEx::OnOK();
}
void SpotsSystemSetDlg::saveParameter(int IDC, string SectionName, string key)
{
	//保存设置
	CString cstr;
	GetDlgItem(IDC)->GetWindowTextW(cstr);
	SettingHelper::AddKey(SectionName, key, StringHelper::CString2string(cstr));
}

void SpotsSystemSetDlg::OnBnClickedCbCamGray()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CB_CAM_GRAY);
	if (pBtn->GetCheck() > 0)
		((CButton*)GetDlgItem(IDC_CB_CAM_BGR))->SetCheck(0);
	else
		((CButton*)GetDlgItem(IDC_CB_CAM_BGR))->SetCheck(1);
}


void SpotsSystemSetDlg::OnBnClickedCbCamBgr()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CB_CAM_BGR);
	if (pBtn->GetCheck() > 0)
		((CButton*)GetDlgItem(IDC_CB_CAM_GRAY))->SetCheck(0);
	else
		((CButton*)GetDlgItem(IDC_CB_CAM_GRAY))->SetCheck(1);
}
