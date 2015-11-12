#pragma once

#include <string>
#include <iostream>
#include <fstream>

using namespace std;

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"


#include "Base\Block.h"
#include "../globle_var.h"
class EdgeInnerDetctor
{
public:
	EdgeInnerDetctor(cv::Mat&, Block*);
	~EdgeInnerDetctor();
private:
	Block *block;
	cv::Mat image;//Ô­Ê¼Í¼Ïñ

	double getFrechetDistance(vector<double> lineA, vector<double> lineB);
	void saveVector(vector<vector<double>> lines, string flodername, string filename);
	void splitImgX(int start);
	void processAndSaveData(vector<vector<double>>& reduceList, vector<vector<double>>& differList, string prefix);
};

