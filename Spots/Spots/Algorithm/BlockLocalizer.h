#pragma once
#include <vector>
#include <opencv2\opencv.hpp>
#include "Base\Faults.h"
#include "Base\Block.h"

#include <thread>

#define BL_OUTPUT_DEBUG_INFO 1

using namespace std;
//@description ��שλ�ÿ��ٶ�λ������Ԥ������ͼ��
// ���Ȳ����ϱ�Ե��ȡС�����˹ģ������ֵ��
//@author VShawn
class BlockLocalizer
{
public:
	BlockLocalizer(cv::Mat& _img, Block* _block, Faults* _faults);
	~BlockLocalizer();

	void Run();


	bool NotFoundBlockFlag = true;//δ�ҵ���ש���
	bool BrokenEdgeFlag = false;//�б���ȱ�ݱ��


#ifdef BL_OUTPUT_DEBUG_INFO
	//�������е�ͼƬ
	cv::Mat drowDebugDetectLR;
	cv::Mat drowDebugDetectUD;
	cv::Mat drowDebugResult;
#endif


	//parameter
	int THRESHOD = 10;//��ֵ����ֵ
	int ContinuePointCount = 30;//�������ٸ������ж�Ϊ��Ե



private:
	//parameter
	const int COL_SPAN = 50;//x�����϶�����
	const int ROW_SPAN = 50;//y�����϶�����
	const int RANGE_DEFAULT = 400;//��������߶�
	const int RANGE_MINI = 100;//��С�����߶�



	cv::Mat img;
	Faults *p_faults = NULL;
	Block *p_block = NULL;

	//��ˮƽ��Ե���ϡ��£��ϻ��y����ֵ
	//start�������е�
	//��start���¹�range�ߵķ�Χ������
	//scanUp2Down��������Ϊ�ϵ��£�
	int getYOnLine(cv::Point start, int range, bool scanUp2Down = true);
	//����ֱ��Ե�����ң��ϻ��x����ֵ
	//start�������е�
	//��start���ҹ�range��ķ�Χ������
	//scanLeft2right ��������
	int getXOnRow(cv::Point start, int range, bool scanLeft2right = true);

	vector<cv::Point> uppoints;
	vector<cv::Point> downpoints;
	vector<cv::Point> leftpoints;
	vector<cv::Point> rightpoints;



	void FindUp();
	void FindLeft();
	void FindRight();
	void FindDown();


	//�жϸ����Ƿ��ڱ߽���
	//�Ƿ��нϴ�ı���
	//���������߷���
	void Judgement();
	void judgementForOneLine(vector<cv::Point>& points, bool updown, Block::Line& line);
	//�ж�һ����Ƿ���һ��ֱ�ߵģ�����������BrokenEdgeFlag���true
	//����֮�⼴Ϊ�жϴ�ש�Ƿ��б��ߣ�������BrokenEdgeFlag���true
	void judgemanBrokenLine(vector<cv::Point>& points,bool updown);
	//�ж�һ����Ƿ��ڱ߽��ϣ���Ϊ���¼������
	//1.��ʼ��������㶼�ڱ߽��ϣ���ֱ��Ϊ�߽��ߣ�����false
	//2.һ����ڱ߽��ϣ�һ�벻�ڣ�ɾ���ڱ߽��ϵĵ㣬�Ա������ϣ�����true
	bool fixLineOnBorder(vector<cv::Point>& points, Block::Line& line);



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

