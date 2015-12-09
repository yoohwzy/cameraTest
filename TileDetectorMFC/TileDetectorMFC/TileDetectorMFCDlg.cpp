
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
	DDX_Text(pDX, IDC_TB_INFO, m_Info);
	DDX_Text(pDX, IDC_TB_VirtualCamera, m_VirtualCamera);
}
BEGIN_MESSAGE_MAP(CTileDetectorMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_MSG_GRABBING_END, &CTileDetectorMFCDlg::OnMsgGrabbingEnd)//采集结束处理程序
	ON_MESSAGE(WM_MSG_PROCESSING_END, &CTileDetectorMFCDlg::OnMsgProcessingEnd)//处理结束处理程序
	ON_BN_CLICKED(IDC_BTN_SCAN, &CTileDetectorMFCDlg::BtnScan_OnBnClicked)
	ON_BN_CLICKED(IDC_CB_CanBeTiggered, &CTileDetectorMFCDlg::OnBnClickedCbCanbetiggered)
	ON_EN_CHANGE(IDC_TB_VirtualCamera, &CTileDetectorMFCDlg::OnEnChangeTbVirtualcamera)
	ON_EN_KILLFOCUS(IDC_TB_VirtualCamera, &CTileDetectorMFCDlg::OnEnKillfocusTbVirtualcamera)
	ON_BN_CLICKED(IDC_BTN_CALIBRATION, &CTileDetectorMFCDlg::OnBnClickedBtnCalibration)
	ON_MESSAGE(WM_MSG_GRABBINGCalibartion_END, &CTileDetectorMFCDlg::OnMsgGrabbingCalibrationEnd)//定标、采集结束处理程序
	ON_BN_CLICKED(IDC_BTN_Setting, &CTileDetectorMFCDlg::OnBnClickedBtnSetting)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
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



	//读取参数
	globle_var::InitSetting(true);
	globle_var::VirtualCameraFileName = "1杂质凹点A.jpg";

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
	GetDlgItem(IDC_LABLE_IMG_INFO)->SetWindowText(L"");

	UpdateData(false);

	CButton* radio = (CButton*)GetDlgItem(IDC_CB_SAVE_IMG);
	radio->SetCheck(1);

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
	//运行消费者进程处理图像
	if (consumer != NULL && consumer->IsProcessing == false)
	{
		delete consumer;
		consumer = NULL;
	}
	else if (IsConsumerProcessing)
	{
		printf_globle("算法太慢，上一轮运算尚未结束！");
		return 0;
	}
	if (consumer != NULL)
		delete consumer;
	consumer = new Consumer(this->GetSafeHwnd());
	consumer->GrabbingIndex = twag->GrabbingIndex;
	IsConsumerProcessing = true;
	consumer->StartNewProces(twag->Image);



	if (twag->Image.cols > 0)
	{
		DrawPicToHDC(twag->Image, IDC_PIC_Sample);
	}


	CString msg;
	msg.Format(_T("%d 采图完成！\r\n"), twag->GrabbingIndex);
	m_Info += msg;
	UpdateData(false);
	 


	//保存底片
	if (IsDlgButtonChecked(IDC_CB_SAVE_IMG) == BST_CHECKED)
	{
		CString msg;
		msg.Format(_T("正在保存底片%d！\r\n"), twag->GrabbingIndex);
		m_Info += msg;
		stringstream ss;
		UpdateData(false);
		ss << "samples/" << twag->GrabbingIndex << "_o原图.jpg";
		cv::imwrite(ss.str(), twag->OriginalImage);

		m_Info += _T("保存完成\r\n");
		UpdateData(false);
	}
	twag->OriginalImage.release();
	return 1;
}
LRESULT CTileDetectorMFCDlg::OnMsgGrabbingCalibrationEnd(WPARAM wParam, LPARAM lParam)
{

	////运行消费者进程处理图像

	//if (consumer->IsProcessing)
	//{
	//	printf_globle("算法太慢，上一轮运算尚未结束！");
	//	return 0;
	//}

	//delete consumer;
	//consumer = new Consumer(this->GetSafeHwnd());
	//consumer->GrabbingIndex = twag->GrabbingIndex;

	//consumer->StartNewProces4Calibraion(twag->Image);
	//m_Info += _T("定标采图完成！\r\n");
	//UpdateData(false);
	return 1;
}
//wParam = 0表示检测完成，1表示有错误
//lParam 表示错误类型
LRESULT CTileDetectorMFCDlg::OnMsgProcessingEnd(WPARAM wParam, LPARAM subtype)
{
	CString msg;
	msg.Format(_T("%d 处理完成！\r\n"), consumer->GrabbingIndex);
	m_Info += msg;

	if (wParam == 0)
	{
		img_on_show.release();
		img_on_show = consumer->originalImg.clone();
		if (consumer->faults.BrokenEdges.size() > 0)
		{
			CString str;
			str.Format(_T("%d 存在%d处崩边缺陷，红色标出。\r\n"), consumer->GrabbingIndex, consumer->faults.BrokenEdges.size());
			m_Info += str;
			for (size_t i = 0; i < consumer->faults.BrokenEdges.size(); i++)
			{
				cv::circle(img_on_show, consumer->faults.BrokenEdges[i].position, consumer->faults.BrokenEdges[i].length, cv::Scalar(0, 0, 255), 10);
			}
		}
		if (consumer->faults.SomethingBigs.size() > 0)
		{
			CString str;
			str.Format(_T("%d 存在%d处EID缺陷，蓝色标出。\r\n"), consumer->GrabbingIndex, consumer->faults.SomethingBigs.size());
			m_Info += str;
			for (size_t i = 0; i < consumer->faults.SomethingBigs.size(); i++)
			{
				cv::circle(img_on_show, consumer->faults.SomethingBigs[i].position, consumer->faults.SomethingBigs[i].diameter, cv::Scalar(255, 0, 0), 5);
			}
		}
		if (consumer->faults.Scratchs.size() > 0)
		{
			CString str;
			str.Format(_T("%d 存在%d处划痕缺陷，绿色标出。\r\n"), consumer->GrabbingIndex, consumer->faults.Scratchs.size());
			m_Info += str;
			for (size_t i = 0; i < consumer->faults.Scratchs.size(); i++)
			{
				cv::circle(img_on_show, consumer->faults.Scratchs[i].position, consumer->faults.Scratchs[i].length, cv::Scalar(0, 255, 0), 5);
			}
		}
		if (consumer->faults.Holes.size() > 0)
		{
			CString str;
			str.Format(_T("%d 存在%d处凹点缺陷，黄色标出。\r\n"), consumer->GrabbingIndex, consumer->faults.Holes.size());
			m_Info += str;
			for (size_t i = 0; i < consumer->faults.Holes.size(); i++)
			{
				cv::circle(img_on_show, consumer->faults.Holes[i].position, consumer->faults.Holes[i].diameter, cv::Scalar(0, 255, 255), 5);
			}
		}
		if (consumer->faults.MarkPens.size() > 0)
		{
			CString str;
			str.Format(_T("%d 存在%d处人工标记，橙色标出。\r\n"), consumer->GrabbingIndex, consumer->faults.MarkPens.size());
			m_Info += str;
			for (size_t i = 0; i < consumer->faults.MarkPens.size(); i++)
			{
				cv::rectangle(img_on_show, consumer->faults.MarkPens[i].markposition, cv::Scalar(122, 0, 255), 5);
			}
		}
		UpdateData(false);

		DrawPicToHDC(img_on_show, IDC_PIC_Sample);
	}
	else
	{
		m_Info += Consumer::GetErrorDescription(subtype).c_str();
		UpdateData(false);
	}
	delete consumer;
	consumer = NULL;

	IsConsumerProcessing = false;

	//cv::imwrite("result.jpg", img_on_show);

	//if (!twag->ManualTigger())
	//{
	//	//MessageBox(L"当前无法触发！");
	//	printf_globle("当前无法触发!\n");
	//}
	//else
	//{
	//	m_Info = _T("");
	//	UpdateData(false);
	//}


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
		if (!twag->Switch2Virtual(strtmp))
		{
			MessageBox(L"虚拟相机文件不存在！");
			return;
		}
		if (IsDlgButtonChecked(IDC_CB_CanBeTiggered) == BST_CHECKED)
			twag->StartWatch();
		printf_globle("开始使用虚拟相机.\r\n\r\n");
	}
}




