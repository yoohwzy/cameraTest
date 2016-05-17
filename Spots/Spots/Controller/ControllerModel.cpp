#ifndef AFX_DATA
#	include <afxwin.h>
#endif
#include "ControllerModel.h"
#include <Class/Setting/SettingHelper.h>
#include <Class\Helper\StringHelper.h>
#include <shlwapi.h>
#include <Class/Debug/MFCConsole.h>
#pragma comment(lib,"Shlwapi.lib") //文件目录lib 如果没有这行，会出现link错误

//采集完照片后，图像处理前的回调函数，主要用于保存原始图像
void ControllerModel::ImageGetCallBack(cv::Mat img)
{
	if (SAVE_IMG)//是否要保存图片
	{
		std::thread t_tiggerThread(std::mem_fn(&ControllerModel::imageSave), this, img);
		t_tiggerThread.detach();
		//imageSave(img);
	}
	else
		MFCConsole::Output("当前为不保存图片模式\r\n");
}


/*********************  虚拟相机模式方法  ***********************/

void ControllerModel::VirtualSelectImg(cv::Mat image)
{
	if (image.cols == 0)
		MessageBox(0, L"图片读取失败！", L"错误", 0);
	else
	{
		worker1->image = image;
	}
}
void ControllerModel::VirtualWorkerStart()
{
	if (worker1->image.cols == 0)
	{
		MessageBox(0, L"请先加载虚拟底片！", L"错误", 0);
		return;
	}
	if (worker1->MyStatus == Worker::Free)
	{
		worker1->SN = Statistics::TodayAll + 1;
		worker1->StartWork();
	}
	else
	{
		MessageBox(0, L"上一轮处理尚未结束！", L"警告", 0);
	}
}
/*********************虚拟相机模式方法结束***********************/


