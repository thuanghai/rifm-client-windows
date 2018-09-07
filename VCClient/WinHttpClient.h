#pragma once
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

		BOOL Open(std::wstring wstrUrl);
		BOOL Send(std::wstring method, std::string strData);

		std::wstring GetDateTime();
		std::wstring GetFormatLastErrorCode(DWORD dwError);
		VOID SetErrorMessage(std::wstring func, std::wstring msg);
	public:
		WinHttpClient();
		~WinHttpClient();
		
		// DB 'CRUD' operation
		BOOL Create(std::wstring wstrUrl, std::string strData);
		BOOL Read(std::wstring wstrUrl);
		BOOL Update(std::wstring wstrUrl, std::string strData);
		BOOL Delete(std::wstring wstrUrl);
		// Ger response code for 'CRUD'
		DWORD GetStatusCode();
		// Get response body for 'CRUD'
		std::string GetResponseBody();
		// clear open handles
		VOID Close();
		
		std::wstring GetErrorMessage();
	};
}

