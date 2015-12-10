#include "..//opencv2//opencv.hpp"
#include "..//opencv//highgui.h"
#include "..//opencv//ml.h"
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
	Faults *_faults;
	vector<Point> locationpoints;
	vector<vector<Point>> CneedContours;
	vector<vector<vector<Point>>> Warehousecontours;
	Mat Mask_result_big, Mask_result_small;
	float hranges[2];
	const float *ranges[1];
	int channels;
	void linedetect();
	void ProduceItem(ItemRepository *ir, int item);
	void ProducerTask();
	int ConsumeItem(ItemRepository *ir);
	void ConsumerTask();
	void InitItemRepository(ItemRepository *ir);
public:
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

