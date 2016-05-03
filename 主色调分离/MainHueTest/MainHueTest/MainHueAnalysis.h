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
	//����ֱ��ͼͼ��
	//����һάֱ��ͼMat���ڶ�����Ϊ���Ƶ�ͼ��߶�
	cv::Mat drawHist(cv::Mat hist, int histHight = 500);





	//ͳ��ֱ��ͼ
	//������ͳ�Ʋ�ɫֱ��ͼ����hImgΪ32λ������
	//highΪֱ��ͼ��X�᷶Χ���ޣ�binΪX���ϵ���������
	//areaΪ�����ͼ��ĸ����ȡֵ���޼�����
	cv::Mat calcHist(cv::Mat& hImg, int bin);


	int getHistSum(cv::Mat& hist, int leftIndex, int rightIndex);
	void eraseHist(cv::Mat& hist, int leftIndex, int rightIndex);

};

