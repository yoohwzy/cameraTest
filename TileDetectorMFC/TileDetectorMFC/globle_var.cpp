#include "globle_var.h"


globle_var::globle_var()
{
}


globle_var::~globle_var()
{
}

void globle_var::InitSetting()
{
	mdi.width = 4096;
	mdi.height = 1;
	mdi.colorType = mdi.RGB;
	mdi.MaxPics = 10001;
	s = BufferStorage(mdi.MaxPics, mdi.width);
}
void globle_var::InitSetting(string colorType, int maxPics, int width)
{
	mdi.colorType = colorType == "Gray" ? globle_var::mdi.RGB : globle_var::mdi.GRAY;
	mdi.width = width;
	//mdi.height = 1;
	//mdi.colorType = mdi.RGB;
	mdi.MaxPics = maxPics;
	s = BufferStorage(mdi.MaxPics, mdi.width);
}


MicroDisplayInit globle_var::mdi;
BufferStorage globle_var::s = BufferStorage(mdi.MaxPics, mdi.width);
int globle_var::TiggerWaitTimeMS = 400;
string globle_var::VirtualCameraFileName = "";