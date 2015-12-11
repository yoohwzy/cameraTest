#pragma once
#include "../stdafx.h"
#include <opencv2/opencv.hpp>
using namespace std;


////ͼ��洢��
//class BufferStorage
//{
//public:
//	BufferStorage();
//	~BufferStorage();
//
//	//ԭʼͼ��
//	cv::Mat OriginalImage;
//	//����һͼ��
//	cv::Mat Image;
//};
//








//��������ɼ�ʱͼƬд�뻺����
//@author Shawn
class GrabbingBuffer
{
public:
	//1.��������ɼ�����
	//2.�ɼ����
	GrabbingBuffer(int frameCount, int width);
	~GrabbingBuffer();

	//���кϳ�һ�У�
	int NinOne = 3;
	int WriteIndex = 0;
	int ReadIndex = 0;

	//ԭʼͼ��
	cv::Mat OriginalImage;

	//����һͼ��
	cv::Mat Image;

	void Start(){
		WriteIndex = 0;
		ReadIndex = 0;
	};
	//������ͷ����һ֡,�����Ƿ�ֹͣ�ɼ�
	bool AddFrame(cv::Mat& frame);
	//�����е���һ�С�
	void ThreeInOne(int lineIndex);
private:

	int _frameCount = 0;
	int _width = 0;
};

