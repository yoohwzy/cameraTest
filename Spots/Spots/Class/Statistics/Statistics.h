#pragma once

#include <sstream>
#include <string.h>
using namespace std;
// 用于生产计数，静态类，包括ACCESS数据库数据写入读取
// 表现于UI上的数据统计区
class Statistics
{
public:
	Statistics();
	~Statistics();
	static unsigned int TodayTiggerIndex;//今日的触发索引号
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

	// 初始化统计数据，从数据库中读取历史记录
	// 返回是否成功
	static bool InitDate(string ACCDB_PATH);

	//获得合格率
	static double GetFineRate();

	//今日产量加一
	static void AddTodayA();
	static void AddTodayB();
	static void AddTodayC();
	static void AddTodayRejected();
private:
	//数据库是否成功加载？
	static bool accessDBHasOpened;
	//初始化今日的记录数据
	static void initToday();
	//在数据库中更新今日数据
	static void updateTodayAccess();
};

