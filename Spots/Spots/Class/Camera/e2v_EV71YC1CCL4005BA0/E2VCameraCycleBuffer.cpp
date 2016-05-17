#include "E2VCameraCycleBuffer.h"
#include <Class/Debug/MFCConsole.h>
#include <thread>


//�����������ѭ������
//������ͼ���ȣ�4k�����Ƿ�Ϊ�Ҷ�ͼ��bool��
E2VCycleBuffer::E2VCycleBuffer(int width, bool isGray)
{
	colorType = isGray ? CV_8U : CV_8UC3;
	if (isGray)
		Buffer = cv::Mat(BufferLength, width, colorType, cv::Scalar(0, 0, 0));
	else
		Buffer = cv::Mat(BufferLength, width, colorType, cv::Scalar(0, 0, 0));
}
//��һ��д��ѭ������
void E2VCycleBuffer::WriteData(cv::Mat oneline)
{
	//double t1 = (double)cv::getTickCount();//��ͼ��ʱ ΢��
	cv::Mat roi = Buffer(cv::Rect(0, WriteIndex, Buffer.cols, 1));

	//������βſ죿
	//cv::addWeighted(roi, 0, oneline, 1.0, 0, roi);  //debug 48΢�� release 5.28΢��
	oneline.copyTo(roi);	//debug 2΢�� release 1.5΢��


	++WriteIndex;
	if (WriteIndex >= BufferLength)
		WriteIndex = 0;

	//t1 = ((double)cv::getTickCount() - t1) * 1000000 / cv::getTickFrequency();
	//std::cout << t1 << endl;
}
//������ʼ����������ȡһ��ͼ��
//��������startLine = 0,endLine = 2�������һ��0�п�ʼ������Ϊ3���ص�ͼ��
//����startLine = 19000,endLine = 0�������һ��19000�п�ʼ������Ϊ1001���ص�ͼ��
cv::Mat E2VCycleBuffer::GetImage(int startLine, int endLine)
{
	double t = cv::getTickCount();
	//Խ�緵�ؿ�
	if (startLine >= E2VCycleBuffer::BufferLength ||
		startLine < 0 ||
		endLine >= E2VCycleBuffer::BufferLength ||
		endLine < 0
		)
	{
		return cv::Mat(0, 0, colorType);
	}


	if (endLine >= startLine)
	{
		int length = endLine - startLine + 1;
		cv::Mat result = Buffer(cv::Rect(0, startLine, Buffer.cols, length)).clone();

		t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
		stringstream ss;
		ss << "GetImage :" << t << "ms " << result.rows << " * " << result.cols << endl;
		MFCConsole::Output(ss.str());
		return result;
	}
	else
	{
		int length1 = (E2VCycleBuffer::BufferLength - startLine);
		int length2 = (endLine + 1);
		cv::Mat result(length1 + length2, Buffer.cols, colorType, cv::Scalar(0, 0, 0));
		cv::Mat roi1 = result(cv::Rect(0, 0, Buffer.cols, length1));
		cv::Mat roi2 = result(cv::Rect(0, length1, Buffer.cols, length2));

		Buffer(cv::Rect(0, startLine, Buffer.cols, length1)).copyTo(roi1);	//debug 2΢�� release 1.5΢��
		Buffer(cv::Rect(0, 0, Buffer.cols, length2)).copyTo(roi2);

		t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
		stringstream ss;
		ss << "GetImage :" << t << "ms " << result.rows << " * " << result.cols << endl;
		MFCConsole::Output(ss.str());
		return result;
	}
}






















E2VCameraCycleBuffer::E2VCameraCycleBuffer(int width, int colorType, int frameTimeUS, int boardID, int Camport)
	: E2VCameraModel(width, 0, colorType, boardID, Camport)
{
	if (colorType == E2VCameraCycleBuffer::GRAY)
		P_CycleBuffer = new E2VCycleBuffer(4096, true);
	else
		P_CycleBuffer = new E2VCycleBuffer(4096, false);


	//������ͼѭ��
	std::thread t_scanThread(std::mem_fn(&E2VCameraCycleBuffer::freeRun), this);
	auto tn = t_scanThread.native_handle();
	SetThreadPriority(tn, THREAD_PRIORITY_HIGHEST);
	t_scanThread.detach();


	FrameTimeUS = frameTimeUS;
}
E2VCameraCycleBuffer::~E2VCameraCycleBuffer()
{
	if (P_CycleBuffer != NULL)
	{
		delete P_CycleBuffer;
		P_CycleBuffer = NULL;
	}

	StartFlag = false;
	Sleep(100);
}
bool E2VCameraCycleBuffer::freeRun()
{
	if (HasBeenInited)
	{
		//acquire one image per subbuffer
		//1.fg
		//2.�ɼ�����
		//3.�ɼ�֡��
		//4.��
		//5.�����ַ
		if ((Fg_AcquireEx(E2VCameraModel::fg, _camPort, GRAB_INFINITE, ACQ_STANDARD, memHandle)) < 0) {
			fprintf(stderr, "Fg_AcquireEx() failed: %s\n", Fg_getLastErrorDescription(fg));
			errorMessageWait(); return false;
		}

		frameindex_t fcount = 0;
		frameindex_t last_pic_nr = 0;
		frameindex_t cur_pic_nr;

		cv::Mat OriginalImage;

		//��������
		while (StartFlag)
		{
			double t1 = (double)cv::getTickCount();//��ͼ��ʱ ΢��
			cur_pic_nr = Fg_getLastPicNumberBlockingEx(fg, last_pic_nr + 1, _camPort, 100, memHandle);
			if (cur_pic_nr < 0)
			{
				Fg_stopAcquire(fg, _camPort);
				errorMessageWait(); return false;
			}
			unsigned char *bytePtr = (unsigned char*)Fg_getImagePtrEx(fg, cur_pic_nr, 0, memHandle);
			//if (nId != -1)
			//	::DrawBuffer(nId, Fg_getImagePtrEx(fg, lastPicNr, 0, memHandle), (int)lastPicNr, "");

			OriginalImage = cv::Mat(_frameHeight, _width, CV_8UC3, bytePtr);
			if (_colorType == GRAY)
			{
				cv::cvtColor(OriginalImage, OriginalImage, CV_BGR2GRAY);
			}
			P_CycleBuffer->WriteData(OriginalImage);
			t1 = ((double)cv::getTickCount() - t1) * 1000000 / cv::getTickFrequency();


			//�ȴ���FrameTimeUS
			if (FrameTimeUS > 0 && t1 < FrameTimeUS)
			{
				double t2 = (double)cv::getTickCount();
				double tickCountSpan = (FrameTimeUS - t1) * cv::getTickFrequency() / 1000000;
				while (((double)cv::getTickCount() - t2) < tickCountSpan)
				{
					//stringstream ss;
					//ss << ((double)cv::getTickCount() - t2) << " " << tickCountSpan << endl;
					//printf_globle(ss.str());
				}
			}
			fcount++;
		}

		Fg_stopAcquireEx(fg, _camPort, memHandle, 0);
		return true;
	}
	else
		return false;
}