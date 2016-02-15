#include "Pretreatment.h"


Pretreatment::Pretreatment()
{
}


Pretreatment::~Pretreatment()
{
}

void Pretreatment::Run(cv::Mat& img)
{
	img = img * 4;
}
