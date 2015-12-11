#pragma once

#include <opencv.hpp>
using namespace std;

class TimeHelper
{
public:
	void start()
	{
		startTick = cv::getTickCount();
	};
	void stop()
	{
		stopTick = cv::getTickCount();
		timeMicro = ((double)(stopTick - startTick)) * 1000000 / cv::getTickFrequency();
		timeMilli = timeMicro / 1000;
		timeSec = timeMilli / 1000;
	}

	unsigned int getTimeTicks();
	double getTimeMicro();
	double getTimeMilli();
	double getTimeSec();
	unsigned int getCounter();
private:
	double timeMilli = 0;
	double timeMicro = 0;
	double timeSec = 0;
	unsigned int startTick = 0;
	unsigned int stopTick = 0;
};