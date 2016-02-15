#pragma once
#include <opencv2\opencv.hpp>

using namespace std;

class HistoryController
{
public:
	cv::Mat History;
	HistoryController();
	~HistoryController();

	//���һ��ͼƬ����ʷ��¼��
	//type��Ʒ���ͣ�1A 2B 3C 4Rejcet
	void Add(cv::Mat newImg, int type);
private:
	cv::Scalar backGroundColor = cv::Scalar(255, 0, 0);
	int width = 1000;
	int height = 300;
	int piecewidth = 100;
	int pieceheight = 200;
	int piecespan = 100;

};

