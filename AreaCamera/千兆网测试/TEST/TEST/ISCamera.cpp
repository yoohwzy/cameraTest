#include "stdafx.h"
#include "ISCamera.h"

using namespace DShowLib;
using namespace std;

typedef void(*ICSelectLocalizationLanguageW)(const wchar_t* lang);

ICSelectLocalizationLanguageW func;

ISCamera::ISCamera()
{
	DShowLib::InitLibrary();
	grabber = unique_ptr<Grabber>(new Grabber);
	grabber->setOverlayBitmapPathPosition(ePP_NONE);
	if (!setupDeviceFromFile())
	{
		exit(0);
	}
}
ISCamera::~ISCamera()
{
	this->StopCapture();
}

bool ISCamera::Init()
{

	return 0;
}
void ISCamera::StartCapture()
{
	pHandlerSink = FrameHandlerSink::create(tColorformatEnum::eY800, 1);
	pHandlerSink->setSnapMode(true);
	grabber->setSinkType(pHandlerSink);
	grabber->startLive(false);
	Sleep(250);
}

void ISCamera::StopCapture()
{
	grabber->stopLive();
}

void ISCamera::GetFrame(cv::Mat& img)
{
	//snap for 1 frame and with 2000ms time tolerance，200000000 = 2000ms * 1000000
	pHandlerSink->snapImages(1, 1000 * 1000000);
	auto buffer = pHandlerSink->getLastAcqMemBuffer();
	BYTE* imgData = buffer->getPtr();
	auto size = buffer->getSize();
	cv::Size cvsize(size.cx, size.cy);

	//create a Mat header just sealing imageData
	cv::Mat originSrc(cvsize, CV_8UC1, imgData);
	//bayer convert to color image by bayergr
	cv::cvtColor(originSrc, img, CV_BayerGR2BGR);
}

bool ISCamera::IsRunning() const
{

	return grabber->isDevOpen() && grabber->isLive();
}


inline bool	ISCamera::setupDeviceFromFile(const std::string& devStateFilename)
{
	HMODULE hDll = ::LoadLibrary(L"TIS_UDSHL10d.dll");//DEBUG下面用TIS_UDSHL10d.dll，release下面用TIS_UDSHL10.dll
	//LPCSTR str = "zh_cn";
	func = (ICSelectLocalizationLanguageW)::GetProcAddress(hDll, "ICSelectLocalizationLanguageW");

	func(L"zh_cn");

	if (grabber->loadDeviceStateFromFile(devStateFilename))
	{
		std::cout << "Device opened from: <" << devStateFilename << ">." << std::endl;
	}
	else
	{
		std::cout << "File <" << devStateFilename << "> either not found, or device could not be opened from it." << std::endl;
	}
	if (!grabber->showDevicePage() || !grabber->isDevValid())
	{
		return false;
	}
	grabber->saveDeviceStateToFile(devStateFilename, true, true, true);
	return true;
}
