#pragma once

class StateMachine
{
public:
	StateMachine(){};
	~StateMachine(){};

	enum ProducerState//������״̬
	{
		PS_WAITING,//�ȴ�����
		PS_GRABBING,//���ڲɼ�ͼ��
		PS_GRABBING_END,//��ͼ����
	};
	enum CustomerState//������״̬
	{
		CS_WAITING,//�ȴ�����
		CS_PROCESSING,//���ڴ���ͼ��
	};
	enum SofeState//���״̬
	{
		SS_BEFORE_INIT = -1, //��δ��ʼ��
		SS_INITED,//�Ѿ���ʼ������δ��ʼ���
		SS_WATCHING,//���ڼ�ش���������
		SS_ACTIVATED,//������������
		SS_EXIT//��������
	};
	void SetState(int state)
	{
		_cur_state = state;
	}
	int GetState() { return _cur_state; }
private:
	int _cur_state;
};

