#pragma once

#include <string>
#include <iostream>
#include <fstream>

using namespace std;

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"


#include "Base\Block.h"

class EdgeInnerDetctor
{
public:
	EdgeInnerDetctor(cv::Mat&, Block*);
	~EdgeInnerDetctor();
private:
	Block *block;
	cv::Mat image;//Ô­Ê¼Í¼Ïñ
};

