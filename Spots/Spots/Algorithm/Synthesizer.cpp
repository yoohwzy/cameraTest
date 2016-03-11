#include "Synthesizer.h"
#include <Algorithm/BlockLocalizer.h>
#include <Algorithm/BlockEdgeDetector.h>
#include <Class\Debug\MFCConsole.h>

Synthesizer::Synthesizer(int _SN)
{
	SN = _SN;
}


Synthesizer::~Synthesizer()
{
	//delete p_block;
}

//运行算法，返回状态
Synthesizer::Status Synthesizer::Run(cv::Mat TileImg)
{
	p_block = new Block(TileImg.size().width, TileImg.size().height);

	////获取二值化图像
	if (TileImg.cols == 0)
	{
		MFCConsole::Output("Img Empty!\r\n");
		return Status::NoImage;
	}
	cv::Mat grayImg = TileImg;
	if (grayImg.channels() == 3)
		cv::cvtColor(grayImg, grayImg, CV_BGR2GRAY);

	//定位
	int status = positioning(grayImg);
	if (status != Synthesizer::_Status::_NEXT)
	{
		return status == _Status::_NotFound ? Status::NotFound : Status::Rejected; 
	}


	// 边缘缺陷
	status = detectEdge(grayImg);
	if (status != Synthesizer::_Status::_NEXT)
	{
		return Status::Rejected;
	}

	//表面缺陷

	return Status::TypeA;
}


//瓷砖定位，返回是否找到瓷砖
Synthesizer::_Status Synthesizer::positioning(cv::Mat grayImg)
{
	stringstream ss;
	double t = 0;
	if (MFCConsole::IsOpened)
		t = cv::getTickCount();

	//进行定位
	BlockLocalizer bl = BlockLocalizer(grayImg, p_block, &faults);

	if (MFCConsole::IsOpened)
	{
		t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
		cout << "定位算法BlockLocalizer：" << t << endl;
	}

	if (bl.NotFoundBlockFlag == true)
	{
		//未检测到瓷砖
		return _Status::_NotFound;
	}
	if (bl.BrokenEdgeFlag == true)
	{
		//cv::cvtColor(img, img, CV_GRAY2BGR);
		//for (size_t i = 0; i < faults.BrokenEdges.size(); i++)
		//{
		//	cv::circle(img, faults.BrokenEdges[i].position, faults.BrokenEdges[i].length / 2, cv::Scalar(0, 0, 255), 10);
		//}
		//cv::namedWindow("1", 0);
		//cv::imshow("1", img);
		//cv::waitKey(0);
		return _Status::_Edge_Broken;
	}
	return _Status::_NEXT;





	//cv::Mat threshodImg;
	//cv::threshold(grayImg, threshodImg, ConsumerThreshodLow, 255, 0);
	//cv::Mat threshodImgHight;
	//cv::threshold(grayImg, threshodImgHight, ConsumerThreshodHight, 255, 0);
	////瓷砖粗定位
	//if (1 == 1)//使用if隔绝局部变量
	//{
	//	BlocksDetector bd = BlocksDetector(threshodImg, ConsumerLedStartX, ConsumerLedEndX);
	//	double t = (double)cv::getTickCount();
	//	//BlocksDetector加入判断是否检测到完整瓷砖
	//	bool left_right = bd.Start();
	//	bool up = bd.StartUP_DOWN(BlocksDetector::Up);
	//	bool down = bd.StartUP_DOWN(BlocksDetector::Down);
	//	if (!left_right ||
	//		!up ||
	//		!down)
	//	{
	//		if (!left_right)
	//			MFCConsole::Output("左右未找到\r\n");
	//		else if (!up)
	//			MFCConsole::Output("上未找到\r\n");
	//		else if (!down)
	//			MFCConsole::Output("下未找到\r\n");
	//		return false;
	//	}
	//	//ss << "bd.Start() && bd.StartUP_DOWN(BlocksDetector::Up) &&	bd.StartUP_DOWN(BlocksDetector::Down)" << endl;


	//	p_block->UpLine = bd.UpLine;
	//	p_block->DownLine = bd.DownLine;
	//	p_block->LeftLine = bd.LeftLine;
	//	p_block->RightLine = bd.RightLine;


	//	//判断各点坐标是否小于0或大于宽度
	//	if (!p_block->Lines2ABCD())
	//	{
	//		MFCConsole::Output("瓷砖有边角位于图片外\r\n");
	//		p_block->ABCDAdjust();
	//	}

	//	ss << "p_block->ABCD()" << endl;
	//	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	//	ss << SN << " " << "BlocksDetector：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	//	MFCConsole::Output(ss.str());
	//}
	//return false;
}
//边缘缺陷检测
Synthesizer::_Status Synthesizer::detectEdge(cv::Mat grayImg)
{
	stringstream ss;
	double t = 0;
	if (MFCConsole::IsOpened)
		t = cv::getTickCount();

	//进行定位
	BlockEdgeDetector bed = BlockEdgeDetector(grayImg, p_block, &faults);

	if (MFCConsole::IsOpened)
	{
		t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
		cout << "定位算法BlockLocalizer：" << t << endl;
	}
	return _Status::_NEXT;


	//stringstream ss;
	//double t = (double)cv::getTickCount();


	//// 点到拟合直线的距离阈值
	//int distance_threld = 20;
	//// 判定崩边连续点点数量阈值，大于该值认为有连续的点异常，则认为崩边
	//int Edge_threld = 20;

	////瓷砖精确定位  &&  崩边检测
	//Block tmpb = Block(p_block->imageWidth, p_block->imageHeight);
	//tmpb.LeftLine = p_block->LeftLine;
	//tmpb.RightLine = p_block->RightLine;
	//tmpb.UpLine = p_block->UpLine;
	//tmpb.DownLine = p_block->DownLine;
	//tmpb.Lines2ABCD();
	//EdgeDetector ed = EdgeDetector(grayImg, &tmpb, &faults);
	////EdgeDetector ed = EdgeDetector(grayImg, p_block, &faults);
	////ed.ThreshodImgHigh = threshodImgHight;
	////ed.ThreshodImgLow = threshodImg;
	////ed.ThreshodHigh = ConsumerThreshodHight;
	////ed.ThreshodLow = ConsumerThreshodLow;
	////ed.grayImg = grayImg;

	//ed.distance_threld = distance_threld;
	//ed.Edge_threld = Edge_threld;
	//ed.start();

	//t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	//ss << SN << " " << "EdgeDetector：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	//t = (double)cv::getTickCount();
	//if (faults.BrokenEdges.size() > 0)
	//{
	//	ss << SN << " " << "边缘有缺陷，数量：" << faults.BrokenEdges.size() << endl;
	//}
	//MFCConsole::Output(ss.str());
	return _Status::_NEXT;
}
//内部缺陷检测
Synthesizer::_Status Synthesizer::detectInner(cv::Mat grayImg)
{
	stringstream ss;
	double t = (double)cv::getTickCount();

	//瓷砖内部缺陷检测
	//ss << "瓷砖内部缺陷检测 开始" << endl;
	Pretreatment p;
	p.pretreatment(grayImg, p_block, &faults);






	ss << SN << " " << "内部有划痕：" << faults.Scratchs.size() << endl;
	ss << SN << " " << "内部有凹点：" << faults.Holes.size() << endl;
	ss << "瓷砖内部缺陷检测 结束" << endl;

	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	ss << SN << " " << "内部缺陷检测：" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	MFCConsole::Output(ss.str());
	return _Status::_NEXT;
}