// SettingDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TileDetectorMFC.h"
#include "SettingDlg.h"
#include "afxdialogex.h"


// SettingDlg �Ի���

IMPLEMENT_DYNAMIC(SettingDlg, CDialogEx)

SettingDlg::SettingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(SettingDlg::IDD, pParent)
	, set_grabFrameCount(0)
	, set_grabWidth(0)
	, set_grabRGBType(_T(""))
	, set_TiggerWaitTimeMS(0)
{

}

BOOL SettingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//��ȡ�����ļ�
	if (globle_var::InitSetting(true))
	{
		//���ò���
		set_grabFrameCount = globle_var::FrameCount;
		set_grabWidth = globle_var::Width;
		set_grabRGBType = globle_var::ColorType == globle_var::RGB ? "RGB" : "Gray";
		set_TiggerWaitTimeMS = globle_var::TiggerWaitTimeMS;
	}
	else
	{
		MessageBox(L"ĳЩ������ȡʧ�ܣ��뱣����ȷ�Ĳ�����Ϣ��");
	}
	UpdateData(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}


SettingDlg::~SettingDlg()
{
}

void SettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TB_GrabFrameCount, set_grabFrameCount);
	DDX_Text(pDX, IDC_TB_GrabWidth, set_grabWidth);
	DDX_CBString(pDX, IDC_DDL_GrabRGBType, set_grabRGBType);
	DDX_Text(pDX, IDC_TB_TiggerWaitMS, set_TiggerWaitTimeMS);
}


BEGIN_MESSAGE_MAP(SettingDlg, CDialogEx)
	ON_BN_CLICKED(IDC_SAVE, &SettingDlg::OnBnClickedSave)
	ON_BN_CLICKED(IDC_Cancel, &SettingDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// SettingDlg ��Ϣ�������


void SettingDlg::OnBnClickedSave()
{
	UpdateData(true);
	//��������
	globle_var::SetGrabSetting(set_grabRGBType == "RGB" ? "RGB" : "Gray", set_grabFrameCount, set_grabWidth);
	globle_var::TiggerWaitTimeMS = set_TiggerWaitTimeMS;

	//����ini
	globle_var::SaveSetting();


	EndDialog(IDOK);
}


void SettingDlg::OnBnClickedCancel()
{
	EndDialog(IDCANCEL);
}

