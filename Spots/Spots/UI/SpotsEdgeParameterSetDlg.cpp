// E:\��Ŀ����\cameraTest\Spots\Spots\UI\SpotsEdgeParameterSetDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Spots.h"
#include "SpotsEdgeParameterSetDlg.h"
#include "afxdialogex.h"
#include <Class/Setting/SettingHelper.h>


// SpotsEdgeParameterSetDlg �Ի���

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

	int BlockLocalizer_THRESHOD;
	if (SettingHelper::GetKeyInt("EDGE_PARAMETER", "BlockLocalizer_THRESHOD", BlockLocalizer_THRESHOD))
		GetDlgItem(IDC_TB_BlockLocalizer_THRESHOD)->SetWindowText(StringHelper::int2CString(BlockLocalizer_THRESHOD));
	else
		GetDlgItem(IDC_TB_BlockLocalizer_THRESHOD)->SetWindowText(L"10");

	int BlockLocalizer_ContinuePointCount;
	if (SettingHelper::GetKeyInt("EDGE_PARAMETER", "BlockLocalizer_ContinuePointCount", BlockLocalizer_ContinuePointCount))
		GetDlgItem(IDC_TB_BlockLocalizer_ContinuePointCount)->SetWindowText(StringHelper::int2CString(BlockLocalizer_ContinuePointCount));
	else
		GetDlgItem(IDC_TB_BlockLocalizer_ContinuePointCount)->SetWindowText(L"30");






	/*****��Ե���ƶ��㷨*****/
	int bedEnable = 1;
	if (SettingHelper::GetKeyInt("EDGE_PARAMETER", "BlockEdgeDetector_Enable", bedEnable))
		((CButton*)GetDlgItem(IDC_CB_EnableBlockEdgeDetector))->SetCheck(bedEnable != 0);
	else
		((CButton*)GetDlgItem(IDC_CB_EnableBlockEdgeDetector))->SetCheck(1);

	double BlockEdgeDetector_DIFF_THRESHOLD;
	if (SettingHelper::GetKeyDouble("EDGE_PARAMETER", "BlockEdgeDetector_DIFF_THRESHOLD", BlockEdgeDetector_DIFF_THRESHOLD))
		GetDlgItem(IDC_TB_BlockEdgeDetector_DIFF_THRESHOLD)->SetWindowText(StringHelper::double2CString(BlockEdgeDetector_DIFF_THRESHOLD));
	else
		GetDlgItem(IDC_TB_BlockEdgeDetector_DIFF_THRESHOLD)->SetWindowText(L"7");

	int BlockEdgeDetector_FAULTS_SPAN;
	if (SettingHelper::GetKeyInt("EDGE_PARAMETER", "BlockEdgeDetector_FAULTS_SPAN", BlockEdgeDetector_FAULTS_SPAN))
		GetDlgItem(IDC_TB_BlockEdgeDetector_FAULTS_SPAN)->SetWindowText(StringHelper::int2CString(BlockEdgeDetector_FAULTS_SPAN));
	else
		GetDlgItem(IDC_TB_BlockEdgeDetector_FAULTS_SPAN)->SetWindowText(L"4");

	int BlockEdgeDetector_FAULTS_COUNT;
	if (SettingHelper::GetKeyInt("EDGE_PARAMETER", "BlockEdgeDetector_FAULTS_COUNT", BlockEdgeDetector_FAULTS_COUNT))
		GetDlgItem(IDC_TB_BlockEdgeDetector_FAULTS_COUNT)->SetWindowText(StringHelper::int2CString(BlockEdgeDetector_FAULTS_COUNT));
	else
		GetDlgItem(IDC_TB_BlockEdgeDetector_FAULTS_COUNT)->SetWindowText(L"5");


	/*****��Եɨ���㷨*****/
	int beldEnable = 1;
	if (SettingHelper::GetKeyInt("EDGE_PARAMETER", "BlockEdgeLineDetector_Enable", beldEnable))
		((CButton*)GetDlgItem(IDC_CB_EnableBlockEdgeLineDetector))->SetCheck(beldEnable != 0);
	else
		((CButton*)GetDlgItem(IDC_CB_EnableBlockEdgeLineDetector))->SetCheck(1);

	//��ֵ����ֵ
	int BlockEdgeLineDetector_BINARY_THRESHOD;
	if (SettingHelper::GetKeyInt("EDGE_PARAMETER", "BlockEdgeLineDetector_BINARY_THRESHOD", BlockEdgeLineDetector_BINARY_THRESHOD))
		GetDlgItem(IDC_TB_BlockEdgeLineDetector_BINARY_THRESHOD)->SetWindowText(StringHelper::double2CString(BlockEdgeLineDetector_BINARY_THRESHOD));
	else
		GetDlgItem(IDC_TB_BlockEdgeLineDetector_BINARY_THRESHOD)->SetWindowText(L"5");
	//���߳���ֵ
	int BlockEdgeLineDetector_LENGTH_THRESHOD;
	if (SettingHelper::GetKeyInt("EDGE_PARAMETER", "BlockEdgeLineDetector_LENGTH_THRESHOD", BlockEdgeLineDetector_LENGTH_THRESHOD))
		GetDlgItem(IDC_TB_BlockEdgeLineDetector_LENGTH_THRESHOD)->SetWindowText(StringHelper::double2CString(BlockEdgeLineDetector_LENGTH_THRESHOD));
	else
		GetDlgItem(IDC_TB_BlockEdgeLineDetector_LENGTH_THRESHOD)->SetWindowText(L"5");
	//��������ֵ
	int BlockEdgeLineDetector_DEEP_THRESHOD;
	if (SettingHelper::GetKeyInt("EDGE_PARAMETER", "BlockEdgeLineDetector_DEEP_THRESHOD", BlockEdgeLineDetector_DEEP_THRESHOD))
		GetDlgItem(IDC_TB_BlockEdgeLineDetector_DEEP_THRESHOD)->SetWindowText(StringHelper::double2CString(BlockEdgeLineDetector_DEEP_THRESHOD));
	else
		GetDlgItem(IDC_TB_BlockEdgeLineDetector_DEEP_THRESHOD)->SetWindowText(L"5");


	return TRUE;
}
void SpotsEdgeParameterSetDlg::OnPaint()
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
HBRUSH SpotsEdgeParameterSetDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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


