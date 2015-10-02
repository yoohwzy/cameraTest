#include "Contoller.h"


Contoller::Contoller(string virtualImg)
{
	sofeState = SofeState::SS_BEFORE_INIT;
	//���ò���
	mdi.colorType = MicroDisplayInit::RGB;
	mdi.width = 4096;
	mdi.height = 1;
	mdi.MaxPics = 11000;//�ɼ�����֡ͼ��
	s = BufferStorage(mdi);

	if (virtualImg == "")
		initForE2V();
	else
		initForVC(virtualImg);

	//����  ������  ����
	std::thread t_runner(std::mem_fn(&Contoller::RunnerThread), this);
	t_runner.detach();



	sofeState = SofeState::SS_INITED;
}
void Contoller::initForE2V()
{
	USING_VIRTUAL_CAMERA = 0;//�Ƿ�ʹ����������ͷ 1ʹ�� 0��E2V

	//��ʼ���ɼ���
	status = MicroDisplayInit::InitParameter(mdi);
	//status = MicroDisplayInit::InitLoad(mdi, "4096RGB1LineX1.mcf");
	if (status < 0)
	{
		MD_ErrorMessageWait(mdi.fg);
		return;
	}
	MicroDisplayInit::CreateBufferWithOutDiplay(mdi);
	//MicroDisplayInit::CreateBufferWithDiplay(mdi);
	Fg_saveConfig(mdi.fg, "save.mcf");
	//û�б���˵����ʼ����ɣ��д���Զ������˳���
	OutPutDebugInfo("���&�ɼ�����ʼ����ɣ�");


	/**********************��ʼ���ɼ���***********************/
	if (mc100_open(0) >= 0)
	{
		OutPutDebugInfo("�򿪲ɼ���0�ɹ���");
		//��ʼ�����ɼ���
		std::thread t_watcher(std::mem_fn(&Contoller::IoCardWatcherThread), this);
		t_watcher.detach();
		mc100_write_port(0, MC100_PORTA, 0x00);
		IoCardOK = 1;
	}
	else
	{
		OutPutDebugInfo("�򿪲ɼ���0ʧ�ܣ�");
	}
}
void Contoller::initForVC(string virtualImg)
{
	USING_VIRTUAL_CAMERA = 1;
	vc = VirtualCamera(mdi, virtualImg);//��ʼ���������
	OutPutDebugInfo("���&�ɼ�����ʼ����ɣ�");
}










/******************************���̷���**********************************/

//�����ɼ���״̬����
void Contoller::IoCardWatcherThread()
{
	sofeState = SofeState::SS_WATCHING;
	do{
		if (sofeState == SofeState::SS_WATCHING &&
			producerState != ProducerState::PS_GRABBING &&
			mc100_check_pin(0, (MC100_PORTA << 4) | 0) != 1)
		{
			sofeState = SofeState::SS_ACTIVATED;
			//producerState != ProducerState::GRABBING
			OutPutDebugInfo("����������");
			Sleep(10);
			//�ȴ��ɼ��������
			while (producerState == ProducerState::PS_GRABBING)
			{
				Sleep(10);
			}
		}
		else
		{
			Sleep(1);
		}
	} while (sofeState != SofeState::SS_EXIT);
}

void Contoller::RunnerThread()
{
	int grabbingIndex = 0;
	do{
		//����������δ��Ӧ���ҳ���δ�ڲ�ͼ
		if (sofeState == SofeState::SS_ACTIVATED &&
			producerState != ProducerState::PS_GRABBING)
		{

			producerState = ProducerState::PS_GRABBING;
			//���ó���״̬�ص�WATCHING����ʾ�����ź�����Ӧ
			sofeState = SofeState::SS_WATCHING;

			OutPutDebugInfo("��ʼ��ͼ");
			Sleep(DelayTime);

			grabbingIndex += 1;
			if (grabbingIndex > 1000) grabbingIndex = 1;


			//��ʼ������
			s.Start();
			//����������
			std::thread t1(std::mem_fn(&Contoller::producerThread), this);
			t1.detach();

			//����������
			std::thread t2(std::mem_fn(&Contoller::customerThread), this);
			t2.detach();
			Sleep(10);
			customerState = CustomerState::CS_PROCESSING;


			//�ȴ��ɼ��������
			while (producerState == ProducerState::PS_GRABBING)
			{
				Sleep(10);
			}
			while (customerState == CustomerState::CS_PROCESSING)
			{
				Sleep(10);
			}

			////����ͼ���ʱ�����������������㷨����ʾ��һ�ֵĴ�����δ��ɣ��㷨�ٶȲ�����
			//if (customerState == CustomerState::CS_PROCESSING)
			//	ExitWithError("�㷨�ٶȹ�������һ�ִ�����δ��ɣ�");



#ifdef OUTPUT_DEBUG_INFO
			if (OUTPUT_DEBUG_INFO)
			{
				string p1;
				stringstream ss1;
				ss1 << "samples/result" << grabbingIndex << "_oԭͼ.jpg";
				ss1 >> p1;
				string p2;
				stringstream ss2;
				ss2 << "samples/result" << grabbingIndex << "_x3.jpg";
				ss2 >> p2;
				cv::imwrite(p1, s.NowBuffer);
				//cv::imwrite(p2, s.NowBufferImg);
			}
#endif
		}
		else
		{
			Sleep(10);
		}
	} while (sofeState != SofeState::SS_EXIT);
}


