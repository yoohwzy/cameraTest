#pragma once
#include <opencv2\opencv.hpp>
#include <string>
using namespace std;
// 用于生成UI上的历史记录图片
class LogImgGenerator
{

public:
	LogImgGenerator();
	~LogImgGenerator();

	//画板
	cv::Mat DrawingBoard;

	int BoardWidth = 600;
	int BoardHeight = 400;

	const int ItemCount = 10;
	const int ItemWidth = 60;
	const int ItemHeight = 100;
	const int ItemRowSpan = 4;
	const int ItemColSpan = 2;
	const int ItemRows = 2;

	void InitDrawingBoard();

	void AddItem(cv::Mat, string);
private:
};

