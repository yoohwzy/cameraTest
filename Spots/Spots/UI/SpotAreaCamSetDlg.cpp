// D:\��Ŀ����\cameraTest\Spots\Spots\UI\SpotAreaCamSetDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Spots.h"
#include "SpotAreaCamSetDlg.h"
#include "afxdialogex.h"
#include <Class/Setting/SettingHelper.h>


// SpotAreaCamSetDlg �Ի���

IMPLEMENT_DYNAMIC(SpotAreaCamSetDlg, CDialogEx)

SpotAreaCamSetDlg::SpotAreaCamSetDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(SpotAreaCamSetDlg::IDD, pParent)
{
}

SpotAreaCamSetDlg::~SpotAreaCamSetDlg()
{
}


BOOL SpotAreaCamSetDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	int areaCamEnable = 1;
	if (SettingHelper::GetKeyInt("AreaCam", "Enable", areaCamEnable))
		((CButton*)GetDlgItem(IDC_CB_AreaCamEnabled))->SetCheck(areaCamEnable != 0);
	else
		((CButton*)GetDlgItem(IDC_CB_AreaCamEnabled))->SetCheck(1);

	int save_image = 0;
	if (SettingHelper::GetKeyInt("AreaCam", "SAVE_IMG", save_image))
		((CButton*)GetDlgItem(IDC_CB_AreaCam_SAVE_IMG))->SetCheck(save_image != 0);
	else
		((CButton*)GetDlgItem(IDC_CB_AreaCam_SAVE_IMG))->SetCheck(1);

	double WaitTimeMSIn;
	if (SettingHelper::GetKeyDouble("AreaCam", "WaitTimeMSIn", WaitTimeMSIn))
		GetDlgItem(IDC_TB_AreaWaitTimeMSIn)->SetWindowText(StringHelper::double2CString(WaitTimeMSIn));
	else
		GetDlgItem(IDC_TB_AreaWaitTimeMSIn)->SetWindowText(L"300");

	double WaitTimeMSOut;
	if (SettingHelper::GetKeyDouble("AreaCam", "WaitTimeMSOut", WaitTimeMSOut))
		GetDlgItem(IDC_TB_AreaWaitTimeMSOut)->SetWindowText(StringHelper::double2CString(WaitTimeMSOut));
	else
		GetDlgItem(IDC_TB_AreaWaitTimeMSOut)->SetWindowText(L"300");

	string MainHSVs;
	if (SettingHelper::GetKeyString("AreaCam", "MainHSVs", MainHSVs))
		GetDlgItem(IDC_TB_AreaStandardHSVs)->SetWindowText(StringHelper::string2CString(MainHSVs));
	else
		GetDlgItem(IDC_TB_AreaStandardHSVs)->SetWindowText(L"300");


	//double AreaCamDiff;
	//if (SettingHelper::GetKeyDouble("AreaCam", "AreaCamDiff", AreaCamDiff))
	//	GetDlgItem(IDC_TB_AreaCamDiff)->SetWindowText(StringHelper::double2CString(WaitTimeMSOut));
	//else
	//	GetDlgItem(IDC_TB_AreaCamDiff)->SetWindowText(L"90");


	return TRUE;
}
void SpotAreaCamSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}
void SpotAreaCamSetDlg::OnPaint()
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
HBRUSH SpotAreaCamSetDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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


BEGIN_MESSAGE_MAP(SpotAreaCamSetDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &SpotAreaCamSetDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// SpotAreaCamSetDlg ��Ϣ�������
void SpotAreaCamSetDlg::saveParameter(int IDC, string SectionName, string key)
{
	//��������
	CString cstr;
	GetDlgItem(IDC)->GetWindowTextW(cstr);
	SettingHelper::AddKey(SectionName, key, StringHelper::CString2string(cstr));
}// D:\��Ŀ����\cameraTest\Spots\Spots\UI\SpotAreaCamSetDlg.cpp : ʵ���ļ�
//


void SpotAreaCamSetDlg::OnBnClickedOk()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CB_AreaCamEnabled);
	SettingHelper::AddKey("AreaCam", "Enable", pBtn->GetCheck());
	pBtn = (CButton*)GetDlgItem(IDC_CB_AreaCam_SAVE_IMG);
	SettingHelper::AddKey("AreaCam", "SAVE_IMG", pBtn->GetCheck());
	saveParameter(IDC_TB_AreaWaitTimeMSIn, "AreaCam", "WaitTimeMSIn");
	saveParameter(IDC_TB_AreaWaitTimeMSOut, "AreaCam", "WaitTimeMSOut");
	saveParameter(IDC_TB_AreaStandardHSVs, "AreaCam", "MainHSVs");
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnOK();
}
