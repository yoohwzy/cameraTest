#include "Statistics.h"

#include <cstdio>
#include <Class\Access\ASqlHelper.h>

Statistics::Statistics()
{
}


Statistics::~Statistics()
{
}

int Statistics::TodayAll = 0;
int Statistics::TodayA = 0;
int Statistics::TodayB = 0;
int Statistics::TodayC = 0;
int Statistics::TodayRejected = 0;

int Statistics::MonthAll = 0;
int Statistics::MonthA = 0;
int Statistics::MonthB = 0;
int Statistics::MonthC = 0;
int Statistics::MonthRejected = 0;

int Statistics::YearAll = 0;
int Statistics::YearA = 0;
int Statistics::YearB = 0;
int Statistics::YearC = 0;
int Statistics::YearRejected = 0;

bool Statistics::accessDBHasOpened = false;

bool Statistics::InitDate()
{
	if (!ASqlHelper::SetConStr("Provider=Microsoft.ACE.OLEDB.12.0;Data Source=E://��Ŀ����//cameraTest//Spots//��שȱ�ݼ�����ݿ�.mdb;Persist Security Info=False"))
	{
		accessDBHasOpened = false;
		return false;
	}
	else
	{
		accessDBHasOpened = true;
	}

	//update �������� set ���� = format(now(), 'yyyy-mm-dd ')

	//�����������
	_RecordsetPtr record = ASqlHelper::ExecuteRecordset("SELECT * FROM ��ʷ��¼ WHERE ���� = format(now(), 'yyyy-mm-dd ')");
	if (record == NULL)
	{
		initToday();
	}
	else
	{
		Statistics::TodayAll = (int)record->GetCollect("���ղ���");
		Statistics::TodayA = (int)record->GetCollect("A��");
		Statistics::TodayB = (int)record->GetCollect("B��");
		Statistics::TodayC = (int)record->GetCollect("C��");
		Statistics::TodayRejected = (int)record->GetCollect("���ϸ�����");
		ASqlHelper::CloseCnn();
	}


	//����������

	_RecordsetPtr record2 = ASqlHelper::ExecuteRecordset(
		"SELECT SUM(���ղ���) AS [�ܲ���],SUM(A��) AS [sumA��],SUM(B��) AS [sumB��],SUM(C��) AS [sumC��],SUM(���ϸ�����) AS [sum���ϸ�����] FROM ��ʷ��¼ WHERE ���� >= format(now(), 'yyyy-mm-01 ')"
		);
	if (record2 != NULL)
	{
		Statistics::MonthAll = (int)record2->GetCollect("�ܲ���");
		Statistics::MonthA = (int)record2->GetCollect("sumA��");
		Statistics::MonthB = (int)record2->GetCollect("sumB��");
		Statistics::MonthC = (int)record2->GetCollect("sumC��");
		Statistics::MonthRejected = (int)record2->GetCollect("sum���ϸ�����");
		ASqlHelper::CloseCnn();
	}


	//����������

	_RecordsetPtr record3 = ASqlHelper::ExecuteRecordset(
		"SELECT SUM(���ղ���) AS [�ܲ���],SUM(A��) AS [sumA��],SUM(B��) AS [sumB��],SUM(C��) AS [sumC��],SUM(���ϸ�����) AS [sum���ϸ�����] FROM ��ʷ��¼ WHERE ���� >= format(now(), 'yyyy-01-01 ')"
		);
	if (record3 != NULL)
	{
		Statistics::YearAll = (int)record3->GetCollect("�ܲ���");
		Statistics::YearA = (int)record3->GetCollect("sumA��");
		Statistics::YearB = (int)record3->GetCollect("sumB��");
		Statistics::YearC = (int)record3->GetCollect("sumC��");
		Statistics::YearRejected = (int)record3->GetCollect("sum���ϸ�����");
		ASqlHelper::CloseCnn();
	}

	return true;
}

void Statistics::AddTodayA()
{
	TodayA++;
	TodayAll++;
	MonthA++;
	MonthAll++;
	YearA++;
	YearAll++;
	updateTodayAccess();
}
void Statistics::AddTodayB()
{
	TodayB++;
	TodayAll++;
	MonthB++;
	MonthAll++;
	YearB++;
	YearAll++;
	updateTodayAccess();
}
void Statistics::AddTodayC()
{
	TodayC++;
	TodayAll++;
	MonthC++;
	MonthAll++;
	YearC++;
	YearAll++;
	updateTodayAccess();
}
void Statistics::AddTodayRejected()
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
void Statistics::initToday()
{
	string sql = "INSERT INTO ��ʷ��¼ (����,���ղ���,A��,B��,C��,���ϸ�����,�ϸ���) VALUES (format(now(), 'yyyy-mm-dd '),0,0,0,0,0,0)";
	ASqlHelper::ExecuteNonQuery(sql);
	Statistics::TodayAll = 0;
	Statistics::TodayA = 0;
	Statistics::TodayB = 0;
	Statistics::TodayC = 0;
	Statistics::TodayRejected = 0;
}
void Statistics::updateTodayAccess()
{
	if (accessDBHasOpened)
	{
		//string sql = "UPDATE [��ʷ��¼] SET [A��] = 2,[B��] = 2,[C��] = 3,[���ϸ�����] = 2,[���ղ���] = 0,[�ϸ���] = 0.6432 WHERE [����] = format(now(), 'yyyy-mm-dd ')";
		char buffer[255];
		sprintf(buffer, "UPDATE [��ʷ��¼] SET [A��] = %d,[B��] = %d,[C��] = %d,[���ϸ�����] = %d,[���ղ���] = %d,[�ϸ���] = %f WHERE [����] = format(now(), 'yyyy-mm-dd ')",
			Statistics::TodayA,
			Statistics::TodayB,
			Statistics::TodayC,
			Statistics::TodayRejected,
			Statistics::TodayAll,
			(1 - (Statistics::TodayRejected / (float)Statistics::TodayAll))
			);/*������ֵ*/
		string sql(buffer);
		ASqlHelper::ExecuteNonQuery(sql);
	}
}