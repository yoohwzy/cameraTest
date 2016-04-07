#ifndef AFX_DATA
#	include <afxwin.h>
#endif
#include "Controller.h"
#include <Class/Setting/SettingHelper.h>
#include <Class\Helper\StringHelper.h>
#include <shlwapi.h>
#include <Class/Debug/MFCConsole.h>
#pragma comment(lib,"Shlwapi.lib") //文件目录lib 如果没有这行，会出现link错误

void Controller::init(){

	release();

	//开启图像缓存
	p_e2vbuffer = new E2VBuffer(4096, true);
	p_imgscanner = new ImgScanner(p_e2vbuffer);
	bool e2vInitFlag = true;
	//初始化e2v_EV71YC1CCL4005BA0失败
	if (!p_imgscanner->StartFlag)
	{
		if (p_imgscanner != NULL)
		{
			delete p_imgscanner;
			p_imgscanner = NULL;
		}
		if (p_e2vbuffer != NULL)
		{
			delete p_e2vbuffer;
			p_e2vbuffer = NULL;
		}
		e2vInitFlag = false;
	}

	bool pci1761InitFlag = true;
	if (!pci1761.init())
	{
		pci1761InitFlag = false;
	}



	// 配置数据库初始化
	if (1 == 1)
	{
		int accEnable = 0;
		SettingHelper::GetKeyInt("DATABASE", "ACCDB_ENABLE", accEnable);
		if (accEnable != 0)
		{
			string db_path;
			bool accConnFlag = false;
			if (SettingHelper::GetKeyString("DATABASE", "ACCDB_PATH", db_path))//读取数据库所在路径
				accConnFlag = Statistics::InitDate(db_path);
			else
			{
				accConnFlag = Statistics::InitDate("src//..//瓷砖缺陷检测数据库.mdb");
				SettingHelper::AddKey("DATABASE", "ACCDB_PATH", "src//..//瓷砖缺陷检测数据库.mdb");
			}
			// 统计数据初始化
			if (accConnFlag)
				spotsMainView->UpdateStatistics();
			else
				AfxMessageBox(L"无法连接到数据库！");
		}
	}
	// 初始化工作线程
	if (e2vInitFlag && pci1761InitFlag)
	{
		isRealModel = 1;
		//初始化工人
		worker1 = new Worker(p_e2vbuffer);
		worker2 = new Worker(p_e2vbuffer);

		worker1->P_Controller = this;
		worker2->P_Controller = this;

		StartWatch();

		spotsMainView->SwitchModel2Virtual(false);
		MFCConsole::Output("已切换到真实相机模式。\r\n");
	}
	else
	{
		isRealModel = 0;
		if (!e2vInitFlag && !pci1761InitFlag)
			AfxMessageBox(L"线阵相机&pci1761初始化失败！");
		else if (!pci1761InitFlag)
			AfxMessageBox(L"pci1761初始化失败！");
		else if (!e2vInitFlag)
			AfxMessageBox(L"线阵相机初始化失败！");

		//开启虚拟相机
		cv::Mat virtualImg;
		worker1 = new Worker(NULL);
		worker1->image = virtualImg;
		worker1->P_Controller = this;

		spotsMainView->SwitchModel2Virtual(true);

		MFCConsole::Output("已切换到虚拟相机模式。\r\n");
	}

	//读取参数配置
	int si = 0;
	SettingHelper::GetKeyInt("SYS", "SAVE_IMG", si);
	SAVE_IMG = si;

	SettingHelper::GetKeyDouble("EDGE_PARAMETER", "BlockLocalizer_THRESHOD", this->BlockLocalizer_THRESHOD);
	SettingHelper::GetKeyDouble("EDGE_PARAMETER", "BlockLocalizer_ContinuePointCount", this->BlockLocalizer_ContinuePointCount);
	SettingHelper::GetKeyDouble("EDGE_PARAMETER", "BlockEdgeDetector_DIFF_THRESHOLD", this->BlockEdgeDetector_DIFF_THRESHOLD);
	SettingHelper::GetKeyDouble("EDGE_PARAMETER", "BlockEdgeDetector_FAULTS_SPAN", this->BlockEdgeDetector_FAULTS_SPAN);
	SettingHelper::GetKeyDouble("EDGE_PARAMETER", "BlockEdgeDetector_FAULTS_COUNT", this->BlockEdgeDetector_FAULTS_COUNT);
	//设置工人算法参数
	ResetParameter();


	//初始化UI
	cv::Mat white(2, 2, CV_8U, cv::Scalar(255));
	logImg.InitDrawingBoard();
	spotsMainView->ShowBigImg(white);
	spotsMainView->ShowLogImg(logImg.DrawingBoard);
}


void Controller::StartWatch()
{
	if (isRealModel)
	{
		watcher_lock.lock();

		exitFlag = false;
		//开始监控触发
		std::thread t_tiggerThread(std::mem_fn(&Controller::triggerWatcher), this);
		t_tiggerThread.detach();

		watcher_lock.unlock();
	}
}

void Controller::StopWatch()
{
	if (isRealModel)
	{
		watcher_lock.lock();

		exitFlag = true;

		watcher_lock.unlock();
	}
}

