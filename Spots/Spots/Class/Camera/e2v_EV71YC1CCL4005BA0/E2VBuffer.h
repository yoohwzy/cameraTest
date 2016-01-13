#pragma once


#include <opencv2/opencv.hpp>

using namespace std;

class E2VBuffer
{
public:
	E2VBuffer(int width, bool isGray = false);
	//E2VBuffer(int width, int length = 20000, bool isGray = false);
	~E2VBuffer()
	{

	};
	static const int BufferLength = 20000;


	//���OPENCV��ɫ��ģʽ��
	int GetColorType(){ return colorType; };
	//��ȡ��ǰ��дָ��
	int GetWriteIndex(){ return WriteIndex; };

	//��һ��д��ѭ������
	void WriteData(cv::Mat oneline);
	//��ѭ�������ж�ȡͼƬ������ʼ��������������
	void ReadData(int startIndex,int endIndex);
	cv::Mat Buffer;
private:
	int colorType = CV_8U;
	int WriteIndex = 0;
};


