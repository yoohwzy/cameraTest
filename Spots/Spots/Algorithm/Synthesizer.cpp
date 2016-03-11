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

//�����㷨������״̬
Synthesizer::Status Synthesizer::Run(cv::Mat TileImg)
{
	p_block = new Block(TileImg.size().width, TileImg.size().height);

	////��ȡ��ֵ��ͼ��
	if (TileImg.cols == 0)
	{
		MFCConsole::Output("Img Empty!\r\n");
		return Status::NoImage;
	}
	cv::Mat grayImg = TileImg;
	if (grayImg.channels() == 3)
		cv::cvtColor(grayImg, grayImg, CV_BGR2GRAY);

	//��λ
	int status = positioning(grayImg);
	if (status != Synthesizer::_Status::_NEXT)
	{
		return status == _Status::_NotFound ? Status::NotFound : Status::Rejected; 
	}


	// ��Եȱ��
	status = detectEdge(grayImg);
	if (status != Synthesizer::_Status::_NEXT)
	{
		return Status::Rejected;
	}

	//����ȱ��

	return Status::TypeA;
}


//��ש��λ�������Ƿ��ҵ���ש
Synthesizer::_Status Synthesizer::positioning(cv::Mat grayImg)
{
	stringstream ss;
	double t = 0;
	if (MFCConsole::IsOpened)
		t = cv::getTickCount();

	//���ж�λ
	BlockLocalizer bl = BlockLocalizer(grayImg, p_block, &faults);

	if (MFCConsole::IsOpened)
	{
		t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
		cout << "��λ�㷨BlockLocalizer��" << t << endl;
	}

	if (bl.NotFoundBlockFlag == true)
	{
		//δ��⵽��ש
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
	////��ש�ֶ�λ
	//if (1 == 1)//ʹ��if�����ֲ�����
	//{
	//	BlocksDetector bd = BlocksDetector(threshodImg, ConsumerLedStartX, ConsumerLedEndX);
	//	double t = (double)cv::getTickCount();
	//	//BlocksDetector�����ж��Ƿ��⵽������ש
	//	bool left_right = bd.Start();
	//	bool up = bd.StartUP_DOWN(BlocksDetector::Up);
	//	bool down = bd.StartUP_DOWN(BlocksDetector::Down);
	//	if (!left_right ||
	//		!up ||
	//		!down)
	//	{
	//		if (!left_right)
	//			MFCConsole::Output("����δ�ҵ�\r\n");
	//		else if (!up)
	//			MFCConsole::Output("��δ�ҵ�\r\n");
	//		else if (!down)
	//			MFCConsole::Output("��δ�ҵ�\r\n");
	//		return false;
	//	}
	//	//ss << "bd.Start() && bd.StartUP_DOWN(BlocksDetector::Up) &&	bd.StartUP_DOWN(BlocksDetector::Down)" << endl;


	//	p_block->UpLine = bd.UpLine;
	//	p_block->DownLine = bd.DownLine;
	//	p_block->LeftLine = bd.LeftLine;
	//	p_block->RightLine = bd.RightLine;


	//	//�жϸ��������Ƿ�С��0����ڿ��
	//	if (!p_block->Lines2ABCD())
	//	{
	//		MFCConsole::Output("��ש�б߽�λ��ͼƬ��\r\n");
	//		p_block->ABCDAdjust();
	//	}

	//	ss << "p_block->ABCD()" << endl;
	//	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	//	ss << SN << " " << "BlocksDetector��" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	//	MFCConsole::Output(ss.str());
	//}
	//return false;
}
//��Եȱ�ݼ��
Synthesizer::_Status Synthesizer::detectEdge(cv::Mat grayImg)
{
	stringstream ss;
	double t = 0;
	if (MFCConsole::IsOpened)
		t = cv::getTickCount();

	//���ж�λ
	BlockEdgeDetector bed = BlockEdgeDetector(grayImg, p_block, &faults);

	if (MFCConsole::IsOpened)
	{
		t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
		cout << "��λ�㷨BlockLocalizer��" << t << endl;
	}
	return _Status::_NEXT;


	//stringstream ss;
	//double t = (double)cv::getTickCount();


	//// �㵽���ֱ�ߵľ�����ֵ
	//int distance_threld = 20;
	//// �ж������������������ֵ�����ڸ�ֵ��Ϊ�������ĵ��쳣������Ϊ����
	//int Edge_threld = 20;

	////��ש��ȷ��λ  &&  ���߼��
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
	//ss << SN << " " << "EdgeDetector��" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	//t = (double)cv::getTickCount();
	//if (faults.BrokenEdges.size() > 0)
	//{
	//	ss << SN << " " << "��Ե��ȱ�ݣ�������" << faults.BrokenEdges.size() << endl;
	//}
	//MFCConsole::Output(ss.str());
	return _Status::_NEXT;
}
//�ڲ�ȱ�ݼ��
Synthesizer::_Status Synthesizer::detectInner(cv::Mat grayImg)
{
	stringstream ss;
	double t = (double)cv::getTickCount();

	//��ש�ڲ�ȱ�ݼ��
	//ss << "��ש�ڲ�ȱ�ݼ�� ��ʼ" << endl;
	Pretreatment p;
	p.pretreatment(grayImg, p_block, &faults);






	ss << SN << " " << "�ڲ��л��ۣ�" << faults.Scratchs.size() << endl;
	ss << SN << " " << "�ڲ��а��㣺" << faults.Holes.size() << endl;
	ss << "��ש�ڲ�ȱ�ݼ�� ����" << endl;

	t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
	ss << SN << " " << "�ڲ�ȱ�ݼ�⣺" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
	MFCConsole::Output(ss.str());
	return _Status::_NEXT;
}