
// TileDetectorMFCDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TileDetectorMFC.h"
#include "TileDetectorMFCDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CTileDetectorMFCDlg 对话框












CTileDetectorMFCDlg::CTileDetectorMFCDlg(CWnd* pParent /*=NULL*/) : CDialogEx(CTileDetectorMFCDlg::IDD, pParent)

, set_grabMaxPics(0)
, set_grabWidth(0)
, set_grabRGBType(_T(""))
, set_TiggerWaitTimeMS(0)
, m_Info(_T(""))
, m_VirtualCamera(_T(""))
{
	printf_globle("");
	if (FILE_LOG)
		ofstream of(FILE_NAME);
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
	DDX_Text(pDX, IDC_TB_VirtualCamera, m_VirtualCamera);
}
BEGIN_MESSAGE_MAP(CTileDetectorMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_MSG_GRABBING_END, &CTileDetectorMFCDlg::OnMsgGrabbingEnd)//采集结束处理程序
	ON_MESSAGE(WM_MSG_GRABBINGCalibartion_END, &CTileDetectorMFCDlg::OnMsgGrabbingCalibrationEnd)//定标、采集结束处理程序
	ON_MESSAGE(WM_MSG_PROCESSING_END, &CTileDetectorMFCDlg::OnMsgProcessingEnd)//处理结束处理程序
	ON_BN_CLICKED(IDC_BTN_SCAN, &CTileDetectorMFCDlg::BtnScan_OnBnClicked)
	ON_BN_CLICKED(IDC_CB_CanBeTiggered, &CTileDetectorMFCDlg::OnBnClickedCbCanbetiggered)
	ON_BN_CLICKED(IDC_BTN_GRAB_SAVE, &CTileDetectorMFCDlg::OnBnClickedBtnGrabSave)
	ON_BN_CLICKED(IDC_BTN_GRAB_LOAD, &CTileDetectorMFCDlg::OnBnClickedBtnGrabLoad)
	ON_BN_CLICKED(IDC_BTN_TIGGER_SAVE, &CTileDetectorMFCDlg::OnBnClickedBtnTiggerSave)
	ON_BN_CLICKED(IDC_BTN_TIGGER_LOAD, &CTileDetectorMFCDlg::OnBnClickedBtnTiggerLoad)
	ON_EN_CHANGE(IDC_TB_VirtualCamera, &CTileDetectorMFCDlg::OnEnChangeTbVirtualcamera)
	ON_EN_KILLFOCUS(IDC_TB_VirtualCamera, &CTileDetectorMFCDlg::OnEnKillfocusTbVirtualcamera)
	ON_BN_CLICKED(IDC_BTN_CALIBRATION, &CTileDetectorMFCDlg::OnBnClickedBtnCalibration)
END_MESSAGE_MAP()


// CTileDetectorMFCDlg 消息处理程序

BOOL CTileDetectorMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码




	//初始化设定数据
	if (true)//判断配置文件是否存在
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


	globle_var::InitSetting((set_grabRGBType == "RGB" ? "RGB" : "Gray"), set_grabMaxPics, set_grabWidth);
	globle_var::TiggerWaitTimeMS = set_TiggerWaitTimeMS;

	globle_var::VirtualCameraFileName = "result1_o原图.jpg";
	m_VirtualCamera = globle_var::VirtualCameraFileName.c_str();

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
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。
void CTileDetectorMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTileDetectorMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}





















void CTileDetectorMFCDlg::BtnScan_OnBnClicked()
{
	if (!twag->ManualTigger())
	{
		//MessageBox(L"当前无法触发！");
		printf_globle("当前无法触发!\n");
	}
	else
	{
		m_Info = _T("");
		UpdateData(false);
	}
}


void CTileDetectorMFCDlg::OnBnClickedBtnCalibration()
{
	m_Info = _T("");
	m_Info += _T("开始等待标准砖进入...\r\n");
	UpdateData(false);
	twag->StartWatchWithCalibration();
}
LRESULT CTileDetectorMFCDlg::OnMsgGrabbingEnd(WPARAM wParam, LPARAM lParam)
{

	consumer->GrabbingIndex = twag->GrabbingIndex;
	//运行消费者进程处理图像
	if (!consumer->StartNewProces(globle_var::s.BufferImg))
	{
		printf_globle("算法太慢，上一轮运算尚未结束！");
		return 0;
	}
	if (globle_var::s.BufferImg.cols > 0)
	{
		cv::Mat a;
		cv::resize(globle_var::s.BufferImg, a, cv::Size(400, 1100));
		DrawPicToHDC(a, IDC_PIC_Sample);
	}
	CString msg;
	msg.Format(_T("%d 采图完成！\r\n"), twag->GrabbingIndex);
	m_Info += msg;
	UpdateData(false);
	return 1;
}
LRESULT CTileDetectorMFCDlg::OnMsgGrabbingCalibrationEnd(WPARAM wParam, LPARAM lParam)
{
	//consumer->GrabbingIndex = twag->GrabbingIndex;
	//运行消费者进程处理图像
	consumer->Process4Calibraion();
	m_Info += _T("定标采图完成！\r\n");
	UpdateData(false);
	return 1;
}
LRESULT CTileDetectorMFCDlg::OnMsgProcessingEnd(WPARAM wParam, LPARAM lParam)
{
	CString msg;
	msg.Format(_T("%d 处理完成！\r\n"), consumer->GrabbingIndex);
	m_Info += msg;

	if (consumer->EdgeFaults.size() > 0)
	{
		CString str;
		str.Format(_T("%d 存在%d处崩边缺陷。\r"), consumer->GrabbingIndex, consumer->EdgeFaults.size());
		m_Info += str;
		//cv::Mat img(consumer->originalImg);
		for (size_t i = 0; i < consumer->EdgeFaults.size(); i++)
		{
			cv::circle(consumer->originalImg, cv::Point(consumer->EdgeFaults[i].x, consumer->EdgeFaults[i].y), consumer->EdgeFaults[i].z + 20, cv::Scalar(0, 0, 255), 10);
			cv::circle(consumer->originalImg, cv::Point(consumer->EdgeFaults[i].x, consumer->EdgeFaults[i].y), consumer->EdgeFaults[i].z + 3, cv::Scalar(0, 255, 0), 5);
		}
		DrawPicToHDC(consumer->originalImg, IDC_PIC_Sample);
	}

	UpdateData(false);
	return 1;
}
void CTileDetectorMFCDlg::DrawPicToHDC(cv::Mat& img, UINT ID)
{
	IplImage image(img); //原始图像
	CDC *pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	CvvImage cimg;
	cimg.CopyOf(&image); // 复制图片
	cimg.DrawToHDC(hDC, &rect); // 将图片绘制到显示控件的指定区域内
	ReleaseDC(pDC);
}



