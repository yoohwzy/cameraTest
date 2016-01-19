#pragma once
class Recorder
{
public:
	enum Quality
	{
		A,
		B,
		C,
		Good,//�ϸ�
		Reject,//���ϸ�
		Total
	};
	enum TimeSpan
	{
		Today,
		Tomounth,
		Toyear
	};
	Recorder();
	~Recorder();
	//����Ʒ����¼��һ
	static void AddOne(Quality type);
	//�������
	static int GetCount(Quality type, TimeSpan t);
	//��úϸ���
	static double FineRate();
private:
	//���ռ�¼
	static int todayACount;
	static int todayBCount;
	static int todayCCount;
	static int todayRejectCount;

	//���¼�¼
	static int tomounthACount;
	static int tomounthBCount;
	static int tomounthCCount;
	static int tomounthRejectCount;


	//�����¼
	static int toyearACount;
	static int toyearBCount;
	static int toyearCCount;
	static int toyearRejectCount;
};

