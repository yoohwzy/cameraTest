// SettingDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TileDetectorMFC.h"
#include "SettingDlg.h"
#include "afxdialogex.h"


// SettingDlg 对话框

IMPLEMENT_DYNAMIC(SettingDlg, CDialogEx)

SettingDlg::SettingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(SettingDlg::IDD, pParent)
	, set_grabFrameCount(0)
	, set_grabWidth(0)
	, set_grabRGBType(_T(""))
	, set_TiggerWaitTimeMS(0)
	, set_grabTime(0)
{

}

BOOL SettingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//读取配置文件
	if (globle_var::InitSetting(true))
	{
	}
	else
	{
		MessageBox(L"某些参数读取失败，请保存正确的参数信息！");
		globle_var::InitSetting();
	}

	//设置参数
	set_grabFrameCount = globle_var::FrameCount;
	set_grabWidth = globle_var::Width;
	set_grabRGBType = globle_var::ColorType == globle_var::RGB ? "RGB" : "Gray";
	set_TiggerWaitTimeMS = globle_var::TiggerWaitTimeMS;
	set_grabTime = globle_var::GrabTimeMS;
	UpdateData(false);
	calculateFrameTime();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


SettingDlg::~SettingDlg()
{
}

void SettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TB_GrabFrameCount, set_grabFrameCount);
	DDV_MinMaxInt(pDX, set_grabFrameCount, 1, 20000);
	DDX_Text(pDX, IDC_TB_GrabWidth, set_grabWidth);
	DDX_CBString(pDX, IDC_DDL_GrabRGBType, set_grabRGBType);
	DDX_Text(pDX, IDC_TB_TiggerWaitMS, set_TiggerWaitTimeMS);
	DDX_Text(pDX, IDC_TB_GrabTime, set_grabTime);
	DDV_MinMaxInt(pDX, set_grabTime, 1, 20000);
}


BEGIN_MESSAGE_MAP(SettingDlg, CDialogEx)
	ON_BN_CLICKED(IDC_SAVE, &SettingDlg::OnBnClickedSave)
	ON_BN_CLICKED(IDC_Cancel, &SettingDlg::OnBnClickedCancel)
	ON_EN_KILLFOCUS(IDC_TB_GrabTime, &SettingDlg::OnEnKillfocusTbGrabtime)
	ON_EN_CHANGE(IDC_TB_GrabFrameCount, &SettingDlg::OnEnChangeTbGrabframecount)
	ON_EN_CHANGE(IDC_TB_GrabTime, &SettingDlg::OnEnChangeTbGrabtime)
END_MESSAGE_MAP()


// SettingDlg 消息处理程序


void SettingDlg::OnBnClickedSave()
{
	UpdateData(true);
	//更改设置
	globle_var::SetGrabSetting(set_grabRGBType == "RGB" ? "RGB" : "Gray", set_grabFrameCount, set_grabWidth);
	globle_var::TiggerWaitTimeMS = set_TiggerWaitTimeMS;
	globle_var::GrabTimeMS = set_grabTime;
	globle_var::FrameTimeUS = frameTimeUS;

	//存入ini
	globle_var::SaveSetting();

	EndDialog(IDOK);
}


void SettingDlg::OnBnClickedCancel()
{
	EndDialog(IDCANCEL);
}



void SettingDlg::OnEnKillfocusTbGrabtime()
{
	calculateFrameTime();
}


void SettingDlg::OnEnChangeTbGrabframecount()
{
	calculateFrameTime();
}


void SettingDlg::OnEnChangeTbGrabtime()
{
	calculateFrameTime();
}
