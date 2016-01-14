#pragma once

class Contrller;// 解决类互相包含

class SpotsMainView
{
public:
	SpotsMainView()
	{
		
	}
	~SpotsMainView();
protected:
	Contrller* m_contrller;
};

