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
	, set_grabMaxPics(0)
	, set_grabWidth(0)
	, set_grabRGBType(_T(""))
	, set_TiggerWaitTimeMS(0)
{

}

BOOL SettingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//读取配置文件
	if (globle_var::InitSetting(true))
	{
		//设置参数
		set_grabMaxPics = globle_var::mdi().MaxPics;
		set_grabWidth = globle_var::mdi().Width;
		set_grabRGBType = globle_var::mdi().colorType == globle_var::mdi().RGB ? "RGB" : "Gray";
		set_TiggerWaitTimeMS = globle_var::TiggerWaitTimeMS;
	}
	else
	{
		MessageBox(L"某些参数读取失败，请保存正确的参数信息！");
	}
	UpdateData(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


SettingDlg::~SettingDlg()
{
}

void SettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TB_GrabMaxPics, set_grabMaxPics);
	DDX_Text(pDX, IDC_TB_GrabWidth, set_grabWidth);
	DDX_CBString(pDX, IDC_DDL_GrabRGBType, set_grabRGBType);
	DDX_Text(pDX, IDC_TB_TiggerWaitMS, set_TiggerWaitTimeMS);
}


BEGIN_MESSAGE_MAP(SettingDlg, CDialogEx)
	ON_BN_CLICKED(IDC_SAVE, &SettingDlg::OnBnClickedSave)
	ON_BN_CLICKED(IDC_Cancel, &SettingDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// SettingDlg 消息处理程序


void SettingDlg::OnBnClickedSave()
{
	UpdateData(true);
	//更改设置
	globle_var::SetGrabSetting(set_grabRGBType == "RGB" ? "RGB" : "Gray", set_grabMaxPics, set_grabWidth);
	globle_var::TiggerWaitTimeMS = set_TiggerWaitTimeMS;

	//存入ini
	globle_var::SaveSetting();


	EndDialog(IDOK);
}


void SettingDlg::OnBnClickedCancel()
{
	EndDialog(IDCANCEL);
}

