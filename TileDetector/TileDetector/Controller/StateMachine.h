#pragma once

class StateMachine
{
public:

	StateMachine();
	~StateMachine();

	enum State
	{
		BEFORE_INIT = -1, //尚未初始化
		INITED,//已经初始化，还未开始监控
		WATCHING,//正在监控传感器触发
		GRABBING,//正在采集图像
		PROCESSING,//正在处理图像
		EXIT//结束程序
	};
	void SetState(State state)
	{
		_cur_state = state;
	}
	State GetState() { return (State)_cur_state; }

private:
	int _cur_state;
};

