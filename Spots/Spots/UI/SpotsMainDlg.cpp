
// SpotsMainDlg.cpp : ʵ���ļ�
//

#include "../stdafx.h"
#include "../Spots.h"
#include "SpotsMainDlg.h"
#include "afxdialogex.h"

#include <Class\Statistics\Statistics.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "SpotsSystemSetDlg.h"
#include "SpotsEdgeParameterSetDlg.h"
#include "SpotsClassifySet.h"
#include "SpotsSurfaceParaSetDlg.h"
#include <Class/Setting/SettingHelper.h>



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
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_32773, &CSpotsMainDlg::OnMenu_OPEN_SYS_SET_DLG)
	ON_COMMAND(ID_BTN_Menu_EdgeSet, &CSpotsMainDlg::OnBtnMenuEdgeset)
	ON_COMMAND(ID_32778, &CSpotsMainDlg::OnMenuClassiySetClick)
	ON_WM_SIZE()
	ON_COMMAND(ID_32775, &CSpotsMainDlg::OnSurfaceParaSet)
	ON_BN_CLICKED(IDC_BTN_SizeDingBiao, &CSpotsMainDlg::OnBnClickedBtnSizedingbiao)
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
	// �򿪿���̨
	if (__argc > 1)
	{ 
		for (size_t i = 0; i < __argc; i++)
		{
			CString targv = __targv[i];
			if (targv == L"debug")//�������̨ 
			{
				MFCConsole::Init();
				MFCConsole::Output("Debug start.\r\n");
			}
			if (targv == L"virtual")
			{
				p_contrller->IsRealModel = 0;
			}
		}
	}

	// ��Ӳ˵���
	ModifyStyle(WS_THICKFRAME, DS_MODALFRAME);
	menu.LoadMenuW(IDR_MenuMain);
	SetMenu(&menu);

	int Real_WidthMM = 0;
	if (SettingHelper::GetKeyInt("SYS_IMG_CAPTURE", "Real_WidthMM", Real_WidthMM))
		GetDlgItem(IDC_TB_Real_WidthMM)->SetWindowText(StringHelper::int2CString(Real_WidthMM));
	else
		GetDlgItem(IDC_TB_Real_WidthMM)->SetWindowText(L"600");

	int Real_LengthMM = 0;
	if (SettingHelper::GetKeyInt("SYS_IMG_CAPTURE", "Real_LengthMM", Real_LengthMM))
		GetDlgItem(IDC_TB_Real_LengthMM)->SetWindowText(StringHelper::int2CString(Real_LengthMM));
	else
		GetDlgItem(IDC_TB_Real_LengthMM)->SetWindowText(L"300");

	//ϵͳ��ʼ��
	p_contrller->Init();

	save1control(0);//����������
	
	save1control(IDC_BTN_SelectVirtualImg);
	save1control(IDC_BTN_virtualTigger);
	save1control(IDC_BTN_RUN);
	save1control(IDCANCEL);
	save1control(IDC_GB_IMG_BIG);
	save1control(IDC_IMG_BIG);
	save1control(IDC_LABLE_IMG_INFO);
	
	save1control(IDC_GB_LogImg);
	save1control(IDC_IMG_HISTORY);

	save1control(IDC_GB_STATISTIC);
	save1control(IDC_LB11);
	save1control(IDC_LB7);
	save1control(IDC_LB8);
	save1control(IDC_LB9);
	save1control(IDC_LB10);
	save1control(IDC_LB12);
	save1control(IDC_LB4);
	save1control(IDC_LB5);
	save1control(IDC_LB6);


	save1control(IDC_LB_todayTotal);
	save1control(IDC_LB_todayA);
	save1control(IDC_LB_todayB);
	save1control(IDC_LB_todayC);
	save1control(IDC_LB_todayGood);
	save1control(IDC_LB_dayFineRate);

	save1control(IDC_LB_monthTotal);
	save1control(IDC_LB_monthA);
	save1control(IDC_LB_monthB);
	save1control(IDC_LB_monthC);
	save1control(IDC_LB_monthGood);
	save1control(IDC_LB_monthFineRate);

	save1control(IDC_LB_yearTotal);
	save1control(IDC_LB_yearA);
	save1control(IDC_LB_yearB);
	save1control(IDC_LB_yearC);
	save1control(IDC_LB_yearGood);
	save1control(IDC_LB_yearFineRate);

	save1control(IDC_LB_SIZESHOW);
	save1control(IDC_GB_SIZE);
	save1control(IDC_LB_SZIE_X);
	save1control(IDC_TB_Real_WidthMM);
	save1control(IDC_LB_SIZE_Y);
	save1control(IDC_TB_Real_LengthMM);
	save1control(IDC_BTN_SizeDingBiao);
	


	initEndFlag = true;
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE 
}
void CSpotsMainDlg::save1control(int id)
{
	CRect rect;
	if (id > 0)
	{
		GetDlgItem(id)->GetWindowRect(&rect);//��ȡ�ؼ��������Ļ��λ��
		ScreenToClient(rect);//ת��Ϊ�Ի����ϵ����λ��
	}
	else
	{
		GetClientRect(rect);
	}
	controls.insert(pair<int, control>(id, control(id, rect.Width(), rect.Height(), rect.left, rect.top)));
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

		//ShowLogImg(img_on_log);
		//ShowBigImg(img_on_show);
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

	if (pWnd->GetDlgCtrlID() == IDC_BTN_RUN)
	{
		if (isRunning)
		{
			pDC->SetTextColor(RGB(255, 0, 0));  //����������ɫ
			return (HBRUSH)::GetStockObject(BLACK_BRUSH);  // ���ñ���ɫ
		}
		else
		{
			pDC->SetTextColor(RGB(0, 0, 0));  //����������ɫ
			return (HBRUSH)::GetStockObject(WHITE_BRUSH);  // ���ñ���ɫ
		}
	}
	else
	{
		//pDC->SetBkMode(TRANSPARENT);
		pDC->SetBkColor(RGB(255, 255, 255));
		//pDC->SetTextColor(RGB(0, 0, 0));
		//return (HBRUSH)GetStockObject(HOLLOW_BRUSH);//͸����ˢ
		return (HBRUSH)GetStockObject(WHITE_BRUSH);
	}
	return hbr;
}


