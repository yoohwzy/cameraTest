#pragma once

// SpotsMainDlg.h : 头文件
//

#include <View/SpotsMainView.h>
#include <CvvImage.h>

#include <Class\Helper\StringHelper.h>
#include <Class\Debug\MFCConsole.h>

#include <Controller\ControllerCycleBuffer.h>

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
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()

private:
	CMenu menu;
	bool isRunning = true;

	afx_msg   void   PicOnMouseHover(WPARAM wParam, LPARAM lParam){ MessageBox(L"鼠标已进入 "); };
	afx_msg   void   PicOnMouseLeave(WPARAM wParam, LPARAM lParam){ MessageBox(L"鼠标已离开 "); };
	BOOL mouse_in_img = false;
	BOOL img_big_flag = false;
	CPoint mouse_point;
	int zoom = 1;//缩小倍数
	cv::Mat img_on_show;//全尺寸图
	//cv::Mat img_on_log;
	//缩放显示图片
	void ShowImgROI(CPoint point);


	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	afx_msg void OnBnClickedBtnRun();
	afx_msg void OnBnClickedBtnSelectvirtualimg();
	afx_msg void OnBnClickedBtnvirtualtigger();

	
	void saveParameter(int IDC, string SectionName, string key);

	//UI缩放相关
	struct control//用于记录控件原始位置
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
		int owidth;//原始宽度
		int oheight;//原始高度
		int ox;//原始位置，距离程序左上角
		int oy;//原始位置，距离程序左上角
	};
	bool initEndFlag = false;//只有初始化完成后才在onsize中执行UI缩放
	afx_msg void OnSize(UINT nType, int cx, int cy);
	map<int, control> controls;//用于保存所有控件对象的信息
	void save1control(int ID);//保存一个控件的位置大小，窗体初始化时需对所有控件保存一次，输入为控件ID
public:
	void DrawPicToHDC(cv::Mat& img, UINT ID);
	HDC hDC = NULL;
	CDC *p_DC;



	// 供Controller调用 //Override

	void ShowBigImg(cv::Mat) override;
	void ShowLogImg(cv::Mat) override;
	void UpdateSizeInfo(string info) override;
	void UpdateStatistics() override;





	//传入true切换UI到虚拟相机模式，false切换UI到真实相机模式
	void SwitchModel2Virtual(bool switchToV);

	// 供Controller调用 END


	//定标
	afx_msg void OnBnClickedBtnSizedingbiao();
	afx_msg void OnBnClickedOk();
	afx_msg void OnMenu_OPEN_SYS_SET_DLG();
	afx_msg void OnMenuBtnEdgeSet();
	afx_msg void OnMenuBtnClassiySet();
	afx_msg void OnMenuBtnAreaCamMainSet();
	afx_msg void OnMenuBtnSurfaceParaSet();
};
