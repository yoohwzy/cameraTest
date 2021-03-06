// D:\项目资料\cameraTest\Spots\Spots\UI\SpotAreaCamSetDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Spots.h"
#include "SpotAreaCamSetDlg.h"
#include "afxdialogex.h"
#include <Class/Setting/SettingHelper.h>
#include <thread>
#include <Model\MainHueScanner.h>
// SpotAreaCamSetDlg 对话框

IMPLEMENT_DYNAMIC(SpotAreaCamSetDlg, CDialogEx)

SpotAreaCamSetDlg::SpotAreaCamSetDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(SpotAreaCamSetDlg::IDD, pParent)
{
}

SpotAreaCamSetDlg::~SpotAreaCamSetDlg()
{
	biaoDingExitFlag = true;
	while (!biaodingDispThreadEndFlag)//等待线程结束
		Sleep(10);
	if (p_mvcam != NULL)
	{
		delete p_mvcam;
		p_mvcam = NULL;
	}
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
HBRUSH SpotAreaCamSetDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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


BEGIN_MESSAGE_MAP(SpotAreaCamSetDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &SpotAreaCamSetDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_AreaCamSetDlg_BTN_DingBiao, &SpotAreaCamSetDlg::OnBnClickedAreacamsetdlgBtnDingbiao)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// SpotAreaCamSetDlg 消息处理程序
void SpotAreaCamSetDlg::saveParameter(int IDC, string SectionName, string key)
{
	//保存设置
	CString cstr;
	GetDlgItem(IDC)->GetWindowTextW(cstr);
	SettingHelper::AddKey(SectionName, key, StringHelper::CString2string(cstr));
}// D:\项目资料\cameraTest\Spots\Spots\UI\SpotAreaCamSetDlg.cpp : 实现文件
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
	// TODO:  在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}


void SpotAreaCamSetDlg::OnBnClickedAreacamsetdlgBtnDingbiao()
{
	if (biaoDingExitFlag)
	{
		p_mvcam = new MVCAM;
		p_mvcam->ColorType = CV_8UC3;
		p_mvcam->ExposureTimeMS = 50;
		p_mvcam->AnalogGain = 2;
		p_mvcam->Init();
		if (p_mvcam->HasInited)
		{
			SetDlgItemText(IDC_AreaCamSetDlg_BTN_DingBiao, L"结束定标");
			p_mvcam->StartCapture();
			biaoDingExitFlag = false;
			std::thread t_run(std::mem_fn(&SpotAreaCamSetDlg::biaodingDispThread), this);
			t_run.detach();
		}
		else
		{
			AfxMessageBox(L"初始化面阵相机失败！");
		}
	}
	else
	{
		biaoDingExitFlag = true;
		SetDlgItemText(IDC_AreaCamSetDlg_BTN_DingBiao, L"开始定标");
		while (!biaodingDispThreadEndFlag)//等待线程结束
			Sleep(10);

		delete p_mvcam;
		p_mvcam = NULL;
	}
}
void SpotAreaCamSetDlg::biaodingDispThread()
{
	biaoDingExitFlag = false;
	biaodingDispThreadEndFlag = false;
	cv::Mat disp;
	cv::namedWindow("DingBiao");
	while (!biaoDingExitFlag)
	{
		disp = p_mvcam->Grub();
		cv::imshow("DingBiao", disp);
		cv::waitKey(5);
	}
	cv::destroyWindow("DingBiao");
	biaodingDispThreadEndFlag = true;
	MainHueScanner::DingBiao(disp);
	stringstream ss;
	ss << MainHueScanner::Standard_H << "," << MainHueScanner::Standard_S << "," << MainHueScanner::Standard_V;
	GetDlgItem(IDC_TB_AreaStandardHSVs)->SetWindowText(StringHelper::string2CString(ss.str()));
}
