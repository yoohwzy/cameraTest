#pragma once
#include <thread>

//�����㷨�ۺ���
#include <Algorithm\Synthesizer.h>
#include <Class\Debug\MFCConsole.h>


//#define WORKER_DEBUG

class ControllerModel;// ����໥�����


//������
//������������ɼ�ͼ��󣬽����㷨����ĵ���
//�Լ���������ɺ�Ļ�ͼ
class Worker
{
public:
	//��ʼ��ʱ������ָ��ΪNULL�����ʾʹ�����������
	Worker(string WorkerInfo = "");
	~Worker();

	enum WorkerStatus{
		Free,
		InProcessing,
	};
	void StartWork();
	
	int MyStatus = WorkerStatus::Free;
	cv::Mat image;

	ControllerModel *P_Controller = NULL;


	// ��ש���к�
	int SN = 0;

	//edge ����

	//double BlockLocalizer_THRESHOD = 10;			//��Ե���Ҷ�ֵ����ֵ
	//double BlockLocalizer_ContinuePointCount = 30;	//�������ٸ������ж�Ϊ��Ե

	//int BlockEdgeLineDetector_Enable = 1;			//
	//int BlockEdgeLineDetector_BINARY_THRESHOD = 5;	//��ֵ����ֵ
	//int BlockEdgeLineDetector_LENGTH_THRESHOD = 5;	//����������Ȳ���Ϊ��ȱ�ݣ�pix��
	//int BlockEdgeLineDetector_DEEP_THRESHOD = 5;	//���������Ȳ���Ϊ��ȱ�ݣ�pix��

	//int  BlockEdgeDetector_Enable = 1;
	//double BlockEdgeDetector_DIFF_THRESHOLD = 0.4;	//�����ֵ�ٷֱȳ��������ֵ����Ϊ������ȱ��
	//double BlockEdgeDetector_FAULTS_SPAN = 4;		//����DIFF_THRESHOLD�ĵ㸽��������FAULTS_SPAN������
	//double BlockEdgeDetector_FAULTS_COUNT = 5;		//�������������DIFF_THRESHOLD���ж�Ϊȱ��

	/**************�ڲ�����****************/
	int Pretreatment_Enable = 1;

	/**************�ּ�����*************/

	//�߲���
	double Classify_EDGE_SINGLE_LENGTH_A = 0;
	double Classify_EDGE_SINGLE_LENGTH_B = 0;
	double Classify_EDGE_SINGLE_LENGTH_C = 0;
	double Classify_EDGE_SINGLE_LENGTH_ACCEPT = 0;

	double Classify_EDGE_TOTAL_LENGTH_A = 0;
	double Classify_EDGE_TOTAL_LENGTH_B = 0;
	double Classify_EDGE_TOTAL_LENGTH_C = 0;
	double Classify_EDGE_TOTAL_LENGTH_ACCEPT = 0;

	double Classify_EDGE_SINGLE_DEEP_A = 0;
	double Classify_EDGE_SINGLE_DEEP_B = 0;
	double Classify_EDGE_SINGLE_DEEP_C = 0;
	double Classify_EDGE_SINGLE_DEEP_ACCEPT = 0;

	double Classify_EDGE_TOTAL_DEEP_A = 0;
	double Classify_EDGE_TOTAL_DEEP_B = 0;
	double Classify_EDGE_TOTAL_DEEP_C = 0;
	double Classify_EDGE_TOTAL_DEEP_ACCEPT = 0;

	//���Ӳ���
	double Classify_HOAL_DIAMETER_A = 0;
	double Classify_HOAL_DIAMETER_B = 0;
	double Classify_HOAL_DIAMETER_C = 0;
	double Classify_HOAL_DIAMETER_ACCEPT = 0;

	double Classify_HOAL_COUNT_A = 0;
	double Classify_HOAL_COUNT_B = 0;
	double Classify_HOAL_COUNT_C = 0;
	double Classify_HOAL_COUNT_ACCEPT = 0;


	//����
	double Classify_SCRATCH_SINGLE_LENGTH_A = 0;
	double Classify_SCRATCH_SINGLE_LENGTH_B = 0;
	double Classify_SCRATCH_SINGLE_LENGTH_C = 0;
	double Classify_SCRATCH_SINGLE_LENGTH_ACCEPT = 0;

	double Classify_SCRATCH_TOTAL_LENGTH_A = 0;
	double Classify_SCRATCH_TOTAL_LENGTH_B = 0;
	double Classify_SCRATCH_TOTAL_LENGTH_C = 0;
	double Classify_SCRATCH_TOTAL_LENGTH_ACCEPT = 0;


private:
	string workerInfo = "";
	//double axis_x_mmPerPix = 1;//x�᷽����ÿ���ش�����ٺ���
	//double axis_y_mmPerPix = 1;//y�᷽����ÿ���ش�����ٺ���

	// ֱ�Ӷ�ȡN��ͼƬ
	void work();
	// ��ѭ��������ȡ��ͼƬ
	// ��ʼ�ɼ����У��ɼ�������Ϊ0ʱ�ȴ��½��ػ�ɼ���������frameCountsOut
	cv::Mat getPhoto(int startFrame, int length = 0);

	// ����ȱ��
	void drawFaults(cv::Mat&, Faults& faults);
};