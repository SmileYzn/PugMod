#pragma once

#define _T(Key) (gPugLang.Get(Key))

class CPugLang
{
public:
	void ServerActivate();
	const char* Get(const char* Head);

private:
	std::unordered_map<std::string, std::map<std::string, std::string>> m_Data = {};
};

extern CPugLang gPugLang;