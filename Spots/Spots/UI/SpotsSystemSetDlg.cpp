// E:\��Ŀ����\cameraTest\Spots\Spots\UI\SpotsSystemSetDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Spots.h"
#include "SpotsSystemSetDlg.h"
#include "afxdialogex.h"
#include <Class/Setting/SettingHelper.h>


// SpotsSystemSetDlg �Ի���

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
	bool accConnFlag = false;
	if (SettingHelper::GetKeyString("DATABASE", "ACCDB_PATH", db_path))//��ȡ���ݿ�����·��
	{
		GetDlgItem(IDC_TB_AccessPath)->SetWindowText(StringHelper::string2LPWSTR(db_path));
	}
	else
	{
		GetDlgItem(IDC_TB_AccessPath)->SetWindowText(L"src//..//��שȱ�ݼ�����ݿ�.mdb");
	}
	int accEnable = 0;
	if (SettingHelper::GetKeyInt("DATABASE", "ACCDB_ENABLE", accEnable))//��ȡ���ݿ�����·��
		((CButton*)GetDlgItem(IDC_CB_EnableAccess))->SetCheck(accEnable != 0);
	else
		((CButton*)GetDlgItem(IDC_CB_EnableAccess))->SetCheck(1);

	if (SettingHelper::GetKeyInt("SYS", "SAVE_IMG", accEnable))//��ȡ���ݿ�����·��
		((CButton*)GetDlgItem(IDC_CB_SAVE_IMG))->SetCheck(accEnable != 0);
	else
		((CButton*)GetDlgItem(IDC_CB_SAVE_IMG))->SetCheck(0);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void SpotsSystemSetDlg::OnPaint()
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
HBRUSH SpotsSystemSetDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void SpotsSystemSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(SpotsSystemSetDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDOK, &SpotsSystemSetDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// SpotsSystemSetDlg ��Ϣ�������


void SpotsSystemSetDlg::OnBnClickedOk()
{
	//�������ݿ�����
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CB_EnableAccess);
	SettingHelper::AddKey("DATABASE", "ACCDB_ENABLE", pBtn->GetCheck());
	CString cstr;
	GetDlgItem(IDC_TB_AccessPath)->GetWindowTextW(cstr);
	SettingHelper::AddKey("DATABASE", "ACCDB_PATH", StringHelper::CString2string(cstr));
	pBtn = (CButton*)GetDlgItem(IDC_CB_SAVE_IMG);
	SettingHelper::AddKey("SYS", "SAVE_IMG", pBtn->GetCheck());
	CDialogEx::OnOK();
}
