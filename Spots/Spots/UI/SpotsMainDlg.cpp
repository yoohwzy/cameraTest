
// SpotsMainDlg.cpp : 实现文件
//

#include "../stdafx.h"
#include "../Spots.h"
#include "SpotsMainDlg.h"
#include "afxdialogex.h"

#include <Class\Statistics\Statistics.h>
#include <Class/Setting/SettingHelper.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "SpotsSystemSetDlg.h"
#include "SpotsEdgeParameterSetDlg.h"
#include "SpotsClassifySet.h"



// CSpotsMainDlg 对话框

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
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_MESSAGE(WM_MENU_OPEN_SYS_SET_DLG, OnWM_MENU_OPEN_SYS_SET_DLG)
	ON_COMMAND(ID_32773, &CSpotsMainDlg::On32773)
	ON_COMMAND(ID_BTN_Menu_EdgeSet, &CSpotsMainDlg::OnBtnMenuEdgeset)
	ON_COMMAND(ID_32778, &CSpotsMainDlg::OnMenuClassiySetClick)
END_MESSAGE_MAP()


// CSpotsMainDlg 消息处理程序

BOOL CSpotsMainDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	// 打开控制台
	if (__argc > 1)
	{ 
		MFCConsole::Init();
		MFCConsole::Output("Debug start.\r\n");
	}

	// 添加菜单栏
	ModifyStyle(WS_THICKFRAME, DS_MODALFRAME);
	menu.LoadMenuW(IDR_MenuMain);
	SetMenu(&menu);

	//系统初始化
	p_contrller->init();


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSpotsMainDlg::OnPaint()
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
		CRect rect;
		CPaintDC dc(this);
		GetClientRect(rect);
		dc.FillSolidRect(rect, RGB(255, 255, 255));
		dc.FillPath();

		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSpotsMainDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


HBRUSH CSpotsMainDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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


//#include <shlwapi.h>
//#pragma comment(lib,"Shlwapi.lib") //如果没有这行，会出现link错误
void CSpotsMainDlg::OnBnClickedOk()
{
	//if (!PathIsDirectory(L"Images"))
	//{
	//	CreateDirectory(L"Images", NULL);
	//}
	//if (!PathIsDirectory(L"Images\\2016_03_12"))
	//{
	//	CreateDirectory(L"Images\\2016_03_12", NULL);
	//}
}
void CSpotsMainDlg::DrawPicToHDC(cv::Mat& img, UINT ID)
{
	IplImage image(img); //原始图像
	//if (hDC == NULL)
	{
		p_DC = GetDlgItem(ID)->GetDC();
		hDC = p_DC->GetSafeHdc();
	}
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	CvvImage cimg;
	cimg.CopyOf(&image); // 复制图片
	cimg.DrawToHDC(hDC, &rect); // 将图片绘制到显示控件的指定区域内
	ReleaseDC(p_DC);
}


void CSpotsMainDlg::ShowBigImg(cv::Mat img)
{
	img_on_show = img;
	DrawPicToHDC(img, IDC_IMG_BIG);
}
void CSpotsMainDlg::ShowLogImg(cv::Mat img)
{
	DrawPicToHDC(img, IDC_IMG_HISTORY);
}

