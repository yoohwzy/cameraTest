#include "EdgeInnerDetctor.h"


EdgeInnerDetctor::EdgeInnerDetctor(cv::Mat &img, Block *_block, Faults *_faults)
{
	image = img;
	//if (img.channels() == 1)
	//	image = img.clone();
	//else
	//	cv::cvtColor(img, image, CV_RGB2GRAY);
	p_block = _block;
	p_faults = _faults;
	_block = NULL;
	_faults = NULL;


	p_block->ABCD2Lines();


	thread t_up = thread(std::mem_fn(&EdgeInnerDetctor::doUp), this);
	thread t_down = thread(std::mem_fn(&EdgeInnerDetctor::doDown), this);
	t_up.join();
	t_down.join();

	thread t_Left = thread(std::mem_fn(&EdgeInnerDetctor::doLeft), this);
	thread t_Right = thread(std::mem_fn(&EdgeInnerDetctor::doRight), this);
	t_Left.join();
	t_Right.join();
}


void EdgeInnerDetctor::doUp()
{
	int index = 0;
	vector<cv::Mat> reduceList;
	vector<cv::Point3f> points;

	//上边界
	int startX = p_block->A.x + 30;
	int endX = p_block->B.x - 30;
	int inc = (float)(endX - startX) / 30 + 0.5;//范围增量
	for (int x = startX; x < endX; x += inc, index++)
	{
		int y = p_block->GetPonintByX(x, &p_block->UpLine).y;
		if (y < 0)
			y = 0;
		if (y >= image.rows)
			y = image.rows - 1;

		cv::Mat tmpROI;
		if ((x + inc) > endX)
			tmpROI = image(cv::Rect(x, y, (endX - x), 200)).clone();
		else
			tmpROI = image(cv::Rect(x, y, inc, 200)).clone();
		cv::Mat reduceImg;

		cv::reduce(tmpROI, reduceImg, 1, CV_REDUCE_AVG);
		cv::resize(reduceImg, reduceImg, cv::Size(reduceImg.cols, reduceImg.rows / 2));//高度缩减为一半
		cv::GaussianBlur(reduceImg, reduceImg, cv::Size(5, 5), 0);


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

		reduceImg.convertTo(reduceImg, CV_64F);
		reduceList.push_back(reduceImg);
		points.push_back(cv::Point3f(x, p_block->GetPonintByX(x, &p_block->UpLine).y, inc));
	}
	processAndSaveData(reduceList, points, "U\\上");
}
void EdgeInnerDetctor::doDown()
{
	int index = 0;
	vector<cv::Mat> reduceList;
	vector<cv::Point3f> points;
	//下边界
	int startX = p_block->D.x + 30;
	int endX = p_block->C.x - 30;
	int inc = (float)(endX - startX) / 30 + 0.5;//范围增量
	for (int x = startX; x < endX; x += inc, index++)
	{
		int y = p_block->GetPonintByX(x, &p_block->DownLine).y;
		if (y < 0)
			y = 0;
		if (y >= image.rows)
			y = image.rows - 1;

		cv::Mat tmpROI;
		if ((x + inc) > endX)
			tmpROI = image(cv::Rect(x, y - 200, (endX - x), 200)).clone();
		else
			tmpROI = image(cv::Rect(x, y - 200, inc, 200)).clone();
		cv::Mat reduceImg;

		cv::reduce(tmpROI, reduceImg, 1, CV_REDUCE_AVG);
		cv::resize(reduceImg, reduceImg, cv::Size(reduceImg.cols, reduceImg.rows / 2));//高度缩减为一半
		cv::GaussianBlur(reduceImg, reduceImg, cv::Size(5, 5), 0);

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

		reduceImg.convertTo(reduceImg, CV_64F);
		reduceList.push_back(reduceImg);
		points.push_back(cv::Point3f(x, p_block->GetPonintByX(x, &p_block->DownLine).y, inc));
	}
	processAndSaveData(reduceList, points, "D\\下");
}
void EdgeInnerDetctor::doLeft()
{
	int index = 0;
	vector<cv::Mat> reduceList;
	vector<cv::Point3f> points;
	//左边界
	int startY = p_block->A.y + 100;
	int endY = p_block->D.y - 100;
	int inc = (float)(endY - startY) / 60 + 0.5;//范围增量
	for (int y = startY; y < endY; y += inc, index++)
	{
		int x = p_block->GetPonintByY(y, &p_block->LeftLine).x;
		if (x < 0) 
			x = 0;
		cv::Mat tmpROI;
		if ((y + inc) > endY)
			tmpROI = image(cv::Rect(x, y, 200, (endY - y))).clone();
		else
			tmpROI = image(cv::Rect(x, y, 200, inc)).clone();

		cv::Mat reduceImg;

		cv::reduce(tmpROI, reduceImg, 0, CV_REDUCE_AVG);
		cv::resize(reduceImg, reduceImg, cv::Size(reduceImg.cols / 2, reduceImg.rows));//宽缩减为一半
		cv::GaussianBlur(reduceImg, reduceImg, cv::Size(5, 5), 0);

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

		tmpROI.release();

		reduceImg.convertTo(reduceImg, CV_64F);
		reduceList.push_back(reduceImg);
		points.push_back(cv::Point3f(p_block->GetPonintByY(y, &p_block->LeftLine).x, y, inc));

	}
	processAndSaveData(reduceList, points, "L\\左");
}
void EdgeInnerDetctor::doRight()
{
	int index = 0;
	vector<cv::Mat> reduceList;
	vector<cv::Point3f> points;
	//右边界
	int startY = p_block->A.y + 100;
	int endY = p_block->D.y - 100;
	int inc = (float)(endY - startY) / 60 + 0.5;//范围增量
	for (int y = startY; y < endY; y += inc, index++)
	{
		int x = p_block->GetPonintByY(y, &p_block->RightLine).x;
		if (x < 0)
			x = 0;
		cv::Mat tmpROI;
		if ((y + inc) > endY)
			tmpROI = image(cv::Rect(x - 200, y, 200, (endY - y))).clone();
		else
			tmpROI = image(cv::Rect(x - 200, y, 200, inc)).clone();
		cv::Mat reduceImg;

		cv::reduce(tmpROI, reduceImg, 0, CV_REDUCE_AVG);
		cv::resize(reduceImg, reduceImg, cv::Size(reduceImg.cols / 2, reduceImg.rows));//宽缩减为一半
		cv::GaussianBlur(reduceImg, reduceImg, cv::Size(5, 5), 0);

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

		tmpROI.release();

		reduceImg.convertTo(reduceImg, CV_64F);
		reduceList.push_back(reduceImg);
		points.push_back(cv::Point3f(p_block->GetPonintByY(y, &p_block->RightLine).x, y, inc));
	}
	processAndSaveData(reduceList, points, "R\\右");
}



