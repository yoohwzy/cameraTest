#include "Contoller.h"


Contoller::Contoller()
{
	sm.SetState(StateMachine::State::BEFORE_INIT);
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

	sm.SetState(StateMachine::State::INITED);
}

//�����ɼ���״̬����
void Contoller::IoCardWatcherThread()
{
	do{
		// ��ȡPA0���ŵĵ�ƽΪ��,�ҳ���δ�ڲ�ͼʱ
		if (sm.GetState != StateMachine::State::GRABBING && mc100_check_pin(0, (MC100_PORTA << 4) | 0) != 1)
		{
			sm.SetState(StateMachine::State::GRABBING);
			OutPutDebugInfo("����������");
			Sleep(10);
			//�ȴ��ɼ��������
			while (!producerEndFlag)
			{
				Sleep(10);
			}
		}
		else
		{
			Sleep(1);
		}
	} while (!exitFlag);
}