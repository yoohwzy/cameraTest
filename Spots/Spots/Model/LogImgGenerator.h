#pragma once
#include <opencv2\opencv.hpp>
#include <string>
using namespace std;
// ��������UI�ϵ���ʷ��¼ͼƬ
class LogImgGenerator
{

public:
	LogImgGenerator();
	~LogImgGenerator();

	//����
	cv::Mat DrawingBoard;

	int BoardWidth = 600;
	int BoardHeight = 400;

	
	const int ItemCount = 5;//һ����ʾ���ٸ�����ͼ
	const int ItemWidth = 100;//����ͼ��
	const int ItemHeight = 60;//����ͼ��
	const int ItemRowSpan = 4;//����ͼ֮����
	const int ItemColSpan = 2;//�м��
	const int ItemRows = 4;//����

	void InitDrawingBoard();

	void AddItem(cv::Mat, string);
private:
};

