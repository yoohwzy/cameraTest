
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
, m_Info(_T(""))
, m_VirtualCamera(_T(""))
, img_index(0)
, consumerThreshod(0)
, consumerLedStartX(0)
, consumerLedEndX(0)
, consumerThreshodHigh(0)
{
	printf_globle("");
}


void CTileDetectorMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TB_INFO, m_Info);
	DDX_Text(pDX, IDC_TB_VirtualCamera, m_VirtualCamera);
	DDX_Text(pDX, IDC_LABLE_IMG_Index, img_index);
	DDX_Text(pDX, IDC_TB_THRESHOD, consumerThreshod);
	DDV_MinMaxInt(pDX, consumerThreshod, 0, 255);
	DDX_Text(pDX, IDC_TB_LED_FROM, consumerLedStartX);
	DDV_MinMaxInt(pDX, consumerLedStartX, 0, 4094);
	DDX_Text(pDX, IDC_TB_LED_TO, consumerLedEndX);
	DDV_MinMaxInt(pDX, consumerLedEndX, 1, 4095);
	DDX_Text(pDX, IDC_TB_THRESHOD_HIGH, consumerThreshodHigh);
	DDV_MinMaxInt(pDX, consumerThreshodHigh, 1, 254);
}
BEGIN_MESSAGE_MAP(CTileDetectorMFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_MSG_GRABBING_END, &CTileDetectorMFCDlg::OnMsgGrabbingEnd)//�ɼ������������
	ON_MESSAGE(WM_MSG_PROCESSING_END, &CTileDetectorMFCDlg::OnMsgProcessingEnd)//��������������
	ON_BN_CLICKED(IDC_BTN_SCAN, &CTileDetectorMFCDlg::BtnScan_OnBnClicked)
	ON_BN_CLICKED(IDC_CB_CanBeTiggered, &CTileDetectorMFCDlg::OnBnClickedCbCanbetiggered)
	ON_EN_CHANGE(IDC_TB_VirtualCamera, &CTileDetectorMFCDlg::OnEnChangeTbVirtualcamera)
	ON_EN_KILLFOCUS(IDC_TB_VirtualCamera, &CTileDetectorMFCDlg::OnEnKillfocusTbVirtualcamera)
	ON_BN_CLICKED(IDC_BTN_CALIBRATION, &CTileDetectorMFCDlg::OnBnClickedBtnCalibration)
	ON_MESSAGE(WM_MSG_GRABBINGCalibartion_END, &CTileDetectorMFCDlg::OnMsgGrabbingCalibrationEnd)//���ꡢ�ɼ������������
	ON_BN_CLICKED(IDC_BTN_Setting, &CTileDetectorMFCDlg::OnBnClickedBtnSetting)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_EN_CHANGE(IDC_TB_THRESHOD, &CTileDetectorMFCDlg::OnEnChangeTbThreshod)
	ON_EN_CHANGE(IDC_TB_LED_FROM, &CTileDetectorMFCDlg::OnEnChangeTbLedFrom)
	ON_EN_CHANGE(IDC_TB_LED_TO, &CTileDetectorMFCDlg::OnEnChangeTbLedTo)
	ON_BN_CLICKED(IDC_CB_DO_THREEINONE, &CTileDetectorMFCDlg::OnBnClickedCbDoThreeinone)
END_MESSAGE_MAP()


// CTileDetectorMFCDlg ��Ϣ�������

