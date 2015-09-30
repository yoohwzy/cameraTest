#pragma once

class StateMachine
{
public:

	StateMachine();
	~StateMachine();

	enum State
	{
		BEFORE_INIT = -1, //��δ��ʼ��
		INITED,//�Ѿ���ʼ������δ��ʼ���
		WATCHING,//���ڼ�ش���������
		GRABBING,//���ڲɼ�ͼ��
		PROCESSING,//���ڴ���ͼ��
		EXIT//��������
	};
	void SetState(State state)
	{
		_cur_state = state;
	}
	State GetState() { return (State)_cur_state; }

private:
	int _cur_state;
};

