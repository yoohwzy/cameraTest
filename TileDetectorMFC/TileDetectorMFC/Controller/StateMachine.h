#pragma once

#include "../stdafx.h"


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
	SS_WATCHING,//���ڼ�ش���������
	SS_ACTIVATED,//��Ǵ��������������ȴ������ɼ�
	SS_EXIT//��������
};