//������
void Contoller::producerThread()
{
	double t = (double)cv::getTickCount();
	if (!USING_VIRTUAL_CAMERA)
	{
		MicroDisplayControler::FreeRunning(mdi, s);
	}
	else
	{
		vc.FreeRunning(mdi, s);
	}
	//������ʱ
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "producer:" << mdi.width << "x" << mdi.MaxPics << "��" << t << "  End  at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;


	//t = (double)cv::getTickCount();
	////ͬ�����й��ս��������
	//for (s.BufferReadIndex = 0; s.BufferReadIndex < mdi.MaxPics; s.BufferReadIndex++)
	//{
	//	//��δд�뻺�棬�ȴ�
	//	while (s.BufferReadIndex + s.NinOne >= s.BufferWriteIndex && s.BufferWriteIndex != mdi.MaxPics)
	//	{
	//		Sleep(1);
	//	}
	//	s.ThreeInOne(s.BufferReadIndex);
	//}
	//t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	//std::cout << "ThreeInOne��" << t << endl;
	//��������߹�������
	producerState = ProducerState::PS_GRABBING_END;
}
//������
void Contoller::customerThread()
{
	threeInOne();

	std::cout << "customer��Start at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	//ͼƬ�ϳ���ɺ�ű�׼��ʼ����
	customerState = CustomerState::CS_PROCESSING;
	//��ʼ��ʱ
	double t = (double)cv::getTickCount();

	cv::Mat DetectedImg = s.NowBufferImg.clone();
	if (DetectedImg.channels() > 0)
		cv::cvtColor(DetectedImg, DetectedImg, CV_BGR2GRAY);
	//��ȡ��ֵ��ͼ��
	Processor::Binaryzation(DetectedImg);

	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "Binaryzation��" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	t = (double)cv::getTickCount();

	Processor::Gaussian_Blur(DetectedImg);

	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "Gaussian_Blur��" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	t = (double)cv::getTickCount();


	//��ש��Ե���
	BlocksDetector bd = BlocksDetector(DetectedImg);
	t = (double)cv::getTickCount();
	bd.Start();
	bd.StartUP_DOWN(BlocksDetector::Up);
	bd.StartUP_DOWN(BlocksDetector::Down);
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "BlocksDetector��" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;

#ifdef OUTPUT_DEBUG_INFO
	if (OUTPUT_DEBUG_INFO)
	{
		cv::imwrite("samples/00drowDebugDetectLR.jpg", bd.drowDebugDetectLR);
		cv::imwrite("samples/01drowDebugDetectUD.jpg", bd.drowDebugDetectUD);
		cv::imwrite("samples/02drowDebugResult.jpg", bd.drowDebugResult);
	}
#endif
	//��������߹�������
	customerState = CustomerState::CS_WAITING;

	std::cout << "customer��End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
}
//ͼƬ�ϳ�
void Contoller::threeInOne()
{
	std::cout << "ThreeInOne��Start at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	double t = (double)cv::getTickCount();
	//ͬ�����й��ս��������
	for (s.BufferReadIndex = 0; s.BufferReadIndex < mdi.MaxPics; s.BufferReadIndex++)
	{
		//��δд�뻺�棬�ȴ�
		while (s.BufferReadIndex + s.NinOne >= s.BufferWriteIndex && s.BufferWriteIndex != mdi.MaxPics)
		{
			Sleep(1);
		}
		s.ThreeInOne(s.BufferReadIndex);
	}
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "ThreeInOne��" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
}