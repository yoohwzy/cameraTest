// OpenCV.cpp : �������̨Ӧ�ó������ڵ㡣
//
//BIG5 TRANS ALLOWED
#include "stdafx.h"
#include "windows.h"
#include <opencv2/opencv.hpp>
#include <process.h>

#pragma comment(lib,"MVCAMSDK.lib")

#include "./include/CameraApi.h"

using namespace std;
using namespace cv;

UINT            m_threadID;		//ͼ��ץȡ�̵߳�ID
HANDLE          m_hDispThread;	//ͼ��ץȡ�̵߳ľ��
BOOL            m_bExit;		//����֪ͨͼ��ץȡ�߳̽���
CameraHandle    m_hCamera;		//��������������ͬʱʹ��ʱ���������������	
BYTE*           m_pFrameBuffer; //���ڽ�ԭʼͼ������ת��ΪRGB�Ļ�����
tSdkFrameHead   m_sFrInfo;		//���ڱ��浱ǰͼ��֡��֡ͷ��Ϣ

int	            m_iDispFrameNum;	//���ڼ�¼��ǰ�Ѿ���ʾ��ͼ��֡������
float           m_fDispFps;			//��ʾ֡��
float           m_fCapFps;			//����֡��
tSdkFrameStatistic  m_sFrameCount;
tSdkFrameStatistic  m_sFrameLast;
int					m_iTimeLast;
char		    g_CameraName[64];


bool endThread = 0;
/*
USE_CALLBACK_GRAB_IMAGE
�����Ҫʹ�ûص������ķ�ʽ���ͼ�����ݣ���ע�ͺ궨��USE_CALLBACK_GRAB_IMAGE.
���ǵ�SDKͬʱ֧�ֻص��������������ýӿ�ץȡͼ��ķ�ʽ�����ַ�ʽ��������"�㿽��"���ƣ������ĳ̶ȵĽ���ϵͳ���ɣ���߳���ִ��Ч�ʡ�
��������ץȡ��ʽ�Ȼص������ķ�ʽ�������������ó�ʱ�ȴ�ʱ��ȣ����ǽ�����ʹ�� uiDisplayThread �еķ�ʽ
*/
//#define USE_CALLBACK_GRAB_IMAGE 

#ifdef USE_CALLBACK_GRAB_IMAGE
/*ͼ��ץȡ�ص�����*/
IplImage *g_iplImage = NULL;

void _stdcall GrabImageCallback(CameraHandle hCamera, BYTE *pFrameBuffer, tSdkFrameHead* pFrameHead, PVOID pContext)
{

	CameraSdkStatus status;


	//����õ�ԭʼ����ת����RGB��ʽ�����ݣ�ͬʱ����ISPģ�飬��ͼ����н��룬������������ɫУ���ȴ���
	//�ҹ�˾�󲿷��ͺŵ������ԭʼ���ݶ���Bayer��ʽ��
	status = CameraImageProcess(hCamera, pFrameBuffer, m_pFrameBuffer, pFrameHead);

	//�ֱ��ʸı��ˣ���ˢ�±���
	if (m_sFrInfo.iWidth != pFrameHead->iWidth || m_sFrInfo.iHeight != pFrameHead->iHeight)
	{
		m_sFrInfo.iWidth = pFrameHead->iWidth;
		m_sFrInfo.iHeight = pFrameHead->iHeight;
	}

	if (status == CAMERA_STATUS_SUCCESS)
	{
		//����SDK��װ�õ���ʾ�ӿ�����ʾͼ��,��Ҳ���Խ�m_pFrameBuffer�е�RGB����ͨ��������ʽ��ʾ������directX,OpengGL,�ȷ�ʽ��
		CameraImageOverlay(hCamera, m_pFrameBuffer, pFrameHead);
		if (g_iplImage)
		{
			cvReleaseImageHeader(&g_iplImage);
		}
		g_iplImage = cvCreateImageHeader(cvSize(pFrameHead->iWidth, pFrameHead->iHeight), IPL_DEPTH_8U, sFrameInfo.uiMediaType == CAMERA_MEDIA_TYPE_MONO8 ? 1 : 3);
		cvSetData(g_iplImage, m_pFrameBuffer, pFrameHead->iWidth*(sFrameInfo.uiMediaType == CAMERA_MEDIA_TYPE_MONO8 ? 1 : 3));
		cvShowImage(g_CameraName, g_iplImage);
		m_iDispFrameNum++;
		waitKey(30);
	}

	memcpy(&m_sFrInfo, pFrameHead, sizeof(tSdkFrameHead));

}