BOOL CSpotsMainDlg::PreTranslateMessage(MSG* pMsg)
{
	//����ESC�رմ���/
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	//���λس��رմ���,���ᵼ�»س��ڴ�����ʧЧ.
	if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN && pMsg->wParam) return TRUE;
	else
		return CDialog::PreTranslateMessage(pMsg);
}
void CSpotsMainDlg::OnOK()
{
	//CDialogEx::OnOK();
}


//#include <shlwapi.h>
//#pragma comment(lib,"Shlwapi.lib") //���û�����У������link����
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
	zoom = 1;
	img_on_show = img;
	DrawPicToHDC(img, IDC_IMG_BIG);
}
void CSpotsMainDlg::ShowLogImg(cv::Mat img)
{
	//img_on_log = img;
	DrawPicToHDC(img, IDC_IMG_HISTORY);
}

void CSpotsMainDlg::UpdateStatistics()
{
	GetDlgItem(IDC_LB_todayTotal)->SetWindowText(StringHelper::int2CString(Statistics::TodayAll));
	GetDlgItem(IDC_LB_todayA)->SetWindowText(StringHelper::int2CString(Statistics::TodayA));
	GetDlgItem(IDC_LB_todayB)->SetWindowText(StringHelper::int2CString(Statistics::TodayB));
	GetDlgItem(IDC_LB_todayC)->SetWindowText(StringHelper::int2CString(Statistics::TodayC));
	GetDlgItem(IDC_LB_todayGood)->SetWindowText(StringHelper::int2CString(Statistics::TodayAll - Statistics::TodayRejected));

	GetDlgItem(IDC_LB_monthTotal)->SetWindowText(StringHelper::int2CString(Statistics::MonthAll));
	GetDlgItem(IDC_LB_monthA)->SetWindowText(StringHelper::int2CString(Statistics::MonthA));
	GetDlgItem(IDC_LB_monthB)->SetWindowText(StringHelper::int2CString(Statistics::MonthB));
	GetDlgItem(IDC_LB_monthC)->SetWindowText(StringHelper::int2CString(Statistics::MonthC));
	GetDlgItem(IDC_LB_monthGood)->SetWindowText(StringHelper::int2CString(Statistics::MonthAll - Statistics::MonthRejected));

	GetDlgItem(IDC_LB_yearTotal)->SetWindowText(StringHelper::int2CString(Statistics::YearAll));
	GetDlgItem(IDC_LB_yearA)->SetWindowText(StringHelper::int2CString(Statistics::YearA));
	GetDlgItem(IDC_LB_yearB)->SetWindowText(StringHelper::int2CString(Statistics::YearB));
	GetDlgItem(IDC_LB_yearC)->SetWindowText(StringHelper::int2CString(Statistics::YearC));
	GetDlgItem(IDC_LB_yearGood)->SetWindowText(StringHelper::int2CString(Statistics::YearAll - Statistics::YearRejected));

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
		p_contrller->PauseFlag = 1;
		GetDlgItem(IDC_BTN_RUN)->SetWindowText(L"��ʼ����");
	}
	else
	{
		p_contrller->PauseFlag = 0;
		GetDlgItem(IDC_BTN_RUN)->SetWindowText(L"��ͣ����");
	}
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
	cv::Mat img = cv::imread(StringHelper::CString2string(strFile));
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




void CSpotsMainDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	if (p_contrller->PauseFlag)
	{
		if (img_big_flag != false)
		{
			ShowImgROI(point);
		}
	}
	CDialogEx::OnMouseMove(nFlags, point);
}
void CSpotsMainDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (p_contrller->PauseFlag)
	{
		img_big_flag = !img_big_flag;//��ǷŴ�/���Ŵ�ͼ��
		//if (img_big_flag)
		//	zoom = 1;
		zoom = 1;
		ShowImgROI(point);
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}
//������
BOOL CSpotsMainDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint point)
{
	if (p_contrller->PauseFlag)
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
		GetDlgItem(IDC_IMG_BIG)->ClientToScreen(rect);//�����ǽ���򿪵�APP�пͻ������������Ϣת��Ϊ������Ļ������
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
					DrawPicToHDC(img_on_show, IDC_IMG_BIG);
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
					DrawPicToHDC(roi, IDC_IMG_BIG);

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
				DrawPicToHDC(img_on_show, IDC_IMG_BIG);
				GetDlgItem(IDC_LABLE_IMG_INFO)->SetWindowText(L"ȫ��ʾ");
			}
		}
		else
		{
			DrawPicToHDC(img_on_show, IDC_IMG_BIG);
			GetDlgItem(IDC_LABLE_IMG_INFO)->SetWindowText(L"ȫ��ʾ");
			img_big_flag = false;//��ǲ��Ŵ�ͼ��
		}
	}
}

void CSpotsMainDlg::OnMenu_OPEN_SYS_SET_DLG()
{
	SpotsSystemSetDlg m;
	if (m.DoModal() == IDOK)
	{
		p_contrller->ResetParameter();
	}
}

void CSpotsMainDlg::OnBtnMenuEdgeset()
{
	SpotsEdgeParameterSetDlg m;
	if (m.DoModal() == IDOK)
	{
		// ������
		p_contrller->ResetParameter();
	}
}
void CSpotsMainDlg::OnMenuClassiySetClick()
{
	CSpotsClassifySet m;
	if (m.DoModal() == IDOK)
	{
		// ������
		p_contrller->ResetParameter();
	}
}

void CSpotsMainDlg::OnSurfaceParaSet()
{
	SpotsSurfaceParaSetDlg m;
	if (m.DoModal() == IDOK)
	{
		// ������
		p_contrller->ResetParameter();
	}
}


void CSpotsMainDlg::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);


	//UI���������
	if (initEndFlag)
	{
		int key = 0;
		while (!(controls.count(key) > 0))
		{
			if (key >= controls.size() - 1)break;
			key++;
		}
		CRect rect;
		GetClientRect(rect);
		int newwidth = rect.Width();
		int newheight = rect.Height();
		int owidth = controls[key].owidth;
		int oheight = controls[key].oheight;

		double k1 = (double)newwidth / (double)owidth;
		double k2 = (double)newheight / (double)oheight;


		map<int, control>::iterator   it = controls.begin();
		for (; it != controls.end(); ++it)
		{
			if (it->first > 0)
			{
				int width = it->second.owidth * k1;
				int height = it->second.oheight * k2;
				int x = it->second.ox * k1;
				int y = it->second.oy * k2;
				GetDlgItem(it->first)->MoveWindow(x, y, width, height);
			}
		}
	}
}



void CSpotsMainDlg::OnBnClickedBtnSizedingbiao()
{
	saveParameter(IDC_TB_Real_WidthMM, "SYS_IMG_CAPTURE", "Real_WidthMM");
	saveParameter(IDC_TB_Real_LengthMM, "SYS_IMG_CAPTURE", "Real_LengthMM");


}
void CSpotsMainDlg::saveParameter(int IDC, string SectionName, string key)
{
	//��������
	CString cstr;
	GetDlgItem(IDC)->GetWindowText(cstr);
	SettingHelper::AddKey(SectionName, key, StringHelper::CString2string(cstr));
}