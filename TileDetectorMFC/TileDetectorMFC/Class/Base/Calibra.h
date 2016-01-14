#pragma once
#include "../stdafx.h"

#ifndef CV_BGR2GRAY
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#endif

using namespace cv;
using namespace std;

class Scales//定标标准参数
{
public:
	struct standard_part{
		int gray_v;//标准灰度值
		bool level;//检测级别0级或1级
	};

	void Clear()
	{
		Standard_parts.clear();
	};
	vector<standard_part> Standard_parts;
};