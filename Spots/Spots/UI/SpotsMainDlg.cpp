
// SpotsMainDlg.cpp : ʵ���ļ�
//

#include "../stdafx.h"
#include "../Spots.h"
#include "SpotsMainDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSpotsMainDlg �Ի���



CSpotsMainDlg::CSpotsMainDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSpotsMainDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSpotsMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSpotsMainDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CSpotsMainDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_RUN, &CSpotsMainDlg::OnBnClickedBtnRun)
	ON_BN_CLICKED(IDC_BTN_SelectVirtualImg, &CSpotsMainDlg::OnBnClickedBtnSelectvirtualimg)
	ON_BN_CLICKED(IDC_BTN_virtualTigger, &CSpotsMainDlg::OnBnClickedBtnvirtualtigger)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CSpotsMainDlg ��Ϣ�������

BOOL CSpotsMainDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	ModifyStyle(WS_THICKFRAME, DS_MODALFRAME);
	menu.LoadMenuW(IDR_MenuMain);
	SetMenu(&menu);

	p_contrller->init();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CSpotsMainDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
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

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CSpotsMainDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


HBRUSH CSpotsMainDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

void CSpotsMainDlg::OnBnClickedOk()
{
}
void CSpotsMainDlg::DrawPicToHDC(cv::Mat& img, UINT ID)
{
	IplImage image(img); //ԭʼͼ��
	//if (hDC == NULL)
	{
		p_DC = GetDlgItem(ID)->GetDC();
		hDC = p_DC->GetSafeHdc();
	}
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	CvvImage cimg;
	cimg.CopyOf(&image); // ����ͼƬ
	cimg.DrawToHDC(hDC, &rect); // ��ͼƬ���Ƶ���ʾ�ؼ���ָ��������
	ReleaseDC(p_DC);
}


void CSpotsMainDlg::ShowBigImg(cv::Mat img)
{
	DrawPicToHDC(img, IDC_IMG_BIG);
}
void CSpotsMainDlg::UpdateRecord()
{
	GetDlgItem(IDC_LB_todayA)->SetWindowText(StringHelper::Int2CString(Recorder::GetCount(Recorder::Quality::A, Recorder::TimeSpan::Today)));
	GetDlgItem(IDC_LB_todayB)->SetWindowText(StringHelper::Int2CString(Recorder::GetCount(Recorder::Quality::B, Recorder::TimeSpan::Today)));
	GetDlgItem(IDC_LB_todayC)->SetWindowText(StringHelper::Int2CString(Recorder::GetCount(Recorder::Quality::C, Recorder::TimeSpan::Today)));
	GetDlgItem(IDC_LB_todayGood)->SetWindowText(StringHelper::Int2CString(Recorder::GetCount(Recorder::Quality::Good, Recorder::TimeSpan::Today)));
	GetDlgItem(IDC_LB_todayTotal)->SetWindowText(StringHelper::Int2CString(Recorder::GetCount(Recorder::Quality::Total, Recorder::TimeSpan::Today)));
}
void CSpotsMainDlg::SwitchModel2Virtual(bool switchToV)
{
	if (switchToV)
	{
		(CButton*)GetDlgItem(IDC_BTN_SelectVirtualImg)->ShowWindow(true);
		(CButton*)GetDlgItem(IDC_BTN_virtualTigger)->ShowWindow(true);
	}
	else
	{
		(CButton*)GetDlgItem(IDC_BTN_SelectVirtualImg)->ShowWindow(false);
		(CButton*)GetDlgItem(IDC_BTN_virtualTigger)->ShowWindow(false);
	}
}
void CSpotsMainDlg::OnBnClickedBtnRun()
{
	isRunning = !isRunning;
	if (!isRunning)
		GetDlgItem(IDC_BTN_RUN)->SetWindowText(L"��ʼ");
	else
		GetDlgItem(IDC_BTN_RUN)->SetWindowText(L"��ͣ");
}


void CSpotsMainDlg::OnBnClickedBtnSelectvirtualimg()
{
	CString strFile = _T("");

	CFileDialog    dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("ͼƬ�ļ� (*.jpg;*.bmp;*.png)|*.jpg;*.bmp;*.png|�����ļ� (*.*)|*.*||"), NULL);
	dlgFile.m_ofn.lpstrTitle = L"�����������Ƭ";

	if (dlgFile.DoModal())
	{
		strFile = dlgFile.GetPathName();
	}
	cv::Mat img = cv::imread(StringHelper::CString2String(strFile));
	if (img.cols == 0)
		MessageBox(L"ͼƬ��ȡʧ�ܣ�", L"����");
	else
	{
		p_contrller->VirtualSelectImg(img);
	}
	return;
}


void CSpotsMainDlg::OnBnClickedBtnvirtualtigger()
{
	p_contrller->VirtualWorkerStart();
}
