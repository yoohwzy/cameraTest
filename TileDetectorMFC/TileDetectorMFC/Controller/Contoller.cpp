#include "Contoller.h"

Contoller::Contoller(string virtualImg)
{
	sofeState = SofeState::SS_BEFORE_INIT;
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
	//printf_globle("�������\r\n");


	if (virtualImg == "")
		initForE2V();
	else
		initForVC(virtualImg);

	//����  ������  ����
	std::thread t_runner(std::mem_fn(&Contoller::RunnerThread), this);
	t_runner.detach();

	sofeState = SofeState::SS_WATCHING;
}
void Contoller::initForE2V()
{
	USING_VIRTUAL_CAMERA = 0;//�Ƿ�ʹ����������ͷ 1ʹ�� 0��E2V
	//��ʼ���ɼ���
	status = MicroDisplayInit::InitParameter(globle_var::mdi);
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
	printf_globle("���&�ɼ�����ʼ����ɣ�\r\n");


	/**********************��ʼ���ɼ���***********************/
	if (mc100_open(0) >= 0)
	{
		printf_globle("�򿪲ɼ���0�ɹ���\r\n");
		//��ʼ�����ɼ���
		std::thread t_watcher(std::mem_fn(&Contoller::IoCardWatcherThread), this);
		t_watcher.detach();
		mc100_write_port(0, MC100_PORTA, 0x00);
		IoCardOK = 1;
	}
	else
	{
		printf_globle("�򿪲ɼ���0ʧ�ܣ�\r\n");
	}
}
void Contoller::initForVC(string virtualImg)
{
	USING_VIRTUAL_CAMERA = 1;
	vc = VirtualCamera(globle_var::mdi, virtualImg);//��ʼ���������
	printf_globle("���&�ɼ�����ʼ����ɣ�\r\n");
}










/******************************���̷���**********************************/

