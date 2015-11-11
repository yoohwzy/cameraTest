#include "EdgeInnerDetctor.h"


EdgeInnerDetctor::EdgeInnerDetctor(cv::Mat& img,Block *b)
{
	if (img.channels() == 1)
		image = img.clone();
	else
		cv::cvtColor(img, image, CV_RGB2GRAY);
	block = b;
	  
	block->ABCD2Lines();


	vector<cv::Mat> reduceList;
	for (size_t x = block->A.x + 30; x < block->B.x - 30; x += 100)
	{
		cv::Mat tmpROI = image(cv::Rect(x, block->GetPonintByX(x, &block->UpLine).y, 100, 200)).clone();
		cv::GaussianBlur(tmpROI, tmpROI, cv::Size(5, 5), 0);
		cv::Mat reduceImg;
		cv::reduce(tmpROI, reduceImg, 1, CV_REDUCE_AVG);
		tmpROI.release();

		reduceList.push_back(reduceImg);
	}
}


EdgeInnerDetctor::~EdgeInnerDetctor()
{

}
 