void CSpotsMainDlg::UpdateStatistics()
{
	GetDlgItem(IDC_LB_todayTotal)->SetWindowText(StringHelper::Int2CString(Statistics::TodayAll));
	GetDlgItem(IDC_LB_todayA)->SetWindowText(StringHelper::Int2CString(Statistics::TodayA));
	GetDlgItem(IDC_LB_todayB)->SetWindowText(StringHelper::Int2CString(Statistics::TodayB));
	GetDlgItem(IDC_LB_todayC)->SetWindowText(StringHelper::Int2CString(Statistics::TodayC));
	GetDlgItem(IDC_LB_todayGood)->SetWindowText(StringHelper::Int2CString(Statistics::TodayAll - Statistics::TodayRejected));

	GetDlgItem(IDC_LB_monthTotal)->SetWindowText(StringHelper::Int2CString(Statistics::MonthAll));
	GetDlgItem(IDC_LB_monthA)->SetWindowText(StringHelper::Int2CString(Statistics::MonthA));
	GetDlgItem(IDC_LB_monthB)->SetWindowText(StringHelper::Int2CString(Statistics::MonthB));
	GetDlgItem(IDC_LB_monthC)->SetWindowText(StringHelper::Int2CString(Statistics::MonthC));
	GetDlgItem(IDC_LB_monthGood)->SetWindowText(StringHelper::Int2CString(Statistics::MonthAll - Statistics::MonthRejected));

	GetDlgItem(IDC_LB_yearTotal)->SetWindowText(StringHelper::Int2CString(Statistics::YearAll));
	GetDlgItem(IDC_LB_yearA)->SetWindowText(StringHelper::Int2CString(Statistics::YearA));
	GetDlgItem(IDC_LB_yearB)->SetWindowText(StringHelper::Int2CString(Statistics::YearB));
	GetDlgItem(IDC_LB_yearC)->SetWindowText(StringHelper::Int2CString(Statistics::YearC));
	GetDlgItem(IDC_LB_yearGood)->SetWindowText(StringHelper::Int2CString(Statistics::YearAll - Statistics::YearRejected));

	if (Statistics::TodayAll > 0)
	{
		float dayFineRate = (Statistics::TodayAll - Statistics::TodayRejected) / (float)Statistics::TodayAll * 100;
		CString strfineRate;
		strfineRate.Format(L"%.2f", dayFineRate);
		GetDlgItem(IDC_LB_dayFineRate)->SetWindowText(strfineRate + L"%");
	}
	else
		GetDlgItem(IDC_LB_dayFineRate)->SetWindowText(L"NaN");

	if (Statistics::MonthAll > 0)
	{
		float monthFineRate = (Statistics::MonthAll - Statistics::MonthRejected) / (float)Statistics::MonthAll * 100;
		CString strfineRate;
		strfineRate.Format(L"%.2f", monthFineRate);
		GetDlgItem(IDC_LB_monthFineRate)->SetWindowText(strfineRate + L"%");
	}
	else
		GetDlgItem(IDC_LB_monthFineRate)->SetWindowText(L"NaN");

	if (Statistics::YearAll > 0)
	{
		float yearFineRate = (Statistics::YearAll - Statistics::YearRejected) / (float)Statistics::YearAll * 100;
		CString strfineRate;
		strfineRate.Format(L"%.2f", yearFineRate);
		GetDlgItem(IDC_LB_yearFineRate)->SetWindowText(strfineRate + L"%");
	}
	else
		GetDlgItem(IDC_LB_yearFineRate)->SetWindowText(L"NaN");
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
	{
		p_contrller->StopWatch();
		GetDlgItem(IDC_BTN_RUN)->SetWindowText(L"开始");
	}
	else
	{
		p_contrller->StartWatch();
		GetDlgItem(IDC_BTN_RUN)->SetWindowText(L"暂停");
	}
}


void CSpotsMainDlg::OnBnClickedBtnSelectvirtualimg()
{
	CString strFile = _T("");

	CFileDialog    dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("图片文件 (*.jpg;*.bmp;*.png)|*.jpg;*.bmp;*.png|所有文件 (*.*)|*.*||"), NULL);
	dlgFile.m_ofn.lpstrTitle = L"打开线阵相机底片";

	if (dlgFile.DoModal())
	{
		strFile = dlgFile.GetPathName();
	}
	cv::Mat img = cv::imread(StringHelper::CString2string(strFile));
	if (img.cols == 0)
		MessageBox(L"图片读取失败！", L"错误");
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




void CSpotsMainDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	ShowImgROI(point);
	CDialogEx::OnMouseMove(nFlags, point);
}
void CSpotsMainDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	img_big_flag = !img_big_flag;//标记放大/不放大图像
	//if (img_big_flag)
	//	zoom = 1;
	zoom = 1;
	ShowImgROI(point);
	CDialogEx::OnLButtonDown(nFlags, point);
}
//鼠标滚轮
BOOL CSpotsMainDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint point)
{
	//zDelta：大于0时为向上滚动，小于0时为向下滚动。
	if (img_on_show.rows > 0)
	{
		if (mouse_in_img)   //鼠标是否在控件范围之内
		{
			//if (!img_big_flag)
			//	img_big_flag = true;
			if (zDelta > 0)
			{
				zoom = zoom % 2 == 0 ? zoom - 2 : zoom - 1;
				zoom = zoom < 1 ? 1 : zoom;
			}
			else
			{
				zoom = zoom % 2 == 0 ? zoom + 2 : zoom + 1;
				zoom = zoom > 4 ? 4 : zoom;
			}
		}
		ShowImgROI(CPoint(0, 0));
	}
	return CDialogEx::OnMouseWheel(nFlags, zDelta, point);
}
void CSpotsMainDlg::ShowImgROI(CPoint point = CPoint(0, 0))
{
	if (img_on_show.rows > 0)
	{
		if (point != CPoint(0, 0))
		{
			ClientToScreen(&point);
			mouse_point = point;
		}
		CRect rect;
		GetDlgItem(IDC_IMG_BIG)->GetClientRect(rect);
		GetDlgItem(IDC_IMG_BIG)->ClientToScreen(rect);//函数是将你打开的APP中客户区的坐标点信息转换为整个屏幕的坐标
		bool inRect = rect.PtInRect(point);
		bool inRectFlag = (point == CPoint(0, 0) && mouse_in_img);
		if (inRect || inRectFlag)   //鼠标是否在控件范围之内
		{
			if (point == CPoint(0, 0))
				point = mouse_point;
			mouse_in_img = true;//标记鼠标移入图像
			if (img_big_flag)
			{
				int idc_width = rect.right - rect.left;//图像控件宽
				int idc_height = rect.bottom - rect.top;//图像控件高

				//将鼠标所在位置换算为原始图像坐标点
				int x = (float)(point.x - rect.left) / idc_width * img_on_show.cols;
				int y = (float)(point.y - rect.top) / idc_height * img_on_show.rows;

				//根据放大系数转换ROI长宽
				int zoom_width = zoom * idc_width;
				int zoom_height = zoom * idc_height;
				if (zoom_width >= img_on_show.cols || zoom_height >= img_on_show.rows)//若放大系数过小，则显示全图
				{
					img_big_flag = false;//标记不放大图像
					DrawPicToHDC(img_on_show, IDC_IMG_BIG);
					GetDlgItem(IDC_LABLE_IMG_INFO)->SetWindowText(L"全显示");
				}
				else//放大显示
				{
					int startx = x - zoom_width / 2;
					int starty = y - zoom_height / 2;

					//防止越界
					if (startx < 0)startx = 0;
					if (starty < 0)starty = 0;
					if (startx + zoom_width > img_on_show.cols)
						startx = img_on_show.cols - zoom_width;
					if (starty + zoom_height > img_on_show.rows)
						starty = img_on_show.rows - zoom_height;
					//ROI
					cv::Mat roi = img_on_show(cv::Rect(startx, starty, zoom_width, zoom_height));
					DrawPicToHDC(roi, IDC_IMG_BIG);

					stringstream ss;
					ss << "(" << x << "," << y << ") " << zoom << "×";

					int dwLen = ss.str().length() + 1;//strlen(ss.str().c_str()) + 1;
					int nwLen = MultiByteToWideChar(CP_ACP, 0, ss.str().c_str(), dwLen, NULL, 0);//算出合适的长度
					LPWSTR lpszPath = new WCHAR[dwLen];
					MultiByteToWideChar(CP_ACP, 0, ss.str().c_str(), dwLen, lpszPath, nwLen);

					GetDlgItem(IDC_LABLE_IMG_INFO)->SetWindowText(lpszPath);
				}
			}
			else// if(!inRect && !inRectFlag)
			{
				mouse_in_img = false;//标记鼠标移出图像
				DrawPicToHDC(img_on_show, IDC_IMG_BIG);
				GetDlgItem(IDC_LABLE_IMG_INFO)->SetWindowText(L"全显示");
			}
		}
		else
		{
			DrawPicToHDC(img_on_show, IDC_IMG_BIG);
			GetDlgItem(IDC_LABLE_IMG_INFO)->SetWindowText(L"全显示");
			img_big_flag = false;//标记不放大图像
		}
	}
}



LRESULT CSpotsMainDlg::OnWM_MENU_OPEN_SYS_SET_DLG(WPARAM wParam, LPARAM lParam)
{
	SpotsSystemSetDlg s;
	s.DoModal();
	return 0;
}

void CSpotsMainDlg::On32773()
{
	SpotsSystemSetDlg m;
	if (m.DoModal() == IDOK)
	{
		int si = 0;
		SettingHelper::GetKeyInt("SYS", "SAVE_IMG", si);
		p_contrller->SAVE_IMG = si;
		p_contrller->ResetParameter();
	}
}

void CSpotsMainDlg::OnBtnMenuEdgeset()
{
	SpotsEdgeParameterSetDlg m;
	if (m.DoModal() == IDOK)
	{
		// 控制器
		SettingHelper::GetKeyDouble("EDGE_PARAMETER", "BlockLocalizer_THRESHOD", p_contrller->BlockLocalizer_THRESHOD);
		SettingHelper::GetKeyDouble("EDGE_PARAMETER", "BlockLocalizer_ContinuePointCount", p_contrller->BlockLocalizer_ContinuePointCount);
		SettingHelper::GetKeyDouble("EDGE_PARAMETER", "BlockEdgeDetector_DIFF_THRESHOLD", p_contrller->BlockEdgeDetector_DIFF_THRESHOLD);
		SettingHelper::GetKeyDouble("EDGE_PARAMETER", "BlockEdgeDetector_FAULTS_SPAN", p_contrller->BlockEdgeDetector_FAULTS_SPAN);
		SettingHelper::GetKeyDouble("EDGE_PARAMETER", "BlockEdgeDetector_FAULTS_COUNT", p_contrller->BlockEdgeDetector_FAULTS_COUNT);
		p_contrller->ResetParameter();
	}
}
void CSpotsMainDlg::OnMenuClassiySetClick()
{
	CSpotsClassifySet m;
	if (m.DoModal() == IDOK)
	{

	}
}
