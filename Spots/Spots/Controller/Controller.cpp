#ifndef AFX_DATA
#	include <afxwin.h>
#endif
#include "Controller.h"
#include <Class/Setting/SettingHelper.h>
#include <Class\Helper\StringHelper.h>
#include <shlwapi.h>
#include <Class/Debug/MFCConsole.h>
#pragma comment(lib,"Shlwapi.lib") //�ļ�Ŀ¼lib ���û�����У������link����

void Controller::init(){

	release();

	//����ͼ�񻺴�
	p_e2vbuffer = new E2VBuffer(4096, true);
	p_imgscanner = new ImgScanner(p_e2vbuffer);
	bool e2vInitFlag = true;
	//��ʼ��e2v_EV71YC1CCL4005BA0ʧ��
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



	// ���ò�����ʼ��
	string db_path;
	bool accConnFlag = false;
	if (SettingHelper::GetKeyString("DATABASE", "ACCDB_PATH", db_path))//��ȡ���ݿ�����·��
		accConnFlag = Statistics::InitDate(db_path);
	else
	{
		accConnFlag = Statistics::InitDate("src//..//��שȱ�ݼ�����ݿ�.mdb");
		SettingHelper::AddKey("DATABASE", "ACCDB_PATH", "src//..//��שȱ�ݼ�����ݿ�.mdb");
	}

	// ͳ�����ݳ�ʼ��
	if (accConnFlag)
		spotsMainView->UpdateStatistics();
	else
		AfxMessageBox(L"�޷����ӵ����ݿ⣡");

	// ��ʼ�������߳�
	if (e2vInitFlag && pci1761InitFlag)
	{
		isRealModel = 1;
		//��ʼ������
		worker1 = new Worker(p_e2vbuffer);
		worker1->BlockLocalizer_THRESHOD = BlockLocalizer_THRESHOD;
		worker1->BlockLocalizer_ContinuePointCount = BlockLocalizer_ContinuePointCount;
		worker1->BlockEdgeDetector_DIFF_THRESHOLD = BlockEdgeDetector_DIFF_THRESHOLD;
		worker1->BlockEdgeDetector_FAULTS_SPAN = BlockEdgeDetector_FAULTS_SPAN;
		worker1->BlockEdgeDetector_FAULTS_COUNT = BlockEdgeDetector_FAULTS_COUNT;

		worker2 = new Worker(p_e2vbuffer);
		worker1->BlockLocalizer_THRESHOD = BlockLocalizer_THRESHOD;
		worker1->BlockLocalizer_ContinuePointCount = BlockLocalizer_ContinuePointCount;
		worker1->BlockEdgeDetector_DIFF_THRESHOLD = BlockEdgeDetector_DIFF_THRESHOLD;
		worker1->BlockEdgeDetector_FAULTS_SPAN = BlockEdgeDetector_FAULTS_SPAN;
		worker1->BlockEdgeDetector_FAULTS_COUNT = BlockEdgeDetector_FAULTS_COUNT;

		worker1->P_Controller = this;
		worker2->P_Controller = this;

		StartWatch();

		spotsMainView->SwitchModel2Virtual(false);
		MFCConsole::Output("���л�����ʵ���ģʽ��\r\n");
	}
	else
	{
		isRealModel = 0;
		if (!e2vInitFlag && !pci1761InitFlag)
			AfxMessageBox(L"�������&pci1761��ʼ��ʧ�ܣ�");
		else if (!pci1761InitFlag)
			AfxMessageBox(L"pci1761��ʼ��ʧ�ܣ�");
		else if (!e2vInitFlag)
			AfxMessageBox(L"���������ʼ��ʧ�ܣ�");

		//�����������
		cv::Mat virtualImg;
		worker1 = new Worker(NULL);
		worker1->image = virtualImg;
		worker1->P_Controller = this;

		spotsMainView->SwitchModel2Virtual(true);

		MFCConsole::Output("���л����������ģʽ��\r\n");
	}

	//��ʼ��UI
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
		//��ʼ��ش���
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

	if (SAVE_IMG)
	{
		std::thread t_tiggerThread(std::mem_fn(&Controller::imageSave), this, img);
		t_tiggerThread.detach();
		//imageSave(img);
	}
}


/*********************  ����ģʽ����  ***********************/

void Controller::VirtualSelectImg(cv::Mat image)
{
	if (image.cols == 0)
		MessageBox(0, L"ͼƬ��ȡʧ�ܣ�", L"����", 0);
	else
	{
		worker1->image = image;
	}
}
void Controller::VirtualWorkerStart()
{
	if (worker1->image.cols == 0)
	{
		MessageBox(0, L"���ȼ��������Ƭ��", L"����", 0);
		return;
	}
	if (worker1->MyStatus == Worker::Done || worker1->MyStatus == Worker::Free)
	{
		worker1->StartWork();
	}
	else
	{
		MessageBox(0, L"��һ�ִ�����δ������", L"����", 0);
	}
}
/*********************����ģʽ��������***********************/

void Controller::ResetParameter()
{
	if (worker1 != NULL)
	{
		worker1->BlockLocalizer_THRESHOD = BlockLocalizer_THRESHOD;
		worker1->BlockLocalizer_ContinuePointCount = BlockLocalizer_ContinuePointCount;
		worker1->BlockEdgeDetector_DIFF_THRESHOLD = BlockEdgeDetector_DIFF_THRESHOLD;
		worker1->BlockEdgeDetector_FAULTS_SPAN = BlockEdgeDetector_FAULTS_SPAN;
		worker1->BlockEdgeDetector_FAULTS_COUNT = BlockEdgeDetector_FAULTS_COUNT;
	}
	if (worker2 != NULL)
	{
		worker2->BlockLocalizer_THRESHOD = BlockLocalizer_THRESHOD;
		worker2->BlockLocalizer_ContinuePointCount = BlockLocalizer_ContinuePointCount;
		worker2->BlockEdgeDetector_DIFF_THRESHOLD = BlockEdgeDetector_DIFF_THRESHOLD;
		worker2->BlockEdgeDetector_FAULTS_SPAN = BlockEdgeDetector_FAULTS_SPAN;
		worker2->BlockEdgeDetector_FAULTS_COUNT = BlockEdgeDetector_FAULTS_COUNT;
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
		if (pci1761.GetRisingEdgeIDI(7))//�����ؿ�ʼ��ͼ
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
		else if (pci1761.GetTrailingEdgeIDI(7))//�½��ؽ�����ͼ
		{
			t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
			stringstream ss;
			ss << "Worker Stop : Timespan:" << t << "ms" << endl;
			MFCConsole::Output(ss.str());

			// ��ǹ���ֹͣ��ͼ
			lastestWorker->GetPhotoOn = false;
		}
		else
			this_thread::sleep_for(chrono::microseconds(1));
	}
}

void Controller::imageSave(cv::Mat img)
{
	image_write_lock.lock();

	//�ж��ļ����Ƿ����
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