#include "BlockEdgeLineDetector.h"
#include <Class\Debug\MFCConsole.h>

bool BlockEdgeLineDetector::Enabled = true;
int BlockEdgeLineDetector::BINARY_THRESHOD = 5;//边界确定阈值
int BlockEdgeLineDetector::LENGTH_THRESHOD = 10;//崩边长度要超过这个
int BlockEdgeLineDetector::DEEP_THRESHOD = 5;//崩边深度要超过这个

BlockEdgeLineDetector::BlockEdgeLineDetector(cv::Mat& _img, Block* _block, Faults* _faults)
{
	image = _img;
	if (image.channels() == 3)
		cv::cvtColor(image, image, CV_BGR2GRAY);
	p_block = _block;
	p_faults = _faults;
}
void BlockEdgeLineDetector::Run()
{
#ifdef BELD_OUTPUT_DEBUG_INFO
	drowDebugResult = image.clone();
	if (drowDebugResult.channels() == 1)
		cv::cvtColor(drowDebugResult, drowDebugResult, CV_GRAY2BGR);
	doUp();
	doDown(); 
	doLeft();
	doRight();
#else
	thread t1 = thread(std::mem_fn(&BlockEdgeLineDetector::doUp), this);
	thread t2 = thread(std::mem_fn(&BlockEdgeLineDetector::doDown), this);
	thread t3 = thread(std::mem_fn(&BlockEdgeLineDetector::doLeft), this);
	thread t4 = thread(std::mem_fn(&BlockEdgeLineDetector::doRight), this);
	t1.join();
	t2.join();
	t3.join();
	t4.join();
#endif

	minLengthPix = minLengthPix == 9999 ? 0 : minLengthPix;
	minDeepPix = minDeepPix == 9999 ? 0 : minDeepPix;
	stringstream ss;
	ss << "BLED定标消息：最大崩边长度=" << maxLengthPix << "pix，最大深度 = " << maxDeepPix << "pix" << endl;
	ss << "BLED定标消息：最小崩边长度=" << minLengthPix << "pix，最小深度 = " << minDeepPix << "pix" << endl;
	MFCConsole::Output(ss.str());
}

