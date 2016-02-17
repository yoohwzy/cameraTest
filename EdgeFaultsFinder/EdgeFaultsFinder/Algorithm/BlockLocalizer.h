#pragma once
#include <vector>
#include <opencv2\opencv.hpp>
#include "Base\Block.h"

#define BD_OUTPUT_DEBUG_INFO 1

using namespace std;
//@description ��שλ�ÿ��ٶ�λ������Ԥ������ͼ��
// ���Ȳ����ϱ�Ե��ȡС�����˹ģ������ֵ��
//@author VShawn
//@last modify date 2015-9-29 22:39:36 By VShawn
class BlockLocalizer
{
public:
	BlockLocalizer(cv::Mat& Img);
	~BlockLocalizer();

#ifdef BD_OUTPUT_DEBUG_INFO
	//�������е�ͼƬ
	cv::Mat drowDebugDetectLR;
	cv::Mat drowDebugDetectUD;
	cv::Mat drowDebugResult;
#endif

	void FindUp();
	void FindLeft();
private:
	cv::Mat img;
	//��ˮƽ��Ե���ϡ��£��ϻ��y����ֵ
	int getYOnLine(cv::Point start, int range);
	//����ֱ��Ե�����ң��ϻ��x����ֵ
	int getXOnRow(cv::Point start, int range);

	vector<cv::Point> uppoints;
	vector<cv::Point> downpoints;
	vector<cv::Point> leftpoints;
	vector<cv::Point> rightpoints;


	void rectFix(cv::Rect& rect)
	{
		if (rect.x < 0)
			rect.x = 0;
		if (rect.x >= img.cols)
			rect.x = img.cols - 1;
		if (rect.x + rect.width > img.cols)
			rect.width = img.cols - rect.x;

		if (rect.y < 0)
			rect.y = 0;
		if (rect.y >= img.rows)
			rect.y = img.rows - 1;
		if (rect.y + rect.height > img.rows)
			rect.height = img.rows - rect.y;
	}
	//��LIST<POINT>����
	static bool ORDER_BY_Y_ASC(cv::Point i, cv::Point j) { return i.y < j.y; }
	static bool ORDER_BY_Y_DESC(cv::Point i, cv::Point j) { return i.y > j.y; }
	static bool ORDER_BY_X_ASC(cv::Point i, cv::Point j) { return i.x < j.x; }
	static bool ORDER_BY_X_DESC(cv::Point i, cv::Point j) { return i.x > j.x; }
};

