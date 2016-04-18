// E:\项目资料\cameraTest\Spots\Spots\UI\SpotsClassifySet.cpp : 实现文件
//

#include "stdafx.h"
#include "Spots.h"
#include "SpotsClassifySet.h"
#include "afxdialogex.h"
#include <Class/Setting/SettingHelper.h>


// CSpotsClassifySet 对话框

IMPLEMENT_DYNAMIC(CSpotsClassifySet, CDialogEx)

CSpotsClassifySet::CSpotsClassifySet(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSpotsClassifySet::IDD, pParent)
{

}

CSpotsClassifySet::~CSpotsClassifySet()
{
}
BOOL CSpotsClassifySet::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//缺边 长

	int TB_EDGE_SINGLE_LENGTH_A;
	if (SettingHelper::GetKeyInt("Classify", "EDGE_SINGLE_LENGTH_A", TB_EDGE_SINGLE_LENGTH_A))//读取数据库所在路径
		GetDlgItem(IDC_TB_EDGE_SINGLE_LENGTH_A)->SetWindowText(StringHelper::int2CString(TB_EDGE_SINGLE_LENGTH_A));
	else
		GetDlgItem(IDC_TB_EDGE_SINGLE_LENGTH_A)->SetWindowText(L"0");

	int TB_EDGE_SINGLE_LENGTH_B;
	if (SettingHelper::GetKeyInt("Classify", "EDGE_SINGLE_LENGTH_B", TB_EDGE_SINGLE_LENGTH_B))//读取数据库所在路径
		GetDlgItem(IDC_TB_EDGE_SINGLE_LENGTH_B)->SetWindowText(StringHelper::int2CString(TB_EDGE_SINGLE_LENGTH_B));
	else
		GetDlgItem(IDC_TB_EDGE_SINGLE_LENGTH_B)->SetWindowText(L"4");

	int TB_EDGE_SINGLE_LENGTH_C;
	if (SettingHelper::GetKeyInt("Classify", "EDGE_SINGLE_LENGTH_C", TB_EDGE_SINGLE_LENGTH_C))//读取数据库所在路径
		GetDlgItem(IDC_TB_EDGE_SINGLE_LENGTH_C)->SetWindowText(StringHelper::int2CString(TB_EDGE_SINGLE_LENGTH_C));
	else
		GetDlgItem(IDC_TB_EDGE_SINGLE_LENGTH_C)->SetWindowText(L"15");

	int TB_EDGE_SINGLE_LENGTH_ACCEPT;
	if (SettingHelper::GetKeyInt("Classify", "EDGE_SINGLE_LENGTH_ACCEPT", TB_EDGE_SINGLE_LENGTH_ACCEPT))//读取数据库所在路径
		GetDlgItem(IDC_TB_EDGE_SINGLE_LENGTH_ACCEPT)->SetWindowText(StringHelper::int2CString(TB_EDGE_SINGLE_LENGTH_ACCEPT));
	else
		GetDlgItem(IDC_TB_EDGE_SINGLE_LENGTH_ACCEPT)->SetWindowText(L"15");

	//总长
	int TB_EDGE_TOTAL_LENGTH_A;
	if (SettingHelper::GetKeyInt("Classify", "EDGE_TOTAL_LENGTH_A", TB_EDGE_TOTAL_LENGTH_A))//读取数据库所在路径
		GetDlgItem(IDC_TB_EDGE_TOTAL_LENGTH_A)->SetWindowText(StringHelper::int2CString(TB_EDGE_TOTAL_LENGTH_A));
	else
		GetDlgItem(IDC_TB_EDGE_TOTAL_LENGTH_A)->SetWindowText(L"0");

	int TB_EDGE_TOTAL_LENGTH_B;
	if (SettingHelper::GetKeyInt("Classify", "EDGE_TOTAL_LENGTH_B", TB_EDGE_TOTAL_LENGTH_B))//读取数据库所在路径
		GetDlgItem(IDC_TB_EDGE_TOTAL_LENGTH_B)->SetWindowText(StringHelper::int2CString(TB_EDGE_TOTAL_LENGTH_B));
	else
		GetDlgItem(IDC_TB_EDGE_TOTAL_LENGTH_B)->SetWindowText(L"4");

	int TB_EDGE_TOTAL_LENGTH_C;
	if (SettingHelper::GetKeyInt("Classify", "EDGE_TOTAL_LENGTH_C", TB_EDGE_TOTAL_LENGTH_C))//读取数据库所在路径
		GetDlgItem(IDC_TB_EDGE_TOTAL_LENGTH_C)->SetWindowText(StringHelper::int2CString(TB_EDGE_TOTAL_LENGTH_C));
	else
		GetDlgItem(IDC_TB_EDGE_TOTAL_LENGTH_C)->SetWindowText(L"22");

	int TB_EDGE_TOTAL_LENGTH_ACCEPT;
	if (SettingHelper::GetKeyInt("Classify", "EDGE_TOTAL_LENGTH_ACCEPT", TB_EDGE_TOTAL_LENGTH_ACCEPT))//读取数据库所在路径
		GetDlgItem(IDC_TB_EDGE_TOTAL_LENGTH_ACCEPT)->SetWindowText(StringHelper::int2CString(TB_EDGE_TOTAL_LENGTH_ACCEPT));
	else
		GetDlgItem(IDC_TB_EDGE_TOTAL_LENGTH_ACCEPT)->SetWindowText(L"22");



	//深度
	int TB_EDGE_SINGLE_DEEP_A;
	if (SettingHelper::GetKeyInt("Classify", "EDGE_SINGLE_DEEP_A", TB_EDGE_SINGLE_DEEP_A))//读取数据库所在路径
		GetDlgItem(IDC_TB_EDGE_SINGLE_DEEP_A)->SetWindowText(StringHelper::int2CString(TB_EDGE_SINGLE_DEEP_A));
	else
		GetDlgItem(IDC_TB_EDGE_SINGLE_DEEP_A)->SetWindowText(L"0");

	int TB_EDGE_SINGLE_DEEP_B;
	if (SettingHelper::GetKeyInt("Classify", "EDGE_SINGLE_DEEP_B", TB_EDGE_SINGLE_DEEP_B))//读取数据库所在路径
		GetDlgItem(IDC_TB_EDGE_SINGLE_DEEP_B)->SetWindowText(StringHelper::int2CString(TB_EDGE_SINGLE_DEEP_B));
	else
		GetDlgItem(IDC_TB_EDGE_SINGLE_DEEP_B)->SetWindowText(L"2");

	int TB_EDGE_SINGLE_DEEP_C;
	if (SettingHelper::GetKeyInt("Classify", "EDGE_SINGLE_DEEP_C", TB_EDGE_SINGLE_DEEP_C))//读取数据库所在路径
		GetDlgItem(IDC_TB_EDGE_SINGLE_DEEP_C)->SetWindowText(StringHelper::int2CString(TB_EDGE_SINGLE_DEEP_C));
	else
		GetDlgItem(IDC_TB_EDGE_SINGLE_DEEP_C)->SetWindowText(L"2");

	int TB_EDGE_SINGLE_DEEP_ACCEPT;
	if (SettingHelper::GetKeyInt("Classify", "EDGE_SINGLE_DEEP_ACCEPT", TB_EDGE_SINGLE_DEEP_ACCEPT))//读取数据库所在路径
		GetDlgItem(IDC_TB_EDGE_SINGLE_DEEP_ACCEPT)->SetWindowText(StringHelper::int2CString(TB_EDGE_SINGLE_DEEP_ACCEPT));
	else
		GetDlgItem(IDC_TB_EDGE_SINGLE_DEEP_ACCEPT)->SetWindowText(L"2");

	//总深度
	int TB_EDGE_TOTAL_DEEP_A;
	if (SettingHelper::GetKeyInt("Classify", "EDGE_TOTAL_DEEP_A", TB_EDGE_TOTAL_DEEP_A))//读取数据库所在路径
		GetDlgItem(IDC_TB_EDGE_TOTAL_DEEP_A)->SetWindowText(StringHelper::int2CString(TB_EDGE_TOTAL_DEEP_A));
	else
		GetDlgItem(IDC_TB_EDGE_TOTAL_DEEP_A)->SetWindowText(L"0");

	int TB_EDGE_TOTAL_DEEP_B;
	if (SettingHelper::GetKeyInt("Classify", "EDGE_TOTAL_DEEP_B", TB_EDGE_TOTAL_DEEP_B))//读取数据库所在路径
		GetDlgItem(IDC_TB_EDGE_TOTAL_DEEP_B)->SetWindowText(StringHelper::int2CString(TB_EDGE_TOTAL_DEEP_B));
	else
		GetDlgItem(IDC_TB_EDGE_TOTAL_DEEP_B)->SetWindowText(L"5");

	int TB_EDGE_TOTAL_DEEP_C;
	if (SettingHelper::GetKeyInt("Classify", "EDGE_TOTAL_DEEP_C", TB_EDGE_TOTAL_DEEP_C))//读取数据库所在路径
		GetDlgItem(IDC_TB_EDGE_TOTAL_DEEP_C)->SetWindowText(StringHelper::int2CString(TB_EDGE_TOTAL_DEEP_C));
	else
		GetDlgItem(IDC_TB_EDGE_TOTAL_DEEP_C)->SetWindowText(L"5");

	int TB_EDGE_TOTAL_DEEP_ACCEPT;
	if (SettingHelper::GetKeyInt("Classify", "EDGE_TOTAL_DEEP_ACCEPT", TB_EDGE_TOTAL_DEEP_ACCEPT))//读取数据库所在路径
		GetDlgItem(IDC_TB_EDGE_TOTAL_DEEP_ACCEPT)->SetWindowText(StringHelper::int2CString(TB_EDGE_TOTAL_DEEP_ACCEPT));
	else
		GetDlgItem(IDC_TB_EDGE_TOTAL_DEEP_ACCEPT)->SetWindowText(L"5");



	//凹凸

	int TB_HOAL_DIAMETER_A;
	if (SettingHelper::GetKeyInt("Classify", "HOAL_DIAMETER_A", TB_HOAL_DIAMETER_A))//读取数据库所在路径
		GetDlgItem(IDC_TB_HOAL_DIAMETER_A)->SetWindowText(StringHelper::int2CString(TB_HOAL_DIAMETER_A));
	else
		GetDlgItem(IDC_TB_HOAL_DIAMETER_A)->SetWindowText(L"0");

	int TB_HOAL_DIAMETER_B;
	if (SettingHelper::GetKeyInt("Classify", "HOAL_DIAMETER_B", TB_HOAL_DIAMETER_B))//读取数据库所在路径
		GetDlgItem(IDC_TB_HOAL_DIAMETER_B)->SetWindowText(StringHelper::int2CString(TB_HOAL_DIAMETER_B));
	else
		GetDlgItem(IDC_TB_HOAL_DIAMETER_B)->SetWindowText(L"1");

	int TB_HOAL_DIAMETER_C;
	if (SettingHelper::GetKeyInt("Classify", "HOAL_DIAMETER_C", TB_HOAL_DIAMETER_C))//读取数据库所在路径
		GetDlgItem(IDC_TB_HOAL_DIAMETER_C)->SetWindowText(StringHelper::int2CString(TB_HOAL_DIAMETER_C));
	else
		GetDlgItem(IDC_TB_HOAL_DIAMETER_C)->SetWindowText(L"1");

	int TB_HOAL_DIAMETER_ACCEPT;
	if (SettingHelper::GetKeyInt("Classify", "HOAL_DIAMETER_ACCEPT", TB_HOAL_DIAMETER_ACCEPT))//读取数据库所在路径
		GetDlgItem(IDC_TB_HOAL_DIAMETER_ACCEPT)->SetWindowText(StringHelper::int2CString(TB_HOAL_DIAMETER_ACCEPT));
	else
		GetDlgItem(IDC_TB_HOAL_DIAMETER_ACCEPT)->SetWindowText(L"1");



	int TB_HOAL_COUNT_A;
	if (SettingHelper::GetKeyInt("Classify", "HOAL_COUNT_A", TB_HOAL_COUNT_A))//读取数据库所在路径
		GetDlgItem(IDC_TB_HOAL_COUNT_A)->SetWindowText(StringHelper::int2CString(TB_HOAL_COUNT_A));
	else
		GetDlgItem(IDC_TB_HOAL_COUNT_A)->SetWindowText(L"0");

	int TB_HOAL_COUNT_B;
	if (SettingHelper::GetKeyInt("Classify", "HOAL_COUNT_B", TB_HOAL_COUNT_B))//读取数据库所在路径
		GetDlgItem(IDC_TB_HOAL_COUNT_B)->SetWindowText(StringHelper::int2CString(TB_HOAL_COUNT_B));
	else
		GetDlgItem(IDC_TB_HOAL_COUNT_B)->SetWindowText(L"3");

	int TB_HOAL_COUNT_C;
	if (SettingHelper::GetKeyInt("Classify", "HOAL_COUNT_C", TB_HOAL_COUNT_C))//读取数据库所在路径
		GetDlgItem(IDC_TB_HOAL_COUNT_C)->SetWindowText(StringHelper::int2CString(TB_HOAL_COUNT_C));
	else
		GetDlgItem(IDC_TB_HOAL_COUNT_C)->SetWindowText(L"10");

	int TB_HOAL_COUNT_ACCEPT;
	if (SettingHelper::GetKeyInt("Classify", "HOAL_COUNT_ACCEPT", TB_HOAL_COUNT_ACCEPT))//读取数据库所在路径
		GetDlgItem(IDC_TB_HOAL_COUNT_ACCEPT)->SetWindowText(StringHelper::int2CString(TB_HOAL_COUNT_ACCEPT));
	else
		GetDlgItem(IDC_TB_HOAL_COUNT_ACCEPT)->SetWindowText(L"10");




	//划痕 长

	int TB_SCRATCH_SINGLE_LENGTH_A;
	if (SettingHelper::GetKeyInt("Classify", "SCRATCH_SINGLE_LENGTH_A", TB_SCRATCH_SINGLE_LENGTH_A))//读取数据库所在路径
		GetDlgItem(IDC_TB_SCRATCH_SINGLE_LENGTH_A)->SetWindowText(StringHelper::int2CString(TB_SCRATCH_SINGLE_LENGTH_A));
	else
		GetDlgItem(IDC_TB_SCRATCH_SINGLE_LENGTH_A)->SetWindowText(L"20");

	int TB_SCRATCH_SINGLE_LENGTH_B;
	if (SettingHelper::GetKeyInt("Classify", "SCRATCH_SINGLE_LENGTH_B", TB_SCRATCH_SINGLE_LENGTH_B))//读取数据库所在路径
		GetDlgItem(IDC_TB_SCRATCH_SINGLE_LENGTH_B)->SetWindowText(StringHelper::int2CString(TB_SCRATCH_SINGLE_LENGTH_B));
	else
		GetDlgItem(IDC_TB_SCRATCH_SINGLE_LENGTH_B)->SetWindowText(L"30");

	int TB_SCRATCH_SINGLE_LENGTH_C;
	if (SettingHelper::GetKeyInt("Classify", "SCRATCH_SINGLE_LENGTH_C", TB_SCRATCH_SINGLE_LENGTH_C))//读取数据库所在路径
		GetDlgItem(IDC_TB_SCRATCH_SINGLE_LENGTH_C)->SetWindowText(StringHelper::int2CString(TB_SCRATCH_SINGLE_LENGTH_C));
	else
		GetDlgItem(IDC_TB_SCRATCH_SINGLE_LENGTH_C)->SetWindowText(L"50");

	int TB_SCRATCH_SINGLE_LENGTH_ACCEPT;
	if (SettingHelper::GetKeyInt("Classify", "SCRATCH_SINGLE_LENGTH_ACCEPT", TB_SCRATCH_SINGLE_LENGTH_ACCEPT))//读取数据库所在路径
		GetDlgItem(IDC_TB_SCRATCH_SINGLE_LENGTH_ACCEPT)->SetWindowText(StringHelper::int2CString(TB_SCRATCH_SINGLE_LENGTH_ACCEPT));
	else
		GetDlgItem(IDC_TB_SCRATCH_SINGLE_LENGTH_ACCEPT)->SetWindowText(L"50");

	//总长
	int TB_SCRATCH_TOTAL_LENGTH_A;
	if (SettingHelper::GetKeyInt("Classify", "SCRATCH_TOTAL_LENGTH_A", TB_SCRATCH_TOTAL_LENGTH_A))//读取数据库所在路径
		GetDlgItem(IDC_TB_SCRATCH_TOTAL_LENGTH_A)->SetWindowText(StringHelper::int2CString(TB_SCRATCH_TOTAL_LENGTH_A));
	else
		GetDlgItem(IDC_TB_SCRATCH_TOTAL_LENGTH_A)->SetWindowText(L"30");

	int TB_SCRATCH_TOTAL_LENGTH_B;
	if (SettingHelper::GetKeyInt("Classify", "SCRATCH_TOTAL_LENGTH_B", TB_SCRATCH_TOTAL_LENGTH_B))//读取数据库所在路径
		GetDlgItem(IDC_TB_SCRATCH_TOTAL_LENGTH_B)->SetWindowText(StringHelper::int2CString(TB_SCRATCH_TOTAL_LENGTH_B));
	else
		GetDlgItem(IDC_TB_SCRATCH_TOTAL_LENGTH_B)->SetWindowText(L"60");

	int TB_SCRATCH_TOTAL_LENGTH_C;
	if (SettingHelper::GetKeyInt("Classify", "SCRATCH_TOTAL_LENGTH_C", TB_SCRATCH_TOTAL_LENGTH_C))//读取数据库所在路径
		GetDlgItem(IDC_TB_SCRATCH_TOTAL_LENGTH_C)->SetWindowText(StringHelper::int2CString(TB_SCRATCH_TOTAL_LENGTH_C));
	else
		GetDlgItem(IDC_TB_SCRATCH_TOTAL_LENGTH_C)->SetWindowText(L"100");

	int TB_SCRATCH_TOTAL_LENGTH_ACCEPT;
	if (SettingHelper::GetKeyInt("Classify", "SCRATCH_TOTAL_LENGTH_ACCEPT", TB_SCRATCH_TOTAL_LENGTH_ACCEPT))//读取数据库所在路径
		GetDlgItem(IDC_TB_SCRATCH_TOTAL_LENGTH_ACCEPT)->SetWindowText(StringHelper::int2CString(TB_SCRATCH_TOTAL_LENGTH_ACCEPT));
	else
		GetDlgItem(IDC_TB_SCRATCH_TOTAL_LENGTH_ACCEPT)->SetWindowText(L"100");






	return TRUE;
}
void CSpotsClassifySet::OnPaint()
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
HBRUSH CSpotsClassifySet::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void CSpotsClassifySet::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSpotsClassifySet, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSpotsClassifySet::OnBnClickedOk)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CSpotsClassifySet 消息处理程序