BOOL CTileDetectorMFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

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

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������


	//��ȡ����
	globle_var::InitSetting(true);
	globle_var::VirtualCameraFileName = "1_oԭͼ.jpg";

	m_VirtualCamera = globle_var::VirtualCameraFileName.c_str();



	p_twag = new TiggerWatcherAndGrabber(this->GetSafeHwnd(), globle_var::VirtualCameraFileName);
	p_consumer = new Consumer(this->GetSafeHwnd());

	if (IsDlgButtonChecked(IDC_CB_CanBeTiggered) == BST_CHECKED)
	{
		p_twag->StartWatch();
	}
	else
	{
		p_twag->StopWatch();
	}
	GetDlgItem(IDC_LABLE_IMG_INFO)->SetWindowText(L"");

	consumerThreshod = 5;
	consumerThreshodHigh = 9;
	consumerLedStartX = 0;
	consumerLedEndX = 4095;

	UpdateData(false);

	//CButton* radio = (CButton*)GetDlgItem(IDC_CB_SAVE_IMG);
	//radio->SetCheck(1);

	CButton* radio2 = (CButton*)GetDlgItem(IDC_CB_DO_THREEINONE);
	radio2->SetCheck(1);

	//����ϵͳ��־
	LogHelper::Log("ϵͳ����\r\n");


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

