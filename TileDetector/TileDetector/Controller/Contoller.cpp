#include "Contoller.h"


Contoller::Contoller()
{
	sofeState.SetState(StateMachine::SofeState::SS_BEFORE_INIT);
	//���ò���
	mdi.colorType = MicroDisplayInit::RGB;
	mdi.width = 4096;
	mdi.height = 1;
	mdi.MaxPics = 11000;//�ɼ�����֡ͼ��
	s = BufferStorage(mdi);

	if (!USING_VIRTUAL_CAMERA)
	{
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
	}
	else
	{
		vc = new VirtualCamera(mdi, "D31����_oԭͼ.jpg");//��ʼ���������
	}
	OutPutDebugInfo("���&�ɼ�����ʼ����ɣ�");


	/**********************��ʼ���ɼ���***********************/
	if (mc100_open(0) >= 0)
	{
		OutPutDebugInfo("�򿪲ɼ���0�ɹ���");
		//��ʼ�����ɼ���
		//std::thread t_watcher(watcher);
		//t_watcher.detach();
		mc100_write_port(0, MC100_PORTA, 0x00);
	}
	else
	{
		OutPutDebugInfo("�򿪲ɼ���0ʧ�ܣ�");
	}
	/*
	std::thread t_runner(runner);
	t_runner.detach();
	*/

	sofeState.SetState(StateMachine::SofeState::SS_INITED);
}

//�����ɼ���״̬����
void Contoller::IoCardWatcherThread()
{
	sofeState.SetState(StateMachine::SofeState::SS_WATCHING);
	do{
		if (sofeState.GetState == StateMachine::SofeState::SS_WATCHING &&
			producerState.GetState != StateMachine::ProducerState::PS_GRABBING &&
			mc100_check_pin(0, (MC100_PORTA << 4) | 0) != 1)
		{
			sofeState.SetState(StateMachine::SofeState::SS_ACTIVATED);
			//producerState.GetState != StateMachine::ProducerState::GRABBING
			OutPutDebugInfo("����������");
			Sleep(10);
			//�ȴ��ɼ��������
			while (producerState.GetState == StateMachine::ProducerState::PS_GRABBING)
			{
				Sleep(10);
			}
		}
		else
		{
			Sleep(1);
		}
	} while (sofeState.GetState != StateMachine::SofeState::SS_EXIT);
}

void Contoller::RunnerThread()
{
	int grabbingIndex = 0;
	do{
		//����������δ��Ӧ���ҳ���δ�ڲ�ͼ
		if (sofeState.GetState == StateMachine::SofeState::SS_ACTIVATED &&
			producerState.GetState != StateMachine::ProducerState::PS_GRABBING)
		{
			//���ó���״̬�ص�WATCHING����ʾ�����ź�����Ӧ
			sofeState.SetState(StateMachine::SofeState::SS_WATCHING);

			OutPutDebugInfo("��ʼ��ͼ");
			Sleep(DelayTime);

			grabbingIndex += 1;
			if (grabbingIndex > 1000) grabbingIndex = 1;

			//��ʼ������
			s.Start();
			producerState.SetState(StateMachine::ProducerState::PS_GRABBING);
			std::thread t1(std::mem_fn(Contoller::producerThread));
			t1.detach();

			//�ȴ��ɼ��������
			while (producerState.GetState == StateMachine::ProducerState::PS_GRABBING)
			{
				Sleep(10);
			}

			//����ͼ���ʱ�����������������㷨����ʾ��һ�ֵĴ�����δ��ɣ��㷨�ٶȲ�����
			if (customerState.GetState == StateMachine::CustomerState::CS_PROCESSING)
				ExitWithError("�㷨�ٶȹ�������һ�ִ�����δ��ɣ�");
#ifdef OUTPUT_DEBUG_INFO
			if (OUTPUT_DEBUG_INFO)
			{
				//string p1;
				//stringstream ss1;
				//ss1 << "samples/result" << grabbingIndex << "_oԭͼ.jpg";
				//ss1 >> p1;
				//string p2;
				//stringstream ss2;
				//ss2 << "samples/result" << grabbingIndex << "_x3.jpg";
				//ss2 >> p2;
				//cv::imwrite(p1, s.NowBuffer);
				//cv::imwrite(p2, s.NowBufferImg);
			}
#endif
		}
		else
		{
			Sleep(10);
		}
	} while (sofeState.GetState != StateMachine::SofeState::SS_EXIT);
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
		vc->FreeRunning(mdi, s);
	}
	//������ʱ
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << mdi.width << "x" << mdi.MaxPics << "��" << t << endl;


	t = (double)cv::getTickCount();
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
	std::cout << "ThreeInOne��" << t << endl;
	//��������߹�������
	producerState.SetState(StateMachine::ProducerState::PS_GRABBING_END);
}
