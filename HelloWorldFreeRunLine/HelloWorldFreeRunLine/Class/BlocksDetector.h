#pragma once

#include "../globle.h"
#include "BufferStorage.h";
#include "MicroDisplay\MicroDisplayInit.h";
class BlocksDetector
{
public:
	BlocksDetector(BufferStorage *ss, MicroDisplayInit *mdii);
	~BlocksDetector();

	void Start();

	//检测过程中的图片
	cv::Mat drowDebugDetect;
	cv::Mat drowDebugResult;

	cv::Point A;
	cv::Point B;
	cv::Point C;
	cv::Point D;
private:
	BufferStorage *s;
	MicroDisplayInit *mdi;

	//逼近法求出左上角，已知endline的x列为边界
	int GetEdgeLeftx3(cv::Point start, int range = 200);
	int GetEdgeLeftApproach(cv::Point start, cv::Point end, int range = 200);
	int GetEdgeLeft(cv::Point start, int range = 200);
	int GetEdgeRightx3(cv::Point start, int range = 200);
	int GetEdgeRightApproach(cv::Point start, cv::Point end, int range = 200);
	int GetEdgeRight(cv::Point start, int range = 200);

	vector < cv::Point > leftBorder;
	vector < cv::Point > rightBorder;
	//vector<int> leftEdgeLine;//y坐标，第几行
	//map<int, int> leftEdgeX;//根据Y坐标存储X坐标，第几列
	//vector<int> rightEdgeLine;//y坐标，第几行
	//map<int, int> rightEdgeX;
	const int linespan = 100;
	const int margin = 300;
	const int orange = 400;

	static bool comp(cv::Point i, cv::Point j) { return i.y < j.y; }
};

