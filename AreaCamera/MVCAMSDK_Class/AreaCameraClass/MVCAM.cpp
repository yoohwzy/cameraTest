#include "MVCAM.h"

//为500W工业相机封装的类，调用前确保MVCAMSDK.lib存在
MVCAM::MVCAM()
{

}


MVCAM::~MVCAM()
{
}

bool MVCAM::Init()
{
	//相机的设备信息
	tSdkCameraDevInfo sCameraList[10];
	INT iCameraNums = 10;//调用CameraEnumerateDevice前，先设置iCameraNums = 10，表示最多只读取10个设备，如果需要枚举更多的设备，请更改sCameraList数组的大小和iCameraNums的值
	CameraSdkStatus status;

	if (CameraEnumerateDevice(sCameraList, &iCameraNums) != CAMERA_STATUS_SUCCESS || iCameraNums == 0)
	{
		printf("No camera was found!");
		return FALSE;
	}

	//该示例中，我们只假设连接了一个相机。因此，只初始化第一个相机。(-1,-1)表示加载上次退出前保存的参数，如果是第一次使用该相机，则加载默认参数.
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
	CameraGetCapability(m_hCamera, &sCameraInfo);// 获得相机的属性


	CameraSetAeState(m_hCamera, FALSE);//设置相机曝光的模式。自动或者手动。bState：TRUE，使能自动曝光；FALSE，停止自动曝光。
	CameraSetExposureTime(m_hCamera, 80000);//曝光时间10ms = 10000微秒
	CameraSetAnalogGain(m_hCamera, 10);//设置模拟增益16=1.6
	//CameraSetMonochrome(m_hCamera, TRUE);//设置黑白图像
	CameraSetFrameSpeed(m_hCamera, sCameraInfo.iFrameSpeedDesc - 1);//设定相机输出图像的帧率。iFrameSpeedSel：选择的帧率模式索引号，范围从 0 到CameraGetCapability 获得的信息结构体中	iFrameSpeedDesc - 1


	//申请空间
	m_pFrameBuffer = (BYTE *)CameraAlignMalloc(sCameraInfo.sResolutionRange.iWidthMax*sCameraInfo.sResolutionRange.iWidthMax * 3, 16);


	if (sCameraInfo.sIspCapacity.bMonoSensor)//ISP 能力描述，BOOL bMonoSensor; //表示该型号相机是否为黑白相机,如果是黑白相机，则颜色相关的功能都无法调节
	{
		CameraSetIspOutFormat(m_hCamera, CAMERA_MEDIA_TYPE_MONO8);
	}




	//char g_CameraName[64];
	//strcpy_s(g_CameraName, sCameraList[0].acFriendlyName);
	//创建该相机的属性配置窗口。
	CameraCreateSettingPage(m_hCamera, NULL, "cam", NULL, NULL, 0);//"通知SDK内部建该相机的属性页面";



//#ifdef USE_CALLBACK_GRAB_IMAGE //如果要使用回调函数方式，定义USE_CALLBACK_GRAB_IMAGE这个宏
//	//Set the callback for image capture
//	CameraSetCallbackFunction(m_hCamera, GrabImageCallback, 0, NULL);//"设置图像抓取的回调函数";
//#else
//	m_hDispThread = (HANDLE)_beginthreadex(NULL, 0, &uiDisplayThread, (PVOID)m_hCamera, 0, &m_threadID);
//	SetThreadPriority(m_hDispThread, THREAD_PRIORITY_HIGHEST);
//#endif

	CameraPlay(m_hCamera);//调用 CameraPlay 函数，让相机进入工作模式，并且 SDK 开始接收来自相机的图像。


	//相机初始化完成

	CameraShowSettingPage(m_hCamera, TRUE);//TRUE显示相机配置界面。FALSE则隐藏。

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
	CameraSetImageResolution(m_hCamera, &sRoiResolution);//设置预览的分辨率。


	MVCAM::HasInited = true;
}

bool MVCAM::HasInited = 0;