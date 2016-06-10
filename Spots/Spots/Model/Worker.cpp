#include "Worker.h"
#include <Controller/ControllerModel.h>
#include <Class/Debug/MFCConsole.h>


//�����㷨

#include <Algorithm/BlockLocalizer.h>
#include <Algorithm/BlockEdgeSimilarDetector.h>
#include <Algorithm/BlockEdgeLineDetector.h>
#include <Algorithm/EdgeFaultLineDetector.h>
Worker::Worker(string WorkerInfo)
{
	workerInfo = WorkerInfo;
}


Worker::~Worker()
{
	if (P_Faults != NULL)
	{
		delete P_Faults;
		P_Faults = NULL;
	}
	if (P_Block != NULL)
	{
		delete P_Block;
		P_Block = NULL;
	}
}

void Worker::StartWork()
{
	std::thread t_workThread(std::mem_fn(&Worker::work), this);
	t_workThread.detach();
	//work();
}
void Worker::work()
{
	if (P_Faults != NULL)
	{
		delete P_Faults;
		P_Faults = NULL;
	}
	if (P_Block != NULL)
	{
		delete P_Block;
		P_Block = NULL;
	}
	P_Faults = new Faults();
	P_Block = new Block(image.cols, image.rows);
	MyStatus = WorkerStatus::InProcessing;
	cv::Mat grayImg = image;
	/********************************************/
	//��ʼͼ����
	if (image.channels() == 3)
		cv::cvtColor(grayImg, grayImg, CV_BGR2GRAY);


	//TODO:��ʱ�����滯����ͼƬ
	cv::normalize(grayImg, grayImg, 0, 100, cv::NORM_MINMAX);


	//��ʼͼ����
	Status status = algorithmSynthesize(grayImg);
	
	//cv::Mat tmp = grayImg.clone();
	if (status != Worker::Status::NotFound)
	{
		int type = 1;//��Ʒ����  1 A�� 2 B�� 3 C�� 4 ���ϸ�
		if (status == Status::SizeError)
		{
			type = 4;
		}
		else
		{
			//����ȱ��ͼ��
			drawFaults(grayImg);
			cv::line(grayImg, cv::Point(0, P_Block->UpLine.k * (0 - P_Block->UpLine.x0) + P_Block->UpLine.y0), cv::Point(grayImg.cols, P_Block->UpLine.k * (grayImg.cols - P_Block->UpLine.x0) + P_Block->UpLine.y0), cv::Scalar(0, 0, 255), 1);
			cv::line(grayImg, cv::Point(0, P_Block->DownLine.k * (0 - P_Block->DownLine.x0) + P_Block->DownLine.y0), cv::Point(grayImg.cols, P_Block->DownLine.k * (grayImg.cols - P_Block->DownLine.x0) + P_Block->DownLine.y0), cv::Scalar(0, 255, 255), 1);
			cv::line(grayImg, cv::Point((grayImg.rows - P_Block->LeftLine.y0) / P_Block->LeftLine.k + P_Block->LeftLine.x0, grayImg.rows), cv::Point((0 - P_Block->LeftLine.y0) / P_Block->LeftLine.k + P_Block->LeftLine.x0, 0), cv::Scalar(0, 255, 0), 1);
			cv::line(grayImg, cv::Point((grayImg.rows - P_Block->RightLine.y0) / P_Block->RightLine.k + P_Block->RightLine.x0, grayImg.rows), cv::Point((0 - P_Block->RightLine.y0) / P_Block->RightLine.k + P_Block->RightLine.x0, 0), cv::Scalar(255, 0, 0), 1);

			//��Ʒ�ּ�
			if (P_Faults->EdgeFaults.size() > 0)
			{
				type = 4;
			}
			if (type != 4)//���߷ּ�
			{
				double _EDGE_TOTAL_LENGTH = 0;//�����ܳ���
				double _EDGE_TOTAL_DEEP = 0;//���������
				for (int i = 0; i < P_Faults->BrokenEdges.size(); i++)
				{
					_EDGE_TOTAL_LENGTH += P_Faults->BrokenEdges[i].length_mm;
					_EDGE_TOTAL_DEEP += P_Faults->BrokenEdges[i].deep_mm;
					if (P_Faults->BrokenEdges[i].length_mm > Classify_EDGE_SINGLE_LENGTH_C || P_Faults->BrokenEdges[i].deep_mm > Classify_EDGE_SINGLE_DEEP_C)
					{
						type = 4;
						break;
					}
					if (type < 3 && (P_Faults->BrokenEdges[i].length_mm > Classify_EDGE_SINGLE_LENGTH_B || P_Faults->BrokenEdges[i].deep_mm > Classify_EDGE_SINGLE_DEEP_B))
					{
						type = 3;
					}

					if (type < 2 && (P_Faults->BrokenEdges[i].length_mm > Classify_EDGE_SINGLE_LENGTH_A || P_Faults->BrokenEdges[i].deep_mm > Classify_EDGE_SINGLE_DEEP_A))
					{
						type = 2;
					}
				}
				if (_EDGE_TOTAL_LENGTH > Classify_EDGE_TOTAL_LENGTH_C || _EDGE_TOTAL_DEEP > Classify_EDGE_TOTAL_DEEP_C)
					type = 4;
				if (type < 3 && (_EDGE_TOTAL_LENGTH > Classify_EDGE_TOTAL_LENGTH_B || _EDGE_TOTAL_DEEP > Classify_EDGE_TOTAL_DEEP_B))
					type = 3;
				if (type < 2 && (_EDGE_TOTAL_LENGTH > Classify_EDGE_TOTAL_LENGTH_A || _EDGE_TOTAL_DEEP > Classify_EDGE_TOTAL_DEEP_A))
					type = 2;

				stringstream ss;
				ss << workerInfo << "�����ܳ�=" << _EDGE_TOTAL_LENGTH << "mm" << endl;
				ss << workerInfo << "��������=" << _EDGE_TOTAL_DEEP << "mm" << endl;
				MFCConsole::Output(ss.str());
			}
			if (type != 4)//�ڲ�ȱ�ݷּ�
			{
				for (size_t i = 0; i < P_Faults->Crazings.size(); i++)
				{

				}
			}
			if (1 == 1)
			{
				stringstream ss;
				ss << workerInfo << "type=" << type << endl;
				MFCConsole::Output(ss.str());
			}
		}
		P_Controller->ImgProcessOverCallBack(SN, grayImg, *(P_Block), type);
		//if (type > 1)
		//	P_Controller->imageSave(tmp);
	}
	else
	{
		P_Controller->ImgProcessOverCallBack(SN, grayImg, *(P_Block), 0);
	}


	//��ʾ���
	MyStatus = WorkerStatus::Free;
	return;
}
Worker::Status Worker::algorithmSynthesize(cv::Mat img)
{
	P_Block = new Block(img.size().width, img.size().height);

	if (img.cols == 0)
	{
		MFCConsole::Output("Img Empty!\r\n");
		return Status::NoImage;
	}
	cv::Mat grayImg = img;
	if (grayImg.channels() == 3)
		cv::cvtColor(grayImg, grayImg, CV_BGR2GRAY);


	//��λ
	if (positioning(grayImg) == 0)
		return Status::NotFound;

	//��û�б궨���ݣ�����Լ����Ի��ȱ�ݳߴ�
	if (Block::X_mmPerPix < 0)
	{
		Block::X_mmPerPix = Block::Standard_Width_mm / (double)abs(P_Block->B.x - P_Block->A.x);
		Block::Y_mmPerPix = Block::Standard_Length_mm / (double)abs(P_Block->C.y - P_Block->B.y);
	}
	//����Խ���
	Block::Calculate(P_Block);
	double ac_bd_diff_mm = abs(P_Block->ac_mm - P_Block->bd_mm);



	// ��Եȱ��
	if (detectEdge(grayImg))
	{
		return Status::Rejected;
	}

	//����ȱ��
	detectInner(grayImg);
	return Status::TypeA;
}
//��ש��λ�������Ƿ��ҵ���ש
//1 �ҵ�
//0 δ�ҵ�
//-1 ��ȱ��
int Worker::positioning(cv::Mat grayImg)
{
	double t = 0;
	if (MFCConsole::IsOpened)
		t = cv::getTickCount();

	//���ж�λ
	BlockLocalizer bl = BlockLocalizer(grayImg, P_Block, P_Faults);
	bl.Run();


	if (MFCConsole::IsOpened)
	{
		stringstream ss;
		t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
		ss << "��λ�㷨BlockLocalizer��" << t << endl;
		MFCConsole::Output(ss.str());
	}

	if (bl.NotFoundBlockFlag == true)
	{
		//δ��⵽��ש
		return 0;
	}
	//if (bl.BrokenEdgeFlag == true)
	//{
	//	//cv::cvtColor(img, img, CV_GRAY2BGR);
	//	//for (size_t i = 0; i < P_Faults->BrokenEdges.size(); i++)
	//	//{
	//	//	cv::circle(img, P_Faults->BrokenEdges[i].position, P_Faults->BrokenEdges[i].length / 2, cv::Scalar(0, 0, 255), 10);
	//	//}
	//	//cv::namedWindow("1", 0);
	//	//cv::imshow("1", img);
	//	//cv::waitKey(0);
	//	return _Status::_Edge_Broken;
	//}
	return 1;
}
//��Եȱ�ݼ��
//1 �ҵ�
//-1 ��ȱ��
int Worker::detectEdge(cv::Mat grayImg)
{
	double t = 0;
	P_Block->Lines2ABCD();

	if (BlockEdgeLineDetector::Enabled)
	{
		if (MFCConsole::IsOpened)
			t = cv::getTickCount();
		BlockEdgeLineDetector  beld = BlockEdgeLineDetector(grayImg, P_Block, P_Faults);
		beld.Run();
		if (MFCConsole::IsOpened)
		{
			stringstream ss;
			t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
			ss << "�㷨BlockEdgeLineDetector��ʱ��" << t << endl;
			MFCConsole::Output(ss.str());
		}
	}
	if (BlockEdgeSimilarDetector::Enabled)
	{
		if (MFCConsole::IsOpened)
			t = cv::getTickCount();
		BlockEdgeSimilarDetector besd = BlockEdgeSimilarDetector(grayImg, P_Block, P_Faults);
		besd.Run();
		if (MFCConsole::IsOpened)
		{
			stringstream ss;
			t = ((double)cv::getTickCount() - t) * 1000 / cv::getTickFrequency();
			ss << "�㷨BlockEdgeSimilarDetector��ʱ��" << t << endl;
			MFCConsole::Output(ss.str());
		}
	}
	return 1;
}

