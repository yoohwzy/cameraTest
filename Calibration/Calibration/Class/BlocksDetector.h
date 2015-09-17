#pragma once

#include "../globle.h"
#include "BufferStorage.h";
#include "MicroDisplay\MicroDisplayInit.h";
class BlocksDetector
{

public:
	BlocksDetector(BufferStorage *ss, MicroDisplayInit *mdii);
	~BlocksDetector();
	struct Line
	{
		int x0;
		int y0;
		double k;
		double dx;
		double dy;
	};
	enum BorderType{
		Left,
		Up,
		Right,
		Down
	};


	void Start();
	// Ѱ�����±�Ե
	void StartUP_DOWN(BorderType bt);

	//���ĳһ�е�����Ե�����ر�Ե��x���꣬-1��ʾδ�ҵ�
	//�������Ϊ�ڵڼ��м�⣬�������������leftY��
	int DetectOneLineLeft(int y = -1);
	int DetectOneLineRight(int y = -1);

	//������ĸ�����(ͼ���ϵ��ĸ�����)
	void ABCD();


#ifdef OUTPUT_DEBUG_INFO
	//�������е�ͼƬ
	cv::Mat drowDebugDetectLR;
	cv::Mat drowDebugDetectUD;
	cv::Mat drowDebugResult;
#endif

	cv::Point A;
	cv::Point B;
	cv::Point C;
	cv::Point D;

	vector < cv::Point > LeftBorder;
	Line LeftLine;
	vector < cv::Point > RightBorder;
	Line RightLine;
	vector < cv::Point > UpBorder;
	Line UpLine;
	vector < cv::Point > DownBorder;
	Line DownLine;

	//�洢�߽綥�㣬��ʱ��ţ����Ҫ���з���
	vector < cv::Point > LeftUp;
	vector < cv::Point > LeftDown;
	vector < cv::Point > RightUp;
	vector < cv::Point > RightDown;

	vector < cv::Point > UpLeft;
	vector < cv::Point > DownLeft;
	vector < cv::Point > UpRight;
	vector < cv::Point > DownRight;
private:
	BufferStorage *s;
	MicroDisplayInit *mdi;

	vector<cv::Point> tmpLeftList;
	vector<cv::Point> allLeftList;//��¼�����ҵ��ĵ㣬Ԥ����
	vector<cv::Point> tmpRightList;
	vector<cv::Point> allRightList;

	vector<cv::Point> tmpUpList;
	vector<cv::Point> allUpList;
	vector<cv::Point> tmpDownList;
	vector<cv::Point> allDownList;


	int leftY;//�ڵڼ��м��
	int leftX;//�����е�
	int leftRnage;//���е���Χ���������ڼ��
	int leftNoneCount = 0;//����û���ҵ���Ե��������
	bool leftNeedReFind = false;//�Ը����Ƿ���Ҫ����range��������

	int rightY;
	int rightX;
	int rightRnage;
	int rightNoneCount = 0;//����û���ҵ���Ե��������
	bool rightNeedReFind = false;//�Ը����Ƿ���Ҫ����range��������



	int GetEdgeX3(cv::Point start, int range, BorderType bt);
	//�ƽ���������ֱ��Ե����
	//start����ʼ��  end�������� range��������Χ  bt����Ե����  Target�����ĸ���������ݹ��ã�
	int GetEdgeVerticalApproach(cv::Point start, cv::Point end, int range, BorderType bt, cv::Point Target);
	int GetEdgeVertical(cv::Point start, int range, bool isLeft);
	//isLeft true���ƽ��󶥵㣬false���ƽ��Ҷ���
	int GetEdgeHorizontalApproach(cv::Point start, cv::Point end, int range, BorderType bt, cv::Point Target);
	int GetEdgeHorizontal(cv::Point start, int range, bool isUp);
	//�ж�ĳ���Ƿ���ĳ�����ϡ�
	bool IsPointNearline(Line line, cv::Point point);









	 




	 

	//����Եʱ���ۼӶ��ٸ����ص�
	const int SUM_COUNT = 20;
	//ƽ��ÿ����֮��Ҫ��SUM_AVG_THRESHOD�������Ǳ߽磬��������ֵΪSUM_COUNT*SUM_AVG_THRESHOD;
	const int SUM_AVG_THRESHOD = 5;
	const int SUM_THRESHOD = SUM_COUNT * SUM_AVG_THRESHOD;


	//�����в���һ��
	const int ROW_SPAN = 111;
	//Ĭ������ɨ������ĵ㣬���ΪORANGE_MARGIN_LINE���ұ�Ϊwidth-ORANGE_MARGIN_LINE
	const int ORANGE_MARGIN_ROW = 300;
	//Ĭ�ϵģ���һ�����Ҷ��ٷ�Χ�ڽ���ɨ��
	const int ORANGE_RANGE_ROW = 200;

	//�����в���һ��
	const int COL_SPAN = 100;
	//Ĭ�ϵģ���һ�����¶��ٷ�Χ�ڽ���ɨ��
	const int ORANGE_RANGE_COL = 200;

	//ÿ��������������Χ
	const int RANGE_MINI = 35;		//�ο�ֵ50
	//ÿ�η�Χ���ٶ���
	const int RANGE_REDUCE_BY = 50;


	//��LIST<POINT>����
	static bool ORDER_BY_Y_ASC(cv::Point i, cv::Point j) { return i.y < j.y; }
	static bool ORDER_BY_Y_DESC(cv::Point i, cv::Point j) { return i.y > j.y; }
	static bool ORDER_BY_X_ASC(cv::Point i, cv::Point j) { return i.x < j.x; }
	static bool ORDER_BY_X_DESC(cv::Point i, cv::Point j) { return i.x > j.x; }
};