void Controller::ImageGetCallBack(cv::Mat img)
{
	//异步保存底片
	//int index = p_twag->GrabbingIndex;
	//cv::Mat img = p_twag->OriginalImage.clone();
	//if (IsDlgButtonChecked(IDC_CB_SAVE_IMG) == BST_CHECKED)
	//{
	//	thread t_saveimg = thread([index,img]{
	//		stringstream ss;
	//		ss << "samples/" << index << "_o原图.jpg";
	//		cv::imwrite(ss.str(), img);
	//	});
	//	t_saveimg.detach();
	//}

	if (SAVE_IMG)
	{
		std::thread t_tiggerThread(std::mem_fn(&Controller::imageSave), this, img);
		t_tiggerThread.detach();
		//imageSave(img);
	}
}


/*********************  虚拟模式方法  ***********************/

void Controller::VirtualSelectImg(cv::Mat image)
{
	if (image.cols == 0)
		MessageBox(0, L"图片读取失败！", L"错误", 0);
	else
	{
		worker1->image = image;
	}
}
void Controller::VirtualWorkerStart()
{
	if (worker1->image.cols == 0)
	{
		MessageBox(0, L"请先加载虚拟底片！", L"错误", 0);
		return;
	}
	if (worker1->MyStatus == Worker::Done || worker1->MyStatus == Worker::Free)
	{
		worker1->StartWork();
	}
	else
	{
		MessageBox(0, L"上一轮处理尚未结束！", L"警告", 0);
	}
}
/*********************虚拟模式方法结束***********************/

void Controller::ResetParameter()
{
	if (worker1 != NULL)
	{
		worker1->BlockLocalizer_THRESHOD = BlockLocalizer_THRESHOD;
		worker1->BlockLocalizer_ContinuePointCount = BlockLocalizer_ContinuePointCount;
		worker1->BlockEdgeDetector_DIFF_THRESHOLD = BlockEdgeDetector_DIFF_THRESHOLD;
		worker1->BlockEdgeDetector_FAULTS_SPAN = BlockEdgeDetector_FAULTS_SPAN;
		worker1->BlockEdgeDetector_FAULTS_COUNT = BlockEdgeDetector_FAULTS_COUNT;

		worker1->WaitTimeMSIn = Worker_WaitTimeMSIn;
		worker1->WaitTimeMSOut = Worker_WaitTimeMSOut;
		worker1->FrameTimeOut = Worker_FrameTimeOut;
	}
	if (worker2 != NULL)
	{
		worker2->BlockLocalizer_THRESHOD = BlockLocalizer_THRESHOD;
		worker2->BlockLocalizer_ContinuePointCount = BlockLocalizer_ContinuePointCount;
		worker2->BlockEdgeDetector_DIFF_THRESHOLD = BlockEdgeDetector_DIFF_THRESHOLD;
		worker2->BlockEdgeDetector_FAULTS_SPAN = BlockEdgeDetector_FAULTS_SPAN;
		worker2->BlockEdgeDetector_FAULTS_COUNT = BlockEdgeDetector_FAULTS_COUNT;

		worker2->WaitTimeMSIn = Worker_WaitTimeMSIn;
		worker2->WaitTimeMSOut = Worker_WaitTimeMSOut;
		worker2->FrameTimeOut = Worker_FrameTimeOut;
	}
}




//private:

void Controller::release()
{
	p_e2vbuffer = NULL;
	if (worker1 != NULL)
	{
		delete worker1;
		worker1 = NULL;
	}
	if (worker2 != NULL)
	{
		delete worker2;
		worker2 = NULL;
	}

	if (p_imgscanner != NULL)
	{
		delete p_imgscanner;
		p_imgscanner = NULL;
	}
	if (p_e2vbuffer != NULL)
	{
		delete p_e2vbuffer;
		p_e2vbuffer = NULL;
	}
}

void Controller::triggerWatcher()
{
	short int tiggerindex = 0;
	double t = cv::getTickCount();
	while (!exitFlag)
	{
		if (pci1761.GetRisingEdgeIDI(7))//上升沿开始采图
		{
			t = cv::getTickCount();

			tiggerindex++;
			if (tiggerindex >= 100)
				tiggerindex = 0;

			if (tiggerindex % 2 == 1)
			{
				MFCConsole::Output("\r\n\r\n-------------------------Worker1 Start Work\r\n");
				worker1->StartWork();
				lastestWorker = worker1;
			}
			else
			{
				MFCConsole::Output("\r\n\r\n-------------------------Worker2 Start Work\r\n");

				worker2->StartWork();
				lastestWorker = worker2;
			}
			Sleep(100);
		}
		else if (pci1761.GetTrailingEdgeIDI(7))//下降沿结束采图
		{
			t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
			stringstream ss;
			ss << "Worker Stop : Timespan:" << t << "ms" << endl;
			MFCConsole::Output(ss.str());

			// 标记工人停止采图
			lastestWorker->GetPhotoOn = false;
		}
		else
			this_thread::sleep_for(chrono::microseconds(1));
	}
}

void Controller::imageSave(cv::Mat img)
{
	image_write_lock.lock();

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

	cv::Mat img2 = img.clone();
	stringstream ss;

	cv::imwrite(StringHelper::CString2string(imageName), img2);

	image_write_lock.unlock();
}