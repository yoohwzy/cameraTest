#include "../../stdafx.h"
#include "../../mc100.h"

class mc100USBCard
{
public:
	mc100USBCard(){
	};
	~mc100USBCard(){
		if (hasOpened)
			mc100_close(0);
	};

	bool init(unsigned int id = 0)
	{
		if (hasOpened)
			return hasOpened;
		if (mc100_open(id) >= 0)
		{
			mc100_write_port(0, MC100_PORTA, 0x00);
			hasOpened = true;
		}
		else
		{
			hasOpened = false;
		}
		return hasOpened;
	}
	//��ȡPORTA�������أ�����trueʱ����
	bool GetRisingEdgePORTA()
	{
		if (!hasOpened)
			return false;

		bool now = mc100_check_pin(0, (MC100_PORTA << 4) | 0) == 1;
		bool last = lastSignalPORTA;
		lastSignalPORTA = now;
		if (last == false && now == true)
		{
			Sleep(1);
			if (mc100_check_pin(0, (MC100_PORTA << 4) | 0) == 1)
				return true;
		}
		return false;
	}
	//��ȡPORTA���½��أ�����trueʱ����
	bool GetTrailingEdgePORTA()
	{
		if (!hasOpened)
			return false;

		bool now = mc100_check_pin(0, (MC100_PORTA << 4) | 0) == 1;
		bool last = lastSignalPORTA;
		lastSignalPORTA = now;
		if (last == true && now == false)
		{
			Sleep(1);
			if (mc100_check_pin(0, (MC100_PORTA << 4) | 0) != 1)
				return true;
		}
		return false;
	}
private:
	bool hasOpened = false;
	bool lastSignalPORTA = false;
};