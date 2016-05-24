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


class Singleton//����ͳ�ƶ�д��
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
	vector<int> &Getvector()//��ȡͳ������
	{
		return statistics_list;
	}
	vector<int> &Getresult()
	{
		return statistics_result;
	}
	int OffervectorMin()//�����ܵ���ֵʵ��Ϊ��Сֵ
	{
		return statistics_result[0];
	}
	int OffervectorMax()//�����ܵ�����ʵ��Ϊ�ϴ�ֵ
	{
		return statistics_result[1];
	}

private:
	Singleton(){ block_num = 10; statistics_list.resize(2 * block_num, 50); statistics_result.resize(2, 52); }//ͳ�Ʒֿ��ש�����еĻҶ���ֵ��������ʼ����ֵ��Ϊ50���˺�ÿ�θ���Ϊǰһ��ͼ������
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
		int item_buffer[kItemRepositorySize]; // ��Ʒ������, ��� read_position �� write_position ģ�ͻ��ζ���.
		size_t read_position; // �����߶�ȡ��Ʒλ��.
		size_t write_position; // ������д���Ʒλ��.
		std::mutex mtx; // ������,������Ʒ������
		std::condition_variable repo_not_full; // ��������, ָʾ��Ʒ��������Ϊ��.
		std::condition_variable repo_not_empty; // ��������, ָʾ��Ʒ��������Ϊ��.
	} gItemRepository; // ��Ʒ��ȫ�ֱ���, �����ߺ������߲����ñ���.

	typedef struct ItemRepository ItemRepository;
	int size;
	float hranges[2];
	const float *ranges[1];
	int channels;

	static const int STAMP_WIDTH = 30;
	static const int  STAMP_HEIGHT = 30;
	static const int STAMP_SIZE = STAMP_WIDTH*STAMP_HEIGHT;
	static const int kItemsToProduce = 10;   // ����������������
	int flagdata = 0;//���ݳ�ʼ����ʶ
	Rect recImg = Rect(Point(0, 0), Point(0, 0));

	Faults *_faults;//����ȱ�ݼ�
	vector<Point> pointlist;//��ש�ڲ���λ���꼯
	vector<Point> pointlist_r;//ԭʼ��λ���꼯
	vector<Point> in_or_out;//��С��Ӿ����Ķ������꼯
	Rect location_rect;
	vector<Point> locationpoints;
	vector<Rect> CneedContours;
	vector<vector<Rect>> Warehousecontours;
	vector<Rect> needContour;
	Mat Mask_result_big, Mask_result_small, MidImg, original_Img_D, original_Img_L, ThImg, LMidImg, CannyImg, PMidImg, re_Img_small, Mask_result_line;


	Mat Grow(const Mat &image, const Point &seedpoint, const int th_v);//���ӵ���������
	Mat Equalize(const Mat &_Img);//��ש����Ԥ�����˹����˲���ֱ��ͼ���⻯
	void ProcessArea(Block *blockin);//ȷ����ש�����ڲ�λ�ã�A�����ϣ�˳ʱ��
	int otsuThreshold(int _width, int _height, float origin, const MatND &hist);//�ֲ�Ѱ�ҿɿ���ֵ
	int Maxdistance(vector<Point> const &vec);//��һ��㼯�������������������ʱ�临�Ӷ�Ϊnlogn
	void Handwriting(const Mat &_img);//�ж��Ƿ�����˹��ʼ�
	void line2preprocess();
	void linedetect();
	void ProduceItem(ItemRepository *ir, int item);
	void ProducerTask();
	int ConsumeItem(ItemRepository *ir);
	void ConsumerTask();
	void InitItemRepository(ItemRepository *ir);
	inline void Dataload();
	bool defect_YoN(const Mat &_Img);
	bool line_YoN(const Rect &_linesrect);

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

