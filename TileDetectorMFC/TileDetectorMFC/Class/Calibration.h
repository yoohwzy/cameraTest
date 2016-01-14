#include "..//opencv2//opencv.hpp"
#include "..//opencv//highgui.h"
#include "opencv2/stitching/stitcher.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <queue>
#include <mutex>
#include <thread>
#include <cstdlib>
#include <condition_variable>
#include "Base\Block.h"
#include "Pretreatment.h"





using namespace cv;
using namespace std;

#include "Base\Calibra.h"
#include "Base\Faults.h"



#pragma once
class Calibration:public Pretreatment
{
private:
	static const int kItemRepositorySize = 200; // Item buffer size.
	struct ItemRepository {
		int item_buffer[kItemRepositorySize]; // 产品缓冲区, 配合 read_position 和 write_position 模型环形队列.
		size_t read_position; // 消费者读取产品位置.
		size_t write_position; // 生产者写入产品位置.
		std::mutex mtx; // 互斥量,保护产品缓冲区
		std::condition_variable repo_not_full; // 条件变量, 指示产品缓冲区不为满.
		std::condition_variable repo_not_empty; // 条件变量, 指示产品缓冲区不为空.
	} gItemRepository; // 产品库全局变量, 生产者和消费者操作该变量.

	typedef struct ItemRepository ItemRepository;
	int size;
	int kItemsToProduce = 10;   // How many items we plan to produce.
	int m = 0;
	int faultnum = 0;
	vector<Point3f> repoint;
	Mat MidImg, ThImg, LMidImg, PMidImg;
	vector<vector<cv::Point>> needContour;
	Rect recImg = Rect(Point(0, 0), Point(0, 0));
	vector<vector<cv::Point>> dilateneedcontours;
	vector<Point> pointlist;
	vector<Point> in_or_out;
	vector<Point> Containpoints;
	vector<Point> locationpoints;
	vector<vector<Point>> CneedContours;
	vector<vector<vector<Point>>> Warehousecontours;
	vector<vector<Point>> Linecontours;
	vector<vector<cv::Point>> ecliptours;
	Mat CannyImg, BlurImg, lookUpTable, lookUpTable_main;
	float hranges[2];
	const float *ranges[1];
	int channels;
	bool WhetherLine(Mat &oneImg, Mat &G_Img, bool cor, bool boe);
	void img2clone();
	void img2zoom();
	void linedetect();
	void tem_2plate();
	void ProduceItem(ItemRepository *ir, int item);
	void ProducerTask();
	int ConsumeItem(ItemRepository *ir);
	void ConsumerTask();
	void InitItemRepository(ItemRepository *ir);
public:
	void C_pretreatment(Mat &image, Block *_block, Scales *_scales);
	Mat C_Grow(Mat &image, Point seedpoint, double th_v)
	{
		return Grow(image,seedpoint,th_v);
	}
	int C_otsuThreshold(Mat &frame, MatND hist)
	{
		return otsuThreshold(frame, hist);
	}
	Calibration()
	{
		size = 256;
		hranges[0] = 0.0;
		hranges[1] = 255.0;
		ranges[0] = hranges;
		channels = 0;
	}
	~Calibration();

};



