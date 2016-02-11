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

	StatisticsController::TodayAll = 0;
	StatisticsController::TodayA = 0;
	StatisticsController::TodayB = 0;
	StatisticsController::TodayC = 0;
	StatisticsController::TodayRejected = 0;

	StatisticsController::MonthAll = 0;
	StatisticsController::MonthA = 0;
	StatisticsController::MonthB = 0;
	StatisticsController::MonthC = 0;
	StatisticsController::MonthRejected = 0;

	StatisticsController::YearAll = 10;
	StatisticsController::YearA = 0;
	StatisticsController::YearB = 0;
	StatisticsController::YearC = 0;
	StatisticsController::YearRejected = 0;

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
}
void StatisticsController::AddTodayB()
{
	TodayB++;
	TodayAll++;
	MonthB++;
	MonthAll++;
	YearB++;
	YearAll++;
}
void StatisticsController::AddTodayC()
{
	TodayC++;
	TodayAll++;
	MonthC++;
	MonthAll++;
	YearC++;
	YearAll++;
}
void StatisticsController::AddTodayRejected()
{
	TodayRejected++;
	TodayAll++;
	MonthRejected++;
	MonthAll++;
	YearRejected++;
	YearAll++;
}