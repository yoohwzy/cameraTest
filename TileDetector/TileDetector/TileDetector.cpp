// HelloWorldFreeRunLine.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "globle.h"
#include "Class\MicroDisplay\MicroDisplayInit.h"
#include "Class\MicroDisplay\MicroDisplayControler.h"
#include "Class\BufferStorage.h"
#include "Class\VirtualCamera.h"
#include "Class\BlocksDetector.h"
#include "Class\Processor.h"


#include <thread>
#include "mc100.h"

BufferStorage s;
int status = 0;
MicroDisplayInit mdi;
VirtualCamera vc;

const bool USING_VIRTUAL_CAMERA = 1;//�Ƿ�ʹ����������ͷ 1ʹ�� 0��E2V

//�ɼ����ڼ���
int grabbingIndex = 0;

//��ʼ�ɼ����ɼ��б�־
bool startFlag = false, grabbingFlag = false;
//��������ɱ�־����������ɱ�־
bool producerEndFlag = false, customerEndFlag = false;

bool exitFlag = false;

//������
void producer()
{
	producerEndFlag = false;

	double t = (double)cv::getTickCount();

	if (!USING_VIRTUAL_CAMERA)
	{
		if (MicroDisplayControler::FreeRunning(mdi, s) < 0)
		{
			MD_ErrorMessageWait(mdi.fg);
			return;
		}
	}
	else
	{
		vc.FreeRunning(mdi, s);
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
	producerEndFlag = true;
}
//������
void customer()
{
	customerEndFlag = false;

	//��ʼ��ʱ
	double t = (double)cv::getTickCount();

	cv::Mat DetectedImg = s.NowBufferImg.clone();
	if (DetectedImg.channels() > 0)
		cv::cvtColor(DetectedImg, DetectedImg, CV_BGR2GRAY);
	//��ȡ��ֵ��ͼ��
	Processor::Binaryzation(DetectedImg);

	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "Binaryzation��" << t << endl;
	t = (double)cv::getTickCount();

	Processor::Gaussian_Blur(DetectedImg);

	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "Gaussian_Blur��" << t << endl;
	t = (double)cv::getTickCount();


	//��ש��Ե���
	BlocksDetector bd = BlocksDetector(DetectedImg);
	t = (double)cv::getTickCount();
	bd.Start();
	bd.StartUP_DOWN(BlocksDetector::Up);
	bd.StartUP_DOWN(BlocksDetector::Down);
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "BlocksDetector��" << t << endl;

#ifdef OUTPUT_DEBUG_INFO
	if (OUTPUT_DEBUG_INFO)
	{
		cv::imwrite("samples/00drowDebugDetectLR.jpg", bd.drowDebugDetectLR);
		cv::imwrite("samples/01drowDebugDetectUD.jpg", bd.drowDebugDetectUD);
		cv::imwrite("samples/02drowDebugResult.jpg", bd.drowDebugResult);
	}
#endif
	//��������߹�������
	customerEndFlag = true;
}
int init()
{
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
			return -1;
		}
		MicroDisplayInit::CreateBufferWithOutDiplay(mdi);
		Fg_saveConfig(mdi.fg, "save.mcf");
		//MicroDisplayInit::CreateBufferWithDiplay(mdi);//ʹ�ò�ɫ���ʱ�������ٶȻ����
	}
	else
	{
		//��ʼ���������
		vc = VirtualCamera(mdi, "D31����_oԭͼ.jpg");
	}
}
//�����ɼ���״̬
void watcher()
{
	do{
		// ��ȡPA0���ŵĵ�ƽΪ��,�ҳ���δ�ڲ�ͼʱ
		if (grabbingFlag == false && mc100_check_pin(0, (MC100_PORTA << 4) | 0) != 1)
		{
			startFlag = true;
			std::cout << "����������" << endl;
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
//��ͼ����������
void runner()
{
	do{
		// ��ȡPA0���ŵĵ�ƽΪ��,�ҳ���δ�ڲ�ͼʱ
		if (startFlag == true && grabbingFlag == false)
		{
			std::cout << "��ʼ��ͼ" << endl;
			grabbingFlag = true;
			startFlag = false;

			Sleep(410);

			grabbingIndex += 1;
			if (grabbingIndex > 1000) grabbingIndex = 1;

			//��ʼ������
			s.Start();

			std::thread t1(producer);
			t1.detach();

			//�ȴ��ɼ��������
			while (!producerEndFlag)
			{
				Sleep(10);
			}

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

			grabbingFlag = false;
		}
		else
		{
			Sleep(10);
		}
	} while (!exitFlag);
}
int main()
{
	ExitWithError("123");
	init();
	printf("init end��\r\n");

	if (mc100_open(0) >= 0)
	{
		cout << "�򿪲ɼ���0�ɹ���" << endl;
		//��ʼ�����ɼ���
		std::thread t_watcher(watcher);
		t_watcher.detach();
	}
	else
	{
		cout << "�򿪲ɼ���0ʧ�ܣ�" << endl;
	}

	std::thread t_runner(runner);
	t_runner.detach();

	//��ѭ��
	string input;
	mc100_write_port(0, MC100_PORTA, 0x00);
	do{
		std::cout << "����1��ʼ��ͼ��2ִ�������߳���3�Խ���4������ս������ţ�q�˳���";
		std::cin >> input;
		if (input == "1")
		{
			startFlag = true;
			Sleep(10);

			while (grabbingFlag == true)
			{
				Sleep(10);
			}
		}

		else if (input == "2")
		{
			std::thread t2(customer);
			t2.detach();
			Sleep(10);

			while (!customerEndFlag)
			{
				Sleep(10);
			}
		}
		else if (input == "4")
		{
			//��������ͼ��
			int MaxPics = mdi.MaxPics;
			mdi.MaxPics = 10;
			BufferStorage stmp = BufferStorage(mdi);
			stmp.Start();
			if (!USING_VIRTUAL_CAMERA)
			{
				if (MicroDisplayControler::FreeRunning(mdi, stmp) < 0)
				{
					MD_ErrorMessageWait(mdi.fg);
					return 0;
				}
			}
			else
			{
				VirtualCamera vctmp = VirtualCamera(mdi, "����У��_o.jpg");
				vctmp.FreeRunning(mdi, stmp);
			}
			//cv::Mat img = stmp.NowBufferImg;

			//X - ����������ʱֱ����Ӽ���
			cv::Mat SamplesRGBTMP = stmp.NowBufferImg(cv::Rect(0, 2, mdi.width, 1)).clone() / 3;
			cv::imwrite("samples/����У������.jpg", SamplesRGBTMP);
			//RGB
			cv::Mat SamplesRGB = cv::Mat(mdi.height, mdi.width, CV_8UC3, cv::Scalar(50, 50, 50));
			SamplesRGB = SamplesRGB - SamplesRGBTMP;
			SamplesRGB.copyTo(mdi.SamplesRGB);
			//Gray
			cv::cvtColor(SamplesRGB, mdi.SamplesGray, CV_BGR2GRAY);
			cv::Mat SamplesGray = mdi.SamplesGray;
			std::cout << "OK\r\n";

			//���³�ʼ������
			mdi.MaxPics = MaxPics;
			s = BufferStorage(mdi);
		}
		else if (input == "3")
		{
			MicroDisplayInit::Release(mdi);
			mdi.height = 200;
			status = MicroDisplayInit::InitParameter(mdi);
			if (status < 0)
			{
				return -1;
			}
			MicroDisplayInit::CreateBufferWithOutDiplay(mdi);
			//MicroDisplayInit::CreateBufferWithDiplay(mdi);
			MicroDisplayControler::FreeRunningFocusing(mdi);
			MicroDisplayInit::Release(mdi);
			init();
		}
		else
			Sleep(10);
	} while (input != "q");
	exitFlag = true;
	MicroDisplayInit::Release(mdi);
	Sleep(500);
	return 0;
}
