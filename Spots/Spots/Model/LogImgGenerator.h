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

	
	const int ItemCount = 5;//一行显示多少个缩略图
	const int ItemWidth = 100;//缩略图宽
	const int ItemHeight = 60;//缩略图高
	const int ItemRowSpan = 4;//缩略图之间间隔
	const int ItemColSpan = 2;//行间隔
	const int ItemRows = 4;//行数

	void InitDrawingBoard();

	void AddItem(cv::Mat, string);
private:
};

