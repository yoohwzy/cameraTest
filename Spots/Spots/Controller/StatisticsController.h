#pragma once
// 用于生产计数，静态类，包括ACCESS数据库数据写入读取
// 表现于UI上的数据统计区
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

	// 初始化统计数据，从数据库中读取历史记录
	// 返回是否成功
	static bool InitDate();

	//获得合格率
	static double GetFineRate()
	{
		return 0;
	}
};

