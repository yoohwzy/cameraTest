#pragma once

#include "..\globle.h"
#include "..\Class\MicroDisplay\MicroDisplayInit.h"
#include "..\Class\MicroDisplay\MicroDisplayControler.h"
#include "..\Class\BufferStorage.h"
#include "..\Class\VirtualCamera.h"
#include "..\Class\BlocksDetector.h"
#include "..\Class\Processor.h"

#include "StateMachine.h"

#include <thread>
#include "..\mc100.h"



//@description	本类作为整个检测系统软件的控制类，负责控制
//				程序的各个功能，包括生产者、消费者、监视者
//				进程的控制，该类与状态机类耦合
//@author VShawn
//@last modify date 2015-9-30 16:09:57 By VShawn
class Contoller
{
private:
	int DelayTime = 410;//触发后延时(ms)




	StateMachine sofeState;
	StateMachine producerState;//生产者状态
	StateMachine customerState;//消费者状态

	BufferStorage s;
	int status = 0;
	MicroDisplayInit mdi;
	VirtualCamera *vc;

	/*********两个并行进程*********/
	//监听采集卡状态进程
	void IoCardWatcherThread();
	//用于控制生产者 消费者的启动
	void RunnerThread();

	void producerThread();
	void customerThread();


	/***********Runner的两个并行子进程**************/
public:
	const bool USING_VIRTUAL_CAMERA = 1;//是否使用虚拟摄像头 1使用 0用E2V
	//初始化
	Contoller();
	~Contoller(){ delete(vc); };

};

