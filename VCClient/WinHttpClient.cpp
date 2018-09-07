#include "pch.h"
#include "WinHttpClient.h"

namespace RIFMClient
{
	WinHttpClient::WinHttpClient()
	{
		m_wstrAgent = L"RIFM Client 0.4";
	}

	WinHttpClient::~WinHttpClient()
	{
		Close();
	}

	std::wstring WinHttpClient::GetDateTime()
	{
		SYSTEMTIME lt;
		GetLocalTime(&lt);
		// Note: you can use 'GetSystemTime()' for UTC.
		//SYSTEMTIME st;
		//GetSystemTime(&st);
		std::wstringstream wss;
		wss << lt.wYear << L"-" << lt.wMonth << L"-" << lt.wDay << L" " << lt.wHour << L":" << lt.wMinute << L":" << lt.wSecond;
		return wss.str();
	}

	VOID WinHttpClient::SetErrorMessage(std::wstring func, std::wstring msg)
	{
		m_wstrError.clear();
		m_wstrError.append(L"<Error>\n");

		m_wstrError.append(L"Time:");
		m_wstrError.append(GetDateTime());
		m_wstrError.append(L"\n");

		m_wstrError.append(L"Source file : WinHttpClient.cpp\n");

		m_wstrError.append(L"FunctionName:");
		m_wstrError.append(func);
		m_wstrError.append(L"\n");

		m_wstrError.append(L"ErrorMessage:");
		m_wstrError.append(msg);
		m_wstrError.append(L"\n");
	}

	std::wstring WinHttpClient::GetErrorMessage()
	{
		return m_wstrError;
	}

	VOID WinHttpClient::Close()
	{
		if (m_hRequest)
		{
			WinHttpCloseHandle(m_hRequest);
			m_hRequest = NULL;
		}
		if (m_hConnect)
		{
			WinHttpCloseHandle(m_hConnect);
			m_hConnect = NULL;
		}
		if (m_hSession)
		{
			WinHttpCloseHandle(m_hSession);
			m_hSession = NULL;
		}
	}

	std::wstring WinHttpClient::GetFormatLastErrorCode(DWORD dwError)
	{
		WCHAR wszError[16];
		ZeroMemory(wszError, sizeof(wszError) / sizeof(WCHAR));
		swprintf_s(wszError, sizeof(wszError) / sizeof(WCHAR), L"ErrorCode: %u", dwError);
		return wszError;
	}

	BOOL WinHttpClient::Open(std::wstring wstrUrl)
	{
		// check open handles
		Close();
		BOOL bResult;
		// check platform support.
		bResult = WinHttpCheckPlatform();
		if (bResult == FALSE)
		{
			SetErrorMessage(L"WinHttpCheckPlatform()",
				L"The platform is not supported by Microsoft Windows HTTP Services(WinHTTP)");
#ifdef DEBUG
			wprintf(L"%s", GetErrorMessage().c_str());
#endif
			return FALSE;
		}
		// initialize winhttp
		m_hSession = WinHttpOpen(
			m_wstrAgent.c_str(),
			WINHTTP_ACCESS_TYPE_NO_PROXY,
			WINHTTP_NO_PROXY_NAME,
			WINHTTP_NO_PROXY_BYPASS,
			0
		);
		if (m_hSession == NULL)
		{
			SetErrorMessage(L"WinHttpOpen()",
				GetFormatLastErrorCode(GetLastError()));
#ifdef DEBUG
			wprintf(L"%s", GetErrorMessage().c_str());
#endif
			return FALSE;
		}
		// parse URL
		URL_COMPONENTSW tagKUrlComponent;
		ZeroMemory(&tagKUrlComponent, sizeof(tagKUrlComponent));
		tagKUrlComponent.dwStructSize = sizeof(tagKUrlComponent);
		tagKUrlComponent.dwHostNameLength = 1;
		tagKUrlComponent.dwUserNameLength = 1;
		tagKUrlComponent.dwPasswordLength = 1;
		tagKUrlComponent.dwUrlPathLength = 1;
		tagKUrlComponent.dwExtraInfoLength = 1;
		// separates a URL into its component parts such as host name and path
		bResult = WinHttpCrackUrl(
			wstrUrl.c_str(),
			0,
			0,
			&tagKUrlComponent
		);
		if (bResult != TRUE)
		{
			SetErrorMessage(L"WinHttpCrackUrl()",
				GetFormatLastErrorCode(GetLastError()));
#ifdef DEBUG
			wprintf(L"%s", GetErrorMessage().c_str());
#endif
			return FALSE;
		}
		else
		{
			// set 'rifm' server
			m_wstrHost = std::wstring(tagKUrlComponent.lpszHostName, tagKUrlComponent.dwHostNameLength);
			// set 'rifm' url for the dbname and collection name of mongo
			m_wstrUrlPath = std::wstring(tagKUrlComponent.lpszUrlPath, tagKUrlComponent.dwUrlPathLength);
			// set 'rifm' server port
			m_nPort = tagKUrlComponent.nPort;
			// set 'rifm' protocol (http or https)
			m_nScheme = tagKUrlComponent.nScheme;
			return TRUE;
		}
	}

