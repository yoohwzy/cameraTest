#pragma once

#include <opencv2\opencv.hpp>
#include <vector>

using namespace std;
class MainHueAnalysis
{
public:
	struct ColorProp
	{
		float percentage;
		float h;
		float s;
		float v;
		ColorProp(float p, float h, float s, float v)
		{
			this->percentage = p;
			this->h = h;
			this->s = s;
			this->v = v;
		}
		ColorProp(){}
	};

	MainHueAnalysis(cv::Mat img);
	~MainHueAnalysis();

	vector<ColorProp> colorProperties;

private:
	//绘制直方图图像
	//传入一维直方图Mat，第二参数为绘制的图像高度
	cv::Mat drawHist(cv::Mat hist, int histHight = 500);





	//统计直方图
	//若用来统计彩色直方图，则hImg为32位浮点型
	//high为直方图的X轴范围上限，bin为X轴上的区间数量
	//area为传入的图像的各点的取值上限减下限
	cv::Mat calcHist(cv::Mat& hImg, int bin);


	int getHistSum(cv::Mat& hist, int leftIndex, int rightIndex);
	void eraseHist(cv::Mat& hist, int leftIndex, int rightIndex);

};

