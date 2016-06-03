#include "MVCAM.h"

//Ϊ500W��ҵ�����װ���࣬����ǰȷ��MVCAMSDK.lib����
MVCAM::MVCAM()
{

}


MVCAM::~MVCAM()
{
}

bool MVCAM::Init()
{
	//������豸��Ϣ
	tSdkCameraDevInfo sCameraList[10];
	INT iCameraNums = 10;//����CameraEnumerateDeviceǰ��������iCameraNums = 10����ʾ���ֻ��ȡ10���豸�������Ҫö�ٸ�����豸�������sCameraList����Ĵ�С��iCameraNums��ֵ
	CameraSdkStatus status;

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


	//Get properties description for this camera.
	tSdkCameraCapbility sCameraInfo;
	CameraGetCapability(m_hCamera, &sCameraInfo);// ������������


	CameraSetAeState(m_hCamera, FALSE);//��������ع��ģʽ���Զ������ֶ���bState��TRUE��ʹ���Զ��ع⣻FALSE��ֹͣ�Զ��ع⡣
	CameraSetExposureTime(m_hCamera, 80000);//�ع�ʱ��10ms = 10000΢��
	CameraSetAnalogGain(m_hCamera, 10);//����ģ������16=1.6
	//CameraSetMonochrome(m_hCamera, TRUE);//���úڰ�ͼ��
	CameraSetFrameSpeed(m_hCamera, sCameraInfo.iFrameSpeedDesc - 1);//�趨������ͼ���֡�ʡ�iFrameSpeedSel��ѡ���֡��ģʽ�����ţ���Χ�� 0 ��CameraGetCapability ��õ���Ϣ�ṹ����	iFrameSpeedDesc - 1


	//����ռ�
	m_pFrameBuffer = (BYTE *)CameraAlignMalloc(sCameraInfo.sResolutionRange.iWidthMax*sCameraInfo.sResolutionRange.iWidthMax * 3, 16);


	if (sCameraInfo.sIspCapacity.bMonoSensor)//ISP ����������BOOL bMonoSensor; //��ʾ���ͺ�����Ƿ�Ϊ�ڰ����,����Ǻڰ����������ɫ��صĹ��ܶ��޷�����
	{
		CameraSetIspOutFormat(m_hCamera, CAMERA_MEDIA_TYPE_MONO8);
	}




	//char g_CameraName[64];
	//strcpy_s(g_CameraName, sCameraList[0].acFriendlyName);
	//������������������ô��ڡ�
	CameraCreateSettingPage(m_hCamera, NULL, "cam", NULL, NULL, 0);//"֪ͨSDK�ڲ��������������ҳ��";



//#ifdef USE_CALLBACK_GRAB_IMAGE //���Ҫʹ�ûص�������ʽ������USE_CALLBACK_GRAB_IMAGE�����
//	//Set the callback for image capture
//	CameraSetCallbackFunction(m_hCamera, GrabImageCallback, 0, NULL);//"����ͼ��ץȡ�Ļص�����";
//#else
//	m_hDispThread = (HANDLE)_beginthreadex(NULL, 0, &uiDisplayThread, (PVOID)m_hCamera, 0, &m_threadID);
//	SetThreadPriority(m_hDispThread, THREAD_PRIORITY_HIGHEST);
//#endif

	CameraPlay(m_hCamera);//���� CameraPlay ��������������빤��ģʽ������ SDK ��ʼ�������������ͼ��


	//�����ʼ�����

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
	CameraSetImageResolution(m_hCamera, &sRoiResolution);//����Ԥ���ķֱ��ʡ�


	MVCAM::HasInited = true;
}

bool MVCAM::HasInited = 0;