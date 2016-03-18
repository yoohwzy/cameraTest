// E:\项目资料\cameraTest\Spots\Spots\UI\SpotsEdgeParameterSetDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Spots.h"
#include "SpotsEdgeParameterSetDlg.h"
#include "afxdialogex.h"
#include <Class/Setting/SettingHelper.h>


// SpotsEdgeParameterSetDlg 对话框

IMPLEMENT_DYNAMIC(SpotsEdgeParameterSetDlg, CDialogEx)

SpotsEdgeParameterSetDlg::SpotsEdgeParameterSetDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(SpotsEdgeParameterSetDlg::IDD, pParent)
{

}

SpotsEdgeParameterSetDlg::~SpotsEdgeParameterSetDlg()
{
}
void SpotsEdgeParameterSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL SpotsEdgeParameterSetDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	string BlockLocalizer_THRESHOD;
	if (SettingHelper::GetKeyString("EDGE_PARAMETER", "BlockLocalizer_THRESHOD", BlockLocalizer_THRESHOD))//读取数据库所在路径
	{
		GetDlgItem(IDC_TB_BlockLocalizer_THRESHOD)->SetWindowText(StringHelper::string2LPWSTR(BlockLocalizer_THRESHOD));
	}
	else
	{
		GetDlgItem(IDC_TB_BlockLocalizer_THRESHOD)->SetWindowText(L"10");
	}

	string BlockLocalizer_ContinuePointCount;
	if (SettingHelper::GetKeyString("EDGE_PARAMETER", "BlockLocalizer_ContinuePointCount", BlockLocalizer_ContinuePointCount))//读取数据库所在路径
	{
		GetDlgItem(IDC_TB_BlockLocalizer_ContinuePointCount)->SetWindowText(StringHelper::string2LPWSTR(BlockLocalizer_ContinuePointCount));
	}
	else
	{
		GetDlgItem(IDC_TB_BlockLocalizer_ContinuePointCount)->SetWindowText(L"30");
	}

	string BlockEdgeDetector_DIFF_THRESHOLD;
	if (SettingHelper::GetKeyString("EDGE_PARAMETER", "BlockEdgeDetector_DIFF_THRESHOLD", BlockEdgeDetector_DIFF_THRESHOLD))//读取数据库所在路径
	{
		GetDlgItem(IDC_TB_BlockEdgeDetector_DIFF_THRESHOLD)->SetWindowText(StringHelper::string2LPWSTR(BlockEdgeDetector_DIFF_THRESHOLD));
	}
	else
	{
		GetDlgItem(IDC_TB_BlockEdgeDetector_DIFF_THRESHOLD)->SetWindowText(L"7");
	}

	string BlockEdgeDetector_FAULTS_SPAN;
	if (SettingHelper::GetKeyString("EDGE_PARAMETER", "BlockEdgeDetector_FAULTS_SPAN", BlockEdgeDetector_FAULTS_SPAN))//读取数据库所在路径
	{
		GetDlgItem(IDC_TB_BlockEdgeDetector_FAULTS_SPAN)->SetWindowText(StringHelper::string2LPWSTR(BlockEdgeDetector_FAULTS_SPAN));
	}
	else
	{
		GetDlgItem(IDC_TB_BlockEdgeDetector_FAULTS_SPAN)->SetWindowText(L"4");
	}

	string BlockEdgeDetector_FAULTS_COUNT;
	if (SettingHelper::GetKeyString("EDGE_PARAMETER", "BlockEdgeDetector_FAULTS_COUNT", BlockEdgeDetector_FAULTS_COUNT))//读取数据库所在路径
	{
		GetDlgItem(IDC_TB_BlockEdgeDetector_FAULTS_COUNT)->SetWindowText(StringHelper::string2LPWSTR(BlockEdgeDetector_FAULTS_COUNT));
	}
	else
	{
		GetDlgItem(IDC_TB_BlockEdgeDetector_FAULTS_COUNT)->SetWindowText(L"5");
	}

	return TRUE;
}
void SpotsEdgeParameterSetDlg::OnPaint()
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
HBRUSH SpotsEdgeParameterSetDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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


BEGIN_MESSAGE_MAP(SpotsEdgeParameterSetDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDOK, &SpotsEdgeParameterSetDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// SpotsEdgeParameterSetDlg 消息处理程序


void SpotsEdgeParameterSetDlg::OnBnClickedOk()
{
	//保存设置
	saveParameter(IDC_TB_BlockLocalizer_THRESHOD, "EDGE_PARAMETER", "IDC_TB_BlockLocalizer_THRESHOD");
	saveParameter(IDC_TB_BlockLocalizer_ContinuePointCount, "EDGE_PARAMETER", "BlockLocalizer_ContinuePointCount");
	saveParameter(IDC_TB_BlockEdgeDetector_DIFF_THRESHOLD, "EDGE_PARAMETER", "BlockEdgeDetector_DIFF_THRESHOLD");
	saveParameter(IDC_TB_BlockEdgeDetector_FAULTS_SPAN, "EDGE_PARAMETER", "BlockEdgeDetector_FAULTS_SPAN");
	saveParameter(IDC_TB_BlockEdgeDetector_FAULTS_COUNT, "EDGE_PARAMETER", "BlockEdgeDetector_FAULTS_COUNT");
	CDialogEx::OnOK();
}

void SpotsEdgeParameterSetDlg::saveParameter(int IDC, string SectionName, string key)
{
	//保存设置
	CString cstr;
	GetDlgItem(IDC)->GetWindowTextW(cstr);
	SettingHelper::AddKey(SectionName, key, StringHelper::CString2string(cstr));
}