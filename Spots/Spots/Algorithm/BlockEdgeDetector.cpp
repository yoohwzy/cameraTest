#include "BlockEdgeDetector.h"
#include <Class\Debug\MFCConsole.h>

BlockEdgeDetector::BlockEdgeDetector(cv::Mat& _img, Block* _block, Faults* _faults)
{
	image = _img;
	if (image.channels() == 3)
		cv::cvtColor(image, image, CV_BGR2GRAY);
	p_block = _block;
	p_faults = _faults;
}
void BlockEdgeDetector::Run()
{
#ifdef BED_OUTPUT_DEBUG_INFO
	drowDebugResult = image.clone();
	if (drowDebugResult.channels() == 1)
		cv::cvtColor(drowDebugResult, drowDebugResult, CV_GRAY2BGR);
	doUp();
	doDown(); 
	doLeft();
	doRight();
#else
	thread t1 = thread(std::mem_fn(&BlockEdgeDetector::doUp), this);
	thread t2 = thread(std::mem_fn(&BlockEdgeDetector::doDown), this);
	thread t3 = thread(std::mem_fn(&BlockEdgeDetector::doLeft), this);
	thread t4 = thread(std::mem_fn(&BlockEdgeDetector::doRight), this);
	t1.join();
	t2.join();
	t3.join();
	t4.join();
#endif


	double maxdiff = (maxdiff_X > maxdiff_Y) ? maxdiff_X : maxdiff_Y;
	int maxDeep = (maxDeep_X > maxDeep_Y) ? maxDeep_X : maxDeep_Y;
	stringstream ss;
	ss << "BED定标消息：最大不相似度=" << maxdiff << "，缺陷最大深度 = " << maxDeep << "pix" << endl;
	MFCConsole::Output(ss.str());
}

