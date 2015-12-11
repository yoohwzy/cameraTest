#include "EdgeInnerDetctor.h"


EdgeInnerDetctor::EdgeInnerDetctor(cv::Mat &img, Block *_block, Faults *_faults)
{
	if (img.channels() == 1)
		image = img.clone();
	else
		cv::cvtColor(img, image, CV_RGB2GRAY);
	block = _block;
	faults = _faults;


	block->ABCD2Lines();
	/*cv::line(image, block->A, block->GetPonintByY(3000, &block->LeftLine), cv::Scalar(255));
	cv::line(image, block->A, block->GetPonintByX(3000, &block->UpLine), cv::Scalar(255));
	cv::line(image, block->D, block->GetPonintByX(3000, &block->DownLine), cv::Scalar(255));
	cv::line(image, block->C, block->GetPonintByY(3000, &block->RightLine), cv::Scalar(255));*/



	if (1 == 1)
	{
		int index = 0;
		vector<cv::Mat> reduceList;
		vector<cv::Point3f> points;

		//上边界
		int startX = block->A.x + 30;
		int endX = block->B.x - 30;
		int inc = (float)(endX - startX) / 30 + 0.5;//范围增量
		for (int x = startX; x < endX; x += inc, index++)
		{
			cv::Mat tmpROI;
			if ((x + inc) > endX)
				tmpROI = image(cv::Rect(x, block->GetPonintByX(x, &block->UpLine).y, (endX - x), 200)).clone();
			else
				tmpROI = image(cv::Rect(x, block->GetPonintByX(x, &block->UpLine).y, inc, 200)).clone();
			cv::Mat reduceImg;

			cv::reduce(tmpROI, reduceImg, 1, CV_REDUCE_AVG);
			cv::resize(reduceImg, reduceImg, cv::Size(reduceImg.cols, reduceImg.rows / 2));//高度缩减为一半
			cv::GaussianBlur(reduceImg, reduceImg, cv::Size(5, 5), 0);


#ifdef WRITEIMG
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
			points.push_back(cv::Point3f(x, block->GetPonintByX(x, &block->UpLine).y, inc / 2));
		}
		processAndSaveData(reduceList, points, "U\\上");
	}
	if (2 == 2)
	{
		int index = 0;
		vector<cv::Mat> reduceList;
		vector<cv::Point3f> points;
		//下边界
		int startX = block->D.x + 30;
		int endX = block->C.x - 30;
		int inc = (float)(endX - startX) / 30 + 0.5;//范围增量
		for (int x = startX; x < endX; x += inc, index++)
		{
			cv::Mat tmpROI = image(cv::Rect(x, block->GetPonintByX(x, &block->DownLine).y - 200, inc, 200)).clone();
			if ((x + inc) > endX)
				tmpROI = image(cv::Rect(x, block->GetPonintByX(x, &block->DownLine).y - 200, (endX - x), 200)).clone();
			cv::Mat reduceImg;

			cv::reduce(tmpROI, reduceImg, 1, CV_REDUCE_AVG);
			cv::resize(reduceImg, reduceImg, cv::Size(reduceImg.cols, reduceImg.rows / 2));//高度缩减为一半
			cv::GaussianBlur(reduceImg, reduceImg, cv::Size(5, 5), 0);

#ifdef WRITEIMG
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
			points.push_back(cv::Point3f(x, block->GetPonintByX(x, &block->DownLine).y, inc / 2));
		}
		processAndSaveData(reduceList, points, "D\\下");
	}


	/*


	if (3 == 3)
	{
	int index = 0;
	vector<vector<double>> reduceList;
	vector<vector<double>> differList;
	//左边界
	int startY = block->A.y + 100;
	int endY = block->D.y - 100;
	int inc = (float)(endY - startY) / 60 + 0.5;//范围增量
	for (int y = startY; y < endY; y += inc, index++)
	{
	cv::Mat tmpROI = image(cv::Rect(block->GetPonintByY(y, &block->LeftLine).x, y, 200, inc)).clone();
	if ((y + inc) > endY)
	tmpROI = image(cv::Rect(block->GetPonintByY(y, &block->LeftLine).x, y, 200, (endY - y))).clone();
	cv::Mat reduceImg;

	cv::reduce(tmpROI, reduceImg, 0, CV_REDUCE_AVG);
	cv::resize(reduceImg, reduceImg, cv::Size(reduceImg.cols/2, reduceImg.rows));//宽缩减为一半
	cv::GaussianBlur(reduceImg, reduceImg, cv::Size(5, 5), 0);

	#ifdef WRITEIMG
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

	vector<double> reduceMat;
	vector<double> diffMat;

	for (int j = 0; j < reduceImg.cols - 1; j++)
	{
	reduceMat.push_back(reduceImg.ptr<uchar>(0)[j]);
	int different = reduceImg.ptr<uchar>(0)[j + 1] - reduceImg.ptr<uchar>(0)[j];
	diffMat.push_back(different);
	}

	reduceList.push_back(reduceMat);
	differList.push_back(diffMat);
	}
	processAndSaveData(reduceList, differList, "L\\左");
	}


	if (4 == 4)
	{
	int index = 0;
	vector<vector<double>> reduceList;
	vector<vector<double>> differList;
	//右边界
	int startY = block->A.y + 100;
	int endY = block->D.y - 100;
	int inc = (float)(endY - startY) / 60 + 0.5;//范围增量
	for (int y = startY; y < endY; y += inc, index++)
	{
	cv::Point p = block->GetPonintByY(y, &block->RightLine);
	cv::Mat tmpROI = image(cv::Rect(block->GetPonintByY(y, &block->RightLine).x - 200, y, 200, inc)).clone();
	if ((y + inc) > endY)
	tmpROI = image(cv::Rect(block->GetPonintByY(y, &block->RightLine).x - 200, y, 200, (endY - y))).clone();
	cv::Mat reduceImg;

	cv::reduce(tmpROI, reduceImg, 0, CV_REDUCE_AVG);
	cv::resize(reduceImg, reduceImg, cv::Size(reduceImg.cols / 2, reduceImg.rows));//宽缩减为一半
	cv::GaussianBlur(reduceImg, reduceImg, cv::Size(5, 5), 0);

	#ifdef WRITEIMG
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

	vector<double> reduceMat;
	vector<double> diffMat;

	for (int j = 0; j < reduceImg.cols - 1; j++)
	{
	reduceMat.push_back(reduceImg.ptr<uchar>(0)[j]);
	int different = reduceImg.ptr<uchar>(0)[j + 1] - reduceImg.ptr<uchar>(0)[j];
	diffMat.push_back(different);
	}

	reduceList.push_back(reduceMat);
	differList.push_back(diffMat);
	}
	processAndSaveData(reduceList, differList, "R\\右");
	}
	*/
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
	for (int offset = 0; offset < 1; offset++)
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
				y2 = (double)lineA.ptr<double>(0)[x2];
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
			faults->SomethingBigs.push_back(sb);
		}
	}


#ifdef WRITEIMG
	stringstream ss;
	ss << "EdgeInner\\" << prefix << "预测弗雷歇距离.txt";
	ofstream of_DiffFrechetDist(ss.str(), ios::out);
	for (int i = 0; i < reduceList.size(); i++)
	{
		of_DiffFrechetDist << distancesL2R[i] << ",";
	}
	of_DiffFrechetDist << "\r\n";
	for (int i = 0; i < reduceList.size(); i++)
	{
		of_DiffFrechetDist << distancesR2L[i] << ",";
	}
	of_DiffFrechetDist << "\r\n";
	for (int i = 0; i < reduceList.size(); i++)
	{
		of_DiffFrechetDist << distances[i] << ",";
	}
#endif
	}




EdgeInnerDetctor::~EdgeInnerDetctor()
{
	block = NULL;
	faults = NULL;
}
