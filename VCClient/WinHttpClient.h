#pragma once
#include "CommonString.h"

#include <string>
#include <sstream>
#include <vector>
#include <codecvt>
#include <Windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp")

namespace RIFMClient
{
	class WinHttpClient
	{
	public:
		WinHttpClient();
		~WinHttpClient();

		typedef struct _RESPONSE
		{
			DWORD Code;
			std::string Body;
		} RESPONSE, RESULT;

	private:
		HINTERNET m_hSession;
		HINTERNET m_hConnect;
		HINTERNET m_hRequest;

		// 'rifm' server
		std::wstring m_wstrHost;
		// 'rifm' url path for the dbname and collection name of mongo
		std::wstring m_wstrUrlPath;
		// 'rifm' server port
		INT m_nPort;
		// 'rifm' protocol (http or https)
		INT m_nScheme;

		// version of rifm client class
		std::wstring m_wstrAgent;
		// error message
		std::wstring m_wstrError;

		BOOL Open(IN std::string strUrl);
		BOOL Send(IN std::wstring wstrMethod, IN std::string strData);
		VOID Recv(IN OUT RESULT &tagResult);
		VOID Close();

		std::wstring GetDateTime();
		std::wstring GetFormatLastErrorCode(IN DWORD dwError);
		VOID SetErrorMessage(IN std::wstring func, IN std::wstring msg);

	public:
		VOID SetAgent(IN std::wstring strAgent);
		
		// DB 'CRUD' operation
		BOOL Create(
			IN std::string strUrl, 
			IN std::string strData,
			IN OUT RESULT &tagResult
		);
		BOOL Read(IN std::string strUrl, IN OUT RESULT &tagResult);
		BOOL Update(
			IN std::string strUrl, 
			IN std::string strData,
			IN OUT RESULT &tagResult
		);
		BOOL Delete(IN std::string strUrl, IN OUT RESULT &tagResult);

		// Get error message of interact with rifm
		std::wstring GetErrorMessage();
	};
}

