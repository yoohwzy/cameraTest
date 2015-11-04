
// TileDetectorMFCDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TileDetectorMFC.h"
#include "TileDetectorMFCDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTileDetectorMFCDlg �Ի���












CTileDetectorMFCDlg::CTileDetectorMFCDlg(CWnd* pParent /*=NULL*/) : CDialogEx(CTileDetectorMFCDlg::IDD, pParent)

, set_grabMaxPics(0)
, set_grabWidth(0)
, set_grabRGBType(_T(""))
, set_TiggerWaitTimeMS(0)
, m_Info(_T(""))
{
	printf_globle("");
	if (FILE_LOG)
		ofstream of(FILE_NAME);

	//AllocConsole();
	//string title = "debug info";
	//size_t size = title.length();
	//wchar_t *buffer = new wchar_t[size + 1];
	//MultiByteToWideChar(CP_ACP, 0, title.c_str(), size, buffer, size * sizeof(wchar_t));
	//buffer[size] = 0;  // ȷ���� '\0' ��β 
	//SetConsoleTitle(buffer);
	//delete buffer;
}




void CTileDetectorMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TB_GrabMaxPics, set_grabMaxPics);
	DDV_MinMaxInt(pDX, set_grabMaxPics, 100, 40000);
	DDX_Text(pDX, IDC_TB_GrabWidth, set_grabWidth);
	DDV_MinMaxInt(pDX, set_grabWidth, 0, 4096);
	DDX_CBString(pDX, IDC_COMBO1, set_grabRGBType);
	DDX_Text(pDX, IDC_TB_TiggerWaitMS, set_TiggerWaitTimeMS);
	DDX_Text(pDX, IDC_TB_INFO, m_Info);
}
BEGIN_MESSAGE_MAP(CTileDetectorMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_MSG_GRABBING_END, &CTileDetectorMFCDlg::OnMsgGrabbingEnd)//�ɼ�������������
	ON_MESSAGE(WM_MSG_PROCESSING_END, &CTileDetectorMFCDlg::OnMsgProcessingEnd)//����������������
	ON_BN_CLICKED(IDC_BTN_SCAN, &CTileDetectorMFCDlg::BtnScan_OnBnClicked)
	ON_BN_CLICKED(IDC_CB_CanBeTiggered, &CTileDetectorMFCDlg::OnBnClickedCbCanbetiggered)
	ON_BN_CLICKED(IDC_BTN_GRAB_SAVE, &CTileDetectorMFCDlg::OnBnClickedBtnGrabSave)
	ON_BN_CLICKED(IDC_BTN_GRAB_LOAD, &CTileDetectorMFCDlg::OnBnClickedBtnGrabLoad)
	ON_BN_CLICKED(IDC_BTN_TIGGER_SAVE, &CTileDetectorMFCDlg::OnBnClickedBtnTiggerSave)
	ON_BN_CLICKED(IDC_BTN_TIGGER_LOAD, &CTileDetectorMFCDlg::OnBnClickedBtnTiggerLoad)
END_MESSAGE_MAP()


// CTileDetectorMFCDlg ��Ϣ��������

BOOL CTileDetectorMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵������ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ����Ӷ���ĳ�ʼ������






	globle_var::VirtualCameraFileName = "result11_oԭͼ.jpg";
	//��ʼ���趨����
	if (true)//�ж������ļ��Ƿ����
	{
		string s;
		if (Setting::GetKeyString(Setting::GRAB_ColorType, s))
		{
			CString cs(s.c_str());
			set_grabRGBType = cs;
		}
		else
		{
			set_grabRGBType = globle_var::mdi.colorType == globle_var::mdi.RGB ? "RGB" : "Gray";
		}
		if (!Setting::GetKeyInt(Setting::GRAB_MaxPics, set_grabMaxPics) || set_grabMaxPics < 1000)
		{
			set_grabMaxPics = globle_var::mdi.MaxPics;
		}
		if (!Setting::GetKeyInt(Setting::GRAB_Width, set_grabWidth) || set_grabWidth < 1000)
		{
			set_grabWidth = globle_var::mdi.width;
		}
		if (!Setting::GetKeyInt(Setting::TIGGER_WaitTime, set_TiggerWaitTimeMS))
		{
			set_TiggerWaitTimeMS = globle_var::TiggerWaitTimeMS;
		}
	}

	globle_var::mdi.colorType = set_grabRGBType == "RGB" ? globle_var::mdi.RGB : globle_var::mdi.GRAY;
	globle_var::mdi.MaxPics = set_grabMaxPics;
	globle_var::mdi.width = set_grabWidth;

	globle_var::TiggerWaitTimeMS = set_TiggerWaitTimeMS;


	twag = new TiggerWatcherAndGrabber(this->GetSafeHwnd(), globle_var::VirtualCameraFileName);
	consumer = new Consumer(this->GetSafeHwnd());

	if (IsDlgButtonChecked(IDC_CB_CanBeTiggered) == BST_CHECKED)
	{
		twag->StartWatch();
	}
	else
	{
		twag->StopWatch();
	}

	UpdateData(false);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CTileDetectorMFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի���������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�
void CTileDetectorMFCDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CTileDetectorMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}





















void CTileDetectorMFCDlg::BtnScan_OnBnClicked()
{
	if (!twag->ManualTigger())
	{
		//MessageBox(L"��ǰ�޷�������");
		printf_globle("��ǰ�޷�������\n");
	}
	else
	{
		m_Info = _T("");
		UpdateData(false);
	}
}
LRESULT CTileDetectorMFCDlg::OnMsgGrabbingEnd(WPARAM wParam, LPARAM lParam)
{

	consumer->GrabbingIndex = twag->GrabbingIndex;
	if (!consumer->StartNewProces(globle_var::s.NowBufferImg))
	{
		printf_globle("�㷨̫������һ��������δ������");
		return 0;
	}
	cv::Mat a;
	cv::resize(globle_var::s.NowBufferImg, a, cv::Size(400, 1100));
	DrawPicToHDC(a, IDC_PIC_Sample);


	m_Info += _T("��ͼ��ɣ�\r\n");
	UpdateData(false);
	return 1;
}
LRESULT CTileDetectorMFCDlg::OnMsgProcessingEnd(WPARAM wParam, LPARAM lParam)
{
	m_Info += _T("������ɣ�\r\n");
	UpdateData(false);
	return 1;
}

void CTileDetectorMFCDlg::DrawPicToHDC(cv::Mat& img, UINT ID)
{
	IplImage image(img); //ԭʼͼ��
	CDC *pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	CvvImage cimg;
	cimg.CopyOf(&image); // ����ͼƬ
	cimg.DrawToHDC(hDC, &rect); // ��ͼƬ���Ƶ���ʾ�ؼ���ָ��������
	ReleaseDC(pDC);
}



void CTileDetectorMFCDlg::OnBnClickedCbCanbetiggered()
{
	if (IsDlgButtonChecked(IDC_CB_CanBeTiggered) == BST_CHECKED)
	{
		twag->StartWatch();
	}
	else
	{
		twag->StopWatch();
	}
}








//����ɼ�����
void CTileDetectorMFCDlg::OnBnClickedBtnGrabSave()
{
	UpdateData(true);
	//��������
	globle_var::InitSetting((set_grabRGBType == "RGB" ? "RGB" : "Gray"), set_grabMaxPics, set_grabWidth);

	//�����ļ�
	stringstream ss;
	ss << set_grabMaxPics;
	Setting::AddKey(Setting::GRAB_MaxPics, ss.str());
	ss.str("");
	ss << set_grabWidth;
	Setting::AddKey(Setting::GRAB_Width, ss.str());
	ss.str("");
	Setting::AddKey(Setting::GRAB_ColorType, (LPWSTR)(LPCWSTR)set_grabRGBType.GetBuffer(0));

	twag->StopWatch();
	delete twag;
	twag = new TiggerWatcherAndGrabber(this->GetSafeHwnd(), globle_var::VirtualCameraFileName);
}
//��ȡ�ɼ�����
void CTileDetectorMFCDlg::OnBnClickedBtnGrabLoad()
{
	string s = "";
	Setting::GetKeyString(Setting::GRAB_ColorType, s);
	CString cs(s.c_str());
	set_grabRGBType = cs;

	Setting::GetKeyInt(Setting::GRAB_MaxPics, set_grabMaxPics);
	Setting::GetKeyInt(Setting::GRAB_Width, set_grabWidth);

	if ((set_grabRGBType != "RGB" && set_grabRGBType != "Gray") || set_grabMaxPics < 100 || set_grabWidth < 100)
	{
		MessageBox(L"�����ļ����ݴ��󣬽�ʹ��Ĭ�ϲ���!");
		globle_var::InitSetting();
	}
	else
	{
		globle_var::InitSetting((set_grabRGBType == "RGB" ? "RGB" : "Gray"), set_grabMaxPics, set_grabWidth);
	}
	UpdateData(false);


	twag->StopWatch();
	delete twag;
	twag = new TiggerWatcherAndGrabber(this->GetSafeHwnd(), globle_var::VirtualCameraFileName);
}

//���津������
void CTileDetectorMFCDlg::OnBnClickedBtnTiggerSave()
{
	UpdateData(true);
	globle_var::TiggerWaitTimeMS = set_TiggerWaitTimeMS;

	stringstream ss;
	ss << set_TiggerWaitTimeMS;
	Setting::AddKey(Setting::TIGGER_WaitTime, ss.str());

}
//��ȡ��������
void CTileDetectorMFCDlg::OnBnClickedBtnTiggerLoad()
{
	if (Setting::GetKeyInt(Setting::TIGGER_WaitTime, set_TiggerWaitTimeMS))
	{
		globle_var::TiggerWaitTimeMS = set_TiggerWaitTimeMS;
	}
	UpdateData(false);


}