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





using namespace cv;
using namespace std;

#include "Base\Faults.h"


#pragma once
class Pretreatment
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
	Faults *_faults;//各类缺陷集
	vector<Point> pointlist;//瓷砖内部定位坐标集
	vector<Point> pointlist_r;//原始定位坐标集
	vector<Point> in_or_out;//最小外接矩形四顶点坐标集
	Rect location_rect;
	vector<Point> locationpoints;
	vector<Rect> CneedContours;
	vector<vector<Rect>> Warehousecontours;
	vector<vector<Point>> Linecontours;
	vector<vector<cv::Point>> ecliptours;
	float hranges[2];
	const float *ranges[1];
	int channels;
	Mat Mask_result_big, Mask_result_small, BlurImg, lookUpTable, lookUpTable_main;
	
public:
	
	Mat Grow(Mat &image, Point seedpoint, int th_v);//种子点区域生长
	int otsuThreshold(Mat &frame, MatND hist);//局部寻找可靠阈值
	Mat Equalize(Mat &_Img);//瓷砖表面预处理高斯差分滤波与直方图均衡化
	void ProcessArea(Block *blockin);//确定瓷砖表面内部位置，A是左上，顺时针
	int Maxdistance(vector<Point> vec);//给一组点集求出任意两点间的最大距离时间复杂度为nlogn
	void Handwriting(Mat &_img);//判断是否存在人工笔迹
	void line2preprocess();
	void linedetect();
	void data_import();
	void ProduceItem(ItemRepository *ir, int item);
	void ProducerTask();
	int ConsumeItem(ItemRepository *ir);
	void ConsumerTask();
	void InitItemRepository(ItemRepository *ir);
	void pretreatment(Mat &image, Block *_block, Faults *faults);
	Pretreatment()
	{
		size = 256;
		hranges[0] = 0.0;
		hranges[1] = 255.0;
		ranges[0] = hranges;
		channels = 0;
	}
	~Pretreatment();

};

