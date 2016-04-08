#pragma once

// SpotsMainDlg.h : 头文件
//

#include <View/SpotsMainView.h>
#include <CvvImage.h>

#include <Class\Helper\StringHelper.h>
#include <Class\Debug\MFCConsole.h>

#include <Controller\Controller.h>

#import "c:\program files\common files\system\ado\msado15.dll" no_namespace rename("EOF","adoEOF")

// CSpotsMainDlg 对话框
class CSpotsMainDlg : public CDialogEx
	, public SpotsMainView
{
// 构造
public:
	CSpotsMainDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CSpotsMainDlg()
	{
		ReleaseDC(p_DC);
	}
// 对话框数据
	enum { IDD = IDD_SPOTS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
//屏蔽ESC
	virtual BOOL PreTranslateMessage(MSG* pMsg);  // PreTranslateMessage是消息在送给TranslateMessage函数之前被调用的
	virtual void OnOK();
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CMenu menu;
	bool isRunning = true;

	afx_msg   void   PicOnMouseHover(WPARAM wParam, LPARAM lParam){ MessageBox(L"鼠标已进入 "); };
	afx_msg   void   PicOnMouseLeave(WPARAM wParam, LPARAM lParam){ MessageBox(L"鼠标已离开 "); };
	BOOL mouse_in_img = false;
	BOOL img_big_flag = false;
	CPoint mouse_point;
	int zoom = 1;//放大倍数
	cv::Mat img_on_show;//全尺寸图
	//缩放显示图片
	void ShowImgROI(CPoint point);


	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);


	afx_msg void OnBnClickedBtnRun();
	afx_msg void OnBnClickedBtnSelectvirtualimg();
	afx_msg void OnBnClickedBtnvirtualtigger();


	LRESULT OnWM_MENU_OPEN_SYS_SET_DLG(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedOk();
	void DrawPicToHDC(cv::Mat& img, UINT ID);
	HDC hDC = NULL;
	CDC *p_DC;

	// 供Controller调用 //Override

	void ShowBigImg(cv::Mat);

	void ShowLogImg(cv::Mat);

	// 更新UI上显示的记录数据
	void UpdateStatistics();
	//传入true切换UI到虚拟相机模式，false切换UI到真实相机模式
	void SwitchModel2Virtual(bool switchToV);

	// 供Controller调用 END
	afx_msg void On32773();
	afx_msg void OnBtnMenuEdgeset();
	afx_msg void OnMenuClassiySetClick();
};