BlockEdgeLineDetector::~BlockEdgeLineDetector()
{
	p_faults = NULL;
	p_block = NULL;
}
void BlockEdgeLineDetector::doUp()
{
	Block::Line line = p_block->UpLine;
	int startX = p_block->A.x + 5;
	if (startX < 0)
		startX = 0;
	int endX = p_block->B.x - 5;
	if (endX >= image.cols)
		endX = image.cols - 1;

	vector<Faults::BrokenEdge> vbs;
	//细找崩边，统计崩边长度
	int brokenEdgeIndex = -1;//用于表示是否正在一个崩边中，为-1时表示不再，否则为 p_faults->BrokenEdges的索引

	for (int point_x = startX; point_x <= endX; point_x++)
	{
		int point_y = p_block->GetPonintByX(point_x, &line).y;
		if (point_y < 0 || point_y >= image.rows)
			continue;
		int deep = getDeepUp(cv::Point(point_x, point_y));
		if (deep > maxDeepPix) maxDeepPix = deep;
		if (deep > 0 && deep < minDeepPix) minDeepPix = deep;
		if (deep > DEEP_THRESHOD)
		{
#ifdef BELD_OUTPUT_DEBUG_INFO
			drowDebugResult.ptr<uchar>(point_y)[point_x * 3 + 2] = 255;
#endif
			if (brokenEdgeIndex == -1)
			{
				Faults::BrokenEdge b;
				b.deep = deep;
				b.length = 1;
				b.position = cv::Point(point_x, point_y);
				vbs.push_back(b);
				brokenEdgeIndex = vbs.size() - 1;
			}
			else
			{
				if (vbs[brokenEdgeIndex].deep < deep)
					vbs[brokenEdgeIndex].deep = deep;
				vbs[brokenEdgeIndex].length++;
				//vbs[brokenEdgeIndex].position = cv::Point(point_x - vbs[brokenEdgeIndex].length / 2, point_y);
			}
		}
		else
		{
			//一个崩边缺陷的结束
			brokenEdgeIndex = -1;
		}
	}
	processVBS(vbs, 1);
}
void BlockEdgeLineDetector::doDown()
{
	Block::Line line = p_block->DownLine;
	int startX = p_block->D.x + 5;
	if (startX < 0)
		startX = 0;
	int endX = p_block->C.x - 5;
	if (endX >= image.cols)
		endX = image.cols - 1;

	vector<Faults::BrokenEdge> vbs;
	//细找崩边，统计崩边长度
	int brokenEdgeIndex = -1;//用于表示是否正在一个崩边中，为-1时表示不再，否则为 p_faults->BrokenEdges的索引

	for (int point_x = startX; point_x <= endX; point_x++)
	{
		int point_y = p_block->GetPonintByX(point_x, &line).y;
		if (point_y < 0 || point_y >= image.rows)
			continue;
		int deep = getDeepDown(cv::Point(point_x, point_y));
		if (deep > maxDeepPix) maxDeepPix = deep;
		if (deep > 0 && deep < minDeepPix) minDeepPix = deep;
		if (deep > DEEP_THRESHOD)
		{
#ifdef BELD_OUTPUT_DEBUG_INFO
			drowDebugResult.ptr<uchar>(point_y)[point_x * 3 + 2] = 255;
#endif
			if (brokenEdgeIndex == -1)
			{
				Faults::BrokenEdge b;
				b.deep = deep;
				b.length = 1;
				b.position = cv::Point(point_x, point_y);
				vbs.push_back(b);
				brokenEdgeIndex = vbs.size() - 1;
			}
			else
			{
				if (vbs[brokenEdgeIndex].deep < deep)
					vbs[brokenEdgeIndex].deep = deep;
				vbs[brokenEdgeIndex].length++;
				//vbs[brokenEdgeIndex].position = cv::Point(point_x - vbs[brokenEdgeIndex].length / 2, point_y);
			}
		}
		else
		{
			//一个崩边缺陷的结束
			brokenEdgeIndex = -1;
		}
	}
	processVBS(vbs, 1);
}
void BlockEdgeLineDetector::doLeft()
{
	Block::Line line = p_block->LeftLine;
	int startY = p_block->A.y + 5;
	if (startY < 0)
		startY = 0;
	int endY = p_block->D.y - 5;
	if (endY >= image.rows)
		endY = image.rows - 1;

	vector<Faults::BrokenEdge> vbs;
	//细找崩边，统计崩边长度
	int brokenEdgeIndex = -1;//用于表示是否正在一个崩边中，为-1时表示不再，否则为 p_faults->BrokenEdges的索引

	for (int point_y = startY; point_y <= endY; point_y++)
	{
		int point_x = p_block->GetPonintByY(point_y, &line).x;
		if (point_x < 0 || point_x >= image.cols)
			continue;
		int deep = getDeepLeft(cv::Point(point_x, point_y));
		if (deep > maxDeepPix) maxDeepPix = deep;
		if (deep > 0 && deep < minDeepPix) minDeepPix = deep;
		if (deep > DEEP_THRESHOD)
		{
#ifdef BELD_OUTPUT_DEBUG_INFO
			drowDebugResult.ptr<uchar>(point_y)[point_x * 3 + 2] = 255;
#endif
			if (brokenEdgeIndex == -1)
			{
				Faults::BrokenEdge b;
				b.deep = deep;
				b.length = 1;
				b.position = cv::Point(point_x, point_y);
				vbs.push_back(b);
				brokenEdgeIndex = vbs.size() - 1;
			}
			else
			{
				if (vbs[brokenEdgeIndex].deep < deep)
					vbs[brokenEdgeIndex].deep = deep;
				vbs[brokenEdgeIndex].length++;
				//vbs[brokenEdgeIndex].position = cv::Point(point_x, point_y - vbs[brokenEdgeIndex].length / 2);
			}
		}
		else
		{
			//一个崩边缺陷的结束
			brokenEdgeIndex = -1;
		}
	}
	processVBS(vbs, 0);
}
void BlockEdgeLineDetector::doRight()
{
	Block::Line line = p_block->RightLine;
	int startY = p_block->B.y + 5;
	if (startY < 0)
		startY = 0;
	int endY = p_block->C.y - 5;
	if (endY >= image.rows)
		endY = image.rows - 1;

	vector<Faults::BrokenEdge> vbs;
	//细找崩边，统计崩边长度
	int brokenEdgeIndex = -1;//用于表示是否正在一个崩边中，为-1时表示不再，否则为 p_faults->BrokenEdges的索引

	for (int point_y = startY; point_y <= endY; point_y++)
	{
		int point_x = p_block->GetPonintByY(point_y, &line).x;
		if (point_x < 0 || point_x >= image.cols)
			continue;
		int deep = getDeepRight(cv::Point(point_x, point_y));
		if (deep > maxDeepPix) maxDeepPix = deep;
		if (deep > 0 && deep < minDeepPix) minDeepPix = deep;
		if (deep > DEEP_THRESHOD)
		{
#ifdef BELD_OUTPUT_DEBUG_INFO
			drowDebugResult.ptr<uchar>(point_y)[point_x * 3 + 2] = 255;
#endif
			if (brokenEdgeIndex == -1)
			{
				Faults::BrokenEdge b;
				b.deep = deep;
				b.length = 1;
				b.position = cv::Point(point_x, point_y);
				vbs.push_back(b);
				brokenEdgeIndex = vbs.size() - 1;
			}
			else
			{
				if (vbs[brokenEdgeIndex].deep < deep)
					vbs[brokenEdgeIndex].deep = deep;
				vbs[brokenEdgeIndex].length++;
				//vbs[brokenEdgeIndex].position = cv::Point(point_x - vbs[brokenEdgeIndex].length / 2, point_y);
			}
		}
		else
		{
			//一个崩边缺陷的结束
			brokenEdgeIndex = -1;
		}
	}
	processVBS(vbs, 0);
}

