// HelloWorldFreeRunLine.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "globle.h"
#include "Class\MicroDisplay\MicroDisplayInit.h"
#include "Class\MicroDisplay\MicroDisplayControler.h"
#include "Class\BufferStorage.h"
#include "Class\VirtualCamera.h"
#include "Class\BlocksDetector.h"
#include <thread>

BufferStorage s;
int status = 0;
MicroDisplayInit mdi;
VirtualCamera vc;

const bool USING_VIRTUAL_CAMERA = false;//�Ƿ�ʹ����������ͷ 1ʹ�� 0��E2V


bool producerEndFlag = false, customerEndFlag = false;


//������
void producer()
{
	double t = (double)cv::getTickCount();

	if (!USING_VIRTUAL_CAMERA)
	{
		if (MicroDisplayControler::FreeRunning(mdi, s) < 0)
		{
			ErrorMessageWait(mdi.fg);
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

	//��������߹�������
	producerEndFlag = true;
}
//������
void customer()
{
	BlocksDetector bd = BlocksDetector(&s, &mdi);

	//��ʼ��ʱ
	double t1 = (double)cv::getTickCount();

	//�����㷨

	//���˵ڼ���
	int i = 0;
	//״̬���0��ʾ������-1��ʾ��Ҫ�ȴ���һ֡д��
	int flag = 0;
	do{
		if (s.EndWriteFlag)
			break;
		cv::Mat f;
		flag = s.GetFrame(f);
		if (flag == -1)
		{
			Sleep(1);
			continue;
		}
		if (flag == 0)
			break;

		//����㷨
		cv::Mat oneLine = s.NowBufferImg(cv::Rect(0, i, mdi.width, 1));
		int elementCount = mdi.width;//ÿ��Ԫ����
		uchar* lineheadRGB = oneLine.ptr<uchar>(0);//ÿ�е���ʼ��ַ

		i++;
	} while (flag != 0);
	t1 = ((double)cv::getTickCount() - t1) / cv::getTickFrequency();
	std::cout << "���д�����ʱ��" << t1 << endl;

	while (!s.EndWriteFlag)
	{
		Sleep(10);
	}


	double t = (double)cv::getTickCount();
	bd.Start();
	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	std::cout << "�ǲ��д�����ʱ��" << t << endl;

#ifdef OUTPUT_DEBUG_INFO
	if (OUTPUT_DEBUG_INFO)
	{
		cv::imwrite("samples/drowDebugDetectLR.jpg", bd.drowDebugDetectLR);
		cv::imwrite("samples/drowDebugDetectUD.jpg", bd.drowDebugDetectUD);
		cv::imwrite("samples/drowDebugResult.jpg", bd.drowDebugResult);
	}
#endif
	//��������߹�������
	customerEndFlag = true;
}

int main()
{
	mdi.colorType = mdi.GRAY;
	mdi.width = 4096;
	mdi.height = 1;
	mdi.MaxPics = 10000;//�ɼ�����֡ͼ��
	s = BufferStorage(mdi);

	if (!USING_VIRTUAL_CAMERA)
	{
		//��ʼ���ɼ���
		//status = MicroDisplayInit::InitParameter(mdi);
		status = MicroDisplayInit::InitLoad(mdi,"4096gray1lineGainX2.mcf");
		if (status < 0)
		{
			ErrorMessageWait(mdi.fg);
			return -1;
		}
		//MicroDisplayInit::CreateBufferWithOutDiplay(mdi);
		MicroDisplayInit::CreateBufferWithDiplay(mdi);
	}
	else
	{
		//��ʼ���������
		//vc = VirtualCamera(mdi, "��Ʒ2_o.jpg");
		//vc = VirtualCamera(mdi, "��שȱ��2_o.jpg");
		vc = VirtualCamera(mdi, "��ש����1_o.jpg");
	}




	int grabbingIndex = 0;
	//��ѭ��
	string input;
	do{
		std::cout << "����1��ʼ��ͼ��2������ս������ţ�q�˳���";
		std::cin >> input;
		
		if (input == "1")
		{
			grabbingIndex += 1;
			if (grabbingIndex > 1000) grabbingIndex = 1;

			//����

			//��ʼ������
			s.Start();

			std::thread t1(producer);
			t1.join();

			std::thread t2(customer);
			t2.join();

			//�ȴ������̴߳������
			while (!customerEndFlag || !producerEndFlag)
			{
				Sleep(10);
			}

			string p1;
			stringstream ss1;
			ss1 << "samples/result" << grabbingIndex << "_o.jpg";
			ss1 >> p1;
			string p2;
			stringstream ss2;
			ss2 << "samples/result" << grabbingIndex << "_x3.jpg";
			ss2 >> p2;
			//cv::imwrite(p1, s.NowBuffer);
			//cv::imwrite("result1.jpg", s.NowBufferGray);
			cv::imwrite(p2, s.NowBufferImg);
		}
		if (input == "2")
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
					ErrorMessageWait(mdi.fg);
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
		else
			Sleep(10);
	} while (input != "q");

	MicroDisplayInit::Release(mdi);
	return 0;
}