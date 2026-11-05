#pragma once

struct P_CURL_MEMORY
{
	char* Memory;
	size_t Size;
	struct curl_slist* Headers;
};

class CPugCurl
{
public:
	void ServerActivate();
	void StartFrame();
	void PostJSON(const char* url, long Timeout, std::string BearerToken, std::string PostData);
	static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp);
	void CallbackResult(CURL* ch, size_t Size, const char* Memory);
private:
	CURLM* m_MultiHandle = NULL;
	long m_RequestIndex = 0;
	std::map<long, P_CURL_MEMORY> m_Data;
};

extern CPugCurl gPugCurl;