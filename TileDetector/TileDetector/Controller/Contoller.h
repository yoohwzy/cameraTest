#pragma once

#include "../globle.h"
#include "../Class/MicroDisplay/MicroDisplayInit.h"
#include "../Class/MicroDisplay/MicroDisplayControler.h"
#include "../Class/BufferStorage.h"
#include "../Class/VirtualCamera.h"
#include "../Class/BlocksDetector.h"
#include "../Class/EdgeDetector.h"
#include "../Class/Processor.h"

#include "StateMachine.h"

#include <thread>
#include "../mc100.h"



//@description	������Ϊ�������ϵͳ����Ŀ����࣬�������
//				����ĸ������ܣ����������ߡ������ߡ�������
//				���̵Ŀ��ƣ�������״̬�������
//@author VShawn
//@last modify date 2015-9-30 16:09:57 By VShawn
class Contoller
{
private:
	bool USING_VIRTUAL_CAMERA = 1;//�Ƿ�ʹ����������ͷ 1ʹ�� 0��E2V
	bool IoCardOK = 0;//�ɼ����Ƿ��ʼ���ɹ�
	int DelayTime = 410;//��������ʱ(ms)


	BufferStorage s;
	int status = 0;
	MicroDisplayInit mdi;
	VirtualCamera vc;

	/*********�������н���*********/
	//�����ɼ���״̬����
	void IoCardWatcherThread();
	//���ڿ��������� �����ߵ�����
	void RunnerThread();

	void producerThread();
	void threeInOne();
	void customerThread();

	/***********Runner�����������ӽ���**************/

	//ʹ����ʵ�����ʼ��
	void initForE2V();
	//ʹ�����������ʼ��
	void initForVC(string virtualImg);

public:
	SofeState sofeState;//����״̬
	ProducerState producerState;//������״̬
	CustomerState customerState;//������״̬

	//��ʼ��
	Contoller(string virtualImg = "");
	~Contoller(){ };
};

