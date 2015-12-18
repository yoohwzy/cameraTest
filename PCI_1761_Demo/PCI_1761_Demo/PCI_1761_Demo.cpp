// PCI_1761_Demo.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Class\IOCard\PCI1761.h"


using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	PCI1761 pci1761;
	if (pci1761.init())
	{
		do
		{
			if (pci1761.GetIDI(7))
			{
				printf("1\r\n");
				pci1761.SetR(7, 1);
			}
			else
			{
				printf("0\r\n");
				pci1761.SetR(7, 0);
			}

			//上升沿与下降沿不能同时检测

			if (pci1761.GetRisingEdgeIDI(7))
			{
				printf("RisingEdgeIDI7\r\n");
				pci1761.SetR(5, 1);
				SLEEP(1);
			}
			else
				pci1761.SetR(5, 0);


			if (pci1761.GetTrailingEdgeIDI(7))
			{
				printf("TrailingEdgeIDI7\r\n");
				pci1761.SetR(6, 1);
				SLEEP(1);
			}
			else
			{
				pci1761.SetR(6, 0);
			}
			//printf("again\r\n");
			//SLEEP(1);
		} while (true);
	}
	return 0;
}

