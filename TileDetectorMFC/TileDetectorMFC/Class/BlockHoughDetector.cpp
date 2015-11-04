#include "BlockHoughDetector.h"


BlockHoughDetector::BlockHoughDetector(cv::Mat img)
{
	if (img.channels() > 1)
		cv::cvtColor(img, img, CV_BGR2GRAY);
	cv::Mat left = img.clone()(cv::Rect(100, 0, 400, img.rows));

	//vector<int> o;
	//o.push_back(-1);
	//o.push_back(-2);
	//o.push_back(-2);
	//o.push_back(-2);
	//o.push_back(0);//0点表示
	//o.push_back(1);
	//o.push_back(1);
	//o.push_back(1);
	//o.push_back(1);
	//o.push_back(1);
	//o.push_back(1);
	//o.push_back(1);
	//Caculte(left, o);


	//char o[] = {
	//	-5, 0, 0, 1, 1, 1, 1,
	//	-5, 0, 0, 1, 1, 1, 1,
	//	-5, 0, 0, 1, 1, 1, 1 };
	char o[] = {
		-5, 0, 0, 1, 1, 1, 1 ,1,1,1};

	cv::Point center = cv::Point(3, 2);
	cv::Mat _operator(3, 7, CV_8S);
	InitMat(_operator, o);
	InitMat(_operator, o);
	Caculte(left, _operator, center);
}

//用一个一维数组来初始化矩阵
void BlockHoughDetector::InitMat(cv::Mat& m, char* num)
{
	for (int i = 0; i<m.rows; i++)
		for (int j = 0; j<m.cols; j++)
			m.at<char>(i, j) = *(num + i*m.cols + j);
}

BlockHoughDetector::~BlockHoughDetector()
{
}
cv::Mat BlockHoughDetector::Caculte(cv::Mat& img, cv::Mat& _operator, cv::Point operatorCenter)
{
	if (operatorCenter.x <= 0)
		operatorCenter.x = 1;
	if (operatorCenter.y <= 0)
		operatorCenter.y = 1;

	cv::Mat result(img.size(), CV_8U, cv::Scalar(0));
	for (int i = operatorCenter.y - 1; i < img.rows - (_operator.rows - operatorCenter.y + 1); i++)
	{
		//cout << i - (operatorCenter.y - 1)  << endl;
		for (int j = operatorCenter.x - 1; j < img.cols - (_operator.cols - operatorCenter.x); j++)
		{
			cv::Mat roi = img(cv::Rect(j - (operatorCenter.x - 1), i - (operatorCenter.y - 1), _operator.cols, _operator.rows));
			int sum = 0;
			//img.ptr<uchar>(i)[j] = 255;
			//cout << i << " " << j << endl;
			for (int m = 0; m < _operator.rows; m++)
			{
				for (int n = 0; n < _operator.cols; n++)
				{
					sum += roi.ptr<uchar>(m)[n] * _operator.ptr<char>(m)[n];
				}
			}
			if (sum > 255)
				sum = 255;
			if (sum < 0)
				sum = 0;

			if (sum > 25)
				result.ptr<uchar>(i)[j] = 255;
			else
				result.ptr<uchar>(i)[j] = sum;

		}
	}
	return result;
}