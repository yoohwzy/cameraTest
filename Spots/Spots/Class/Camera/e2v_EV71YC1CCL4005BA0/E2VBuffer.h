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
	int GetWriteIndex()
	{ 
		return WriteIndex; 
	};
	//������ʼ����������ȡһ��ͼ��
	//��������startLine = 0,endLine = 2�������һ��0�п�ʼ������Ϊ3���ص�ͼ��
	//����startLine = 19000,endLine = 0�������һ��19000�п�ʼ������Ϊ1001���ص�ͼ��
	cv::Mat GetImage(int startLine, int endLine);


	//��һ��д��ѭ������
	void WriteData(cv::Mat oneline);
	//��ѭ�������ж�ȡͼƬ������ʼ��������������
	void ReadData(int startIndex,int endIndex);
	cv::Mat Buffer;
private:
	int colorType = CV_8U;
	int WriteIndex = 0;
};