int BlockEdgeLineDetector::getDeepUp(cv::Point p)
{
	int point_x = p.x;
	int point_y = p.y;

	if (point_x < 0 || point_x >= image.cols)
		return 0;
	if (point_y < 0 || point_y >= image.rows)
		return 0;

	int deep = 0;
	for (; deep < 50; deep++)
	{
		if (point_y + deep + 4 >= image.rows)
			break;
		if (image.ptr<uchar>(point_y + deep)[point_x] >= BINARY_THRESHOD &&
			image.ptr<uchar>(point_y + deep + 1)[point_x] >= BINARY_THRESHOD &&
			image.ptr<uchar>(point_y + deep + 2)[point_x] >= BINARY_THRESHOD &&
			image.ptr<uchar>(point_y + deep + 3)[point_x] >= BINARY_THRESHOD &&
			image.ptr<uchar>(point_y + deep + 4)[point_x] >= BINARY_THRESHOD
			)
			break;
	}
	return deep;
}
int BlockEdgeLineDetector::getDeepDown(cv::Point p)
{
	int point_x = p.x;
	int point_y = p.y;

	if (point_x < 0 || point_x >= image.cols)
		return 0;
	if (point_y < 50 + 4 || point_y >= image.rows)
		return 0;

	int deep = 0;
	for (; deep < 50; deep++)
	{
		if (point_y - deep - 4 < 0)
			break;
		if (image.ptr<uchar>(point_y - deep)[point_x] >= BINARY_THRESHOD &&
			image.ptr<uchar>(point_y - deep - 1)[point_x] >= BINARY_THRESHOD &&
			image.ptr<uchar>(point_y - deep - 2)[point_x] >= BINARY_THRESHOD &&
			image.ptr<uchar>(point_y - deep - 3)[point_x] >= BINARY_THRESHOD &&
			image.ptr<uchar>(point_y - deep - 4)[point_x] >= BINARY_THRESHOD
			)
			break;
	}
	return deep;
}
int BlockEdgeLineDetector::getDeepLeft(cv::Point p)
{
	int point_x = p.x;
	int point_y = p.y;

	if (point_x < 0 || point_x >= image.cols)
		return 0;
	if (point_y < 0 || point_y >= image.rows)
		return 0;

	int deep = 0;
	for (; deep < 50; deep++)
	{
		if (point_x + deep + 4 >= image.cols)
			break;
		if (image.ptr<uchar>(point_y)[point_x + deep] >= BINARY_THRESHOD &&
			image.ptr<uchar>(point_y)[point_x + deep + 1] >= BINARY_THRESHOD &&
			image.ptr<uchar>(point_y)[point_x + deep + 2] >= BINARY_THRESHOD &&
			image.ptr<uchar>(point_y)[point_x + deep + 3] >= BINARY_THRESHOD &&
			image.ptr<uchar>(point_y)[point_x + deep + 4] >= BINARY_THRESHOD
			)
			break;
	}
	return deep;
}
int BlockEdgeLineDetector::getDeepRight(cv::Point p)
{
	int point_x = p.x;
	int point_y = p.y;

	if (point_x < 0 || point_x >= image.cols)
		return 0;
	if (point_y < 0 || point_y >= image.rows)
		return 0;

	int deep = 0;
	for (; deep < 50; deep++)
	{
		if (point_x - deep - 4 < 0)
			break;
		if (image.ptr<uchar>(point_y)[point_x - deep] >= BINARY_THRESHOD &&
			image.ptr<uchar>(point_y)[point_x - deep - 1] >= BINARY_THRESHOD &&
			image.ptr<uchar>(point_y)[point_x - deep - 2] >= BINARY_THRESHOD &&
			image.ptr<uchar>(point_y)[point_x - deep - 3] >= BINARY_THRESHOD &&
			image.ptr<uchar>(point_y)[point_x - deep - 4] >= BINARY_THRESHOD
			)
			break;
	}
	return deep;
}

void BlockEdgeLineDetector::processVBS(vector<Faults::BrokenEdge> vbs, bool isUpDown)
{
	for (int i = 0; i < vbs.size(); i++)
	{
		if (MFCConsole::IsOpened)
		{
			stringstream ss;
			ss << "BELD" << isUpDown << ":(" << vbs[i].position.x << "," << vbs[i].position.y << ") => length=" << vbs[i].length << ", maxdeep = " << vbs[i].deep << endl;
			MFCConsole::Output(ss.str());
		}
		if (vbs[i].length > maxLengthPix) maxLengthPix = vbs[i].length;
		if (vbs[i].length > 0 && vbs[i].length < minLengthPix) minLengthPix = vbs[i].length;
		if (vbs[i].length < LENGTH_THRESHOD)
			continue;
		else
		{
			if (isUpDown)
			{
				vbs[i].length_mm = vbs[i].length * Block::X_mmPerPix;
				vbs[i].deep_mm = vbs[i].deep * Block::Y_mmPerPix;
			}
			else
			{
				vbs[i].length_mm = vbs[i].length * Block::Y_mmPerPix;
				vbs[i].deep_mm = vbs[i].deep * Block::X_mmPerPix;
			}
			p_faults->BrokenEdges.push_back(vbs[i]);
		}
	}
}