void CTileDetectorMFCDlg::OnBnClickedCbCanbetiggered()
{
	if (IsDlgButtonChecked(IDC_CB_CanBeTiggered) == BST_CHECKED)
		twag->StartWatch();
	else
		twag->StopWatch();
}








//保存采集参数
void CTileDetectorMFCDlg::OnBnClickedBtnGrabSave()
{
	UpdateData(true);
	//存入设置
	globle_var::InitSetting((set_grabRGBType == "RGB" ? "RGB" : "Gray"), set_grabMaxPics, set_grabWidth);

	//存入文件
	stringstream ss;
	ss << set_grabMaxPics;
	Setting::AddKey(Setting::GRAB_MaxPics, ss.str());
	ss.str("");
	ss << set_grabWidth;
	Setting::AddKey(Setting::GRAB_Width, ss.str());
	ss.str("");
	Setting::AddKey(Setting::GRAB_ColorType, (LPWSTR)(LPCWSTR)set_grabRGBType.GetBuffer(0));

	twag->StopWatch();
	if (IsDlgButtonChecked(IDC_CB_CanBeTiggered) == BST_CHECKED)
	{
		twag->StartWatch();
	}
	else
	{
		twag->StopWatch();
	}
}
//读取采集参数
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
		MessageBox(L"配置文件数据错误，将使用默认参数!");
		globle_var::InitSetting();
	}
	else
	{
		globle_var::InitSetting((set_grabRGBType == "RGB" ? "RGB" : "Gray"), set_grabMaxPics, set_grabWidth);
	}
	UpdateData(false);


	twag->StopWatch();
	twag->Init();
	if (IsDlgButtonChecked(IDC_CB_CanBeTiggered) == BST_CHECKED)
		twag->StartWatch();
}

//保存触发参数
void CTileDetectorMFCDlg::OnBnClickedBtnTiggerSave()
{
	UpdateData(true);
	globle_var::TiggerWaitTimeMS = set_TiggerWaitTimeMS;

	stringstream ss;
	ss << set_TiggerWaitTimeMS;
	Setting::AddKey(Setting::TIGGER_WaitTime, ss.str());

}
//读取触发参数
void CTileDetectorMFCDlg::OnBnClickedBtnTiggerLoad()
{
	if (Setting::GetKeyInt(Setting::TIGGER_WaitTime, set_TiggerWaitTimeMS))
	{
		globle_var::TiggerWaitTimeMS = set_TiggerWaitTimeMS;
	}
	UpdateData(false);


}


void CTileDetectorMFCDlg::OnEnChangeTbVirtualcamera()
{

}


void CTileDetectorMFCDlg::OnEnKillfocusTbVirtualcamera()
{
	UpdateData(true);
	m_VirtualCamera.Trim();
	CStringA stra(m_VirtualCamera.GetBuffer(0));
	std::string strtmp = stra.GetBuffer(0);
	stra.ReleaseBuffer();

	if (globle_var::VirtualCameraFileName != "" && m_VirtualCamera == "")
	{
		globle_var::VirtualCameraFileName = strtmp;
		twag->StopWatch();
		twag->Switch2Real();
		if (IsDlgButtonChecked(IDC_CB_CanBeTiggered) == BST_CHECKED)
			twag->StartWatch();
		printf_globle("开始使用真实相机.\r\n\r\n");
	}
	else if ((globle_var::VirtualCameraFileName == "" && m_VirtualCamera != "") || (globle_var::VirtualCameraFileName != strtmp))
	{
		globle_var::VirtualCameraFileName = strtmp;
		twag->StopWatch();
		twag->Switch2Virtual(strtmp, true);
		if (IsDlgButtonChecked(IDC_CB_CanBeTiggered) == BST_CHECKED)
			twag->StartWatch();
		printf_globle("开始使用虚拟相机.\r\n\r\n");

		if (twag->vc->buffer.cols == 0)
			MessageBox(L"虚拟相机文件不存在！");
	}
}


