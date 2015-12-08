#include "TiggerWatcherAndGrabber.h"


TiggerWatcherAndGrabber::TiggerWatcherAndGrabber(HWND _hwnd, string virtualImg)
{
	hwnd = _hwnd;
	vc = NULL;
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
	if (vc == NULL)
		delete vc;
	if (!USING_VIRTUAL_CAMERA)
	{
		MicroDisplayInit::Release(globle_var::_mdi);
	}
	printf_globle("TiggerWatcherAndGrabber unload!\n");
}


void TiggerWatcherAndGrabber::Init(string virtualImg)
{
	if (virtualImg == "")
	{
		printf_globle("TiggerWatcherAndGrabber load! ��ǰģʽΪ��ʵ�����\n");
		Switch2Real(true);
	}
	else
	{
		printf_globle("TiggerWatcherAndGrabber load! ��ǰģʽΪ���������\n");
		Switch2Virtual(virtualImg, true);
	}
}
void TiggerWatcherAndGrabber::Switch2Virtual(string virtualImg, bool refresh)
{
	IsGrabbing = false;
	if (vc == NULL)
		delete vc;
	if (USING_VIRTUAL_CAMERA && !refresh)
		return;
	USING_VIRTUAL_CAMERA = true;
	if (globle_var::_mdi.fg != NULL)
		MicroDisplayInit::Release(globle_var::_mdi);




	vc = new VirtualCamera(globle_var::mdi(), virtualImg);//��ʼ���������
	printf_globle("���������ʼ����ɣ�\n");
}
void TiggerWatcherAndGrabber::Switch2Real(bool refresh)
{
	IsGrabbing = false;
	//if (vc == NULL)
	//	delete vc;
	//if (!USING_VIRTUAL_CAMERA && !refresh)
	//	return;
	//USING_VIRTUAL_CAMERA = false;

	////��ʼ���ɼ���
	//int status = MicroDisplayInit::InitParameter(globle_var::_mdi);
	////status = MicroDisplayInit::InitLoad(globle_var::_mdi, "4096RGB1LineX1.mcf");
	//if (status < 0)
	//{
	//	MD_ErrorMessageWait(globle_var::mdi().fg);
	//	return;
	//}
	//MicroDisplayInit::CreateBufferWithOutDiplay(globle_var::_mdi);
	////MicroDisplayInit::CreateBufferWithDiplay(globle_var::_mdi);
	//Fg_saveConfig(globle_var::mdi().fg, "save.mcf");
	////û�б���˵����ʼ����ɣ��д���Զ������˳���
	//printf_globle("���&�ɼ�����ʼ����ɣ�\n");


	///**********************��ʼ���ɼ���***********************/
	//if (mc100_open(0) >= 0)
	//{
	//	printf_globle("�򿪲ɼ���0�ɹ���\n");
	//	mc100_write_port(0, MC100_PORTA, 0x00);
	//}
	//else
	//{
	//	printf_globle("�򿪲ɼ���0ʧ�ܣ�\n");
	//}
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

			if (s != NULL)
				delete s;
			s = new BufferStorage(globle_var::mdi().MaxPics, globle_var::mdi().Width);

			if (GrabbingIndex > 10000000) GrabbingIndex = 1;

			stringstream ss;
			ss << GrabbingIndex << " " << "producer: Start  at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
			printf_globle(ss.str());
			ss.str("");

			IsGrabbing = true;

			s->Start();

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
			//threeInOne();

			////ͼƬ��ȡ��ɣ����Ƶ�ȫ�ֱ�������ɾ��s
			//BufferOrg = s->Buffer.clone();
			//BufferImg = s->BufferImg.clone();

			//delete s;
			//s = NULL;

			//if (!IsCalibration)
			//{
			//	if (hwnd != NULL)
			//	{
			//		PostMessage(hwnd, WM_USER + 100, 0, 0);
			//	}
			//	//��������߹�������
			//	IsGrabbing = false;
			//}
			//else//���Ǳ궨��������ֹͣ���
			//{
			//	IsCalibration = false;
			//	StopWatch();
			//	PostMessage(hwnd, WM_USER + 102, 0, 0);//���ꡢ�ɼ������������
			//}
		}
		else
		{
			Sleep(10);
		}
	}
}
void TiggerWatcherAndGrabber::capture()
{
	MicroDisplay *md = new MicroDisplay(globle_var::_mdi.MaxPics, globle_var::_mdi.Width);
	md->Capture();

	//double t = (double)cv::getTickCount();
	//if (!USING_VIRTUAL_CAMERA)
	//{
	//	MicroDisplayControler::FreeRunning(globle_var::_mdi, s);
	//}
	//else
	//{
	//	if (vc->buffer.cols == 0 || vc->buffer.rows == 0)
	//	{
	//		printf_globle("���������Ƭ�����ڡ�");
	//		IsGrabbing = false;
	//		if (hwnd != NULL)
	//		{
	//			PostMessage(hwnd, WM_USER + 100, 0, 0);
	//		}
	//		return;
	//	}
	//	vc->FreeRunning(globle_var::_mdi, s);
	//}
	////������ʱ
	//t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	//stringstream ss;
	//ss << GrabbingIndex << " " << "producer:" << globle_var::mdi().Width << "x" << globle_var::mdi().MaxPics << "��" << t << "  End  at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	//printf_globle(ss.str());
	//ss.str("");
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
	for (s->BufferReadIndex = 0; s->BufferReadIndex < globle_var::mdi().MaxPics; s->BufferReadIndex++)
	{
		//��δд�뻺�棬�ȴ�
		while (s->BufferReadIndex + s->NinOne >= s->BufferWriteIndex && s->BufferWriteIndex < globle_var::mdi().MaxPics)
		{
			Sleep(1);
		}
		s->ThreeInOne(s->BufferReadIndex);
	}
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	ss << GrabbingIndex << " " << "ThreeInOne��" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");
}