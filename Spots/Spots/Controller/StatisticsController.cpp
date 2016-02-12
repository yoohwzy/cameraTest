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
	ASqlHelper::CnnStr = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=D://cameraTest//Spots//瓷砖缺陷检测数据库.mdb;Persist Security Info=False";
	//update 程序设置 set 日期 = format(now(), 'yyyy-mm-dd ')


	//载入今日数据

	_RecordsetPtr record = ASqlHelper::ExecuteRecordset("SELECT * FROM 历史记录 WHERE 日期 = format(now(), 'yyyy-mm-dd ')");
	if (record == NULL)
	{
		initToday();
	}
	else
	{
		StatisticsController::TodayAll = (int)record->GetCollect("今日产量");
		StatisticsController::TodayA = (int)record->GetCollect("A类");
		StatisticsController::TodayB = (int)record->GetCollect("B类");
		StatisticsController::TodayC = (int)record->GetCollect("C类");
		StatisticsController::TodayRejected = (int)record->GetCollect("不合格数量");
		ASqlHelper::CloseCnn();
	}


	//载入月数据

	_RecordsetPtr record2 = ASqlHelper::ExecuteRecordset(
		"SELECT SUM(今日产量) AS [总产量],SUM(A类) AS [sumA类],SUM(B类) AS [sumB类],SUM(C类) AS [sumC类],SUM(不合格数量) AS [sum不合格数量] FROM 历史记录 WHERE 日期 >= format(now(), 'yyyy-mm-01 ')"
		);
	if (record2 != NULL)
	{
		StatisticsController::MonthAll = (int)record2->GetCollect("总产量");
		StatisticsController::MonthA = (int)record2->GetCollect("sumA类");
		StatisticsController::MonthB = (int)record2->GetCollect("sumB类");
		StatisticsController::MonthC = (int)record2->GetCollect("sumC类");
		StatisticsController::MonthRejected = (int)record2->GetCollect("sum不合格数量");
		ASqlHelper::CloseCnn();
	}


	//载入年数据

	_RecordsetPtr record3 = ASqlHelper::ExecuteRecordset(
		"SELECT SUM(今日产量) AS [总产量],SUM(A类) AS [sumA类],SUM(B类) AS [sumB类],SUM(C类) AS [sumC类],SUM(不合格数量) AS [sum不合格数量] FROM 历史记录 WHERE 日期 >= format(now(), 'yyyy-01-01 ')"
		);
	if (record3 != NULL)
	{
		StatisticsController::YearAll = (int)record3->GetCollect("总产量");
		StatisticsController::YearA = (int)record3->GetCollect("sumA类");
		StatisticsController::YearB = (int)record3->GetCollect("sumB类");
		StatisticsController::YearC = (int)record3->GetCollect("sumC类");
		StatisticsController::YearRejected = (int)record3->GetCollect("sum不合格数量");
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
	string sql = "INSERT INTO 历史记录 (日期,今日产量,A类,B类,C类,不合格数量,合格率) VALUES (format(now(), 'yyyy-mm-dd '),0,0,0,0,0,0)";
	ASqlHelper::ExecuteNonQuery(sql);
	StatisticsController::TodayAll = 0;
	StatisticsController::TodayA = 0;
	StatisticsController::TodayB = 0;
	StatisticsController::TodayC = 0;
	StatisticsController::TodayRejected = 0;
}
void StatisticsController::updateTodayAccess()
{
	//string sql = "UPDATE [历史记录] SET [A类] = 2,[B类] = 2,[C类] = 3,[不合格数量] = 2,[今日产量] = 0,[合格率] = 0.6432 WHERE [日期] = format(now(), 'yyyy-mm-dd ')";

	char buffer[255];
	sprintf(buffer, "UPDATE [历史记录] SET [A类] = %d,[B类] = %d,[C类] = %d,[不合格数量] = %d,[今日产量] = %d,[合格率] = %f WHERE [日期] = format(now(), 'yyyy-mm-dd ')", 
		StatisticsController::TodayA,
		StatisticsController::TodayB, 
		StatisticsController::TodayC, 
		StatisticsController::TodayRejected, 
		StatisticsController::TodayAll,
		(1 - (StatisticsController::TodayRejected / (float)StatisticsController::TodayAll))
		);/*赋予数值*/
	string sql(buffer);
	ASqlHelper::ExecuteNonQuery(sql);
}