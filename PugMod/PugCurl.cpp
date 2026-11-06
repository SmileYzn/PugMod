#include "precompiled.h"

CPugCurl gPugCurl;

void CPugCurl::ServerActivate()
{
	if (!this->m_MultiHandle)
	{
		this->m_RequestIndex = 0;

		this->m_Data.clear();

		curl_global_init(CURL_GLOBAL_ALL);

		this->m_MultiHandle = curl_multi_init();
	}
}

void CPugCurl::StartFrame()
{
	if (this->m_MultiHandle)
	{
		CURLMcode MsgCode = {};

		int HandleCount = 0;

		CURLMsg *MsgInfo = NULL;

		int MsgQueue = 0;

		MsgCode = curl_multi_perform(this->m_MultiHandle, &HandleCount);

		if (MsgCode == CURLM_OK)
		{
			while ((MsgInfo = curl_multi_info_read(this->m_MultiHandle, &MsgQueue)))
			{
				if (MsgInfo && (MsgInfo->msg == CURLMSG_DONE))
				{
					char *PrivateData = nullptr;

					curl_easy_getinfo(MsgInfo->easy_handle, CURLINFO_PRIVATE, &PrivateData);

					long PrivateDataIndex = (long)(intptr_t)PrivateData;

					struct curl_slist *Headers = nullptr;

					if (this->m_Data.find(PrivateDataIndex) != this->m_Data.end())
					{
						this->CallbackResult(MsgInfo->easy_handle, this->m_Data[PrivateDataIndex].Size, this->m_Data[PrivateDataIndex].Memory);

						if (this->m_Data[PrivateDataIndex].Memory)
						{
							free(this->m_Data[PrivateDataIndex].Memory);
						}

						Headers = this->m_Data[PrivateDataIndex].Headers;

						this->m_Data.erase(PrivateDataIndex);
					}

					curl_multi_remove_handle(this->m_MultiHandle, MsgInfo->easy_handle);

					curl_easy_cleanup(MsgInfo->easy_handle);

					if (Headers)
					{
						curl_slist_free_all(Headers);
					}
				}
			}
		}
	}
}

void CPugCurl::PostJSON(const char *url, long Timeout, std::string BearerToken, std::string PostData)
{
	if (this->m_MultiHandle)
	{
		if (url)
		{
			CURL *ch = curl_easy_init();

			if (ch)
			{
				this->m_Data[this->m_RequestIndex] = {};

				curl_easy_setopt(ch, CURLOPT_URL, url);

				curl_easy_setopt(ch, CURLOPT_TIMEOUT, (Timeout) > 0 ? Timeout : 10L);

				curl_easy_setopt(ch, CURLOPT_FOLLOWLOCATION, 1L);

				curl_easy_setopt(ch, CURLOPT_MAXREDIRS, 3L);

				curl_easy_setopt(ch, CURLOPT_SSL_VERIFYPEER, 1L);

				curl_easy_setopt(ch, CURLOPT_SSL_VERIFYHOST, 2L);

				curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, this->WriteMemoryCallback);

				curl_easy_setopt(ch, CURLOPT_NOPROGRESS, 1L);

				curl_easy_setopt(ch, CURLOPT_POST, 1L);

				curl_easy_setopt(ch, CURLOPT_POSTFIELDSIZE, (long)PostData.size());

				curl_easy_setopt(ch, CURLOPT_COPYPOSTFIELDS, PostData.c_str());

				struct curl_slist *chHeaders = curl_slist_append(NULL, "Content-Type: application/json");

				if (BearerToken.length() > 0)
				{
					std::string AuthorizationHeader = "Authorization: Bearer " + BearerToken;

					chHeaders = curl_slist_append(chHeaders, AuthorizationHeader.c_str());
				}

				curl_easy_setopt(ch, CURLOPT_HTTPHEADER, chHeaders);

				this->m_Data[this->m_RequestIndex].Headers = chHeaders;

				curl_easy_setopt(ch, CURLOPT_WRITEDATA, (void *)&this->m_Data[this->m_RequestIndex]);

				curl_easy_setopt(ch, CURLOPT_PRIVATE, (void *)(intptr_t)this->m_RequestIndex);

				curl_multi_add_handle(this->m_MultiHandle, ch);

				this->m_RequestIndex++;
			}
		}
	}
}

size_t CPugCurl::WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	if (contents)
	{
		if (userp)
		{
			size_t realsize = size * nmemb;

			if (realsize > 0)
			{
				P_CURL_MEMORY *mem = (P_CURL_MEMORY *)(userp);

				char *ptr = (char *)realloc(mem->Memory, mem->Size + realsize + 1);

				if (ptr)
				{
					mem->Memory = ptr;

					Q_memcpy(&(mem->Memory[mem->Size]), contents, realsize);

					mem->Size += realsize;

					mem->Memory[mem->Size] = 0;

					return realsize;
				}
				else
				{
					if (mem->Memory)
					{
						free(mem->Memory);
					}

					mem->Memory = nullptr;

					mem->Size = 0;

					return 0;
				}
			}
		}
	}

	return 0;
}

void CPugCurl::CallbackResult(CURL *ch, size_t Size, const char *Memory)
{
	if (ch)
	{
		long HttpResponseCode = 0;

		if (curl_easy_getinfo(ch, CURLINFO_RESPONSE_CODE, &HttpResponseCode) == CURLE_OK)
		{
			if (HttpResponseCode == 200)
			{
				if (gPugCvar.m_API_Execute->value > 0.0f)
				{
					if (Memory)
					{
						if (Memory[0u] != '\0')
						{
							char *pszEffectiveUrl = nullptr;

							curl_easy_getinfo(ch, CURLINFO_EFFECTIVE_URL, &pszEffectiveUrl);

							if (pszEffectiveUrl && _strnicmp(pszEffectiveUrl, "https://", 8) == 0)
							{
								try
								{
									auto Data = nlohmann::ordered_json::parse(Memory, nullptr, true, true);

									if (!Data.empty())
									{
										if (Data.contains("ServerExecute"))
										{
											if (Data["ServerExecute"].is_string())
											{
												auto String = Data["ServerExecute"].get<std::string>();

												if (!String.empty())
												{
													LOG_CONSOLE(PLID, "[%s] API ServerExecute: %s", Plugin_info.logtag, String.c_str());

													gPugUtil.ServerCommand("%s", String.c_str());
												}
											}
										}
									}
								}
								catch (nlohmann::json::exception &e)
								{
									LOG_CONSOLE(PLID, "[%s] %s", __func__, e.what());
								}
							}
							else
							{
								LOG_CONSOLE(PLID, "[%s] API ServerExecute ignored: HTTPS is required.", Plugin_info.logtag);
							}
						}
					}
				}
			}
			else
			{
				LOG_CONSOLE(PLID, "[%s] Response: HTTP Code %ld, check pug_api_address and pug_api_timeout.", Plugin_info.logtag, HttpResponseCode);
			}
		}
	}
}