BlockEdgeDetector::~BlockEdgeDetector()
{
	p_faults = NULL;
	p_block = NULL;
}
void BlockEdgeDetector::doUp()
{
	const int ROI_WIDTH = 51;
	const int ROI_HEIGHT = 77;
	int inc = 69;//(float)(endX - startX) / 30 + 0.5;//范围增量

	int index = 0;
	vector<cv::Mat> reduceList;
	vector<cv::Point> points;

	//上边界
	int startX = p_block->A.x + 111;
	if (startX < 0)
		startX = 0;
	int endX = p_block->B.x - 111;
	if (endX >= image.cols)
		endX = image.cols - 1;


	for (int x = startX; x < endX && x < image.cols; x += inc, index++)
	{
		int x1 = x;
		if (x < 0 || x >= image.cols)
			continue;
		if ((x + ROI_WIDTH) >= endX)
			x1 = endX - ROI_WIDTH - 1;
		if ((x + ROI_WIDTH) >= image.cols)
			x1 = image.cols - ROI_WIDTH - 1;

		int y = p_block->GetPonintByX(x1, &p_block->UpLine).y;
		if (y < 0 || y >= image.rows)
			continue;

		cv::Mat tmpROI = image(cv::Rect(x1, y, ROI_WIDTH, ROI_HEIGHT)).clone();
		cv::GaussianBlur(tmpROI, tmpROI, cv::Size(5, 5), 0);
#ifdef BED_OUTPUT_DEBUG_INFO
		debug_ups.push_back(tmpROI);
#endif
		tmpROI.convertTo(tmpROI, CV_32F);

		cv::Mat reduceImg;
		cv::reduce(tmpROI, reduceImg, 1, CV_REDUCE_AVG);
		cv::resize(reduceImg, reduceImg, cv::Size(reduceImg.cols, reduceImg.rows / 2));//高度缩减为一半

#ifdef SAVE_IMG
		//保存图片
		char num[10];
		sprintf(num, "%03d", index);
		string strnum(num);
		stringstream ss;
		ss << "EdgeInner\\U\\上_" << strnum << "_reduce.jpg";
		cv::imwrite(ss.str(), reduceImg);
		ss.str("");
		ss << "EdgeInner\\U\\上_" << strnum << ".jpg";
		cv::imwrite(ss.str(), tmpROI);
#endif

		tmpROI.release();
		reduceList.push_back(reduceImg);
		points.push_back(cv::Point(x, p_block->GetPonintByX(x, &p_block->UpLine).y));
	}
	process(reduceList, points, "up");
}
void BlockEdgeDetector::doDown()
{
	const int ROI_WIDTH = 51;
	const int ROI_HEIGHT = 77;
	int inc = 69;//(float)(endX - startX) / 30 + 0.5;//范围增量

	int index = 0;
	vector<cv::Mat> reduceList;
	vector<cv::Point> points;
	//下边界
	int startX = p_block->D.x + 111;
	if (startX < 0)
		startX = 0;
	int endX = p_block->C.x - 111;
	if (endX >= image.cols)
		endX = image.cols - 1;

	for (int x = startX; x < endX && x < image.cols; x += inc, index++)
	{
		int x1 = x;
		if (x < 0 || x >= image.cols)
			continue;
		if ((x + ROI_WIDTH) >= endX)
			x1 = endX - ROI_WIDTH - 1;
		if ((x + ROI_WIDTH) >= image.cols)
			x1 = image.cols - ROI_WIDTH - 1;


		int y = p_block->GetPonintByX(x1, &p_block->DownLine).y;
		if (y < 0 || y >= image.rows)
			continue;

		cv::Mat tmpROI = image(cv::Rect(x1, y - ROI_HEIGHT, ROI_WIDTH, ROI_HEIGHT)).clone();

		cv::GaussianBlur(tmpROI, tmpROI, cv::Size(5, 5), 0);
#ifdef BED_OUTPUT_DEBUG_INFO
		debug_downs.push_back(tmpROI);
#endif
		tmpROI.convertTo(tmpROI, CV_32F);

		cv::Mat reduceImg;
		cv::reduce(tmpROI, reduceImg, 1, CV_REDUCE_AVG);
		cv::resize(reduceImg, reduceImg, cv::Size(reduceImg.cols, reduceImg.rows / 2));//高度缩减为一半

#ifdef SAVE_IMG
		//保存图片
		char num[10];
		sprintf(num, "%03d", index);
		string strnum(num);
		stringstream ss;
		ss << "EdgeInner\\D\\下_" << strnum << "_reduce.jpg";
		cv::imwrite(ss.str(), reduceImg);
		ss.str("");
		ss << "EdgeInner\\D\\下_" << strnum << ".jpg";
		cv::imwrite(ss.str(), tmpROI);
#endif

		tmpROI.release();

		reduceList.push_back(reduceImg);
		points.push_back(p_block->GetPonintByX(x, &p_block->DownLine));
	}
	process(reduceList, points, "down");
}
void BlockEdgeDetector::doLeft()
{
	const int ROI_WIDTH = 76;
	const int ROI_HEIGHT = 71;
	int inc = 69;//(float)(endY - startY) / 60 + 0.5;//范围增量

	int index = 0;
	vector<cv::Mat> reduceList;
	vector<cv::Point> points;
	//左边界
	int startY = p_block->A.y + 69;
	if (startY < 0)
		startY = 0;
	int endY = p_block->D.y - 69;
	if (endY >= image.rows)
		endY = image.rows - 1;
	for (int y = startY; y < endY && y < image.rows; y += inc, index++)
	{
		int y1 = y;
		if (y1 < 0 || y1 >= image.rows)
			continue;
		if ((y1 + ROI_HEIGHT) >= endY)
			y1 = endY - ROI_HEIGHT - 1;
		if ((y1 + ROI_HEIGHT) >= image.rows)
			y1 = image.rows - ROI_HEIGHT - 1;

		int x = p_block->GetPonintByY(y1, &p_block->LeftLine).x;
		if (x < 0 || x >= image.cols)
			continue;

		cv::Mat tmpROI = image(cv::Rect(x, y1, ROI_WIDTH, ROI_HEIGHT)).clone();
		cv::GaussianBlur(tmpROI, tmpROI, cv::Size(9, 9), 0);
#ifdef BED_OUTPUT_DEBUG_INFO
		debug_lefts.push_back(tmpROI);
#endif
		tmpROI.convertTo(tmpROI, CV_32F);

		cv::Mat reduceImg;
		cv::reduce(tmpROI, reduceImg, 0, CV_REDUCE_AVG);
		cv::resize(reduceImg, reduceImg, cv::Size(reduceImg.cols / 2, reduceImg.rows));//宽缩减为一半

#ifdef SAVE_IMG
		//保存图片
		char num[10];
		sprintf(num, "%03d", index);
		string strnum(num);
		stringstream ss;
		ss << "EdgeInner\\L\\左_" << strnum << "_reduce.jpg";
		cv::imwrite(ss.str(), reduceImg);
		ss.str("");
		ss << "EdgeInner\\L\\左_" << strnum << ".jpg";
		cv::imwrite(ss.str(), tmpROI);
#endif

		//tmpROI.release();

		reduceImg = reduceImg.t();
		//reduceImg.convertTo(reduceImg, CV_32F);
		reduceList.push_back(reduceImg);
		points.push_back(p_block->GetPonintByY(y, &p_block->LeftLine));

	}
	process(reduceList, points, "左");
	//processAndSaveData(reduceList, points, "L\\左");
}
void BlockEdgeDetector::doRight()
{
	const int ROI_WIDTH = 76;
	const int ROI_HEIGHT = 71;
	int inc = 69;//(float)(endY - startY) / 60 + 0.5;//范围增量

	int index = 0;
	vector<cv::Mat> reduceList;
	vector<cv::Point> points;
	//右边界
	int startY = p_block->B.y + 69;
	if (startY < 0)
		startY = 0;
	int endY = p_block->D.y - 69;
	if (endY >= image.rows)
		endY = image.rows - 1;
	for (int y = startY; y < endY && y < image.rows; y += inc, index++)
	{	
		int y1 = y;
		if (y1 < 0 || y1 >= image.rows)
			continue;
		if ((y1 + ROI_HEIGHT) >= endY)
			y1 = endY - ROI_HEIGHT - 1;
		if ((y1 + ROI_HEIGHT) >= image.rows)
			y1 = image.rows - ROI_HEIGHT - 1;

		int x = p_block->GetPonintByY(y1, &p_block->RightLine).x;
		if (x < 0 || x >= image.cols)
			continue;

		cv::Mat tmpROI = image(cv::Rect(x - ROI_WIDTH, y1, ROI_WIDTH, ROI_HEIGHT)).clone();
		cv::GaussianBlur(tmpROI, tmpROI, cv::Size(5, 5), 0);
#ifdef BED_OUTPUT_DEBUG_INFO
		debug_rights.push_back(tmpROI);
#endif
		tmpROI.convertTo(tmpROI, CV_32F);

		cv::Mat reduceImg;
		cv::reduce(tmpROI, reduceImg, 0, CV_REDUCE_AVG);
		cv::resize(reduceImg, reduceImg, cv::Size(reduceImg.cols / 2, reduceImg.rows));//宽缩减为一半

#ifdef SAVE_IMG
		//保存图片
		char num[10];
		sprintf(num, "%03d", index);
		string strnum(num);
		stringstream ss;
		ss << "EdgeInner\\R\\右_" << strnum << "_reduce.jpg";
		cv::imwrite(ss.str(), reduceImg);
		ss.str("");
		ss << "EdgeInner\\R\\右_" << strnum << ".jpg";
		cv::imwrite(ss.str(), tmpROI);
#endif

		reduceImg = reduceImg.t();
		tmpROI.release();
		reduceList.push_back(reduceImg);
		points.push_back(p_block->GetPonintByY(y, &p_block->RightLine));
	}
	process(reduceList, points, "right");
}