#else 
/*ͼ��ץȡ�̣߳���������SDK�ӿں�����ȡͼ��*/
UINT WINAPI uiDisplayThread(LPVOID lpParam)
{
	endThread = 0;
	tSdkFrameHead 	sFrameInfo;
	CameraHandle    hCamera = (CameraHandle)lpParam;
	BYTE*			pbyBuffer;
	CameraSdkStatus status;
	IplImage *iplImage = NULL;

	INT poFrameSpeed = 0;
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* lines = NULL;



	int imgindex = 0;
	while (!m_bExit)
	{
		if (CameraGetImageBuffer(hCamera, &sFrameInfo, &pbyBuffer, 1000) == CAMERA_STATUS_SUCCESS)
		{
			double t = (double)getTickCount();
			//����õ�ԭʼ����ת����RGB��ʽ�����ݣ�ͬʱ����ISPģ�飬��ͼ����н��룬������������ɫУ���ȴ���
			//�ҹ�˾�󲿷��ͺŵ������ԭʼ���ݶ���Bayer��ʽ��
			status = CameraImageProcess(hCamera, pbyBuffer, m_pFrameBuffer, &sFrameInfo);//����ģʽ

			//�ֱ��ʸı��ˣ���ˢ�±���
			if (m_sFrInfo.iWidth != sFrameInfo.iWidth || m_sFrInfo.iHeight != sFrameInfo.iHeight)
			{
				m_sFrInfo.iWidth = sFrameInfo.iWidth;
				m_sFrInfo.iHeight = sFrameInfo.iHeight;
				//ͼ���С�ı䣬֪ͨ�ػ�
			}

			if (status == CAMERA_STATUS_SUCCESS)
			{
				//����SDK��װ�õ���ʾ�ӿ�����ʾͼ��,��Ҳ���Խ�m_pFrameBuffer�е�RGB����ͨ��������ʽ��ʾ������directX,OpengGL,�ȷ�ʽ��
				CameraImageOverlay(hCamera, m_pFrameBuffer, &sFrameInfo);
				if (iplImage)
				{
					cvReleaseImageHeader(&iplImage);
				}
				iplImage = cvCreateImageHeader(cvSize(sFrameInfo.iWidth, sFrameInfo.iHeight), IPL_DEPTH_8U, sFrameInfo.uiMediaType == CAMERA_MEDIA_TYPE_MONO8 ? 1 : 3);
				cvSetData(iplImage, m_pFrameBuffer, sFrameInfo.iWidth*(sFrameInfo.uiMediaType == CAMERA_MEDIA_TYPE_MONO8 ? 1 : 3));

				//cvFlip(iplImage, iplImage);
				cv::Mat img(iplImage);
				cv::imshow("123", img);
				t = ((double)getTickCount() - t) / getTickFrequency();
				CameraGetFrameSpeed(m_hCamera, &poFrameSpeed);
				std::cout << "the time is :" << t << " s, " << 1 / (t + 0.001) << "Hz, Camera Out:" << poFrameSpeed << endl;
				imgindex++;
				if (imgindex > 100000)imgindex = 0;
				stringstream ss;
				ss << "img/" << imgindex << ".jpg";
				//cv::imwrite(ss.str(), img);
			}

			//�ڳɹ�����CameraGetImageBuffer�󣬱������CameraReleaseImageBuffer���ͷŻ�õ�buffer��
			//�����ٴε���CameraGetImageBufferʱ�����򽫱�����֪�������߳��е���CameraReleaseImageBuffer���ͷ���buffer
			CameraReleaseImageBuffer(hCamera, pbyBuffer);

			memcpy(&m_sFrInfo, &sFrameInfo, sizeof(tSdkFrameHead));
		}
		int c = waitKey(5);

		if (c == 'q' || c == 'Q' || (c & 255) == 27)
		{
			break;
		}
	}

	if (iplImage)
	{
		cvReleaseImageHeader(&iplImage);
	}
	endThread = 1;
	_endthreadex(0);
	return 0;
}
#endif


