#include <iostream>
#include <fstream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/stitching/stitcher.hpp"
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

#pragma once
class Pretreatment
{
private:
	int size;
	float hranges[2];
	const float *ranges[1];
	int channels;
public:
	vector<Point3f> pretreatment(Mat &image, vector<Point> pointlist);
	Pretreatment()
	{
		size = 256;
		hranges[0] = 0.0;
		hranges[1] = 255.0;
		ranges[0] = hranges;
		channels = 0;
	}
	~Pretreatment();

};

