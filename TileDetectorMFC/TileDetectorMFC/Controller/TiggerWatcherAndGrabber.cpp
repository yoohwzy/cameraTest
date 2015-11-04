#include "TiggerWatcherAndGrabber.h"


TiggerWatcherAndGrabber::TiggerWatcherAndGrabber(HWND _hwnd, string virtualImg)
{

	hwnd = _hwnd;

	//���ò���
	globle_var::initMDI();
	globle_var::initBufferStorage();

	/*��ȡ����ͼƬ ����*/
	//BlocksDetector bd_Standard = BlocksDetector(cv::imread("A9���۰���_x3��ֵ��.jpg"));
	//bd_Standard.Start();
	//bd_Standard.StartUP_DOWN(BlocksDetector::Up);
	//bd_Standard.StartUP_DOWN(BlocksDetector::Down);
	//bd_Standard.ABCD();
	//Measurer m = Measurer(&bd_Standard, &globle_var::mdi, 300, 600);		 //����ͼƬ��ʼ��
	//printf_globle("�������\n");


	if (virtualImg == "")
	{
		printf_globle("TiggerWatcherAndGrabber load! ��ǰģʽΪ��ʵ�����\n");
		initForE2V();
	}
	else
	{
		printf_globle("TiggerWatcherAndGrabber load! ��ǰģʽΪ���������\n");
		initForVC(virtualImg);
	}
}

void TiggerWatcherAndGrabber::initForE2V()
{
	USING_VIRTUAL_CAMERA = 0;//�Ƿ�ʹ����������ͷ 1ʹ�� 0��E2V
	//��ʼ���ɼ���
	int status = MicroDisplayInit::InitParameter(globle_var::mdi);
	//status = MicroDisplayInit::InitLoad(globle_var::mdi, "4096RGB1LineX1.mcf");
	if (status < 0)
	{
		MD_ErrorMessageWait(globle_var::mdi.fg);
		return;
	}
	MicroDisplayInit::CreateBufferWithOutDiplay(globle_var::mdi);
	//MicroDisplayInit::CreateBufferWithDiplay(globle_var::mdi);
	Fg_saveConfig(globle_var::mdi.fg, "save.mcf");
	//û�б���˵����ʼ����ɣ��д���Զ������˳���
	printf_globle("���&�ɼ�����ʼ����ɣ�\n");


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
}
void TiggerWatcherAndGrabber::initForVC(string virtualImg)
{
	USING_VIRTUAL_CAMERA = 1;
	vc = VirtualCamera(globle_var::mdi, virtualImg);//��ʼ���������
	printf_globle("���������ʼ����ɣ�\n");
}


TiggerWatcherAndGrabber::~TiggerWatcherAndGrabber()
{
	printf_globle("TiggerWatcherAndGrabber unload!\n");
}



void TiggerWatcherAndGrabber::StartWatch()
{
	if (!IsWatching)
	IsWatching = true;
	std::thread t_watcher(std::mem_fn(&TiggerWatcherAndGrabber::watcherThread), this);
	t_watcher.detach();
	printf_globle("��ʼ��ش����ź�\n");
}
void TiggerWatcherAndGrabber::StopWatch()
{
	printf_globle("ֹͣ��ش����ź�\n");
	IsWatching = false;
}
bool TiggerWatcherAndGrabber::ManualTigger()
{
	if (IsGrabbing || !IsWatching)
		return false;
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
			if (GrabbingIndex > 10000000) GrabbingIndex = 1;

			stringstream ss;
			ss << GrabbingIndex << " " << "producer: Start  at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
			printf_globle(ss.str());
			ss.str("");

			IsGrabbing = true;
			globle_var::s.Start();

			double t = (double)cv::getTickCount();


			ss << GrabbingIndex << " " << "TiggerWaitTimeMS: " << globle_var::TiggerWaitTimeMS << endl;
			printf_globle(ss.str());
			ss.str("");
			//�����󣬵ȴ�ש������������
			Sleep(globle_var::TiggerWaitTimeMS);

			std::thread t_3in1(std::mem_fn(&TiggerWatcherAndGrabber::threeInOne), this);
			t_3in1.detach();


			if (!USING_VIRTUAL_CAMERA)
			{
				MicroDisplayControler::FreeRunning(globle_var::mdi, globle_var::s);
			}
			else
			{
				vc.FreeRunning(globle_var::mdi, globle_var::s);
			}
			//������ʱ
			t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();

			ss << GrabbingIndex << " " << "producer:" << globle_var::mdi.width << "x" << globle_var::mdi.MaxPics << "��" << t << "  End  at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
			printf_globle(ss.str());
			ss.str("");

			while (globle_var::s.BufferReadIndex + globle_var::s.NinOne < globle_var::mdi.MaxPics)
			{
				Sleep(1);
				//printf_globle("Sleep(1)\n");
			};
			//threeInOne();

			if (hwnd != NULL)
			{
				PostMessage(hwnd, WM_USER + 100, 0, 0);
			}
			//��������߹�������
			IsGrabbing = false;
		}
		else
		{
			Sleep(10);
		}
	}
}

//ͼƬ�ϳ�
void TiggerWatcherAndGrabber::threeInOne()
{
	stringstream ss;
	ss << GrabbingIndex << " " << "ThreeInOne��Start at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");

	double t = (double)cv::getTickCount();
	//ͬ�����й��ս��������
	for (globle_var::s.BufferReadIndex = 0; globle_var::s.BufferReadIndex < globle_var::mdi.MaxPics; globle_var::s.BufferReadIndex++)
	{
		//��δд�뻺�棬�ȴ�
		while (globle_var::s.BufferReadIndex + globle_var::s.NinOne >= globle_var::s.BufferWriteIndex && globle_var::s.BufferWriteIndex != globle_var::mdi.MaxPics)
		{
			Sleep(1);
		}
		globle_var::s.ThreeInOne(globle_var::s.BufferReadIndex);
	}
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	ss << GrabbingIndex << " " << "ThreeInOne��" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");
}