// �����Ի��������С����ť������Ҫ����Ĵ���
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
	if (!p_twag->ManualTigger())
	{
		//MessageBox(L"��ǰ�޷�������");
		printf_globle("��ǰ�޷�����!\n");
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
	m_Info += _T("��ʼ�ȴ���׼ש����...\r\n");
	UpdateData(false);
	p_twag->StartWatchWithCalibration();
}
LRESULT CTileDetectorMFCDlg::OnMsgGrabbingEnd(WPARAM wParam, LPARAM lParam)
{
	//if (p_twag->GrabbingIndex % 2 == 0)
	//{
	//	cv::namedWindow("1", 0);
	//	cv::imshow("1", p_twag->Image);
	//	cv::waitKey(0);
	//}


	cv::Mat originalclone = p_twag->OriginalImage.clone();
	m_Info = _T("");
	//���������߽��̴���ͼ��
	if (p_consumer != NULL && p_consumer->IsProcessing == false)
	{
		delete p_consumer;
		p_consumer = NULL;
	}
	else if (IsConsumerProcessing)
	{
		printf_globle("�㷨̫������һ��������δ������");
		return 0;
	}
	if (p_consumer != NULL)
	{
		delete p_consumer;
		p_consumer = NULL;
	}

	//if (!p_twag->ManualTigger())
	//{
	//	printf_globle("��ǰ�޷�����!\n");
	//}
	//return 0;



	//if (!p_twag->ManualTigger())
	//{
	//	//MessageBox(L"��ǰ�޷�������");
	//	printf_globle("��ǰ�޷�����!\n");
	//}
	//else
	//{
	//	m_Info = _T("");
	//	UpdateData(false);
	//}
	//return 0;
	

	UpdateData(true);
	if (IsDlgButtonChecked(IDC_CB_RUN_CONSUMER) == BST_CHECKED)
	{
		p_consumer = new Consumer(this->GetSafeHwnd());
		p_consumer->GrabbingIndex = p_twag->GrabbingIndex;
		p_consumer->ConsumerThreshodLow = consumerThreshod;
		p_consumer->ConsumerThreshodHight = consumerThreshodHigh;
		p_consumer->ConsumerLedStartX = consumerLedStartX;
		p_consumer->ConsumerLedEndX = consumerLedEndX;
		IsConsumerProcessing = true;
		p_consumer->StartNewProces(p_twag->Image);
	}
	else if (p_twag->Image.cols > 0)
	{
		img_on_show.release();
		img_on_show = p_twag->Image.clone();
		img_index = p_twag->GrabbingIndex;
		DrawPicToHDC(img_on_show, IDC_PIC_Sample);
	}
	//delete p_consumer;
	//p_consumer = NULL;
	//IsConsumerProcessing = false;
	//if (!p_twag->ManualTigger())
	//{
	//	printf_globle("��ǰ�޷�����!\n");
	//}
	//return 0;

	 


	//�����Ƭ
	if (IsDlgButtonChecked(IDC_CB_SAVE_IMG) == BST_CHECKED)
	{
		CString msg;
		msg.Format(_T("���ڱ����Ƭ%d��\r\n"), p_twag->GrabbingIndex);
		m_Info += msg;
		stringstream ss;
		UpdateData(false);
		ss << "samples/" << p_twag->GrabbingIndex << "_oԭͼ.jpg";
		cv::imwrite(ss.str(), originalclone);

		m_Info += _T("�������\r\n");
		UpdateData(false);
	}


	//�첽�����Ƭ
	//int index = p_twag->GrabbingIndex;
	//cv::Mat img = p_twag->OriginalImage.clone();
	//if (IsDlgButtonChecked(IDC_CB_SAVE_IMG) == BST_CHECKED)
	//{
	//	thread t_saveimg = thread([index,img]{
	//		stringstream ss;
	//		ss << "samples/" << index << "_oԭͼ.jpg";
	//		cv::imwrite(ss.str(), img);
	//	});
	//	t_saveimg.detach();
	//}
	p_twag->OriginalImage.release();
	return 1;
}
LRESULT CTileDetectorMFCDlg::OnMsgGrabbingCalibrationEnd(WPARAM wParam, LPARAM lParam)
{
	////���������߽��̴���ͼ��

	//if (p_consumer->IsProcessing)
	//{
	//	printf_globle("�㷨̫������һ��������δ������");
	//	return 0;
	//}

	//delete p_consumer;
	//p_consumer = new Consumer(this->GetSafeHwnd());
	//p_consumer->GrabbingIndex = p_twag->GrabbingIndex;

	//p_consumer->StartNewProces4Calibraion(p_twag->Image);
	//m_Info += _T("�����ͼ��ɣ�\r\n");
	//UpdateData(false);
	return 1;
}
//wParam = 0��ʾ�����ɣ�1��ʾ�д���
//lParam ��ʾ��������
LRESULT CTileDetectorMFCDlg::OnMsgProcessingEnd(WPARAM wParam, LPARAM subtype)
{
	IsConsumerProcessing = false;


	//printf_globle("OnMsgProcessingEnd\n");
	//if (!p_twag->ManualTigger())
	//{
	//	printf_globle("��ǰ�޷�����!\n");
	//}
	//return 0;

	CString msg;
	msg.Format(_T("%d ������ɣ�\r\n"), p_consumer->GrabbingIndex);
	m_Info = msg;

	CString clog = L"\r\n";
	img_index = p_consumer->GrabbingIndex;
	if (wParam == 0)
	{
		img_on_show.release();
		img_on_show = p_consumer->originalImg.clone();
		if (p_consumer->faults.BrokenEdges.size() > 0 || 
			p_consumer->faults.BrokenCorners.size() > 0 ||
			p_consumer->faults.Scratchs.size() > 0 || 
			p_consumer->faults.Holes.size() > 0 || 
			p_consumer->faults.MarkPens.size() > 0
			)
		{
			arm.AddAction(0, TimeHelper::GetTimeNow(100));
		}
		if (p_consumer->faults.BrokenEdges.size() > 0)
		{
			CString str;
			str.Format(_T("%d ����%d������ȱ�ݣ���ɫ�����\r\n"), p_consumer->GrabbingIndex, p_consumer->faults.BrokenEdges.size());
			m_Info += str;
			clog += str;
			for (size_t i = 0; i < p_consumer->faults.BrokenEdges.size(); i++)
			{
				cv::circle(img_on_show, p_consumer->faults.BrokenEdges[i].position, p_consumer->faults.BrokenEdges[i].length + 50, cv::Scalar(0, 0, 255), 10);
			}
			//arm.AddAction(0, TimeHelper::GetTimeNow(globle_var::TiggerActionWaitTimeMS));
		}
		if (p_consumer->faults.BrokenCorners.size() > 0)
		{
			CString str;
			str.Format(_T("%d ����%d������ȱ�ݣ��������\r\n"), p_consumer->GrabbingIndex, p_consumer->faults.BrokenCorners.size());
			m_Info += str;
			clog += str;
			for (size_t i = 0; i < p_consumer->faults.BrokenCorners.size(); i++)
			{
				cv::circle(img_on_show, p_consumer->faults.BrokenCorners[i].position, p_consumer->faults.BrokenCorners[i].length + 50, cv::Scalar(127, 0, 228), 5);
			}
			//arm.AddAction(1, TimeHelper::GetTimeNow(globle_var::TiggerActionWaitTimeMS));
		}
		if (p_consumer->faults.SomethingBigs.size() > 0)
		{
			CString str;
			str.Format(_T("%d ����%d��EIDȱ�ݣ���ɫ�����\r\n"), p_consumer->GrabbingIndex, p_consumer->faults.SomethingBigs.size());
			m_Info += str;
			clog += str;
			for (size_t i = 0; i < p_consumer->faults.SomethingBigs.size(); i++)
			{
				cv::circle(img_on_show, p_consumer->faults.SomethingBigs[i].position, p_consumer->faults.SomethingBigs[i].diameter, cv::Scalar(255, 0, 0), 5);
			}
			//arm.AddAction(2, TimeHelper::GetTimeNow(globle_var::TiggerActionWaitTimeMS));
		}
		if (p_consumer->faults.Scratchs.size() > 0)
		{
			CString str;
			str.Format(_T("%d ����%d������ȱ�ݣ���ɫ�����\r\n"), p_consumer->GrabbingIndex, p_consumer->faults.Scratchs.size());
			m_Info += str;
			clog += str;
			for (size_t i = 0; i < p_consumer->faults.Scratchs.size(); i++)
			{
				cv::circle(img_on_show, p_consumer->faults.Scratchs[i].position, p_consumer->faults.Scratchs[i].length, cv::Scalar(0, 255, 0), 5);
			}
			//arm.AddAction(3, TimeHelper::GetTimeNow(globle_var::TiggerActionWaitTimeMS));
		}
		if (p_consumer->faults.Holes.size() > 0)
		{
			CString str;
			str.Format(_T("%d ����%d������ȱ�ݣ���ɫ�����\r\n"), p_consumer->GrabbingIndex, p_consumer->faults.Holes.size());
			m_Info += str;
			clog += str;
			for (size_t i = 0; i < p_consumer->faults.Holes.size(); i++)
			{
				cv::circle(img_on_show, p_consumer->faults.Holes[i].position, p_consumer->faults.Holes[i].diameter, cv::Scalar(0, 255, 255), 5);
			}
		}
		if (p_consumer->faults.MarkPens.size() > 0)
		{
			CString str;
			str.Format(_T("%d ����%d���˹���ǣ���ɫ�����\r\n"), p_consumer->GrabbingIndex, p_consumer->faults.MarkPens.size());
			m_Info += str;
			clog += str;
			for (size_t i = 0; i < p_consumer->faults.MarkPens.size(); i++)
			{
				cv::rectangle(img_on_show, p_consumer->faults.MarkPens[i].markposition, cv::Scalar(122, 0, 255), 5);
			}
		}
		UpdateData(false);

	}
	else
	{
		m_Info += Consumer::GetErrorDescription(subtype).c_str();
		UpdateData(false);
	}

	line(img_on_show, p_consumer->p_block->A, p_consumer->p_block->B, cv::Scalar(0, 255, 0), 2);
	line(img_on_show, p_consumer->p_block->A, p_consumer->p_block->D, cv::Scalar(255, 0, 0), 2);
	line(img_on_show, p_consumer->p_block->C, p_consumer->p_block->B, cv::Scalar(255, 255, 0), 2);
	line(img_on_show, p_consumer->p_block->C, p_consumer->p_block->D, cv::Scalar(0, 255, 255), 2);
	DrawPicToHDC(img_on_show, IDC_PIC_Sample);

	if (p_consumer != NULL)
	{
		delete p_consumer;
		p_consumer = NULL;
	}
	IsConsumerProcessing = false;
	clog += "\r\n";
	LogHelper::Log(clog);


	//cv::imwrite("result.jpg", img_on_show);

	//if (!p_twag->ManualTigger())
	//{
	//	printf_globle("��ǰ�޷�����!\n");
	//}
	//return 0;


	return 1;
}
void CTileDetectorMFCDlg::DrawPicToHDC(cv::Mat& img, UINT ID)
{
	IplImage image(img); //ԭʼͼ��
	CDC *p_DC = GetDlgItem(ID)->GetDC();
	HDC hDC = p_DC->GetSafeHdc();
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	CvvImage cimg;
	cimg.CopyOf(&image); // ����ͼƬ
	cimg.DrawToHDC(hDC, &rect); // ��ͼƬ���Ƶ���ʾ�ؼ���ָ��������
	ReleaseDC(p_DC);
}
void CTileDetectorMFCDlg::DrawPicToHDC(cv::Mat& img, UINT ID, HDC hDC)
{
	IplImage image(img); //ԭʼͼ��
	if (hDC == NULL)
	{
		p_DC = GetDlgItem(ID)->GetDC();
		hDC = p_DC->GetSafeHdc();
	}
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	CvvImage cimg;
	cimg.CopyOf(&image); // ����ͼƬ
	cimg.DrawToHDC(hDC, &rect); // ��ͼƬ���Ƶ���ʾ�ؼ���ָ��������
}



