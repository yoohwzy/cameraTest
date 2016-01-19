#include "Recorder.h"


Recorder::Recorder()
{
}


Recorder::~Recorder()
{
}
//当日品级记录加一
void Recorder::AddOne(Quality type)
{
	switch (type)
	{
	case Recorder::A:
		todayACount++;
		tomounthACount++;
		toyearACount++;
		break;
	case Recorder::B:
		todayBCount++;
		tomounthBCount++;
		toyearBCount++;
		break;
	case Recorder::C:
		todayCCount++;
		tomounthCCount++;
		toyearCCount++;
		break;
	case Recorder::Reject:
		todayRejectCount++;
		tomounthRejectCount++;
		toyearRejectCount++;
		break;
	default:
		break;
	}
};
//获得今日数据
int Recorder::GetCount(Quality type, TimeSpan t)
{
	switch (t)
	{
	case Recorder::Today:
		switch (type)
		{
		case Recorder::A:
			return todayACount;
			break;
		case Recorder::B:
			return todayBCount;
			break;
		case Recorder::C:
			return todayCCount;
			break;
		case Recorder::Good:
			return (todayACount + todayBCount + todayCCount);
			break;
		case Recorder::Reject:
			return todayRejectCount;
			break;
		case Recorder::Total:
			return (todayACount + todayBCount + todayCCount + todayRejectCount);
			break;
		default:
			break;
		}
		break;
	case Recorder::Tomounth:
		switch (type)
		{
		case Recorder::A:
			return tomounthACount;
			break;
		case Recorder::B:
			return tomounthBCount;
			break;
		case Recorder::C:
			return tomounthCCount;
			break;
		case Recorder::Good:
			return (tomounthACount + tomounthBCount + tomounthCCount);
		case Recorder::Reject:
			return tomounthRejectCount;
			break;
		case Recorder::Total:
			return (tomounthACount + tomounthBCount + tomounthCCount + tomounthRejectCount);
			break;
		default:
			break;
		}
		break;
	case Recorder::Toyear:
		switch (type)
		{
		case Recorder::A:
			return toyearACount;
			break;
		case Recorder::B:
			return toyearBCount;
			break;
		case Recorder::C:
			return toyearCCount;
			break;
		case Recorder::Good:
			return (toyearACount + toyearBCount + toyearCCount);
		case Recorder::Reject:
			return toyearRejectCount;
			break;
		case Recorder::Total:
			return (toyearACount + toyearBCount + toyearCCount + toyearRejectCount);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return -1;
};


int Recorder::todayACount = 0;
int Recorder::todayBCount = 0;
int Recorder::todayCCount = 0;
int Recorder::todayRejectCount = 0;


int Recorder::tomounthACount = 0;
int Recorder::tomounthBCount = 0;
int Recorder::tomounthCCount = 0;
int Recorder::tomounthRejectCount = 0;


int Recorder::toyearACount = 0;
int Recorder::toyearBCount = 0;
int Recorder::toyearCCount = 0;
int Recorder::toyearRejectCount = 0;