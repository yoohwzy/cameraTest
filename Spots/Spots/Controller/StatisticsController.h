#pragma once

#include <string.h>
#include <cstdio>
using namespace std;
#include <Class\Access\ASqlHelper.h>
// ����������������̬�࣬����ACCESS���ݿ�����д���ȡ
// ������UI�ϵ�����ͳ����
class StatisticsController
{
public:
	StatisticsController();
	~StatisticsController();

	static int TodayAll;
	static int TodayA;
	static int TodayB;
	static int TodayC;
	static int TodayRejected;

	static int MonthAll;
	static int MonthA;
	static int MonthB;
	static int MonthC;
	static int MonthRejected;

	static int YearAll;
	static int YearA;
	static int YearB;
	static int YearC;
	static int YearRejected;

	// ��ʼ��ͳ�����ݣ������ݿ��ж�ȡ��ʷ��¼
	// �����Ƿ�ɹ�
	static bool InitDate();

	//��úϸ���
	static double GetFineRate();

	//���ղ�����һ
	static void AddTodayA();
	static void AddTodayB();
	static void AddTodayC();
	static void AddTodayRejected();
};

