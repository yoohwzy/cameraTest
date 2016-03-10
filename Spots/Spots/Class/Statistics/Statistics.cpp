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
	if (!ASqlHelper::SetConStr("Provider=Microsoft.ACE.OLEDB.12.0;Data Source=E://项目资料//cameraTest//Spots//瓷砖缺陷检测数据库.mdb;Persist Security Info=False"))
	{
		accessDBHasOpened = false;
		return false;
	}
	else
	{
		accessDBHasOpened = true;
	}

	//update 程序设置 set 日期 = format(now(), 'yyyy-mm-dd ')

	//载入今日数据
	_RecordsetPtr record = ASqlHelper::ExecuteRecordset("SELECT * FROM 历史记录 WHERE 日期 = format(now(), 'yyyy-mm-dd ')");
	if (record == NULL)
	{
		initToday();
	}
	else
	{
		Statistics::TodayAll = (int)record->GetCollect("今日产量");
		Statistics::TodayA = (int)record->GetCollect("A类");
		Statistics::TodayB = (int)record->GetCollect("B类");
		Statistics::TodayC = (int)record->GetCollect("C类");
		Statistics::TodayRejected = (int)record->GetCollect("不合格数量");
		ASqlHelper::CloseCnn();
	}


	//载入月数据

	_RecordsetPtr record2 = ASqlHelper::ExecuteRecordset(
		"SELECT SUM(今日产量) AS [总产量],SUM(A类) AS [sumA类],SUM(B类) AS [sumB类],SUM(C类) AS [sumC类],SUM(不合格数量) AS [sum不合格数量] FROM 历史记录 WHERE 日期 >= format(now(), 'yyyy-mm-01 ')"
		);
	if (record2 != NULL)
	{
		Statistics::MonthAll = (int)record2->GetCollect("总产量");
		Statistics::MonthA = (int)record2->GetCollect("sumA类");
		Statistics::MonthB = (int)record2->GetCollect("sumB类");
		Statistics::MonthC = (int)record2->GetCollect("sumC类");
		Statistics::MonthRejected = (int)record2->GetCollect("sum不合格数量");
		ASqlHelper::CloseCnn();
	}


	//载入年数据

	_RecordsetPtr record3 = ASqlHelper::ExecuteRecordset(
		"SELECT SUM(今日产量) AS [总产量],SUM(A类) AS [sumA类],SUM(B类) AS [sumB类],SUM(C类) AS [sumC类],SUM(不合格数量) AS [sum不合格数量] FROM 历史记录 WHERE 日期 >= format(now(), 'yyyy-01-01 ')"
		);
	if (record3 != NULL)
	{
		Statistics::YearAll = (int)record3->GetCollect("总产量");
		Statistics::YearA = (int)record3->GetCollect("sumA类");
		Statistics::YearB = (int)record3->GetCollect("sumB类");
		Statistics::YearC = (int)record3->GetCollect("sumC类");
		Statistics::YearRejected = (int)record3->GetCollect("sum不合格数量");
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
	string sql = "INSERT INTO 历史记录 (日期,今日产量,A类,B类,C类,不合格数量,合格率) VALUES (format(now(), 'yyyy-mm-dd '),0,0,0,0,0,0)";
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
		//string sql = "UPDATE [历史记录] SET [A类] = 2,[B类] = 2,[C类] = 3,[不合格数量] = 2,[今日产量] = 0,[合格率] = 0.6432 WHERE [日期] = format(now(), 'yyyy-mm-dd ')";
		char buffer[255];
		sprintf(buffer, "UPDATE [历史记录] SET [A类] = %d,[B类] = %d,[C类] = %d,[不合格数量] = %d,[今日产量] = %d,[合格率] = %f WHERE [日期] = format(now(), 'yyyy-mm-dd ')",
			Statistics::TodayA,
			Statistics::TodayB,
			Statistics::TodayC,
			Statistics::TodayRejected,
			Statistics::TodayAll,
			(1 - (Statistics::TodayRejected / (float)Statistics::TodayAll))
			);/*赋予数值*/
		string sql(buffer);
		ASqlHelper::ExecuteNonQuery(sql);
	}
}