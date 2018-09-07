#include "pch.h"
#include "WinHttpClient.h"

namespace RIFMClient
{
	WinHttpClient::WinHttpClient()
	{
		m_wstrAgent = L"RIFM Client 0.2";
	}

	WinHttpClient::~WinHttpClient()
	{
		Close();
	}

	/*
	 * Summary: get current localtime
	 * Parameters: NULL
	 * Return: current localtime string
	 */
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

	/*
	 * Summary: construct error message
	 * Parameters:
	 *  - func: function name when error happen
	 *  - msg: the detial of error message
	 * Return: NULL
	 */
	VOID WinHttpClient::SetErrorMessage(IN std::wstring func, IN std::wstring msg)
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

	/*
	 * Summary: get error message
	 * Parameters: NULL
	 * Return: error message
	 */
	std::wstring WinHttpClient::GetErrorMessage()
	{
		return m_wstrError;
	}


	/*
	 * Summary: connect the database
	 * Parameters: NULL
	 * Return : NULL
	 */
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

	/*
	 * Summary: format error code
	 * Parameters:
	 *  - dwError: input error code
	 * Return: error code string
	 */
	std::wstring WinHttpClient::GetFormatLastErrorCode(IN DWORD dwError)
	{
		WCHAR wszError[16];
		ZeroMemory(wszError, sizeof(wszError) / sizeof(WCHAR));
		swprintf_s(wszError, sizeof(wszError) / sizeof(WCHAR), L"ErrorCode: %u", dwError);
		return wszError;
	}

	/*
	 * Summary: open winhttp function and parse input url
	 * Rarameters:
	 *  - wstrUrl: input url for request rifm server database
	 * Return: true open is success
	 */
	BOOL WinHttpClient::Open(IN std::wstring wstrUrl)
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

	/*
	 * Summary: send request to communicate with rifm server
	 * Parameters:
	 * - method: http method include 'GET', 'POST', 'PUT', 'DELETE'
	 * - strData: http send data, json string is used here
	 * Return: trun if send success
	 */
	BOOL WinHttpClient::Send(IN std::wstring method, IN std::string strData)
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
		// waits to receive the response to an HTTP request
		return WinHttpReceiveResponse(m_hRequest, NULL);
	}

	/*
	 * Summary: get http response code
	 * Parameters: NULL
	 * Return: http response code
	 */
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

	/*
	 * Summary: get http response body
	 * Parameters: NULL
	 * Return: http response string
	 */
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

	/*
	 * Summary: send request to rifm server for create a document in mongodb
	 * Parameters:
	 *  - wstrUrl: url path string for restful interface of creating a document in mongodb
	 *  - strData: json string for a document to be created
	 * Return: true if create operation success
	 */
	BOOL WinHttpClient::Create(IN std::wstring wstrUrl, IN std::string strData)
	{
		if (Open(wstrUrl) == FALSE)
			return FALSE;
		return Send(L"POST", strData);
	}

	/*
	 * Summary: send request to rifm server for read a document in mongodb
	 * Parameters:
	 *  - wstrUrl: url path string for restful interface of reading a document
	 * Return: true if read operation is success
	 */
	BOOL WinHttpClient::Read(IN std::wstring wstrUrl)
	{
		if (Open(wstrUrl) == FALSE)
			return FALSE;
		return Send(L"GET", "");
	}

	/*
	 * Summary: send request to rifm server for update a document in mongodb
	 * Parameters:
	 *  - wstrUrl: url path string for restful interface of updating a document in mongodb
	 *  - strData: json string for a document to be updated
	 * Return: true if update operation success
	 */
	BOOL WinHttpClient::Update(IN std::wstring wstrUrl, IN std::string strData)
	{
		if (Open(wstrUrl) == FALSE)
			return FALSE;
		return Send(L"PUT", strData);
	}

	/*
	 * Summary: send request to rifm server for delete a document in mongodb
	 * Parameters:
	 *  - wstrUrl: url path string for restful interface of deleting a document in mongodb
	 * Return: true if delete operation success
	 */
	BOOL WinHttpClient::Delete(IN std::wstring wstrUrl)
	{
		if (Open(wstrUrl) == FALSE)
			return FALSE;
		return Send(L"DELETE", "");
	}
}
