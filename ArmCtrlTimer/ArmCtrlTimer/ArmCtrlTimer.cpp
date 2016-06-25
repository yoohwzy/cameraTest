// ArmCtrlTimer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Arm.h"
#include "MFCConsole.h"

int _tmain(int argc, _TCHAR* argv[])
{
	MFCConsole::Init();
	Arm arm;

	arm.AddAction(1, 2000);
	arm.AddAction(2, 3000);
	arm.AddAction(3, 4000);
	arm.AddAction(4, 5000);
	arm.AddAction(5, 5000);
	arm.AddAction(6, 5000);

	arm.AddPort(1, 0);
	arm.AddPort(3, 2);
	arm.AddPort(4, 0);
	arm.AddPort(5, 0);
	arm.AddPort(1, -1);

	cv::imshow("1", cv::Mat(10, 20, CV_8U, cv::Scalar(55)));
	cv::waitKey();
	return 0;
}

