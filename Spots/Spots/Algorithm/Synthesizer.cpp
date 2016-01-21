#include "Synthesizer.h"


Synthesizer::Synthesizer()
{
}


Synthesizer::~Synthesizer()
{
}

void Synthesizer::Run(cv::Mat linerGrayImg)
{
	block = new Block(linerGrayImg.size().width, linerGrayImg.size().height);

	cv::Mat grayImg = linerGrayImg;
	if (grayImg.channels() == 3)
		cv::cvtColor(grayImg, grayImg, CV_BGR2GRAY);

	////获取二值化图像
	if (grayImg.cols == 0)
	{
		//printf_globle("Img Empty!\r\n");
	}
	cv::Mat threshodImg;
	cv::threshold(grayImg, threshodImg, ConsumerThreshodLow, 255, 0);
	cv::Mat threshodImgHight;
	cv::threshold(grayImg, threshodImgHight, ConsumerThreshodHight, 255, 0);
}

//瓷砖定位
void Synthesizer::positioning()
{

}
//边缘缺陷检测
void Synthesizer::detectEdge()
{

}
//内部缺陷检测
void Synthesizer::detectInner()
{

}