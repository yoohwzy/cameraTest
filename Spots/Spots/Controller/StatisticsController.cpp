#include "StatisticsController.h"


StatisticsController::StatisticsController()
{
}


StatisticsController::~StatisticsController()
{
}

int StatisticsController::TodayAll = 0;
int StatisticsController::TodayA = 0;
int StatisticsController::TodayB = 0;
int StatisticsController::TodayC = 0;
int StatisticsController::TodayRejected = 0;

int StatisticsController::MonthAll = 0;
int StatisticsController::MonthA = 0;
int StatisticsController::MonthB = 0;
int StatisticsController::MonthC = 0;
int StatisticsController::MonthRejected = 0;

int StatisticsController::YearAll = 0;
int StatisticsController::YearA = 0;
int StatisticsController::YearB = 0;
int StatisticsController::YearC = 0;
int StatisticsController::YearRejected = 0;

bool StatisticsController::InitDate()
{
	ASqlHelper::CnnStr = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=D://cameraTest//Spots//��שȱ�ݼ�����ݿ�.mdb;Persist Security Info=False";
	//update �������� set ���� = format(now(), 'yyyy-mm-dd ')


	//�����������

	_RecordsetPtr record = ASqlHelper::ExecuteRecordset("SELECT * FROM ��ʷ��¼ WHERE ���� = format(now(), 'yyyy-mm-dd ')");
	if (record == NULL)
	{
		initToday();
	}
	else
	{
		StatisticsController::TodayAll = (int)record->GetCollect("���ղ���");
		StatisticsController::TodayA = (int)record->GetCollect("A��");
		StatisticsController::TodayB = (int)record->GetCollect("B��");
		StatisticsController::TodayC = (int)record->GetCollect("C��");
		StatisticsController::TodayRejected = (int)record->GetCollect("���ϸ�����");
		ASqlHelper::CloseCnn();
	}


	//����������

	_RecordsetPtr record2 = ASqlHelper::ExecuteRecordset(
		"SELECT SUM(���ղ���) AS [�ܲ���],SUM(A��) AS [sumA��],SUM(B��) AS [sumB��],SUM(C��) AS [sumC��],SUM(���ϸ�����) AS [sum���ϸ�����] FROM ��ʷ��¼ WHERE ���� >= format(now(), 'yyyy-mm-01 ')"
		);
	if (record2 != NULL)
	{
		StatisticsController::MonthAll = (int)record2->GetCollect("�ܲ���");
		StatisticsController::MonthA = (int)record2->GetCollect("sumA��");
		StatisticsController::MonthB = (int)record2->GetCollect("sumB��");
		StatisticsController::MonthC = (int)record2->GetCollect("sumC��");
		StatisticsController::MonthRejected = (int)record2->GetCollect("sum���ϸ�����");
		ASqlHelper::CloseCnn();
	}


	//����������

	_RecordsetPtr record3 = ASqlHelper::ExecuteRecordset(
		"SELECT SUM(���ղ���) AS [�ܲ���],SUM(A��) AS [sumA��],SUM(B��) AS [sumB��],SUM(C��) AS [sumC��],SUM(���ϸ�����) AS [sum���ϸ�����] FROM ��ʷ��¼ WHERE ���� >= format(now(), 'yyyy-01-01 ')"
		);
	if (record3 != NULL)
	{
		StatisticsController::YearAll = (int)record3->GetCollect("�ܲ���");
		StatisticsController::YearA = (int)record3->GetCollect("sumA��");
		StatisticsController::YearB = (int)record3->GetCollect("sumB��");
		StatisticsController::YearC = (int)record3->GetCollect("sumC��");
		StatisticsController::YearRejected = (int)record3->GetCollect("sum���ϸ�����");
		ASqlHelper::CloseCnn();
	}

	return true;
}

void StatisticsController::AddTodayA()
{
	TodayA++;
	TodayAll++;
	MonthA++;
	MonthAll++;
	YearA++;
	YearAll++;
	updateTodayAccess();
}
void StatisticsController::AddTodayB()
{
	TodayB++;
	TodayAll++;
	MonthB++;
	MonthAll++;
	YearB++;
	YearAll++;
	updateTodayAccess();
}
void StatisticsController::AddTodayC()
{
	TodayC++;
	TodayAll++;
	MonthC++;
	MonthAll++;
	YearC++;
	YearAll++;
	updateTodayAccess();
}
void StatisticsController::AddTodayRejected()
{
	TodayRejected++;
	TodayAll++;
	MonthRejected++;
	MonthAll++;
	YearRejected++;
	YearAll++;
	updateTodayAccess();
}
















/**********************************************************/
void StatisticsController::initToday()
{
	string sql = "INSERT INTO ��ʷ��¼ (����,���ղ���,A��,B��,C��,���ϸ�����,�ϸ���) VALUES (format(now(), 'yyyy-mm-dd '),0,0,0,0,0,0)";
	ASqlHelper::ExecuteNonQuery(sql);
	StatisticsController::TodayAll = 0;
	StatisticsController::TodayA = 0;
	StatisticsController::TodayB = 0;
	StatisticsController::TodayC = 0;
	StatisticsController::TodayRejected = 0;
}
void StatisticsController::updateTodayAccess()
{
	//string sql = "UPDATE [��ʷ��¼] SET [A��] = 2,[B��] = 2,[C��] = 3,[���ϸ�����] = 2,[���ղ���] = 0,[�ϸ���] = 0.6432 WHERE [����] = format(now(), 'yyyy-mm-dd ')";

	char buffer[255];
	sprintf(buffer, "UPDATE [��ʷ��¼] SET [A��] = %d,[B��] = %d,[C��] = %d,[���ϸ�����] = %d,[���ղ���] = %d,[�ϸ���] = %f WHERE [����] = format(now(), 'yyyy-mm-dd ')", 
		StatisticsController::TodayA,
		StatisticsController::TodayB, 
		StatisticsController::TodayC, 
		StatisticsController::TodayRejected, 
		StatisticsController::TodayAll,
		(1 - (StatisticsController::TodayRejected / (float)StatisticsController::TodayAll))
		);/*������ֵ*/
	string sql(buffer);
	ASqlHelper::ExecuteNonQuery(sql);
}