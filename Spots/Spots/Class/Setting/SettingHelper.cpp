#include "SettingHelper.h"
#include <Class\Helper\StringHelper.h>

SettingHelper::SettingHelper()
{
}


SettingHelper::~SettingHelper()
{
}
void SettingHelper::Init()
{
	try{
		remove(StringHelper::LPWSTR2string(path).c_str());
	}
	catch (exception e)
	{

	}
}


void SettingHelper::AddKey(string SectionName, string Key, int value)
{
	std::stringstream ss;
	ss << value;
	AddKey(SectionName, Key, ss.str());
}
void SettingHelper::AddKey(string SectionName, string Key, float value)
{
	std::stringstream ss;
	ss << value;
	AddKey(SectionName, Key, ss.str());
}
void SettingHelper::AddKey(string SectionName, string Key, double value)
{
	std::stringstream ss;
	ss << value;
	AddKey(SectionName, Key, ss.str());
}
void SettingHelper::AddKey(string SectionName, string Key, LPWSTR value)
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

	AddKey(SectionName, Key, result);
}
void SettingHelper::AddKey(string SectionName, string Key, string value)
{
	size_t size = SectionName.length();
	wchar_t *sectionbuffer = new wchar_t[size + 1];
	MultiByteToWideChar(CP_ACP, 0, SectionName.c_str(), size, sectionbuffer, size * sizeof(wchar_t));
	sectionbuffer[size] = 0;  // 确保以 '\0' 结尾 

	size = Key.length();
	wchar_t *keybuffer = new wchar_t[size + 1];
	MultiByteToWideChar(CP_ACP, 0, Key.c_str(), size, keybuffer, size * sizeof(wchar_t));
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
bool SettingHelper::GetKeyInt(string SectionName, string Key, int& value)
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
bool SettingHelper::GetKeyFloat(string SectionName, string Key, float& value)
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

bool SettingHelper::GetKeyDouble(string SectionName, string Key, double& value)
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
bool SettingHelper::GetKeyString(string SectionName, string Key, string& value)
{
	LPWSTR lwpstr;

	size_t size = SectionName.length();
	wchar_t *sectionbuffer = new wchar_t[size + 1];
	MultiByteToWideChar(CP_ACP, 0, SectionName.c_str(), size, sectionbuffer, size * sizeof(wchar_t));
	sectionbuffer[size] = 0;  // 确保以 '\0' 结尾 

	size = Key.length();
	wchar_t *keybuffer = new wchar_t[size + 1];
	MultiByteToWideChar(CP_ACP, 0, Key.c_str(), size, keybuffer, size * sizeof(wchar_t));
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

//本项的意思就是在.exe路径创建一个ini
const LPWSTR SettingHelper::path = _T("src/../Setting.ini");