double EdgeInnerDetctor::getFrechetDistance(vector<double> lineA, vector<double> lineB)
{
	int lineASize = lineA.size();
	int lineBSize = lineB.size();

	double dist = 100000;
	if (lineBSize > lineASize)
		return dist;

	vector<double> tmpDists;
	//计算初始距离
	double tmpDist = -1;
	//左移、右移N次距离
	for (int offset = -2; offset < 2; offset++)
	{
		tmpDist = -1;
		int x2 = offset;
		for (int x1 = 0; x1 < lineASize; x1++)
		{
			if (x2 >(lineBSize - 1))
				break;
			if (x2 < 0)
			{
				x2++;
				continue;
			}
			double y1 = (double)lineA[x1];
			double y2 = (double)lineB[x2];
			double thisTurnDist = sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
			if (tmpDist < thisTurnDist)//保存每轮的最大值
				tmpDist = thisTurnDist;
			x2++;
		}
		if (tmpDist != -1)
			tmpDists.push_back(tmpDist);
		else if (tmpDist == 0)
		{
			return 0;
		}
	}
	for (int i = 0; i < tmpDists.size(); i++)//返回最大值数组中的最小值即是距离
	{
		if (dist > tmpDists[i])
			dist = tmpDists[i];
	}
	return dist;
}
double EdgeInnerDetctor::getFrechetDistance(cv::Mat lineA, cv::Mat lineB)
{
	int lineASize = lineA.rows;
	int lineBSize = lineB.rows;
	if (lineA.rows == 1 && lineB.rows == 1)
	{
		lineASize = lineA.cols;
		lineBSize = lineB.cols;
	}
	else if (lineA.cols == 1 && lineB.cols == 1)
	{

	}
	else
	{
		return 100000;
	}

	double dist = 100000;
	if (lineBSize > lineASize)
		return dist;

	vector<double> tmpDists;
	//计算初始距离
	double tmpDist = -1;
	//左移、右移N次距离
	for (int offset = 0; offset < 10; offset++)
	{
		tmpDist = -1;
		int x2 = offset;
		for (int x1 = 0; x1 < lineASize; x1++)
		{
			if (x2 >(lineBSize - 1))
				break;
			if (x2 < 0)
			{
				x2++;
				continue;
			}
			double y1 = 0;//(double)lineA.ptr<double>(x1)[0];
			double y2 = 0;//(double)lineB.ptr<double>(x2)[0];
			if (lineA.rows == 1 && lineB.rows == 1)
			{
				y1 = (double)lineA.ptr<double>(0)[x1];
				y2 = (double)lineB.ptr<double>(0)[x2];
			}
			else
			{
				y1 = (double)lineA.ptr<double>(x1)[0];
				y2 = (double)lineB.ptr<double>(x2)[0];
			}
			double thisTurnDist = sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
			if (tmpDist < thisTurnDist)//保存每轮的最大值
				tmpDist = thisTurnDist;
			x2++;
		}
		if (tmpDist != -1)
			tmpDists.push_back(tmpDist);
		else if (tmpDist == 0)
		{
			return 0;
		}
	}
	for (int i = 0; i < tmpDists.size(); i++)//返回最大值数组中的最小值即是距离
	{
		if (dist > tmpDists[i])
			dist = tmpDists[i];
	}
	return dist;
}

