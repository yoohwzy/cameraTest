#include <iostream>
#include <fstream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/stitching/stitcher.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>


using namespace cv;
using namespace std;

#pragma once
class Judgement
{
private:
	int size;
	float hranges[2];
	const float *ranges[1];
	int channels;
public:
	int JudgementYON(Mat &image, int n, int m);
	Judgement()
	{
		size = 256;
		hranges[0] = 0.0;
		hranges[1] = 255.0;
		ranges[0] = hranges;
		channels = 0;
	}
	~Judgement();

};

