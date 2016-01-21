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

	////��ȡ��ֵ��ͼ��
	if (grayImg.cols == 0)
	{
		//printf_globle("Img Empty!\r\n");
	}
	cv::Mat threshodImg;
	cv::threshold(grayImg, threshodImg, ConsumerThreshodLow, 255, 0);
	cv::Mat threshodImgHight;
	cv::threshold(grayImg, threshodImgHight, ConsumerThreshodHight, 255, 0);
}

//��ש��λ
void Synthesizer::positioning()
{

}
//��Եȱ�ݼ��
void Synthesizer::detectEdge()
{

}
//�ڲ�ȱ�ݼ��
void Synthesizer::detectInner()
{

}