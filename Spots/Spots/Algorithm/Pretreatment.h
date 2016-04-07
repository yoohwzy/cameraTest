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
		int item_buffer[kItemRepositorySize]; // ��Ʒ������, ��� read_position �� write_position ģ�ͻ��ζ���.
		size_t read_position; // �����߶�ȡ��Ʒλ��.
		size_t write_position; // ������д���Ʒλ��.
		std::mutex mtx; // ������,������Ʒ������
		std::condition_variable repo_not_full; // ��������, ָʾ��Ʒ��������Ϊ��.
		std::condition_variable repo_not_empty; // ��������, ָʾ��Ʒ��������Ϊ��.
	} gItemRepository; // ��Ʒ��ȫ�ֱ���, �����ߺ������߲����ñ���.

	typedef struct ItemRepository ItemRepository;
	int size;
	Faults *_faults;//����ȱ�ݼ�
	vector<Point> pointlist;//��ש�ڲ���λ���꼯
	vector<Point> pointlist_r;//ԭʼ��λ���꼯
	vector<Point> in_or_out;//��С��Ӿ����Ķ������꼯
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
	
	Mat Grow(Mat &image, Point seedpoint, int th_v);//���ӵ���������
	int otsuThreshold(Mat &frame, MatND hist);//�ֲ�Ѱ�ҿɿ���ֵ
	Mat Equalize(Mat &_Img);//��ש����Ԥ�����˹����˲���ֱ��ͼ���⻯
	void ProcessArea(Block *blockin);//ȷ����ש�����ڲ�λ�ã�A�����ϣ�˳ʱ��
	int Maxdistance(vector<Point> vec);//��һ��㼯�������������������ʱ�临�Ӷ�Ϊnlogn
	void Handwriting(Mat &_img);//�ж��Ƿ�����˹��ʼ�
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

