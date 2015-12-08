#include "TiggerWatcherAndGrabber.h"


TiggerWatcherAndGrabber::TiggerWatcherAndGrabber(HWND _hwnd, string virtualImg)
{
	hwnd = _hwnd;
	Init(virtualImg);

	/*��ȡ����ͼƬ ����*/
	//BlocksDetector bd_Standard = BlocksDetector(cv::imread("A9���۰���_x3��ֵ��.jpg"));
	//bd_Standard.Start();
	//bd_Standard.StartUP_DOWN(BlocksDetector::Up);
	//bd_Standard.StartUP_DOWN(BlocksDetector::Down);
	//bd_Standard.ABCD();
	//Measurer m = Measurer(&bd_Standard, &globle_var::mdi(), 300, 600);		 //����ͼƬ��ʼ��
	//printf_globle("�������\n");
}
TiggerWatcherAndGrabber::~TiggerWatcherAndGrabber()
{
	//if (!USING_VIRTUAL_CAMERA)
	//{
	//	MicroDisplayInit::Release(globle_var::_mdi);
	//}
	//printf_globle("TiggerWatcherAndGrabber unload!\n");
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

	/**********************��ʼ���ɼ���***********************/
	if (mc100_open(0) >= 0)
	{
		printf_globle("�򿪲ɼ���0�ɹ���\n");
		mc100_write_port(0, MC100_PORTA, 0x00);
	}
	else
	{
		printf_globle("�򿪲ɼ���0ʧ�ܣ�\n");
	}
	MicroDisplay::TestCam();
	return true;
}




void TiggerWatcherAndGrabber::StartWatch()
{
	if (!IsWatching)
	{
		IsWatching = true;
		std::thread t_watcher(std::mem_fn(&TiggerWatcherAndGrabber::watcherThread), this);
		t_watcher.detach();
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
}
bool TiggerWatcherAndGrabber::ManualTigger()
{
	if (IsGrabbing || !IsWatching)
	{
		printf_globle("IsGrabbing = 1\n");
		return false;
	}
	if (IsGrabbing || !IsWatching)
	{
		printf_globle("IsWatching = 0\n");
		return false;
	}

	BeManualTiggered = true;
	return true;
}
void TiggerWatcherAndGrabber::watcherThread()
{
	while (IsWatching)
	{
		if (!IsGrabbing && (BeManualTiggered || BeAutoTiggered()))
		{
			if (BeManualTiggered)
			{
				printf_globle("�ֶ�����\n");
				BeManualTiggered = false;
			}
			else
				printf_globle("����������\n");

			GrabbingIndex += 1;

			if (gb != NULL)
				delete gb;
			gb = new GrabbingBuffer(globle_var::FrameCount, globle_var::Width);

			if (GrabbingIndex > 10000000) GrabbingIndex = 1;

			stringstream ss;
			ss << GrabbingIndex << " " << "producer: Start  at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
			printf_globle(ss.str());
			ss.str("");

			IsGrabbing = true;


			double t = (double)cv::getTickCount();


			ss << GrabbingIndex << " " << "TiggerWaitTimeMS: " << globle_var::TiggerWaitTimeMS << endl;
			printf_globle(ss.str());
			ss.str("");
			//�����󣬵ȴ�ש������������
			Sleep(globle_var::TiggerWaitTimeMS);

			//std::thread t_3in1(std::mem_fn(&TiggerWatcherAndGrabber::threeInOne), this);
			//std::thread t_capture(std::mem_fn(&TiggerWatcherAndGrabber::capture), this);
			//t_3in1.join();
			//t_capture.join();

			capture(); 
			threeInOne();

			//ͼƬ��ȡ��ɣ����Ƶ�ȫ�ֱ�������ɾ��s
			//OriginalImage.release();
			//Image.release();
			OriginalImage = gb->OriginalImage.clone();
			Image = gb->Image.clone();

			delete gb;
			gb = NULL;

			if (!IsCalibration)
			{
				if (hwnd != NULL)
				{
					PostMessage(hwnd, WM_USER + 100, 0, 0);
				}
				//��������߹�������
				IsGrabbing = false;
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
			Sleep(10);
		}
	}
}
void TiggerWatcherAndGrabber::capture()
{
	double t = (double)cv::getTickCount();
	printf_globle("capture(); START \r\n");
	if (!USING_VIRTUAL_CAMERA)
	{
		MicroDisplay *md = new MicroDisplay(gb, globle_var::FrameCount, globle_var::Width);
		md->Capture();
		delete md;
	}
	else
	{
		VirtualCamera *vc = new VirtualCamera(gb, globle_var::FrameCount, globle_var::Width, _virtualImg);
		vc->Capture();
		delete vc;
	}
	//������ʱ
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	stringstream ss;
	ss << GrabbingIndex << " " << "capture();  END"<<endl << globle_var::Width << "x" << globle_var::FrameCount << "��" << t << "  End  at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
}
//ͼƬ�ϳ�
void TiggerWatcherAndGrabber::threeInOne()
{
	stringstream ss;
	ss << GrabbingIndex << " " << "ThreeInOne��Start at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");

	double t = (double)cv::getTickCount();
	for (gb->ReadIndex = 0; gb->ReadIndex < globle_var::FrameCount; gb->ReadIndex++)
	{
		//��δд�뻺�棬�ȴ�
		while (gb->ReadIndex + gb->NinOne >= gb->WriteIndex && gb->WriteIndex < globle_var::FrameCount)
		{
			Sleep(10);
		}
		gb->ThreeInOne(gb->ReadIndex);
	}
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	ss << GrabbingIndex << " " << "ThreeInOne��" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");
}