void BlockEdgeDetector::process(vector<cv::Mat> reduceList, vector<cv::Point> points, string info)
{
	if (points.size() != reduceList.size())
		return;

	vector<bool> pointFlag;
	for (size_t i = 0; i < points.size(); i++)
	{
		pointFlag.push_back(0);
	}
#ifdef BED_OUTPUT_DEBUG_INFO
	for (size_t i = 0; i < points.size(); i++)
	{
		stringstream ss;
		ss << i;
		cv::putText(drowDebugResult, ss.str(), points[i], CV_FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 255, 0));
		cv::circle(drowDebugResult, points[i], 2, cv::Scalar(255, 0, 255), -1);
	}
#endif
	//vector<int> errorPointsIndex;
	if (reduceList.size() > 2)
	{
		for (int i = 1; i < reduceList.size() - 3; i++)
		{
			double diff = cv::compareHist(reduceList[i], reduceList[i + 2], CV_COMP_CORREL); //越大越像

#ifdef BED_OUTPUT_DEBUG_INFO
			cv::Mat hist1 = reduceList[i];
			cv::Mat hist2 = reduceList[i + 2];
			double diff1 = cv::compareHist(hist1, hist2, CV_COMP_CORREL); //越大越像
			double diff2 = cv::compareHist(hist1, hist2, CV_COMP_CHISQR); //越小越像
			double diff3 = cv::compareHist(hist1, hist2, CV_COMP_INTERSECT); //越大越像
			double diff4 = cv::compareHist(hist1, hist2, CV_COMP_BHATTACHARYYA); //越小越像
#endif

			if (diff < DIFF_THRESHOLD)
			{
				pointFlag[i] = 1;
				pointFlag[i + 2] = 1;
				//errorPointsIndex.push_back(i);
#ifdef BED_OUTPUT_DEBUG_INFO
				stringstream ss;
				ss << i;
				cv::putText(drowDebugResult, ss.str(), points[i], CV_FONT_HERSHEY_COMPLEX, 1, cv::Scalar(0, 0, 255));
				cv::circle(drowDebugResult, points[i], 2, cv::Scalar(0, 255, 255), -1);
#endif
			}
		}
	}
	//不合并缺陷
	if (points.size() > 2)
	{
		int pointSpanX = abs(points[1].x - points[0].x);
		int pointSpanY = abs(points[1].y - points[0].y);
		int length = pointSpanY > pointSpanX ? pointSpanY : pointSpanX;
		length = length + length;
		for (int i = 0; i < pointFlag.size(); i++)
		{
			if (pointFlag[i])
			{
				Faults::EdgeFault ef;
				ef.position = points[i];
				ef.length = length;
				p_faults->EdgeFaults.push_back(ef);
			}
		}
	}
}
