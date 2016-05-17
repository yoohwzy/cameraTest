#ifndef AFX_DATA
#	include <afxwin.h>
#endif
#include "ControllerCycleBuffer.h"
#include <Class/Setting/SettingHelper.h>
#include <Class\Helper\StringHelper.h>
#include <shlwapi.h>
#include <Class/Debug/MFCConsole.h>
#pragma comment(lib,"Shlwapi.lib") //文件目录lib 如果没有这行，会出现link错误

void ControllerCycleBuffer::Init(){

	Release();
	bool e2vInitFlag = true;
	bool pci1761InitFlag = true;

	stringstream ss;

	//ss.str("");
	//ss << "IsRealModel = " << IsRealModel << endl;
	//MFCConsole::Output(ss.str());

	if (IsRealModel)
	{
		int VirtualCamEnable = 1;
		if (SettingHelper::GetKeyInt("E2V", "Virtual_Cam_Enable", VirtualCamEnable))
			IsRealModel = VirtualCamEnable == 0;
	}

	//ss.str("");
	//ss << "IsRealModel = " << IsRealModel << endl;
	//MFCConsole::Output(ss.str());

	if (IsRealModel)
	{
		//初始化E2V相机
		int Cam_FrameCount_PerSecond = 5000;
		SettingHelper::GetKeyInt("E2V", "Cam_FrameCount_PerSecond", Cam_FrameCount_PerSecond);
		int COLOR_TYPE_IS_GRAY = 1;
		SettingHelper::GetKeyInt("E2V", "COLOR_TYPE_IS_GRAY", COLOR_TYPE_IS_GRAY);
		if (COLOR_TYPE_IS_GRAY != 0)
			p_e2v = new	E2VCameraCycleBuffer(4096, CV_8U, 1000000 / Cam_FrameCount_PerSecond, 0, PORT_A);
		else
			p_e2v = new	E2VCameraCycleBuffer(4096, CV_8UC3, 1000000 / Cam_FrameCount_PerSecond, 0, PORT_A);


		if (!p_e2v->HasBeenInited)
		{
			//若初始化e2v_EV71YC1CCL4005BA0失败
			//则删除已实例化的对象，切换为虚拟相机模式
			if (p_e2v != NULL)
			{
				delete p_e2v;
				p_e2v = NULL;
			}
			e2vInitFlag = false;
		}

		
		if (!pci1761.init())
		{
			pci1761InitFlag = false;
		}
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
	if (e2vInitFlag && pci1761InitFlag && IsRealModel)
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
			if (!e2vInitFlag && !pci1761InitFlag)
				AfxMessageBox(L"线阵相机&pci1761初始化失败！已切换到虚拟相机模式。");
			else if (!pci1761InitFlag)
				AfxMessageBox(L"pci1761初始化失败！已切换到虚拟相机模式。");
			else if (!e2vInitFlag)
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
	//LoadParameterFromIni();
	ResetParameter();


	//初始化UI
	cv::Mat white(2, 2, CV_8U, cv::Scalar(255));
	logImg.InitDrawingBoard();
	spotsMainView->ShowBigImg(white);
	spotsMainView->ShowLogImg(logImg.DrawingBoard);
}
void ControllerCycleBuffer::Release()
{
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

	if (p_e2v != NULL)
	{
		delete p_e2v;
		p_e2v = NULL;
	}
}

void ControllerCycleBuffer::TiggerStartWatch()
{
	if (IsRealModel)
	{
		watcher_lock.lock();

		ExitFlag = false;
		//开始监控触发
		std::thread t_tiggerThread(std::mem_fn(&ControllerCycleBuffer::triggerWatcherThread), this);
		auto tn = t_tiggerThread.native_handle();
		SetThreadPriority(tn, THREAD_PRIORITY_ABOVE_NORMAL);
		t_tiggerThread.detach();

		watcher_lock.unlock();
	}
}

void ControllerCycleBuffer::TiggerStopWatch()
{
	if (IsRealModel)
	{
		watcher_lock.lock();

		ExitFlag = true;

		watcher_lock.unlock();
	}
}

//private:



void ControllerCycleBuffer::triggerWatcherThread()
{
	double t = cv::getTickCount();
	double risingSpan = cv::getTickCount();
	while (!ExitFlag)
	{
		if (PauseFlag)//暂停标志
		{
			Sleep(50);
			continue;
		}
		//if (pci1761.GetRisingEdgeIDI(7))//上升沿开始采图
		if (pci1761.GetTrailingEdgeIDI(7))//开始采图
		{
			if (IsGrabbing)
			{
				MFCConsole::Output("IsGrabbing = 1 触发后不响应\r\n");
				continue;
			}

			IsGrabbing = true;
			t = cv::getTickCount();
			risingSpan = (cv::getTickCount() - risingSpan) * 1000 / cv::getTickFrequency();
			stringstream ss;
			ss << "\r\n\r\n与上次触发间隔Timespan:" << risingSpan << "ms" << endl;
			risingSpan = cv::getTickCount();

			if (!IsGrabbing2)
			{
				std::thread t_run(std::mem_fn(&ControllerCycleBuffer::captureAndProcessThread), this);
				//auto tn = t_run.native_handle();
				//SetThreadPriority(tn, THREAD_PRIORITY_ABOVE_NORMAL);
				t_run.detach();
			}
			else
			{
				stringstream ss;
				ss << "        触发后不响应  IsGrabbing2 = 1 ,still running capture();" << endl;
				MFCConsole::Output(ss.str());
			}
			//PauseFlag == 1;
			IsGrabbing = false;
		}
		Sleep(10);
	}
}



void ControllerCycleBuffer::captureAndProcessThread()
{
	IsGrabbing2 = true;

	if (MFCConsole::IsOpened)
	{
		stringstream ss;
		ss << "                                         capture " << endl;
		MFCConsole::Output(ss.str());
	}
	//取图
	int length = 0;//采图长度

	//第一部分，确定起始行并等待相机写到该行
	
	int startFrame = p_e2v->P_CycleBuffer->GetWriteIndex();
	int firstFrame = startFrame;//记录触发时所写行号
	//加入延时时间
	//触发后等待一段时间，砖走到拍摄区域后再获取图像
	int addFrameCountIn = Worker_WaitTimeMSIn * 1000 / p_e2v->FrameTimeUS;
	frameIndexAdd(startFrame, addFrameCountIn);
	//第一个for循环是为了处理循环指针越界的情况，如startFrame = 19500 + 600 = 100，此时GetWriteIndex = 19800 > 100 需要等待GetWriteIndex越界
	for (size_t i = 0; i < 10; i++)
		if (p_e2v->P_CycleBuffer->GetWriteIndex() > startFrame)
			Sleep(10);
		else
			break;
	//第二个循环 等待相机拍摄到起始行（这样写的原因是加入了延时时间，可能存在相机还未拍摄到startFrame的情况）
	int nowFrame = p_e2v->P_CycleBuffer->GetWriteIndex();
	while (
		!(
		(startFrame >= firstFrame && (nowFrame > startFrame || nowFrame < firstFrame)) ||//第一种情况，startFrame未换页，则有两种可能，1.nowFrame也没换页则要求nowFrame>startFrame，2.或nowFrame换页了(即nowFrame<firstFrame)则立即判定为通过。
		(startFrame < firstFrame && nowFrame > startFrame && nowFrame < firstFrame)//第二种情况，startFrame换页了，则判定nowFrame > startFrame即可
		)
		)
	{
		Sleep(10);
		nowFrame = p_e2v->P_CycleBuffer->GetWriteIndex();
	}
#ifdef Controller_DEBUG
	if (1 == 1)
	{
		stringstream ss;
		ss << "capture _ startFrame =" << firstFrame << " + " << addFrameCountIn << " = " << startFrame << endl;
		ss << "真正起始行：" << nowFrame << endl;
		MFCConsole::Output(ss.str());
	}
#endif
	IsGrabbing2 = false;


	//第二部分，确定结束行并等待相机写到该行

	int addFrameCountOut = Worker_WaitTimeMSOut * 1000 / p_e2v->FrameTimeUS;//获得下降沿后，等待瓷砖离开拍摄区域帧长
	if (length == 0)//采集固定长度
		length = Worker_FrameTimeOut * 1000 / p_e2v->FrameTimeUS;

	int endFrameAbso = startFrame;
	frameIndexAdd(endFrameAbso, length);//绝对最后一帧，到了这一帧不管触发器是否有下降沿都停止采集。while循环break。
	int endFrame = endFrameAbso;
#ifdef Controller_DEBUG
	if (1 == 1)
	{
		stringstream ss;
		ss << "capture _ endFrameAbso = " << startFrame << " + " << length << "=" << endFrameAbso << endl;
		MFCConsole::Output(ss.str());
	}
#endif

	bool overtimeflag = false;//超时标记
	//wait capture end
	Sleep(100);
	//循环等待下降沿或采图超时
	//while (pci1761.GetTrailingEdgeIDI(7))
	//while (pci1761.GetRisingEdgeIDI(7))
	while (true)
	{
		int now = p_e2v->P_CycleBuffer->GetWriteIndex();
		//因超时而退出
		if (
			(endFrameAbso >= startFrame && now >= endFrameAbso) ||
			(endFrameAbso < startFrame && now >= endFrameAbso && now < startFrame)
			)
		{
#ifdef Controller_DEBUG
			if (1 == 1)
			{
				stringstream ss;
				ss << "capture _ get endFrameAbso,endFrameAbso =" << endFrameAbso << endl;
				MFCConsole::Output(ss.str());
			}
#endif
			overtimeflag = true;//一直没有获得下降沿信号，标记超时
			break;
		}
		Sleep(2);
	}
	if (overtimeflag == false)//如果是获得下降沿结束，则进一步计算结束时的行号
	{
		//进入此处说明是外部将GetPhotoOn置0，即触发器下降沿信号结束了采集
		//此处重新计算endFrame
		endFrame = p_e2v->P_CycleBuffer->GetWriteIndex();
		frameIndexAdd(endFrame, addFrameCountOut);
#ifdef Controller_DEBUG
		if (1 == 1)
		{
			stringstream ss;
			ss << "capture _ endFrame = ???? + " << addFrameCountOut << " = " << endFrame << endl;
			MFCConsole::Output(ss.str());
		}
#endif


		//TODO::判断每一帧是否是结束。加一个while循环，判断每一帧是否为全黑，全黑说明采集应该结束了。

		while (true)
		{
			//判断是否读够那么多行
			int now = p_e2v->P_CycleBuffer->GetWriteIndex();
			//计数完成而退出
			if (
				endFrame > startFrame && now >= endFrame ||
				(endFrame < startFrame && now >= endFrame && now < startFrame)
				)
			{
				break;
			}
			Sleep(2);
		}
	}

	//第三部分，取图
	cv::Mat image = p_e2v->P_CycleBuffer->GetImage(startFrame, endFrame);
	

	//第四部分 分配工作
	workerindex++;
	if (workerindex > 1) workerindex = 0;
	//分配任务给工人
	stringstream ss;
	if (workerindex == 0)
	{
		if (worker1->MyStatus == Worker::Free)
		{
			worker1->SN = index++;
			
			worker1->image = image;
			ss << "-------------------------Worker1 Start Work\r\n" << worker1->SN << endl;
			MFCConsole::Output(ss.str());
			worker1->StartWork();
		}
		else
		{
			MFCConsole::Output(ss.str());
			MFCConsole::Output("!!!!!!!!!!!!!!!!!!!!!!!!!!Worker1 Is busy\r\n");
		}

	}
	else if (workerindex == 1)
	{
		if (worker2->MyStatus == Worker::Free)
		{
			worker2->SN = index++;
			worker2->image = image;
			ss << "-------------------------Worker2 Start Work\r\n" << worker2->SN << endl;
			MFCConsole::Output(ss.str());
			worker2->StartWork();
		}
		else
		{
			MFCConsole::Output(ss.str());
			MFCConsole::Output("!!!!!!!!!!!!!!!!!!!!!!!!!!Worker2 Is busy\r\n");
		}
	}
}