//�ڲ�ȱ�ݼ��
//1 �ҵ�
//-1 ��ȱ��
int Worker::detectInner(cv::Mat grayImg)
{
	if (Pretreatment_Enable)
	{
		stringstream ss;
		double t = (double)cv::getTickCount();

		//��ש�ڲ�ȱ�ݼ��
		ss << "��ש�ڲ�ȱ�ݼ�� ��ʼ" << endl;
		Pretreatment p;
		p.pretreatment(grayImg, P_Block, P_Faults);

		if (MFCConsole::IsOpened)
		{
			ss << SN << " " << "�ڲ��л��ۣ�" << P_Faults->Scratchs.size() << endl;
			ss << SN << " " << "�ڲ��а��㣺" << P_Faults->Holes.size() << endl;
			ss << "��ש�ڲ�ȱ�ݼ�� ����" << endl;
			t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
			ss << SN << " " << "�ڲ�ȱ�ݼ�⣺" << t << "  End at:" << (double)cv::getTickCount() / cv::getTickFrequency() << endl;
			MFCConsole::Output(ss.str());
		}
	}
	return 1;
}

void Worker::drawFaults(cv::Mat& img)
{
	if (img.channels() == 1)
		cv::cvtColor(img, img, CV_GRAY2RGB);


	if (P_Faults->EdgeFaults.size() > 0)
	{
		if (MFCConsole::IsOpened)
		{
			stringstream ss;
			ss << workerInfo << SN << " ���� " << P_Faults->EdgeFaults.size() << " ����Ե�����ơ�" << endl;
			MFCConsole::Output(ss.str());
		}
		for (size_t i = 0; i < P_Faults->EdgeFaults.size(); i++)
		{
			cv::rectangle(img, cv::Rect(P_Faults->EdgeFaults[i].position.x - P_Faults->EdgeFaults[i].width,
				P_Faults->EdgeFaults[i].position.y - P_Faults->EdgeFaults[i].height,
				P_Faults->EdgeFaults[i].width * 2,
				P_Faults->EdgeFaults[i].height * 2), Scalar(0, 30 * i, 250));
		}
	}
	if (P_Faults->BrokenEdges.size() > 0)
	{
		if (MFCConsole::IsOpened)
		{
			stringstream ss;
			ss << workerInfo << SN << " ���� " << P_Faults->BrokenEdges.size() << " ������ȱ�ݣ���ɫ�����" << endl;
			MFCConsole::Output(ss.str());
		}
		for (size_t i = 0; i < P_Faults->BrokenEdges.size(); i++)
		{
			cv::circle(img, P_Faults->BrokenEdges[i].position, P_Faults->BrokenEdges[i].length, cv::Scalar(0, 0, 255), 4);
		}
	}
	//if (P_Faults->BrokenCorners.size() > 0)
	//{
	//	if (MFCConsole::IsOpened)
	//	{
	//		stringstream ss;
	//		ss << workerInfo << SN << " ���� " << P_Faults->BrokenCorners.size() << " ������ȱ�ݣ��������" << endl;
	//		MFCConsole::Output(ss.str());
	//	}

	//	for (size_t i = 0; i < P_Faults->BrokenCorners.size(); i++)
	//	{
	//		cv::circle(img, P_Faults->BrokenCorners[i].position, P_Faults->BrokenCorners[i].length, cv::Scalar(127, 0, 228), 5);
	//	}
	//}
	//if (P_Faults->SomethingBigs.size() > 0)
	//{
	//	if (MFCConsole::IsOpened)
	//	{
	//		stringstream ss;
	//		ss << SN << " ���� " << P_Faults->BrokenEdges.size() << " ������ȱ�ݣ��������" << endl;
	//		MFCConsole::Output( << ss.str();
	//	}

	//	CString str;
	//	str.Format(_T("%d ����%d��EIDȱ�ݣ���ɫ�����\r\n"), SN, P_Faults->SomethingBigs.size());
	//	m_Info += str;
	//	clog += str;
	//	for (size_t i = 0; i < P_Faults->SomethingBigs.size(); i++)
	//	{
	//		cv::circle(img, P_Faults->SomethingBigs[i].position, P_Faults->SomethingBigs[i].diameter, cv::Scalar(255, 0, 0), 5);
	//	}
	//}
	if (P_Faults->Scratchs.size() > 0)
	{
		if (MFCConsole::IsOpened)
		{
			stringstream ss;
			ss << workerInfo << SN << " ���� " << P_Faults->Scratchs.size() << " ������ȱ�ݣ���ɫ�����" << endl;
			MFCConsole::Output(ss.str());
		}
		for (size_t i = 0; i < P_Faults->Scratchs.size(); i++)
		{
			cv::circle(img, P_Faults->Scratchs[i].position, P_Faults->Scratchs[i].length, cv::Scalar(0, 255, 0), 5);
		}
	}
	if (P_Faults->Holes.size() > 0)
	{
		if (MFCConsole::IsOpened)
		{
			stringstream ss;
			ss << workerInfo << SN << " ���� " << P_Faults->Holes.size() << " ������ȱ�ݣ���ɫ�����" << endl;
			MFCConsole::Output(ss.str());
		}
		for (size_t i = 0; i < P_Faults->Holes.size(); i++)
		{
			cv::circle(img, P_Faults->Holes[i].position, P_Faults->Holes[i].diameter, cv::Scalar(0, 255, 255), 5);
		}
	}
	if (P_Faults->MarkPens.size() > 0)
	{

		if (MFCConsole::IsOpened)
		{
			stringstream ss;
			ss << workerInfo << SN << " ���� " << P_Faults->MarkPens.size() << " ���˹���ǣ���ɫ�����" << endl;
			MFCConsole::Output(ss.str());
		}
		for (size_t i = 0; i < P_Faults->MarkPens.size(); i++)
		{
			cv::rectangle(img, P_Faults->MarkPens[i].markposition, cv::Scalar(122, 0, 255), 5);
		}
	}
}