void CTileDetectorMFCDlg::OnBnClickedCbCanbetiggered()
{
	if (IsDlgButtonChecked(IDC_CB_CanBeTiggered) == BST_CHECKED)
		p_twag->StartWatch();
	else
		p_twag->StopWatch();
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
		p_twag->StopWatch();
		p_twag->Switch2Real();
		if (IsDlgButtonChecked(IDC_CB_CanBeTiggered) == BST_CHECKED)
			p_twag->StartWatch();
		printf_globle("��ʼʹ����ʵ���.\r\n\r\n");
	}
	else if ((globle_var::VirtualCameraFileName == "" && m_VirtualCamera != "") || (globle_var::VirtualCameraFileName != strtmp))
	{
		globle_var::VirtualCameraFileName = strtmp;
		p_twag->StopWatch();
		if (!p_twag->Switch2Virtual(strtmp))
		{
			MessageBox(L"��������ļ������ڣ�");
			return;
		}
		if (IsDlgButtonChecked(IDC_CB_CanBeTiggered) == BST_CHECKED)
			p_twag->StartWatch();
		printf_globle("��ʼʹ���������.\r\n\r\n");
	}
}




//void CTileDetectorMFCDlg::OnBnClickedBtnSavePic()
//{
//	/*CString msg;
//	msg.Format(_T("���ڱ����Ƭ%d��\r\n"), p_twag->GrabbingIndex);
//	m_Info += msg;
//	stringstream ss;
//	UpdateData(false);
//	ss << "samples/" << p_twag->GrabbingIndex << "_oԭͼ.jpg";
//	cv::imwrite(ss.str(), globle_var::ImageBuffer);
//
//	ss.str("");
//	ss << "samples/" << p_twag->GrabbingIndex << "_x3.jpg";
//	cv::imwrite(ss.str(), globle_var::ImageBufferX3);
//
//	m_Info += _T("�������\r\n");
//	UpdateData(false);
//*/
//}


