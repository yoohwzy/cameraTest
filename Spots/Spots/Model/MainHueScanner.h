#pragma once
#include <Controller/ControllerModel.h>
#include <thread>

//�����㷨�ۺ���
#include <Class\Debug\MFCConsole.h>
class ControllerModel;// ����໥�����
#include <Class\Camera\MVCAM\MVCAM.h>

//��ɫ��������
//�����ж���������ȱ�ݣ�����������պ�����������ɫ����
//ʵ������ֱ�ӿ������������ͼ�߳�
class MainHueScanner
{
public:
	MainHueScanner(ControllerModel *pController);
	~MainHueScanner();
	void Run(int SN){ sn = SN; };
	void Pause(){ sn = 0; };
	void Stop(){ stopFlag = true; };
	bool HasInited = false;
private:
	ControllerModel *p_Controller = NULL;
	int sn = 0;//ͼ�����кţ�Ϊ0ʱ��ͣ
	bool stopFlag = false;
	MVCAM mvcam;
	void scanImg();
	//�����Ƿ���
	int analysis(cv::Mat);
};

