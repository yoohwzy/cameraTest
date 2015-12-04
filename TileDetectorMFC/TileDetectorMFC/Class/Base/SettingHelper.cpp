#include "SettingHelper.h"


SettingHelper::SettingHelper()
{
}


SettingHelper::~SettingHelper()
{
}
//void SettingHelper::Init(Key key)
//{
//
//	switch (key)
//	{
//	case SettingHelper::GRAB_MaxPics:
//		break;
//	case SettingHelper::GRAB_Width:
//		break;
//	case SettingHelper::GRAB_ColorType:
//		break;
//	case SettingHelper::TIGGER_WaitTime:
//		break;
//	default:
//		break;
//	}
//}


void SettingHelper::AddKey(Key key, int value)
{
	std::stringstream ss;
	ss << value;
	AddKey(key, ss.str());
}
void SettingHelper::AddKey(Key key, float value)
{
	std::stringstream ss;
	ss << value;
	AddKey(key, ss.str());
}
void SettingHelper::AddKey(Key key, double value)
{
	std::stringstream ss;
	ss << value;
	AddKey(key, ss.str());
}
void SettingHelper::AddKey(Key key, LPWSTR value)
{
	wstring ws(value);
	std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
	setlocale(LC_ALL, "chs");
	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = 2 * ws.size() + 1;
	char *_Dest = new char[_Dsize];
	memset(_Dest, 0, _Dsize);
	wcstombs(_Dest, _Source, _Dsize);
	std::string result = _Dest;
	delete[]_Dest;
	setlocale(LC_ALL, curLocale.c_str());

	AddKey(key, result);
}
void SettingHelper::AddKey(Key key, string value)
{
	string strkey = getKeyName(key);
	string strsection = getSectionName(key);


	size_t size = strsection.length();
	wchar_t *sectionbuffer = new wchar_t[size + 1];
	MultiByteToWideChar(CP_ACP, 0, strsection.c_str(), size, sectionbuffer, size * sizeof(wchar_t));
	sectionbuffer[size] = 0;  // 确保以 '\0' 结尾 

	size = strkey.length();
	wchar_t *keybuffer = new wchar_t[size + 1];
	MultiByteToWideChar(CP_ACP, 0, strkey.c_str(), size, keybuffer, size * sizeof(wchar_t));
	keybuffer[size] = 0;  // 确保以 '\0' 结尾 

	size = value.length();
	wchar_t *valuebuffer = new wchar_t[size + 1];
	MultiByteToWideChar(CP_ACP, 0, value.c_str(), size, valuebuffer, size * sizeof(wchar_t));
	valuebuffer[size] = 0;  // 确保以 '\0' 结尾 

	::WritePrivateProfileStringW(sectionbuffer, keybuffer, valuebuffer, SettingHelper::path);

	delete valuebuffer;
	delete keybuffer;
	delete sectionbuffer;
}
bool SettingHelper::GetKeyInt(Key key, int& value)
{
	string str;
	bool flag = GetKeyString(key, str);
	if (!flag)
		return false;
	stringstream ss;
	ss << str;
	ss >> value;
	//value = atoi(str.c_str());
	return true;
}
bool SettingHelper::GetKeyFloat(Key key, float& value)
{
	string str;
	bool flag = GetKeyString(key, str);
	if (!flag)
		return false;
	stringstream ss;
	ss << str;
	ss >> value;
	//value = atof(str.c_str());
	return true;
}

bool SettingHelper::GetKeyDouble(Key key, double& value)
{
	string str;
	bool flag = GetKeyString(key, str);
	if (!flag)
		return false;

	stringstream ss;
	ss << str;
	ss >> value;
	//value = atof(str.c_str());
	return true;
}
bool SettingHelper::GetKeyString(Key key, string& value)
{
	string strkey = getKeyName(key);
	string strsection = getSectionName(key);

	LPWSTR lwpstr;

	size_t size = strsection.length();
	wchar_t *sectionbuffer = new wchar_t[size + 1];
	MultiByteToWideChar(CP_ACP, 0, strsection.c_str(), size, sectionbuffer, size * sizeof(wchar_t));
	sectionbuffer[size] = 0;  // 确保以 '\0' 结尾 

	size = strkey.length();
	wchar_t *keybuffer = new wchar_t[size + 1];
	MultiByteToWideChar(CP_ACP, 0, strkey.c_str(), size, keybuffer, size * sizeof(wchar_t));
	keybuffer[size] = 0;  // 确保以 '\0' 结尾 

	char tmp[128];
	LPSTR lpstr = tmp;
	lwpstr = (LPWSTR)lpstr;
	::GetPrivateProfileString(sectionbuffer, keybuffer, NULL, lwpstr, 128, SettingHelper::path);

	wstring ws(lwpstr);
	std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
	setlocale(LC_ALL, "chs");
	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = 2 * ws.size() + 1;
	char *_Dest = new char[_Dsize];
	memset(_Dest, 0, _Dsize);
	wcstombs(_Dest, _Source, _Dsize);
	std::string result = _Dest;
	delete[]_Dest;
	setlocale(LC_ALL, curLocale.c_str());
	value = result;
	if (value == "")
		return false;
	return true;
}




string SettingHelper::getSectionName(Key key)
{
	//return "GRAB";
	//return "TIGGER";
	switch (key)
	{
	case SettingHelper::GRAB_MaxPics:
		return "GRAB";
	case SettingHelper::GRAB_Width:
		return "GRAB";
	case SettingHelper::GRAB_ColorType:
		return "GRAB";
	case SettingHelper::TIGGER_WaitTime:
		return "TIGGER";
	default:
		break;
	}
	return "";
}
string SettingHelper::getKeyName(Key key)
{
	switch (key)
	{
	case SettingHelper::GRAB_MaxPics:
		return "GRAB_MaxPics";
	case SettingHelper::GRAB_Width:
		return "GRAB_Width";
	case SettingHelper::GRAB_ColorType:
		return "GRAB_ColorType";
	case SettingHelper::TIGGER_WaitTime:
		return "TIGGER_WaitTime";
	default:
		break;
	}
	return "";
}





const LPWSTR SettingHelper::path = _T("src/../Setting.ini");