//�����ɼ���״̬����
void Contoller::IoCardWatcherThread()
{
	sofeState = SofeState::SS_WATCHING;
	do{
		if (State_CanBeTiggered() &&	mc100_check_pin(0, (MC100_PORTA << 4) | 0) != 1)
		{
			sofeState = SofeState::SS_ACTIVATED;
			//producerState != ProducerState::GRABBING
			printf_globle("����������\r\n");
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
	do{
		//�����߲�δ�ڲ�ͼ
		if (producerState != ProducerState::PS_GRABBING)
		{
			//����������δ��Ӧ
			if (sofeState == SofeState::SS_ACTIVATED)
			{
				producerState = ProducerState::PS_GRABBING;

				printf_globle("��ʼ��ͼ\r\n");
				Sleep(DelayTime);

				grabbingIndex += 1;
				if (grabbingIndex > 10000000) grabbingIndex = 1;


				//��ʼ������
				globle_var::s.Start();
				//����������
				std::thread t1(std::mem_fn(&Contoller::producerThread), this);
				t1.detach();


				//�ȴ��ɼ��������
				while (producerState == ProducerState::PS_GRABBING)
				{
					Sleep(10);
				}

				//����ͼ���ʱ�����������������㷨����ʾ��һ�ֵĴ�����δ��ɣ��㷨�ٶȲ�����
				if (customerState == CustomerState::CS_PROCESSING)
					ExitWithError("�㷨�ٶȹ�������һ�ִ�����δ��ɣ�");

				//����������
				std::thread t2(std::mem_fn(&Contoller::customerThread), this);
				t2.detach();
				Sleep(10);

				//�����������
				customerState = CustomerState::CS_PROCESSING;
				//while (customerState == CustomerState::CS_PROCESSING)//����ȴ����Ϊ˳��ṹ����������ɺ������һ�δ���
				//{
				//	Sleep(10);
				//}

				//#ifdef OUTPUT_DEBUG_INFO
				//			if (OUTPUT_DEBUG_INFO)
				//			{
				//				string p1;
				//				stringstream ss1;
				//				ss1 << "samples/result" << grabbingIndex << "_oԭͼ.jpg";
				//				ss1 >> p1;
				//				string p2;
				//				stringstream ss2;
				//				ss2 << "samples/result" << grabbingIndex << "_x3.jpg";
				//				ss2 >> p2;
				//				cv::imwrite(p1, globle_var::s.NowBuffer);
				//				//cv::imwrite(p2, globle_var::s.NowBufferImg);
				//			}
				//#endif

				//���ó���״̬�ص�WATCHING����ʾ�����ź�����Ӧ
				sofeState = SofeState::SS_WATCHING;
			}
			else
			{
				Sleep(10);
			}
		}
		else if (sofeState == SofeState::SS_ACTIVATED)
			//���ó���״̬�ص�WATCHING����ʾ�����ź�����Ӧ
			sofeState = SofeState::SS_WATCHING;

	} while (sofeState != SofeState::SS_EXIT);
}


//������
void Contoller::producerThread()
{
	double t = (double)cv::getTickCount();
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

	stringstream ss;
	ss << grabbingIndex << " " << "producer:" << globle_var::mdi.width << "x" << globle_var::mdi.MaxPics << "��" << t << "  End  at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");

	//��������߹�������
	producerState = ProducerState::PS_GRABBING_END;
}
//������
void Contoller::customerThread()
{
	stringstream ss;
	ss << grabbingIndex << " " << "customer��Start at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");

	threeInOne();

	//ͼƬ�ϳ���ɺ�ű�׼��ʼ����
	customerState = CustomerState::CS_PROCESSING;
	//��ʼ��ʱ
	double t = (double)cv::getTickCount();

	cv::Mat DetectedImg = globle_var::s.NowBufferImg.clone();
	if (DetectedImg.channels() > 0)
	{
		t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
		cv::cvtColor(DetectedImg, DetectedImg, CV_BGR2GRAY);
		ss << grabbingIndex << " " << "customer-cvtColor��" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
		printf_globle(ss.str());
		ss.str("");

		t = (double)cv::getTickCount();
	}
	////��ȡ��ֵ��ͼ��
	//Processor::Binaryzation(DetectedImg);

	//t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	//ss << grabbingIndex << " " << "Binaryzation��" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	//printf_globle(ss.str());
	//ss.str("");
	//t = (double)cv::getTickCount();

	//Processor::Gaussian_Blur(DetectedImg);

	//t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	//ss << grabbingIndex << " " << "Gaussian_Blur��" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	//printf_globle(ss.str());
	//ss.str("");
	//t = (double)cv::getTickCount();


	//��ש��Ե���
//	BlocksDetector bd = BlocksDetector(DetectedImg);
//	t = (double)cv::getTickCount();
//	bd.Start();
//	bd.StartUP_DOWN(BlocksDetector::Up);
//	bd.StartUP_DOWN(BlocksDetector::Down);
//	bd.ABCD();
//	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
//	ss << grabbingIndex << " " << "BlocksDetector��" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	//printf_globle(ss.str());
	//ss.str("");

//#ifdef OUTPUT_DEBUG_INFO
//	if (OUTPUT_DEBUG_INFO)
//	{
//		cv::imwrite("samples/00drowDebugDetectLR.jpg", bd.drowDebugDetectLR);
//		cv::imwrite("samples/01drowDebugDetectUD.jpg", bd.drowDebugDetectUD);
//		cv::imwrite("samples/02drowDebugResult.jpg", bd.drowDebugResult);
//	}
//#endif
//
//	//��ש��Եȱ�ݼ��
//	EdgeDetector ed = EdgeDetector(DetectedImg, &bd);
//	if (ed.Defects.size() > 0)
//	{
//		ss << grabbingIndex << " " << "��Ե��ȱ��" << endl;
	//printf_globle(ss.str());
	//ss.str("");
//	}
//	else
//	{
//		Measurer m = Measurer();
//		m.CaculteSize(&bd);
//	}
//	BlockHoughDetector bhd = BlockHoughDetector(globle_var::s.NowBufferImg);

	//��������߹�������
	ss << grabbingIndex << " " << "customer��End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");

	customerState = CustomerState::CS_WAITING;
}
//ͼƬ�ϳ�
void Contoller::threeInOne()
{
	stringstream ss;
	ss << grabbingIndex << " " << "customer-ThreeInOne��Start at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
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
	ss << grabbingIndex << " " << "customer-ThreeInOne��" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	printf_globle(ss.str());
	ss.str("");
}