void EdgeInnerDetctor::processAndSaveData(vector<cv::Mat> reduceList, vector<cv::Point3f> points, string prefix)
{
	//正规化
	vector<double> distancesL2R;
	vector<double> distancesR2L;
	vector<double> distances;
	for (int i = 0; i < reduceList.size(); i++)
	{
		distancesL2R.push_back(0);
		distancesR2L.push_back(0);
		distances.push_back(0);
		double maxVal = 0; //最大值一定要赋初值，否则运行时会报错
		cv::Point maxLoc;
		minMaxLoc(reduceList[i], NULL, &maxVal, NULL, &maxLoc);
		if (maxVal > 0)
			reduceList[i] /= maxVal;
	}




	//从左到右预测
	cv::Mat diff = reduceList[1] - reduceList[0];
	float k = 0.4;//遗忘因子
	for (int i = 2; i < reduceList.size(); i++)
	{
		diff = diff * k + (reduceList[i - 1] - reduceList[i - 2]) *(1 - k);
		distancesL2R[i] = getFrechetDistance(reduceList[i] - reduceList[i - 1], diff);
	}

	//从右到左预测
	cv::Mat diffR2L = reduceList[reduceList.size() - 1] - reduceList[reduceList.size() - 2];
	for (int i = reduceList.size() - 3; i >= 0; i--)
	{
		diffR2L = diffR2L * k + (reduceList[i + 1] - reduceList[i + 2]) *(1 - k);
		distancesR2L[i] = getFrechetDistance(reduceList[i] - reduceList[i + 1], diffR2L);
	}

	vector<double> weightsL2R;//L2R距离权重
	weightsL2R.push_back(0);
	weightsL2R.push_back(0);
	double w = 0.05;
	double wadd = 0.9 / (reduceList.size() - 1 - 2);
	for (int i = 2; i < reduceList.size(); i++)
	{
		weightsL2R.push_back(w);
		w += wadd;
	}

	//根据权重生成最终距离
	for (int i = 0; i < reduceList.size(); i++)
	{
		if (weightsL2R[i] < 0.3 || weightsL2R[i] > 0.7)
			distances[i] = distancesL2R[i] * weightsL2R[i] + distancesR2L[i] * (1 - weightsL2R[i]);
		else
			distances[i] = distancesL2R[i] > distancesR2L[i] ? distancesL2R[i] : distancesR2L[i];

		//根据距离判断缺陷
		if (distances[i] > 0.6)
		{
			Faults::SomethingBig sb;
			sb.position.x = points[i].x;
			sb.position.y = points[i].y;
			sb.diameter = points[i].z;
			p_faults->SomethingBigs.push_back(sb);
		}
	}


#ifdef SAVE_IMG
	stringstream ss;
	ss << "EdgeInner\\" << prefix << "预测弗雷歇距离.txt";
	ofstream of_DiffFrechetDist(ss.str(), ios::out);
	of_DiffFrechetDist << "L2R,";
	for (int i = 0; i < reduceList.size(); i++)
	{
		of_DiffFrechetDist << distancesL2R[i] << ",";
	}
	of_DiffFrechetDist << "\r\n";
	of_DiffFrechetDist << "R2L,";
	for (int i = 0; i < reduceList.size(); i++)
	{
		of_DiffFrechetDist << distancesR2L[i] << ",";
	}
	of_DiffFrechetDist << "\r\n";
	of_DiffFrechetDist << "加权距离,";
	for (int i = 0; i < reduceList.size(); i++)
	{
		of_DiffFrechetDist << distances[i] << ",";
	}
#endif
}

