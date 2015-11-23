#pragma once

#include <string>
#include <iostream>
#include <fstream>

using namespace std;

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"


#include "Base\Block.h"
#include "../globle_var.h"

//#define WRITEIMG


class EdgeInnerDetctor
{
public:
	EdgeInnerDetctor(cv::Mat&, Block*);
	~EdgeInnerDetctor();

	vector<cv::Point3f> EIDFaults;
private:
	Block *block;
	cv::Mat image;//Ô­Ê¼Í¼Ïñ

	double getFrechetDistance(vector<double> lineA, vector<double> lineB);
	double getFrechetDistance(cv::Mat lineA, cv::Mat lineB);
	void splitImgX(int start);
	void processAndSaveData(vector<cv::Mat> reduceList,vector<cv::Point3f> points,string prefix);
};