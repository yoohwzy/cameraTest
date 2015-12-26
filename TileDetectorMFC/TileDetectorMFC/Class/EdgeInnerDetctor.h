#pragma once

#include <string>
#include <iostream>
#include <fstream>

using namespace std;

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"


#include "Base\Block.h"
#include "Base\Faults.h"

#include <thread>

//#define SAVE_IMG


class EdgeInnerDetctor
{
public:
	EdgeInnerDetctor(cv::Mat &img, Block *_block, Faults *_faults);
	~EdgeInnerDetctor()
	{
		p_block = NULL;
		p_faults = NULL;
	};
private:
	Block *p_block =  NULL;
	Faults *p_faults = NULL;
	cv::Mat image;//Ô­Ê¼Í¼Ïñ

	void doUp();
	void doDown();
	void doLeft();
	void doRight();

	double getFrechetDistance(vector<double> lineA, vector<double> lineB);
	double getFrechetDistance(cv::Mat lineA, cv::Mat lineB);
	void splitImgX(int start);
	void processAndSaveData(vector<cv::Mat> reduceList,vector<cv::Point3f> points,string prefix);
};