#pragma once

#include "E2VCameraModel.h"


using namespace std;


//E2V����Ĳ����࣬Ϊֱ�Ӵ������ͼ�汾
//@author VShawn
class E2VCameraDirectRead :public E2VCameraModel
{
public:
	//��ʼ���ɼ�����
	//1.�ɼ�ͼ����
	//2.�ɼ�ͼ��߶�
	//3.ɫ��ģʽ ö�� RGB OR GRAY
	//4.�ɼ������Logical number of the board.��Ĭ��Ϊ0��
	//5.�ɼ���Port�ںţ�PORT_A(Ĭ��) OR PORT_B
	E2VCameraDirectRead(int width, int height, int colorType = RGB,int boardID = 0, int Camport = PORT_A);
	~E2VCameraDirectRead();

	bool StartFlag = true;

	cv::Mat GetImage() override;
private:


	//��ʼѭ����ͼ
	bool freeRun();
};