//void CTileDetectorMFCDlg::OnBnClickedBtnSavePic()
//{
//	/*CString msg;
//	msg.Format(_T("正在保存底片%d！\r\n"), twag->GrabbingIndex);
//	m_Info += msg;
//	stringstream ss;
//	UpdateData(false);
//	ss << "samples/" << twag->GrabbingIndex << "_o原图.jpg";
//	cv::imwrite(ss.str(), globle_var::ImageBuffer);
//
//	ss.str("");
//	ss << "samples/" << twag->GrabbingIndex << "_x3.jpg";
//	cv::imwrite(ss.str(), globle_var::ImageBufferX3);
//
//	m_Info += _T("保存完成\r\n");
//	UpdateData(false);
//*/
//}


void CTileDetectorMFCDlg::OnBnClickedBtnSetting()
{
	SettingDlg sd;
	twag->StopWatch();

	CButton* pBtn = (CButton*)GetDlgItem(IDC_CB_CanBeTiggered);
	pBtn->SetCheck(FALSE);

	if (sd.DoModal() == IDOK)
	{
		delete twag;
		twag = new TiggerWatcherAndGrabber(this->GetSafeHwnd(), globle_var::VirtualCameraFileName);
	}
	else
	{
	}
}


void CTileDetectorMFCDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	ShowImgROI(point);
	CDialogEx::OnMouseMove(nFlags, point);
}

void CTileDetectorMFCDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	img_big_flag = !img_big_flag;//标记放大/不放大图像
	//if (img_big_flag)
	//	zoom = 1;
	ShowImgROI(point);
	CDialogEx::OnLButtonDown(nFlags, point);
}
//鼠标滚轮
BOOL CTileDetectorMFCDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint point)
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

void CTileDetectorMFCDlg::ShowImgROI(CPoint point = CPoint(0, 0))
{
	if (img_on_show.rows > 0)
	{
		if (point != CPoint(0, 0))
		{
			ClientToScreen(&point);
			mouse_point = point;
		}
		CRect rect;
		GetDlgItem(IDC_PIC_Sample)->GetClientRect(rect);
		GetDlgItem(IDC_PIC_Sample)->ClientToScreen(rect);//函数是将你打开的APP中客户区的坐标点信息转换为整个屏幕的坐标
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
					DrawPicToHDC(img_on_show, IDC_PIC_Sample);
					GetDlgItem(IDC_LABLE_IMG_INFO)->SetWindowText(L"全显示");
				}
				else//放大显示
				{

					int startx = x - zoom_width / 2;
					int starty = y - zoom_height / 2;

					//防止越界
					if (startx < 0)startx = 0;
					if (starty < 0)starty = 0;
					if (img_on_show.cols - startx < zoom_width)
						startx = img_on_show.cols - startx;
					if (img_on_show.rows - starty < zoom_height)
						starty = img_on_show.rows - starty;
					//ROI
					cv::Mat roi = img_on_show(cv::Rect(startx, starty, zoom_width, zoom_height));
					DrawPicToHDC(roi, IDC_PIC_Sample);

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
				DrawPicToHDC(img_on_show, IDC_PIC_Sample);
				GetDlgItem(IDC_LABLE_IMG_INFO)->SetWindowText(L"全显示");
			}
		}
		else
		{
			DrawPicToHDC(img_on_show, IDC_PIC_Sample);
			GetDlgItem(IDC_LABLE_IMG_INFO)->SetWindowText(L"全显示");
			img_big_flag = false;//标记不放大图像
		}
	}
}