void CSpotsClassifySet::OnBnClickedOk()
{
	saveParameter(IDC_TB_EDGE_SINGLE_LENGTH_A, "Classify", "EDGE_SINGLE_LENGTH_A");
	saveParameter(IDC_TB_EDGE_SINGLE_LENGTH_B, "Classify", "EDGE_SINGLE_LENGTH_B");
	saveParameter(IDC_TB_EDGE_SINGLE_LENGTH_C, "Classify", "EDGE_SINGLE_LENGTH_C");
	saveParameter(IDC_TB_EDGE_SINGLE_LENGTH_ACCEPT, "Classify", "EDGE_SINGLE_LENGTH_ACCEPT");

	saveParameter(IDC_TB_EDGE_TOTAL_LENGTH_A, "Classify", "EDGE_TOTAL_LENGTH_A");
	saveParameter(IDC_TB_EDGE_TOTAL_LENGTH_B, "Classify", "EDGE_TOTAL_LENGTH_B");
	saveParameter(IDC_TB_EDGE_TOTAL_LENGTH_C, "Classify", "EDGE_TOTAL_LENGTH_C");
	saveParameter(IDC_TB_EDGE_TOTAL_LENGTH_ACCEPT, "Classify", "EDGE_TOTAL_LENGTH_ACCEPT");


	saveParameter(IDC_TB_EDGE_SINGLE_DEEP_A, "Classify", "EDGE_SINGLE_DEEP_A");
	saveParameter(IDC_TB_EDGE_SINGLE_DEEP_B, "Classify", "EDGE_SINGLE_DEEP_B");
	saveParameter(IDC_TB_EDGE_SINGLE_DEEP_C, "Classify", "EDGE_SINGLE_DEEP_C");
	saveParameter(IDC_TB_EDGE_SINGLE_DEEP_ACCEPT, "Classify", "EDGE_SINGLE_DEEP_ACCEPT");

	saveParameter(IDC_TB_EDGE_TOTAL_DEEP_A, "Classify", "EDGE_TOTAL_DEEP_A");
	saveParameter(IDC_TB_EDGE_TOTAL_DEEP_B, "Classify", "EDGE_TOTAL_DEEP_B");
	saveParameter(IDC_TB_EDGE_TOTAL_DEEP_C, "Classify", "EDGE_TOTAL_DEEP_C");
	saveParameter(IDC_TB_EDGE_TOTAL_DEEP_ACCEPT, "Classify", "EDGE_TOTAL_DEEP_ACCEPT");

	saveParameter(IDC_TB_HOAL_DIAMETER_A, "Classify", "HOAL_DIAMETER_A");
	saveParameter(IDC_TB_HOAL_DIAMETER_B, "Classify", "HOAL_DIAMETER_B");
	saveParameter(IDC_TB_HOAL_DIAMETER_C, "Classify", "HOAL_DIAMETER_C");
	saveParameter(IDC_TB_HOAL_DIAMETER_ACCEPT, "Classify", "HOAL_DIAMETER_ACCEPT");

	saveParameter(IDC_TB_HOAL_COUNT_A, "Classify", "HOAL_COUNT_A");
	saveParameter(IDC_TB_HOAL_COUNT_B, "Classify", "HOAL_COUNT_B");
	saveParameter(IDC_TB_HOAL_COUNT_C, "Classify", "HOAL_COUNT_C");
	saveParameter(IDC_TB_HOAL_COUNT_ACCEPT, "Classify", "HOAL_COUNT_ACCEPT");


	//划痕
	saveParameter(IDC_TB_SCRATCH_SINGLE_LENGTH_A, "Classify", "SCRATCH_SINGLE_LENGTH_A");
	saveParameter(IDC_TB_SCRATCH_SINGLE_LENGTH_B, "Classify", "SCRATCH_SINGLE_LENGTH_B");
	saveParameter(IDC_TB_SCRATCH_SINGLE_LENGTH_C, "Classify", "SCRATCH_SINGLE_LENGTH_C");
	saveParameter(IDC_TB_SCRATCH_SINGLE_LENGTH_ACCEPT, "Classify", "SCRATCH_SINGLE_LENGTH_ACCEPT");

	saveParameter(IDC_TB_SCRATCH_TOTAL_LENGTH_A, "Classify", "SCRATCH_TOTAL_LENGTH_A");
	saveParameter(IDC_TB_SCRATCH_TOTAL_LENGTH_B, "Classify", "SCRATCH_TOTAL_LENGTH_B");
	saveParameter(IDC_TB_SCRATCH_TOTAL_LENGTH_C, "Classify", "SCRATCH_TOTAL_LENGTH_C");
	saveParameter(IDC_TB_SCRATCH_TOTAL_LENGTH_ACCEPT, "Classify", "SCRATCH_TOTAL_LENGTH_ACCEPT");

	// TODO:  在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}
void CSpotsClassifySet::saveParameter(int IDC, string SectionName, string key)
{
	//保存设置
	CString cstr;
	GetDlgItem(IDC)->GetWindowTextW(cstr);
	SettingHelper::AddKey(SectionName, key, StringHelper::CString2string(cstr));
}