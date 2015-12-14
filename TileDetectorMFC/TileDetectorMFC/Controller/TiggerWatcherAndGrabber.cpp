#include "TiggerWatcherAndGrabber.h"


TiggerWatcherAndGrabber::TiggerWatcherAndGrabber(HWND _hwnd, string virtualImg)
{
	hwnd = _hwnd;
	Init(virtualImg);


	gb = new GrabbingBuffer(globle_var::FrameCount, globle_var::Width);

	/*��ȡ����ͼƬ ����*/
	//BlocksDetector bd_Standard = BlocksDetector(cv::imread("A9���۰���_x3��ֵ��.jpg"));
	//bd_Standard.Start();
	//bd_Standard.StartUP_DOWN(BlocksDetector::Up);
	//bd_Standard.StartUP_DOWN(BlocksDetector::Down);
	//bd_Standard.ABCD();
	//Measurer m = Measurer(&bd_Standard, &globle_var::mdi(), 300, 600);		 //����ͼƬ��ʼ��
	//printf_globle("�������\n");
	printf_globle("TiggerWatcherAndGrabber loaded!\n");
}
TiggerWatcherAndGrabber::~TiggerWatcherAndGrabber()
{
	if (gb != NULL)
	{
		delete gb;
		gb = NULL;
	}
	if (e2v != NULL)
	{
		delete e2v;
		e2v = NULL;
	}

	printf_globle("TiggerWatcherAndGrabber unload!\n");
}


void TiggerWatcherAndGrabber::Init(string virtualImg)
{
	if (virtualImg == "")
	{
		printf_globle("TiggerWatcherAndGrabber load! ��ǰģʽΪ��ʵ�����\n");
		Switch2Real();
	}
	else
	{
		printf_globle("TiggerWatcherAndGrabber load! ��ǰģʽΪ���������\n");
		Switch2Virtual(virtualImg);
	}
}
bool TiggerWatcherAndGrabber::Switch2Virtual(string virtualImg)
{
	_virtualImg = virtualImg;
	IsGrabbing = false;
	USING_VIRTUAL_CAMERA = true;
	printf_globle("����Ϊ�����������ģʽ��\n");
	return VirtualCamera::TestCam(virtualImg);
}
bool TiggerWatcherAndGrabber::Switch2Real()
{
	IsGrabbing = false;

	USING_VIRTUAL_CAMERA = false;
	printf_globle("����Ϊ�����&�ɼ�����ģʽ��\n");

	/**********************��ʼ��IO��***********************/
	//if (mc100.init())
	//	printf_globle("��mc100�ɹ���\n");
	//else
	//	printf_globle("��mc100ʧ�ܣ�\n");
	if (pci1761.init())
		printf_globle("��pci1761�ɹ���\n");
	else
		printf_globle("��pci1761ʧ�ܣ�\n");

	/**********************��ʼ���ɼ���*********************/
	if (e2v != NULL)
		delete e2v;
	e2v = new E2VCamera(globle_var::FrameCount, globle_var::Width);
	return true;
}




void TiggerWatcherAndGrabber::StartWatch()
{
	if (!IsWatching && t_watcher == NULL)
	{
		IsWatching = true;
		//std::thread t_watcher(std::mem_fn(&TiggerWatcherAndGrabber::watcherThread), this);
		t_watcher = new thread(std::mem_fn(&TiggerWatcherAndGrabber::watcherThread), this);
		t_watcher->detach();
		printf_globle("StartWatch����ʼ��ش����ź�\n");
	}
	else{
		printf_globle("StartWatch�����ڼ�أ������ظ���ز�����\n");
	}
}
void TiggerWatcherAndGrabber::StartWatchWithCalibration()
{
	IsCalibration = true;
	StartWatch();
}


