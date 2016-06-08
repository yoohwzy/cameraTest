#pragma once

// SpotsMainDlg.h : ͷ�ļ�
//

#include <View/SpotsMainView.h>
#include <CvvImage.h>

#include <Class\Helper\StringHelper.h>
#include <Class\Debug\MFCConsole.h>

#include <Controller\ControllerCycleBuffer.h>

#import "c:\program files\common files\system\ado\msado15.dll" no_namespace rename("EOF","adoEOF")

// CSpotsMainDlg �Ի���
class CSpotsMainDlg : public CDialogEx
	, public SpotsMainView
{
// ����
public:
	CSpotsMainDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CSpotsMainDlg()
	{
		ReleaseDC(p_DC);
	}
// �Ի�������
	enum { IDD = IDD_SPOTS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
//����ESC
	virtual BOOL PreTranslateMessage(MSG* pMsg);  // PreTranslateMessage����Ϣ���͸�TranslateMessage����֮ǰ�����õ�
	virtual void OnOK();
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()

private:
	CMenu menu;
	bool isRunning = true;

	afx_msg   void   PicOnMouseHover(WPARAM wParam, LPARAM lParam){ MessageBox(L"����ѽ��� "); };
	afx_msg   void   PicOnMouseLeave(WPARAM wParam, LPARAM lParam){ MessageBox(L"������뿪 "); };
	BOOL mouse_in_img = false;
	BOOL img_big_flag = false;
	CPoint mouse_point;
	int zoom = 1;//��С����
	cv::Mat img_on_show;//ȫ�ߴ�ͼ
	//cv::Mat img_on_log;
	//������ʾͼƬ
	void ShowImgROI(CPoint point);


	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	afx_msg void OnBnClickedBtnRun();
	afx_msg void OnBnClickedBtnSelectvirtualimg();
	afx_msg void OnBnClickedBtnvirtualtigger();

	
	void saveParameter(int IDC, string SectionName, string key);

	//UI�������
	struct control//���ڼ�¼�ؼ�ԭʼλ��
	{
		control(){};
		control(int _id, int _owidth, int _oheight, int _ox, int _oy)
		{
			id = _id;
			owidth = _owidth;
			oheight = _oheight;
			ox = _ox;
			oy = _oy;
		};
		int id;
		int owidth;//ԭʼ���
		int oheight;//ԭʼ�߶�
		int ox;//ԭʼλ�ã�����������Ͻ�
		int oy;//ԭʼλ�ã�����������Ͻ�
	};
	bool initEndFlag = false;//ֻ�г�ʼ����ɺ����onsize��ִ��UI����
	afx_msg void OnSize(UINT nType, int cx, int cy);
	map<int, control> controls;
	void save1control(int ID);
public:
	afx_msg void OnBnClickedOk();
	void DrawPicToHDC(cv::Mat& img, UINT ID);
	HDC hDC = NULL;
	CDC *p_DC;

	// ��Controller���� //Override

	void ShowBigImg(cv::Mat);
	void ShowLogImg(cv::Mat);

	// ����UI����ʾ�ļ�¼����
	void UpdateStatistics();
	//����true�л�UI���������ģʽ��false�л�UI����ʵ���ģʽ
	void SwitchModel2Virtual(bool switchToV);

	// ��Controller���� END


	//����
	afx_msg void OnBnClickedBtnSizedingbiao();


	afx_msg void OnMenu_OPEN_SYS_SET_DLG();
	afx_msg void OnMenuBtnEdgeSet();
	afx_msg void OnMenuBtnClassiySet();
	afx_msg void OnMenuBtnAreaCamMainSet();
	afx_msg void OnMenuBtnSurfaceParaSet();
};
