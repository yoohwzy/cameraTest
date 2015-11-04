#pragma once

#include "../stdafx.h"


enum ProducerState//生产者状态
{
	PS_WAITING,//等待触发
	PS_GRABBING,//正在采集图像
	PS_GRABBING_END,//采图结束
};
enum CustomerState//消费者状态
{
	CS_WAITING,//等待触发
	CS_PROCESSING,//正在处理图像
};
enum SofeState//软件状态
{
	SS_BEFORE_INIT = -1, //尚未初始化
	SS_WATCHING,//正在监控传感器触发
	SS_ACTIVATED,//标记触发器被触发，等待启动采集
	SS_EXIT//结束程序
};