void TiggerWatcherAndGrabber::StopWatch()
{
	printf_globle("ֹͣ��ش����ź�\n");
	IsWatching = false;
	Sleep(50);
	t_watcher = NULL;
}
bool TiggerWatcherAndGrabber::ManualTigger()
{
	if (IsGrabbing)
	{
		printf_globle("IsGrabbing = 1\n");
		return false;
	}
	if (!IsWatching)
	{
		printf_globle("IsWatching = 0\n");
		return false;
	}

	BeManualTiggered = true;
	return true;
}
void TiggerWatcherAndGrabber::watcherThread()
{
	double t_span = (double)cv::getTickCount();//���ڼ������δ������

	while (IsWatching)
	{
		if (!IsGrabbing && (BeManualTiggered || pci1761.GetRisingEdgeIDI(7) ))
		{
			IsGrabbing = true;

#ifdef OUTPUT_TO_CONSOLE
			double t = (double)cv::getTickCount();
			stringstream ss;
			if (BeManualTiggered)
			{
				printf_globle("�ֶ�����\n");
			}
			else
				printf_globle("����������\n");
#endif

			if (BeManualTiggered)
				BeManualTiggered = false;

			GrabbingIndex += 1;
			if (GrabbingIndex > 1000000) GrabbingIndex = 1;

			t_span = ((double)cv::getTickCount() - t_span) / cv::getTickFrequency();
			ss << endl << endl << "�� " << GrabbingIndex << "�δ��������ϴδ������Ϊ" << t_span << "��" << endl;
			printf_globle(ss.str());
			LogHelper::Log(ss.str());
			ss.str("");
			t_span = (double)cv::getTickCount();//���ڼ������δ������

#ifdef OUTPUT_TO_CONSOLE

			ss << GrabbingIndex << " " << "producer: Start" << endl;
			printf_globle(ss.str());
			ss.str("");

			t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
			ss << GrabbingIndex << " " << "twag Inint(ms):" << t << endl;
			printf_globle(ss.str());
			LogHelper::Log(ss.str());
			ss.str("");

#endif

			if (globle_var::TiggerWaitTimeMS > 0)
			{
				//�����󣬵ȴ�ש������������
				Sleep(globle_var::TiggerWaitTimeMS);

#ifdef OUTPUT_TO_CONSOLE
				t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
				stringstream ss;
				ss << GrabbingIndex << " " << "Sleep real(ms):" << t << endl;
				printf_globle(ss.str());
#endif
			}


			std::thread t_3in1(std::mem_fn(&TiggerWatcherAndGrabber::threeInOne), this);
			std::thread t_capture(std::mem_fn(&TiggerWatcherAndGrabber::capture), this);
			t_3in1.join();
			t_capture.join();

			//capture();
			//threeInOne();

			//ͼƬ��ȡ��ɣ����Ƶ�ȫ�ֱ�������ɾ��s
			//OriginalImage.release();
			//Image.release();
			OriginalImage = gb->OriginalImage.clone();
			Image = gb->Image.clone();



			IsGrabbing = false;

			if (!IsCalibration)
			{
				if (hwnd != NULL)
				{
					PostMessage(hwnd, WM_USER + 100, 0, 0);
				}
				//��������߹�������
			}
			else//���Ǳ궨��������ֹͣ���
			{
				IsCalibration = false;
				StopWatch();
				PostMessage(hwnd, WM_USER + 102, 0, 0);//���ꡢ�ɼ������������
			}
		}
		else
		{
			Sleep(5);
		}
	}
}
void TiggerWatcherAndGrabber::capture()
{

#ifdef OUTPUT_TO_CONSOLE
	double t = (double)cv::getTickCount();
	printf_globle("  capture(); START \r\n");
#endif

	gb->Start();
	if (!USING_VIRTUAL_CAMERA)
	{
		e2v->Capture(gb);
	}
	else
	{
		VirtualCamera *vc = new VirtualCamera(gb, globle_var::FrameCount, globle_var::Width, _virtualImg);
		vc->Capture();
		delete vc;
	}


#ifdef OUTPUT_TO_CONSOLE
	//������ʱ
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	stringstream ss;
	ss << GrabbingIndex << " " << "capture();  END" << endl << globle_var::Width << "x" << globle_var::FrameCount << "��" << t << "  End  at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
#endif
}
//ͼƬ�ϳ�
void TiggerWatcherAndGrabber::threeInOne()
{
#ifdef OUTPUT_TO_CONSOLE
	double t = (double)cv::getTickCount();
#endif


	for (gb->ReadIndex = 0; gb->ReadIndex < globle_var::FrameCount; gb->ReadIndex++)
	{
		//��δд�뻺�棬�ȴ�
		while (gb->ReadIndex + gb->NinOne >= gb->WriteIndex && gb->WriteIndex < globle_var::FrameCount)
		{
			Sleep(10);
		}
		gb->ThreeInOne(gb->ReadIndex);
	}



#ifdef OUTPUT_TO_CONSOLE
	stringstream ss;
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	ss << GrabbingIndex << " " << "ThreeInOne��" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");
#endif
}