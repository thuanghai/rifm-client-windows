#pragma once
#include "WinHttpClient.h"
#include <string>

namespace RIFMClient
{
	class DataOperate
	{
	public:
		DataOperate();
		~DataOperate();
	private:
		// restful method
		WinHttpClient m_objHttpClient;
		// error message
		std::wstring m_wstrErrorMessage;
	public:
		std::wstring GetErrorMessage();

		BOOL CreateOne(
			IN std::string strUrl, 
			IN std::string data, 
			IN OUT WinHttpClient::RESULT &tagResult
		);
		BOOL ReadOne(
			IN std::string strUrl, 
			IN OUT WinHttpClient::RESULT &tagResult
		);
		BOOL UpdateOne(
			IN std::string strUrl, 
			IN std::string data,
			IN OUT WinHttpClient::RESULT &tagResult
		);
		BOOL DeleteOne(
			IN std::string strUrl, 
			IN OUT WinHttpClient::RESULT &tagResult
		);
	};
}