int _tmain(int argc, _TCHAR* argv[])
{

	tSdkCameraDevInfo sCameraList[10];
	INT iCameraNums;
	CameraSdkStatus status;
	tSdkCameraCapbility sCameraInfo;

	//ö���豸������豸�б�
	iCameraNums = 10;//����CameraEnumerateDeviceǰ��������iCameraNums = 10����ʾ���ֻ��ȡ10���豸�������Ҫö�ٸ�����豸�������sCameraList����Ĵ�С��iCameraNums��ֵ

	if (CameraEnumerateDevice(sCameraList, &iCameraNums) != CAMERA_STATUS_SUCCESS || iCameraNums == 0)
	{
		printf("No camera was found!");
		return FALSE;
	}

	//��ʾ���У�����ֻ����������һ���������ˣ�ֻ��ʼ����һ�������(-1,-1)��ʾ�����ϴ��˳�ǰ����Ĳ���������ǵ�һ��ʹ�ø�����������Ĭ�ϲ���.
	//In this demo ,we just init the first camera.
	if ((status = CameraInit(&sCameraList[0], -1, -1, &m_hCamera)) != CAMERA_STATUS_SUCCESS)
	{
		char msg[128];
		sprintf_s(msg, "Failed to init the camera! Error code is %d", status);
		printf(msg);
		return FALSE;
	}




	//�����ֶ��ع�
	CameraSetAeState(m_hCamera, FALSE);
	CameraSetExposureTime(m_hCamera, 80000);//�ع�ʱ��10ms = 10000΢��
	CameraSetAnalogGain(m_hCamera, 1);//����ģ������16=1.6
	//CameraSetMonochrome(m_hCamera, TRUE);//���úڰ�ͼ��

	//tSdkCameraCapbility tcability;
	//CameraGetCapability(m_hCamera, &tcability);
	CameraSetFrameSpeed(m_hCamera, 2);//����ģʽ



	//Get properties description for this camera.
	CameraGetCapability(m_hCamera, &sCameraInfo);//"��ø��������������"

	m_pFrameBuffer = (BYTE *)CameraAlignMalloc(sCameraInfo.sResolutionRange.iWidthMax*sCameraInfo.sResolutionRange.iWidthMax * 3, 16);


	if (sCameraInfo.sIspCapacity.bMonoSensor)
	{
		CameraSetIspOutFormat(m_hCamera, CAMERA_MEDIA_TYPE_MONO8);
	}




	strcpy_s(g_CameraName, sCameraList[0].acFriendlyName);

	CameraCreateSettingPage(m_hCamera, NULL,g_CameraName, NULL, NULL, 0);//"֪ͨSDK�ڲ��������������ҳ��";

#ifdef USE_CALLBACK_GRAB_IMAGE //���Ҫʹ�ûص�������ʽ������USE_CALLBACK_GRAB_IMAGE�����
	//Set the callback for image capture
	CameraSetCallbackFunction(m_hCamera, GrabImageCallback, 0, NULL);//"����ͼ��ץȡ�Ļص�����";
#else
	m_hDispThread = (HANDLE)_beginthreadex(NULL, 0, &uiDisplayThread, (PVOID)m_hCamera, 0, &m_threadID);
#endif

	CameraPlay(m_hCamera);



	CameraShowSettingPage(m_hCamera, TRUE);//TRUE��ʾ������ý��档FALSE�����ء�

	tSdkImageResolution sRoiResolution;
	memset(&sRoiResolution, 0, sizeof(sRoiResolution));
	sRoiResolution.iIndex = 0xff;
	sRoiResolution.iWidth = 784;
	sRoiResolution.iWidthFOV = sRoiResolution.iWidth * 3;
	sRoiResolution.iHeight = 100;
	sRoiResolution.iHeightFOV = sRoiResolution.iHeight * 3;
	sRoiResolution.iWidthZoomHd = 0;
	sRoiResolution.iHeightZoomHd = 0;
	sRoiResolution.iHOffsetFOV = 128;
	sRoiResolution.iVOffsetFOV = 470;
	sRoiResolution.iWidthZoomSw = 0;
	sRoiResolution.iHeightZoomSw = 0;
	sRoiResolution.uBinAverageMode = 0;
	sRoiResolution.uBinSumMode = 0;
	sRoiResolution.uResampleMask = 0;
	sRoiResolution.uSkipMode = 2;
	CameraSetImageResolution(m_hCamera, &sRoiResolution);

	endThread = 0;
	while (!endThread)
	{
		int c = waitKey(10);
	}

	CameraUnInit(m_hCamera);
	CameraAlignFree(m_pFrameBuffer);
	destroyWindow(g_CameraName);

#ifdef USE_CALLBACK_GRAB_IMAGE
	if (g_iplImage)
	{
		cvReleaseImageHeader(&g_iplImage);
	}
#endif
	return 0;
}