void CTileDetectorMFCDlg::OnBnClickedBtnSetting()
{
	SettingDlg sd;
	p_twag->StopWatch();

	CButton* pBtn = (CButton*)GetDlgItem(IDC_CB_CanBeTiggered);
	pBtn->SetCheck(FALSE);

	if (sd.DoModal() == IDRETRY)
	{
		delete p_twag;
		p_twag = new TiggerWatcherAndGrabber(this->GetSafeHwnd(), globle_var::VirtualCameraFileName);
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
	img_big_flag = !img_big_flag;//��ǷŴ�/���Ŵ�ͼ��
	//if (img_big_flag)
	//	zoom = 1;
	ShowImgROI(point);
	CDialogEx::OnLButtonDown(nFlags, point);
}
//������
BOOL CTileDetectorMFCDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint point)
{
	//zDelta������0ʱΪ���Ϲ�����С��0ʱΪ���¹�����
	if (img_on_show.rows > 0)
	{
		if (mouse_in_img)   //����Ƿ��ڿؼ���Χ֮��
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
		GetDlgItem(IDC_PIC_Sample)->ClientToScreen(rect);//�����ǽ���򿪵�APP�пͻ������������Ϣת��Ϊ������Ļ������
		bool inRect = rect.PtInRect(point);
		bool inRectFlag = (point == CPoint(0, 0) && mouse_in_img);
		if (inRect || inRectFlag)   //����Ƿ��ڿؼ���Χ֮��
		{
			if (point == CPoint(0, 0))
				point = mouse_point;
			mouse_in_img = true;//����������ͼ��
			if (img_big_flag)
			{
				int idc_width = rect.right - rect.left;//ͼ��ؼ���
				int idc_height = rect.bottom - rect.top;//ͼ��ؼ���

				//���������λ�û���Ϊԭʼͼ�������
				int x = (float)(point.x - rect.left) / idc_width * img_on_show.cols;
				int y = (float)(point.y - rect.top) / idc_height * img_on_show.rows;

				//���ݷŴ�ϵ��ת��ROI����
				int zoom_width = zoom * idc_width;
				int zoom_height = zoom * idc_height;
				if (zoom_width >= img_on_show.cols || zoom_height >= img_on_show.rows)//���Ŵ�ϵ����С������ʾȫͼ
				{
					img_big_flag = false;//��ǲ��Ŵ�ͼ��
					DrawPicToHDC(img_on_show, IDC_PIC_Sample);
					GetDlgItem(IDC_LABLE_IMG_INFO)->SetWindowText(L"ȫ��ʾ");
				}
				else//�Ŵ���ʾ
				{
					int startx = x - zoom_width / 2;
					int starty = y - zoom_height / 2;

					//��ֹԽ��
					if (startx < 0)startx = 0;
					if (starty < 0)starty = 0;
					if (startx + zoom_width > img_on_show.cols)
						startx = img_on_show.cols - zoom_width;
					if (starty + zoom_height > img_on_show.rows)
						starty = img_on_show.rows - zoom_height;
					//ROI
					cv::Mat roi = img_on_show(cv::Rect(startx, starty, zoom_width, zoom_height));
					DrawPicToHDC(roi, IDC_PIC_Sample);

					stringstream ss;
					ss << "(" << x << "," << y << ") " << zoom << "��";

					int dwLen = ss.str().length() + 1;//strlen(ss.str().c_str()) + 1;
					int nwLen = MultiByteToWideChar(CP_ACP, 0, ss.str().c_str(), dwLen, NULL, 0);//������ʵĳ���
					LPWSTR lpszPath = new WCHAR[dwLen];
					MultiByteToWideChar(CP_ACP, 0, ss.str().c_str(), dwLen, lpszPath, nwLen);

					GetDlgItem(IDC_LABLE_IMG_INFO)->SetWindowText(lpszPath);
				}
			}
			else// if(!inRect && !inRectFlag)
			{
				mouse_in_img = false;//�������Ƴ�ͼ��
				DrawPicToHDC(img_on_show, IDC_PIC_Sample);
				GetDlgItem(IDC_LABLE_IMG_INFO)->SetWindowText(L"ȫ��ʾ");
			}
		}
		else
		{
			DrawPicToHDC(img_on_show, IDC_PIC_Sample);
			GetDlgItem(IDC_LABLE_IMG_INFO)->SetWindowText(L"ȫ��ʾ");
			img_big_flag = false;//��ǲ��Ŵ�ͼ��
		}
	}
}


void CTileDetectorMFCDlg::OnEnChangeTbThreshod()
{
	UpdateData(true);
}


void CTileDetectorMFCDlg::OnEnChangeTbLedFrom()
{
	UpdateData(true);
}


void CTileDetectorMFCDlg::OnEnChangeTbLedTo()
{
	UpdateData(true);
}


void CTileDetectorMFCDlg::OnBnClickedCbDoThreeinone()
{
	if (p_twag != NULL)
	{
		p_twag->DoThreeInOne = (IsDlgButtonChecked(IDC_CB_DO_THREEINONE) == BST_CHECKED);
	}
}
