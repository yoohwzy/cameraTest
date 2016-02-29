#pragma once

#include <opencv2\opencv.hpp>
#include "Base\Faults.h"
#include "Base\Block.h"

#include <thread>

class BlockEdgeDetector
{
public:
	BlockEdgeDetector(cv::Mat& _img, Block* _block, Faults* _faults);
	~BlockEdgeDetector();

private:
	cv::Mat img;
	Faults *faults = NULL;
	Block *block = NULL;
};