void ControllerModel::ResetParameter()
{
#pragma region 从INI文件中读取配置参数

	int si = 0;
	SettingHelper::GetKeyInt("SYS_IMG_CAPTURE", "SAVE_IMG", si);
	this->SAVE_IMG = si;
	SettingHelper::GetKeyInt("SYS_IMG_CAPTURE", "Real_WidthMM", Real_WidthMM);
	SettingHelper::GetKeyInt("SYS_IMG_CAPTURE", "Real_LengthMM", Real_LengthMM);
	SettingHelper::GetKeyInt("SYS_IMG_CAPTURE", "Worker_WaitTimeMSIn", this->Worker_WaitTimeMSIn);
	SettingHelper::GetKeyInt("SYS_IMG_CAPTURE", "Worker_WaitTimeMSOut", this->Worker_WaitTimeMSOut);
	SettingHelper::GetKeyInt("SYS_IMG_CAPTURE", "Worker_FrameTimeOut", this->Worker_FrameTimeOut);

	/*************边缘缺陷检测参数***********/
	SettingHelper::GetKeyDouble("EDGE_PARAMETER", "BlockLocalizer_THRESHOD", this->BlockLocalizer_THRESHOD);
	SettingHelper::GetKeyDouble("EDGE_PARAMETER", "BlockLocalizer_ContinuePointCount", this->BlockLocalizer_ContinuePointCount);

	int _BlockEdgeDetector_Enable = 1;
	SettingHelper::GetKeyInt("EDGE_PARAMETER", "BlockEdgeDetector_Enable", _BlockEdgeDetector_Enable);
	this->BlockEdgeDetector_Enable = _BlockEdgeDetector_Enable;
	SettingHelper::GetKeyDouble("EDGE_PARAMETER", "BlockEdgeDetector_DIFF_THRESHOLD", this->BlockEdgeDetector_DIFF_THRESHOLD);
	SettingHelper::GetKeyDouble("EDGE_PARAMETER", "BlockEdgeDetector_DIFF_THRESHOLD", this->BlockEdgeDetector_DIFF_THRESHOLD);
	SettingHelper::GetKeyDouble("EDGE_PARAMETER", "BlockEdgeDetector_FAULTS_SPAN", this->BlockEdgeDetector_FAULTS_SPAN);
	SettingHelper::GetKeyDouble("EDGE_PARAMETER", "BlockEdgeDetector_FAULTS_COUNT", this->BlockEdgeDetector_FAULTS_COUNT);

	int _BlockEdgeLineDetector_Enable = 1;
	SettingHelper::GetKeyInt("EDGE_PARAMETER", "BlockEdgeLineDetector_Enable", _BlockEdgeLineDetector_Enable);
	this->BlockEdgeLineDetector_Enable = _BlockEdgeLineDetector_Enable;
	SettingHelper::GetKeyInt("EDGE_PARAMETER", "BlockEdgeLineDetector_BINARY_THRESHOD", this->BlockEdgeLineDetector_BINARY_THRESHOD);
	SettingHelper::GetKeyInt("EDGE_PARAMETER", "BlockEdgeLineDetector_LENGTH_THRESHOD", this->BlockEdgeLineDetector_LENGTH_THRESHOD);
	SettingHelper::GetKeyInt("EDGE_PARAMETER", "BlockEdgeLineDetector_DEEP_THRESHOD", this->BlockEdgeLineDetector_DEEP_THRESHOD);

	/**************分类参数****************/
	//缺边 长
	SettingHelper::GetKeyDouble("Classify", "EDGE_SINGLE_LENGTH_A", this->Classify_EDGE_SINGLE_LENGTH_A);
	SettingHelper::GetKeyDouble("Classify", "EDGE_SINGLE_LENGTH_B", this->Classify_EDGE_SINGLE_LENGTH_B);
	SettingHelper::GetKeyDouble("Classify", "EDGE_SINGLE_LENGTH_C", this->Classify_EDGE_SINGLE_LENGTH_C);
	SettingHelper::GetKeyDouble("Classify", "EDGE_SINGLE_LENGTH_ACCEPT", this->Classify_EDGE_SINGLE_LENGTH_ACCEPT);
	//总长
	SettingHelper::GetKeyDouble("Classify", "EDGE_TOTAL_LENGTH_A", this->Classify_EDGE_TOTAL_LENGTH_A);
	SettingHelper::GetKeyDouble("Classify", "EDGE_TOTAL_LENGTH_B", this->Classify_EDGE_TOTAL_LENGTH_B);
	SettingHelper::GetKeyDouble("Classify", "EDGE_TOTAL_LENGTH_C", this->Classify_EDGE_TOTAL_LENGTH_C);
	SettingHelper::GetKeyDouble("Classify", "EDGE_TOTAL_LENGTH_ACCEPT", this->Classify_EDGE_TOTAL_LENGTH_ACCEPT);
	//深度
	SettingHelper::GetKeyDouble("Classify", "EDGE_SINGLE_DEEP_A", this->Classify_EDGE_SINGLE_DEEP_A);
	SettingHelper::GetKeyDouble("Classify", "EDGE_SINGLE_DEEP_B", this->Classify_EDGE_SINGLE_DEEP_B);
	SettingHelper::GetKeyDouble("Classify", "EDGE_SINGLE_DEEP_C", this->Classify_EDGE_SINGLE_DEEP_C);
	SettingHelper::GetKeyDouble("Classify", "EDGE_SINGLE_DEEP_ACCEPT", this->Classify_EDGE_SINGLE_DEEP_ACCEPT);
	//总深度
	SettingHelper::GetKeyDouble("Classify", "EDGE_TOTAL_DEEP_A", this->Classify_EDGE_TOTAL_DEEP_A);
	SettingHelper::GetKeyDouble("Classify", "EDGE_TOTAL_DEEP_B", this->Classify_EDGE_TOTAL_DEEP_B);
	SettingHelper::GetKeyDouble("Classify", "EDGE_TOTAL_DEEP_C", this->Classify_EDGE_TOTAL_DEEP_C);
	SettingHelper::GetKeyDouble("Classify", "EDGE_TOTAL_DEEP_ACCEPT", this->Classify_EDGE_TOTAL_DEEP_ACCEPT);
	//凹凸
	SettingHelper::GetKeyDouble("Classify", "HOAL_DIAMETER_A", this->Classify_HOAL_DIAMETER_A);
	SettingHelper::GetKeyDouble("Classify", "HOAL_DIAMETER_B", this->Classify_HOAL_DIAMETER_B);
	SettingHelper::GetKeyDouble("Classify", "HOAL_DIAMETER_C", this->Classify_HOAL_DIAMETER_C);
	SettingHelper::GetKeyDouble("Classify", "HOAL_DIAMETER_ACCEPT", this->Classify_HOAL_DIAMETER_ACCEPT);

	SettingHelper::GetKeyDouble("Classify", "HOAL_COUNT_A", this->Classify_HOAL_COUNT_A);
	SettingHelper::GetKeyDouble("Classify", "HOAL_COUNT_B", this->Classify_HOAL_COUNT_B);
	SettingHelper::GetKeyDouble("Classify", "HOAL_COUNT_C", this->Classify_HOAL_COUNT_C);
	SettingHelper::GetKeyDouble("Classify", "HOAL_COUNT_ACCEPT", this->Classify_HOAL_COUNT_ACCEPT);

	//划痕 长
	SettingHelper::GetKeyDouble("Classify", "SCRATCH_SINGLE_LENGTH_A", this->Classify_SCRATCH_SINGLE_LENGTH_A);
	SettingHelper::GetKeyDouble("Classify", "SCRATCH_SINGLE_LENGTH_B", this->Classify_SCRATCH_SINGLE_LENGTH_B);
	SettingHelper::GetKeyDouble("Classify", "SCRATCH_SINGLE_LENGTH_C", this->Classify_SCRATCH_SINGLE_LENGTH_C);
	SettingHelper::GetKeyDouble("Classify", "SCRATCH_SINGLE_LENGTH_ACCEPT", this->Classify_SCRATCH_SINGLE_LENGTH_ACCEPT);
	//总长
	SettingHelper::GetKeyDouble("Classify", "SCRATCH_TOTAL_LENGTH_A", this->Classify_SCRATCH_TOTAL_LENGTH_A);
	SettingHelper::GetKeyDouble("Classify", "SCRATCH_TOTAL_LENGTH_B", this->Classify_SCRATCH_TOTAL_LENGTH_B);
	SettingHelper::GetKeyDouble("Classify", "SCRATCH_TOTAL_LENGTH_C", this->Classify_SCRATCH_TOTAL_LENGTH_C);
	SettingHelper::GetKeyDouble("Classify", "SCRATCH_TOTAL_LENGTH_ACCEPT", this->Classify_SCRATCH_TOTAL_LENGTH_ACCEPT);

#pragma endregion
#pragma region 配置参数应用于worker
	for (int i = 0; i < 2; i++)
	{
		Worker *w = NULL;
		if (i == 0)
			w = worker1;
		else
			w = worker2;
		if (w != NULL)
		{
			w->Real_WidthMM = Real_WidthMM;
			w->Real_LengthMM = Real_LengthMM;
			w->BlockLocalizer_THRESHOD = BlockLocalizer_THRESHOD;
			w->BlockLocalizer_ContinuePointCount = BlockLocalizer_ContinuePointCount;

			w->BlockEdgeDetector_Enable = BlockEdgeDetector_Enable;
			w->BlockEdgeDetector_DIFF_THRESHOLD = BlockEdgeDetector_DIFF_THRESHOLD;
			w->BlockEdgeDetector_FAULTS_SPAN = BlockEdgeDetector_FAULTS_SPAN;
			w->BlockEdgeDetector_FAULTS_COUNT = BlockEdgeDetector_FAULTS_COUNT;

			w->BlockEdgeLineDetector_Enable = BlockEdgeLineDetector_Enable;
			w->BlockEdgeLineDetector_BINARY_THRESHOD = BlockEdgeLineDetector_BINARY_THRESHOD;
			w->BlockEdgeLineDetector_LENGTH_THRESHOD = BlockEdgeLineDetector_LENGTH_THRESHOD;
			w->BlockEdgeLineDetector_DEEP_THRESHOD = BlockEdgeLineDetector_DEEP_THRESHOD;

			w->WaitTimeMSIn = Worker_WaitTimeMSIn;
			w->WaitTimeMSOut = Worker_WaitTimeMSOut;
			w->FrameTimeOut = Worker_FrameTimeOut;


			/**************分级参数*************/

			//边参数
			w->Classify_EDGE_SINGLE_LENGTH_A = Classify_EDGE_SINGLE_LENGTH_A;
			w->Classify_EDGE_SINGLE_LENGTH_B = Classify_EDGE_SINGLE_LENGTH_B;
			w->Classify_EDGE_SINGLE_LENGTH_C = Classify_EDGE_SINGLE_LENGTH_C;
			w->Classify_EDGE_SINGLE_LENGTH_ACCEPT = Classify_EDGE_SINGLE_LENGTH_ACCEPT;

			w->Classify_EDGE_TOTAL_LENGTH_A = Classify_EDGE_TOTAL_LENGTH_A;
			w->Classify_EDGE_TOTAL_LENGTH_B = Classify_EDGE_TOTAL_LENGTH_B;
			w->Classify_EDGE_TOTAL_LENGTH_C = Classify_EDGE_TOTAL_LENGTH_C;
			w->Classify_EDGE_TOTAL_LENGTH_ACCEPT = Classify_EDGE_TOTAL_LENGTH_ACCEPT;

			w->Classify_EDGE_SINGLE_DEEP_A = Classify_EDGE_SINGLE_DEEP_A;
			w->Classify_EDGE_SINGLE_DEEP_B = Classify_EDGE_SINGLE_DEEP_B;
			w->Classify_EDGE_SINGLE_DEEP_C = Classify_EDGE_SINGLE_DEEP_C;
			w->Classify_EDGE_SINGLE_DEEP_ACCEPT = Classify_EDGE_SINGLE_DEEP_ACCEPT;

			w->Classify_EDGE_TOTAL_DEEP_A = Classify_EDGE_TOTAL_DEEP_A;
			w->Classify_EDGE_TOTAL_DEEP_B = Classify_EDGE_TOTAL_DEEP_B;
			w->Classify_EDGE_TOTAL_DEEP_C = Classify_EDGE_TOTAL_DEEP_C;
			w->Classify_EDGE_TOTAL_DEEP_ACCEPT = Classify_EDGE_TOTAL_DEEP_ACCEPT;

			//凹坑参数
			w->Classify_HOAL_DIAMETER_A = Classify_HOAL_DIAMETER_A;
			w->Classify_HOAL_DIAMETER_B = Classify_HOAL_DIAMETER_B;
			w->Classify_HOAL_DIAMETER_C = Classify_HOAL_DIAMETER_C;
			w->Classify_HOAL_DIAMETER_ACCEPT = Classify_HOAL_DIAMETER_ACCEPT;

			w->Classify_HOAL_COUNT_A = Classify_HOAL_COUNT_A;
			w->Classify_HOAL_COUNT_B = Classify_HOAL_COUNT_B;
			w->Classify_HOAL_COUNT_C = Classify_HOAL_COUNT_C;
			w->Classify_HOAL_COUNT_ACCEPT = Classify_HOAL_COUNT_ACCEPT;


			//划痕
			w->Classify_SCRATCH_SINGLE_LENGTH_A = Classify_SCRATCH_SINGLE_LENGTH_A;
			w->Classify_SCRATCH_SINGLE_LENGTH_B = Classify_SCRATCH_SINGLE_LENGTH_B;
			w->Classify_SCRATCH_SINGLE_LENGTH_C = Classify_SCRATCH_SINGLE_LENGTH_C;
			w->Classify_SCRATCH_SINGLE_LENGTH_ACCEPT = Classify_SCRATCH_SINGLE_LENGTH_ACCEPT;

			w->Classify_SCRATCH_TOTAL_LENGTH_A = Classify_SCRATCH_TOTAL_LENGTH_A;
			w->Classify_SCRATCH_TOTAL_LENGTH_B = Classify_SCRATCH_TOTAL_LENGTH_B;
			w->Classify_SCRATCH_TOTAL_LENGTH_C = Classify_SCRATCH_TOTAL_LENGTH_C;
			w->Classify_SCRATCH_TOTAL_LENGTH_ACCEPT = Classify_SCRATCH_TOTAL_LENGTH_ACCEPT;
		}
		w = NULL;
	}
#pragma endregion
}




//private:

void ControllerModel::imageSave(cv::Mat img)
{
	image_write_lock.lock();

	cv::Mat img2 = img.clone();
	//判断文件夹是否存在
	CString folderMonthName;
	CString folderDayName;
	CString imageName;
	CTime t = CTime::GetCurrentTime();

	folderMonthName = L"Images\\" + t.Format("%Y_%m");
	folderDayName = folderMonthName + "\\" + t.Format("%d");
	imageName = folderDayName + "\\" + t.Format("%Y_%m_%d__%H_%M_%S");
	SYSTEMTIME st;
	GetLocalTime(&st);
	CString millsec;
	millsec.Format(L"%d", (int)st.wMilliseconds);
	imageName += (L"_" + millsec + L".jpg");

	if (!PathIsDirectory(L"Images"))
	{
		CreateDirectory(L"Images", NULL);
	}
	if (!PathIsDirectory(folderMonthName))
	{
		CreateDirectory(folderMonthName, NULL);
	}
	if (!PathIsDirectory(folderDayName))
	{
		CreateDirectory(folderDayName, NULL);
	}

	stringstream ss;
	cv::imwrite(StringHelper::CString2string(imageName), img2);

	image_write_lock.unlock();
}