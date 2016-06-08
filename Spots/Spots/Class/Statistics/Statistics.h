#pragma once

#include <sstream>
#include <string.h>
using namespace std;
// ����������������̬�࣬����ACCESS���ݿ�����д���ȡ
// ������UI�ϵ�����ͳ����
class Statistics
{
public:
	Statistics();
	~Statistics();
	static unsigned int TodayTiggerIndex;//���յĴ���������
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
	static bool InitDate(string ACCDB_PATH);

	//��úϸ���
	static double GetFineRate();

	//���ղ�����һ
	static void AddTodayA();
	static void AddTodayB();
	static void AddTodayC();
	static void AddTodayRejected();
private:
	//���ݿ��Ƿ�ɹ����أ�
	static bool accessDBHasOpened;
	//��ʼ�����յļ�¼����
	static void initToday();
	//�����ݿ��и��½�������
	static void updateTodayAccess();
};

