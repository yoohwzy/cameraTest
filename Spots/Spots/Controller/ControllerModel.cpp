#ifndef AFX_DATA
#	include <afxwin.h>
#endif
#include "ControllerModel.h"
#include <Class/Setting/SettingHelper.h>
#include <Class/Helper/StringHelper.h>
#include <Class/Debug/MFCConsole.h>
#include <shlwapi.h>
#pragma comment(lib,"Shlwapi.lib") //文件目录lib 如果没有这行，会出现link错误


#include <Algorithm/BlockLocalizer.h>
#include <Algorithm/BlockEdgeSimilarDetector.h>
#include <Algorithm/BlockEdgeLineDetector.h>
#include <Algorithm/EdgeFaultLineDetector.h>

//通用的初始化代码
//初始化PCI1761、数据库、根据参数将系统切换到真实/虚拟相机模式
//真实模式下调用TiggerStartWatch，开启监视线程
void ControllerModel::baseInit()
{
	stringstream ss;

	//初始化PCI1761
	if (IsRealModel)
	{
		if (!pci1761.init())
		{
			pci1761Inited = false;
		}
		else
			pci1761Inited = true;
	}


	// 数据库配置初始化
	if (1 == 1)
	{
		int accEnable = 0;
		SettingHelper::GetKeyInt("DATABASE", "ACCDB_ENABLE", accEnable);//读取是否启用数据库模块

		ss.clear();
		ss << "ACCDB_ENABLE = " << accEnable << endl;
		MFCConsole::Output(ss.str());
		if (accEnable != 0)
		{
			string db_path;
			bool accConnFlag = false;
			if (SettingHelper::GetKeyString("DATABASE", "ACCDB_PATH", db_path))//读取数据库所在路径
			{
				accConnFlag = Statistics::InitDate(db_path);
				MFCConsole::Output("db_path:");
				MFCConsole::Output(db_path);
				MFCConsole::Output("\r\n");
			}
			else
			{
				accConnFlag = Statistics::InitDate("src//..//瓷砖缺陷检测数据库.mdb");
				SettingHelper::AddKey("DATABASE", "ACCDB_PATH", "src//..//瓷砖缺陷检测数据库.mdb");
				MFCConsole::Output("db_path:src//..//瓷砖缺陷检测数据库.mdb\r\n");
			}
			// 统计数据初始化
			if (accConnFlag)
				spotsMainView->UpdateStatistics();
			else
				AfxMessageBox(L"无法连接到数据库！");
		}
	}
	// 初始化工作线程
	if (e2vInited && pci1761Inited)
	{
		//若为真实相机模式，则实例化两个工人，并启动触发器监视进程

		//初始化工人
		worker1 = new Worker("worker1:");
		worker2 = new Worker("worker2:");

		worker1->P_Controller = this;
		worker2->P_Controller = this;
		TiggerStartWatch();

		spotsMainView->SwitchModel2Virtual(false);

		MFCConsole::Output("已切换到真实相机模式。\r\n");
	}
	else
	{
		PauseFlag = true;
		if (IsRealModel)
		{
			IsRealModel = 0;
			if (!e2vInited && !pci1761Inited)
				AfxMessageBox(L"线阵相机&pci1761初始化失败！已切换到虚拟相机模式。");
			else if (!pci1761Inited)
				AfxMessageBox(L"pci1761初始化失败！已切换到虚拟相机模式。");
			else if (!e2vInited)
				AfxMessageBox(L"线阵相机初始化失败！已切换到虚拟相机模式。");
		}
		else
			MFCConsole::Output("已切换到虚拟相机模式。\r\n");
		//若为虚拟相机模式，则实例化虚拟相机类与1个工人
		//开启虚拟相机
		cv::Mat virtualImg;
		worker1 = new Worker("Virtual Worker");
		worker1->image = virtualImg;
		worker1->P_Controller = this;
		spotsMainView->SwitchModel2Virtual(true);
	}

	//读取参数配置
	//设置工人算法参数
	ResetParameter();


	if (pci1761Inited)
	{
		//初始化面阵相机
		if (MainHueScanner::Enabled)
		{
			P_mainHueScanner = new MainHueScanner(this);
			if (!P_mainHueScanner->HasInited)
			{
				delete P_mainHueScanner;
				P_mainHueScanner = NULL;
				AfxMessageBox(L"未能初始化面阵相机，该模块被禁用！");
			}
		}
	}



	//初始化UI
	cv::Mat white(2, 2, CV_8U, cv::Scalar(255));
	logImg.InitDrawingBoard();
	spotsMainView->ShowBigImg(white);
	spotsMainView->ShowLogImg(logImg.DrawingBoard);
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

	//尺寸相关
	SettingHelper::GetKeyInt("SYS_STANDARD", "Standard_Width_mm", Block::Standard_Width_mm);
	SettingHelper::GetKeyInt("SYS_STANDARD", "Standard_Length_mm", Block::Standard_Length_mm);
	SettingHelper::GetKeyDouble("SYS_STANDARD", "X_mmPerPix", Block::X_mmPerPix);
	SettingHelper::GetKeyDouble("SYS_STANDARD", "Y_mmPerPix", Block::Y_mmPerPix);

	//触发、采图相关
	SettingHelper::GetKeyInt("SYS_IMG_CAPTURE", "WaitTimeMSIn", this->Capture_WaitTimeMSIn);
	SettingHelper::GetKeyInt("SYS_IMG_CAPTURE", "WaitTimeMSOut", this->Capture_WaitTimeMSOut);
	SettingHelper::GetKeyInt("SYS_IMG_CAPTURE", "FrameTimeOut", this->Capture_FrameTimeOut);

	/*************边缘缺陷检测参数***********/
	SettingHelper::GetKeyInt("EDGE_PARAMETER", "BlockLocalizer_THRESHOD", BlockLocalizer::THRESHOD);
	SettingHelper::GetKeyInt("EDGE_PARAMETER", "BlockLocalizer_ContinuePointCount", BlockLocalizer::ContinuePointCount);

	int _BlockEdgeSimilarDetector_Enable = 1;
	SettingHelper::GetKeyInt("EDGE_PARAMETER", "BlockEdgeSimilarDetector_Enable", _BlockEdgeSimilarDetector_Enable);
	BlockEdgeSimilarDetector::Enabled = _BlockEdgeSimilarDetector_Enable;
	SettingHelper::GetKeyDouble("EDGE_PARAMETER", "BlockEdgeSimilarDetector_DIFF_THRESHOLD", BlockEdgeSimilarDetector::DIFF_THRESHOLD);
	SettingHelper::GetKeyDouble("EDGE_PARAMETER", "BlockEdgeSimilarDetector_FAULTS_SPAN", BlockEdgeSimilarDetector::FAULTS_SPAN);
	SettingHelper::GetKeyDouble("EDGE_PARAMETER", "BlockEdgeSimilarDetector_FAULTS_COUNT", BlockEdgeSimilarDetector::FAULTS_COUNT);

	int _BlockEdgeLineDetector_Enabled = 1;
	SettingHelper::GetKeyInt("EDGE_PARAMETER", "BlockEdgeLineDetector_Enable", _BlockEdgeLineDetector_Enabled);
	BlockEdgeLineDetector::Enabled = _BlockEdgeLineDetector_Enabled;
	SettingHelper::GetKeyInt("EDGE_PARAMETER", "BlockEdgeLineDetector_BINARY_THRESHOD", BlockEdgeLineDetector::BINARY_THRESHOD);
	SettingHelper::GetKeyInt("EDGE_PARAMETER", "BlockEdgeLineDetector_LENGTH_THRESHOD", BlockEdgeLineDetector::LENGTH_THRESHOD);
	SettingHelper::GetKeyInt("EDGE_PARAMETER", "BlockEdgeLineDetector_DEEP_THRESHOD", BlockEdgeLineDetector::DEEP_THRESHOD);

	/**************内部参数****************/
	int _Pretreatment_Enable = 1;
	SettingHelper::GetKeyInt("SURFACE", "Pretreatment_Enable", _Pretreatment_Enable);
	this->Pretreatment_Enable = _Pretreatment_Enable;


	/**********************************/
	/*                                */
	/*            面阵参数            */
	/*                                */
	/**********************************/
	int areatmpInt = 0;
	if (SettingHelper::GetKeyInt("AreaCam", "Enable", areatmpInt))
		MainHueScanner::Enabled = areatmpInt;
	areatmpInt = 0;
	if (SettingHelper::GetKeyInt("AreaCam", "SAVE_IMG", areatmpInt))
		MainHueScanner::SAVE_IMG = areatmpInt;
	string para = "para";
	if (SettingHelper::GetKeyString("AreaCam", "MainHSVs", para))
		MainHueScanner::SetStandardHSV(para);
	SettingHelper::GetKeyInt("AreaCam", "WaitTimeMSIn", MainHueScanner::WaitTimeMSIn);
	SettingHelper::GetKeyInt("AreaCam", "WaitTimeMSOut", MainHueScanner::WaitTimeMSOut);



	/**********************************/
	/*                                */
	/*            分级参数            */
	/*                                */
	/**********************************/
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
			w->Pretreatment_Enable = Pretreatment_Enable;


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