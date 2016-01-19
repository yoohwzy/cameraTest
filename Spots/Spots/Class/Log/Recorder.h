#pragma once
class Recorder
{
public:
	enum Quality
	{
		A,
		B,
		C,
		Good,//合格
		Reject,//不合格
		Total
	};
	enum TimeSpan
	{
		Today,
		Tomounth,
		Toyear
	};
	Recorder();
	~Recorder();
	//当日品级记录加一
	static void AddOne(Quality type);
	//获得数据
	static int GetCount(Quality type, TimeSpan t);
	//获得合格率
	static double FineRate();
private:
	//当日记录
	static int todayACount;
	static int todayBCount;
	static int todayCCount;
	static int todayRejectCount;

	//当月记录
	static int tomounthACount;
	static int tomounthBCount;
	static int tomounthCCount;
	static int tomounthRejectCount;


	//当年记录
	static int toyearACount;
	static int toyearBCount;
	static int toyearCCount;
	static int toyearRejectCount;
};

