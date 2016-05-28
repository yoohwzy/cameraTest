#include "..//opencv2//opencv.hpp"
#include "..//opencv//highgui.h"
#include "opencv2/stitching/stitcher.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <omp.h>
#include "Timer.h"  
#include <mutex>
#include <thread>
#include <cstdlib>
#include <condition_variable>
#include "Base\Block.h"


using namespace cv;
using namespace std;

#include "Base\Faults.h"


class Singleton//数据统计读写类
{
public:
	static Singleton *GetInstance()
	{
		return const_cast <Singleton *>(m_Instance);
	}

	static void DestoryInstance()
	{
		if (m_Instance != NULL)
		{
			delete m_Instance;
			m_Instance = NULL;
		}
	}

	int GetTest()
	{
		return block_num;
	}
	vector<int> &Getvector()//获取统计数据
	{
		return statistics_list;
	}
	vector<int> &Getresult()
	{
		return statistics_result;
	}
	int OffervectorMin()//返回总的中值实际为较小值
	{
		return statistics_result[0];
	}
	int OffervectorMax()//返回总的众数实际为较大值
	{
		return statistics_result[1];
	}

private:
	Singleton(){ block_num = 10; statistics_list.resize(2 * block_num, 50); statistics_result.resize(2, 52); }//统计分块瓷砖区域中的灰度中值和众数初始化数值设为50，此后每次更新为前一幅图的数据
	static const Singleton *m_Instance;
	int block_num;
	vector<int> statistics_list;
	vector<int> statistics_result;
};




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
	float hranges[2];
	const float *ranges[1];
	int channels;

	static const int STAMP_WIDTH = 30;//凹凸检测resize宽
	static const int  STAMP_HEIGHT = 30;//凹凸检测resize高
	static const int STAMP_SIZE = STAMP_WIDTH*STAMP_HEIGHT;
	static const int kItemsToProduce = 10;   // 生产者生产的总数
	int flagdata = 0;//数据初始化标识
	Rect recImg = Rect(Point(0, 0), Point(0, 0));

	Faults *_faults;//各类缺陷集
	vector<Point> pointlist;//瓷砖内部定位坐标集
	vector<Point> pointlist_r;//原始定位坐标集
	vector<Point> in_or_out;//最小外接矩形四顶点坐标集
	Rect location_rect;//相对MidImg的位置
	vector<Point> locationpoints;
	vector<Rect> CneedContours;
	vector<vector<Rect>> Warehousecontours;
	vector<Rect> needContour;

	//掩膜大图
	Mat Mask_result_big;
	//掩膜小图
	Mat Mask_result_small;
	//预处理后截取图凹凸用
	Mat MidImg;
	//凹凸检测截取原图
	Mat original_Img_D;
	//划痕检测截取原图
	Mat original_Img_L;
	//预处理后截取图划痕用
	Mat LMidImg;
	//边缘检测图
	Mat CannyImg;
	//掩膜制作用图
	Mat re_Img_small;
	//划痕用掩膜
	Mat Mask_result_line;

	Mat Grow(const Mat &image, const Point &seedpoint, const int th_v);//种子点区域生长
	Mat Equalize(const Mat &_Img);//瓷砖表面预处理高斯差分滤波与直方图均衡化
	void ProcessArea(Block *blockin);//确定瓷砖表面内部位置，A是左上，顺时针
	int otsuThreshold(int _width, int _height, float origin, const MatND &hist);//局部寻找可靠阈值
	int Maxdistance(vector<Point> const &vec);//给一组点集求出任意两点间的最大距离时间复杂度为nlogn
	void Handwriting(const Mat &_img);//判断是否存在人工笔迹
	void line2preprocess();//划痕检测预处理线程
	void linedetect();//划痕检测主线程
	void ProduceItem(ItemRepository *ir, int item);//生产者标记移动
	void ProducerTask();//生产者任务内容
	int ConsumeItem(ItemRepository *ir);//消费者任务内容
	void ConsumerTask();//消费者标记移动
	void InitItemRepository(ItemRepository *ir);//多线程初始化
	inline void Dataload();//算法数据导入初始化
	bool defect_YoN(const Mat &_Img);//凹凸检测核心
	bool line_YoN(const Rect &_linesrect);//划痕检测核心
	void ContoursMegre(vector<vector<cv::Point>> &_contours, vector<Rect>&_RoughRect, vector<vector<Point>> &_combinecontours);//合并轮廓函数

public:
	Pretreatment()
	{
		size = 256;
		hranges[0] = 0.0;
		hranges[1] = 255.0;
		ranges[0] = hranges;
		channels = 0;
	}
	~Pretreatment();
	void pretreatment(Mat &image, Block *_block, Faults *faults);

};

