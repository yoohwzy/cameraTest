#pragma once
#include "../stdafx.h"

#ifndef CV_BGR2GRAY
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#endif

using namespace cv;
using namespace std;

class Scales//�����׼����
{
public:
	struct standard_part{
		int gray_v;//��׼�Ҷ�ֵ
		bool level;//��⼶��0����1��
	};

	void Clear()
	{
		Standard_parts.clear();
	};
	vector<standard_part> Standard_parts;
};