BEGIN_MESSAGE_MAP(SpotsEdgeParameterSetDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDOK, &SpotsEdgeParameterSetDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// SpotsEdgeParameterSetDlg ��Ϣ�������


void SpotsEdgeParameterSetDlg::OnBnClickedOk()
{
	//��������
	saveParameter(IDC_TB_BlockLocalizer_THRESHOD, "EDGE_PARAMETER", "BlockLocalizer_THRESHOD");
	saveParameter(IDC_TB_BlockLocalizer_ContinuePointCount, "EDGE_PARAMETER", "BlockLocalizer_ContinuePointCount");


	CButton* pBtn = (CButton*)GetDlgItem(IDC_CB_EnableBlockEdgeDetector);
	SettingHelper::AddKey("EDGE_PARAMETER", "BlockEdgeDetector_Enable", pBtn->GetCheck());
	saveParameter(IDC_TB_BlockEdgeDetector_DIFF_THRESHOLD, "EDGE_PARAMETER", "BlockEdgeDetector_DIFF_THRESHOLD");
	saveParameter(IDC_TB_BlockEdgeDetector_FAULTS_SPAN, "EDGE_PARAMETER", "BlockEdgeDetector_FAULTS_SPAN");
	saveParameter(IDC_TB_BlockEdgeDetector_FAULTS_COUNT, "EDGE_PARAMETER", "BlockEdgeDetector_FAULTS_COUNT");





	/*****��Եɨ���㷨*****/
	pBtn = (CButton*)GetDlgItem(IDC_CB_EnableBlockEdgeLineDetector);
	SettingHelper::AddKey("EDGE_PARAMETER", "BlockEdgeLineDetector_Enable", pBtn->GetCheck());
	saveParameter(IDC_TB_BlockEdgeLineDetector_BINARY_THRESHOD, "EDGE_PARAMETER", "BlockEdgeLineDetector_BINARY_THRESHOD");
	saveParameter(IDC_TB_BlockEdgeLineDetector_LENGTH_THRESHOD, "EDGE_PARAMETER", "BlockEdgeLineDetector_LENGTH_THRESHOD");
	saveParameter(IDC_TB_BlockEdgeLineDetector_DEEP_THRESHOD, "EDGE_PARAMETER", "BlockEdgeLineDetector_DEEP_THRESHOD");

	CDialogEx::OnOK();
}

void SpotsEdgeParameterSetDlg::saveParameter(int IDC, string SectionName, string key)
{
	//��������
	CString cstr;
	GetDlgItem(IDC)->GetWindowTextW(cstr);
	SettingHelper::AddKey(SectionName, key, StringHelper::CString2string(cstr));
}