	BOOL WinHttpClient::Send(std::wstring method, std::string strData)
	{
		BOOL bResult;
		// specifies the initial target server of an HTTP request
		
		m_hConnect = WinHttpConnect(
			m_hSession,
			m_wstrHost.c_str(),
			m_nPort,
			0
		);
		if (!m_hConnect) // an HINTERNET connection handle to an HTTP session for that initial target
		{
			SetErrorMessage(L"WinHttpConnect()",
				GetFormatLastErrorCode(GetLastError()));
#ifdef DEBUG
			wprintf(L"%s", GetErrorMessage().c_str());
#endif
			return FALSE;
		}
		// creates an HTTP request handle
		m_hRequest = WinHttpOpenRequest(
			m_hConnect,
			method.c_str(),
			m_wstrUrlPath.c_str(),
			NULL,
			WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			m_nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0);
		if (!m_hRequest)
		{
			SetErrorMessage(L"WinHttpOpenRequest()",
				GetFormatLastErrorCode(GetLastError()));
#ifdef DEBUG
			wprintf(L"%s", GetErrorMessage().c_str());
#endif
			return FALSE;
		}
		// sends the specified request to the HTTP server
		if (strData.length() == 0)
		{
			bResult = WinHttpSendRequest(
				m_hRequest,
				WINHTTP_NO_ADDITIONAL_HEADERS,
				0,
				WINHTTP_NO_REQUEST_DATA,
				0,
				0,
				0);
		}
		else
		{
			bResult = WinHttpSendRequest(
				m_hRequest,
				L"Content-Type: application/json\r\n",
				-1L,
				(LPVOID)strData.c_str(),
				strData.length(),
				strData.length(),
				0
			);
		}
		// check result
		if (bResult != TRUE)
		{
			SetErrorMessage(L"WinHttpSendRequest()",
				GetFormatLastErrorCode(GetLastError()));
#ifdef DEBUG
			wprintf(L"%s", GetErrorMessage().c_str());
#endif
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}

	DWORD WinHttpClient::GetStatusCode()
	{
		DWORD dwStatusCode;
		DWORD dwSize = sizeof(DWORD);
		BOOL bRet = WinHttpQueryHeaders(
			m_hRequest,
			WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
			NULL,
			&dwStatusCode,
			&dwSize,
			NULL
		);
		return bRet ? dwStatusCode : 0;
	}

	std::string WinHttpClient::GetResponseBody()
	{
		BOOL bResult;
		DWORD dwReadSize, dwReadedSize;
		std::vector<BYTE> vectorRecv;
		do
		{
			dwReadSize = 0;
			// Check for available data.
			bResult = WinHttpQueryDataAvailable(m_hRequest, &dwReadSize);
			if (bResult != TRUE)
			{
				SetErrorMessage(L"WinHttpQueryDataAvailable()",
					GetFormatLastErrorCode(GetLastError()));
#ifdef DEBUG
				wprintf(L"%s", GetErrorMessage().c_str());
#endif
				break;
			}
			// No more available data.
			if (!dwReadSize)
				break;

			// Allocate space for the buffer.
			BYTE* lpReceivedData = new BYTE[dwReadSize];
			if (!lpReceivedData)
			{
				SetErrorMessage(L"GetResponseBody()",
					L"Allocate space for recieve buffer is out of memory.");
#ifdef DEBUG
				wprintf(L"%s", GetErrorMessage().c_str());
#endif
				break;
			}
			else
			{
				// Read the Data.
				ZeroMemory(lpReceivedData, dwReadSize);

				bResult = WinHttpReadData(
					m_hRequest,
					lpReceivedData,
					dwReadSize,
					&dwReadedSize
				);
				if (bResult == TRUE)
				{
					for (size_t i = 0; i < dwReadSize; i++)
						vectorRecv.push_back(lpReceivedData[i]);
				}
				else
				{
					SetErrorMessage(L"WinHttpReadData()",
						GetFormatLastErrorCode(GetLastError()));
#ifdef DEBUG
					wprintf(L"%s", GetErrorMessage().c_str());
#endif
				}

				// Free the memory allocated to the buffer.
				delete[] lpReceivedData;

				// This condition should never be reached since WinHttpQueryDataAvailable
				// reported that there are bits to read.
				if (!dwReadedSize)
					break;
			}
		} while (dwReadSize > 0);
		// Prepair final response body string.
		std::string strResponseBody;
		strResponseBody.insert(strResponseBody.begin(), vectorRecv.begin(), vectorRecv.end());
		return strResponseBody;
	}

	BOOL WinHttpClient::Create(std::wstring wstrUrl, std::string strData)
	{
		if (Open(wstrUrl) == FALSE)
			return FALSE;
		if (Send(L"POST", strData) == FALSE)
			return FALSE;
		// waits to receive the response to an HTTP request
		return WinHttpReceiveResponse(m_hRequest, NULL);
	}

	BOOL WinHttpClient::Read(std::wstring wstrUrl)
	{
		if (Open(wstrUrl) == FALSE)
			return FALSE;
		if (Send(L"GET", "") == FALSE)
			return FALSE;
		// waits to receive the response to an HTTP request
		return WinHttpReceiveResponse(m_hRequest, NULL);
	}

	BOOL WinHttpClient::Update(std::wstring wstrUrl, std::string strData)
	{
		if (Open(wstrUrl) == FALSE)
			return FALSE;
		if (Send(L"PUT", strData) == FALSE)
			return FALSE;
		// waits to receive the response to an HTTP request
		return WinHttpReceiveResponse(m_hRequest, NULL);
	}

	BOOL WinHttpClient::Delete(std::wstring wstrUrl)
	{
		if (Open(wstrUrl) == FALSE)
			return FALSE;
		if (Send(L"DELETE", "") == FALSE)
			return FALSE;
		// waits to receive the response to an HTTP request
		return WinHttpReceiveResponse(m_hRequest, NULL);
	}
}
