#include "pch.h"
#include "RIFMClient.h"


CommonString::CommonString()
{
}


CommonString::~CommonString()
{
}

std::wstring CommonString::ANSIToUnicode(const std::string& str)
{
	size_t  len = 0;
	len = str.length();
	int  unicodeLen = ::MultiByteToWideChar(CP_ACP,
		0,
		str.c_str(),
		-1,
		NULL,
		0);
	wchar_t *  pUnicode;
	pUnicode = new  wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
	::MultiByteToWideChar(CP_ACP,
		0,
		str.c_str(),
		-1,
		(LPWSTR)pUnicode,
		unicodeLen);
	std::wstring  rt;
	rt = (wchar_t*)pUnicode;
	delete  pUnicode;
	return  rt;
}

std::string CommonString::UnicodeToANSI(const std::wstring& str)
{
	char*     pElementText;
	int    iTextLen;
	// wide char to multi char
	iTextLen = WideCharToMultiByte(CP_ACP,
		0,
		str.c_str(),
		-1,
		NULL,
		0,
		NULL,
		NULL);
	pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, sizeof(char)* (iTextLen + 1));
	::WideCharToMultiByte(CP_ACP,
		0,
		str.c_str(),
		-1,
		pElementText,
		iTextLen,
		NULL,
		NULL);
	std::string strText;
	strText = pElementText;
	delete[] pElementText;
	return strText;
}

std::wstring CommonString::UTF8ToUnicode(const std::string& str)
{
	size_t  len = 0;
	len = str.length();
	int  unicodeLen = ::MultiByteToWideChar(CP_UTF8,
		0,
		str.c_str(),
		-1,
		NULL,
		0);
	wchar_t *  pUnicode;
	pUnicode = new  wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
	::MultiByteToWideChar(CP_UTF8,
		0,
		str.c_str(),
		-1,
		(LPWSTR)pUnicode,
		unicodeLen);
	std::wstring  rt;
	rt = (wchar_t*)pUnicode;
	delete  pUnicode;
	return  rt;
}

std::string CommonString::UnicodeToUTF8(const std::wstring& str)
{
	char*     pElementText;
	int    iTextLen;
	// wide char to multi char
	iTextLen = WideCharToMultiByte(CP_UTF8,
		0,
		str.c_str(),
		-1,
		NULL,
		0,
		NULL,
		NULL);
	pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, sizeof(char)* (iTextLen + 1));
	::WideCharToMultiByte(CP_UTF8,
		0,
		str.c_str(),
		-1,
		pElementText,
		iTextLen,
		NULL,
		NULL);
	std::string strText;
	strText = pElementText;
	delete[] pElementText;
	return strText;
}

namespace RIFMClient
{


	///////////////////
	// WinHttpClient //
	///////////////////

	WinHttpClient::WinHttpClient()
	{
		m_wstrAgent = L"RIFM Client 0.2";
	}

	WinHttpClient::~WinHttpClient()
	{
		Close();
	}

	/*
	* Summary: set http client agent
	* Parameters:
	*  - wstrAgent: user defined http client agent
	* Return: NULL
	*/
	VOID WinHttpClient::SetAgent(std::wstring wstrAgent)
	{
		m_wstrAgent = wstrAgent;
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
		WCHAR wszError[1024];
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
	BOOL WinHttpClient::Open(IN std::string strUrl)
	{
		// check open handles
		Close();
		// clean error message buf
		m_wstrError.clear();

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
		std::wstring wstrUrl = CommonString::ANSIToUnicode(strUrl);
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
	* - wstrMethod: http wstrMethod include 'GET', 'POST', 'PUT', 'DELETE'
	* - strData: http send data, json string is used here
	* Return: trun if send success
	*/
	BOOL WinHttpClient::Send(IN std::wstring wstrMethod, IN std::string strData)
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
			wstrMethod.c_str(),
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
				(DWORD)strData.length(),
				(DWORD)strData.length(),
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
	* Summary: receive http response
	* Parameters:
	*  -tagResult: http response for output
	* Return: NULL
	*/
	VOID WinHttpClient::Recv(IN OUT RESPONSE &tagResult)
	{
		ZeroMemory(&tagResult, sizeof(tagResult));
		BOOL bReturn;
		//
		// set response code
		//
		DWORD dwStatusCode;
		DWORD dwSize = sizeof(DWORD);
		bReturn = WinHttpQueryHeaders(
			m_hRequest,
			WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
			NULL,
			&dwStatusCode,
			&dwSize,
			NULL
			);
		tagResult.Code = bReturn ? dwStatusCode : 0;
		// 
		// set response body
		//
		DWORD dwReadSize, dwReadedSize;
		//
		// anther choice for recieve raw data.
		//std::vector<BYTE> *pRecv = new std::vector<BYTE>;
		do
		{
			dwReadSize = 0;
			// Check for available data.
			bReturn = WinHttpQueryDataAvailable(m_hRequest, &dwReadSize);
			if (bReturn != TRUE)
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

				bReturn = WinHttpReadData(
					m_hRequest,
					lpReceivedData,
					dwReadSize,
					&dwReadedSize
					);
				if (bReturn == TRUE)
				{
					//
					// anther choice for recieve raw data.
					//for (size_t i = 0; i < dwReadSize; i++)
					//	pRecv->push_back(lpReceivedData[i]);
					tagResult.Body.append((CONST CHAR*)lpReceivedData, dwReadedSize);
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
		//
		// anther choice for recieve raw data.
		//// Prepair final response body string.
		//tagResult.Body.insert(
		//	tagResult.Body.begin(),
		//	pRecv->begin(), 
		//	pRecv->end());
		//delete pRecv;
	}

	/*
	* Summary: send request to rifm server for create a document in mongodb
	* Parameters:
	*  - wstrUrl: url path string for restful interface of creating a document in mongodb
	*  - strData: json string for a document to be created
	*  - tagResult: code and body of http response
	* Return: true if create operation success
	*/
	BOOL WinHttpClient::Create(IN std::string strUrl, IN std::string strData, IN OUT RESPONSE &tagResult)
	{
		BOOL bReturn = FALSE;
		if (Open(strUrl) && Send(L"POST", strData))
		{
			Recv(tagResult);
			bReturn = TRUE;
		}
		Close();
		return bReturn;
	}

	/*
	* Summary: send request to rifm server for read a document in mongodb
	* Parameters:
	*  - wstrUrl: url path string for restful interface of reading a document
	*  - tagResult: code and body of http response
	* Return: true if read operation is success
	*/
	BOOL WinHttpClient::Read(IN std::string strUrl, IN OUT RESPONSE &tagResult)
	{
		BOOL bReturn = FALSE;
		if (Open(strUrl) && Send(L"GET", ""))
		{
			Recv(tagResult);
			bReturn = TRUE;
		}
		Close();
		return bReturn;
	}

	/*
	* Summary: send request to rifm server for update a document in mongodb
	* Parameters:
	*  - wstrUrl: url path string for restful interface of updating a document in mongodb
	*  - strData: json string for a document to be updated
	*  - tagResult: code and body of http response
	* Return: true if update operation success
	*/
	BOOL WinHttpClient::Update(IN std::string strUrl, IN std::string strData, IN OUT RESPONSE &tagResult)
	{
		BOOL bReturn = FALSE;
		if (Open(strUrl) && Send(L"PUT", strData))
		{
			Recv(tagResult);
			bReturn = TRUE;
		}
		Close();
		return bReturn;
	}

	/*
	* Summary: send request to rifm server for delete a document in mongodb
	* Parameters:
	*  - wstrUrl: url path string for restful interface of deleting a document in mongodb
	*  - tagResult: code and body of http response
	* Return: true if delete operation success
	*/
	BOOL WinHttpClient::Delete(IN std::string strUrl, IN OUT RESPONSE &tagResult)
	{
		BOOL bReturn = FALSE;
		if (Open(strUrl) && Send(L"DELETE", ""))
		{
			Recv(tagResult);
			bReturn = TRUE;
		}
		Close();
		return bReturn;
	}

	///////////////
	// DataModel //
	//////////////

	DataModel::DataModel()
	{
	}


	DataModel::~DataModel()
	{
	}

	// initialize const key
	//
	// common key
	const std::string DataModel::KeyCommon::ID = "_id";
	const std::string DataModel::KeyCommon::SrcID = "src_id";
	const std::string DataModel::KeyCommon::TimeStamp = "time_stamp";
	const std::string DataModel::KeyCommon::StoragePath = "storage_path";
	//const std::string DataModel::KeyCommon::CreateUser = "create.user";
	//const std::string DataModel::KeyCommon::CreateTime = "create.time";
	//const std::string DataModel::KeyCommon::ModifyUser = "modify.user";
	//const std::string DataModel::KeyCommon::ModifyTime = "modify.time";
	const std::string DataModel::KeyCommon::User = "user";
	// Note: You must compute struct element count for serialize/deserialize json buffer here!
	const int DataModel::CommonSize = 8;

	// signalelement
	const std::string DataModel::KeySignalElement::Satellite = "satellite";
	const std::string DataModel::KeySignalElement::Antenna = "antenna";
	const std::string DataModel::KeySignalElement::Polarity = "polarity";
	const std::string DataModel::KeySignalElement::Frequency = "frequency";
	const std::string DataModel::KeySignalElement::ModulateType = "modulate_type";
	const std::string DataModel::KeySignalElement::ModulateRate = "modulate_rate";
	const std::string DataModel::KeySignalElement::Demodulator = "demodulator";
	const std::string DataModel::KeySignalElement::FrameType = "frame_type";
	const std::string DataModel::KeySignalElement::SourceType = "source_type";
	// Note: You must compute struct element count for serialize/deserialize json buffer here!
	const int DataModel::SignalElementSize = DataModel::CommonSize + 9;

	// framecontrol
	const std::string DataModel::KeyFrameControl::Type = "type";
	// Note: You must compute struct element count for serialize/deserialize json buffer here!
	const int DataModel::FrameControlSize = DataModel::CommonSize + 1;

	// ip
	const std::string DataModel::KeyIp::EmbeddedSrcID = "embedded_src_id";
	const std::string DataModel::KeyIp::Encrypted = "encrypted";
	const std::string DataModel::KeyIp::Protocol = "protocol";
	const std::string DataModel::KeyIp::SrcIp = "src_ip";
	const std::string DataModel::KeyIp::DstIp = "dst_ip";
	const std::string DataModel::KeyIp::SrcPort = "src_port";
	const std::string DataModel::KeyIp::DstPort = "dst_port";
	// Note: You must compute struct element count for serialize/deserialize json buffer here!
	const int DataModel::IpSize = DataModel::CommonSize + 7;

	// email
	const std::string DataModel::KeyEmail::Title = "title";
	const std::string DataModel::KeyEmail::From = "from";
	const std::string DataModel::KeyEmail::To = "to";
	const std::string DataModel::KeyEmail::AttachmentType = "attachment_type";
	// Note: You must compute struct element count for serialize/deserialize json buffer here!
	const int DataModel::EmailSize = DataModel::CommonSize + 4;

	// http
	const std::string DataModel::KeyHttp::Title = "title";
	const std::string DataModel::KeyHttp::Type = "type";
	// Note: You must compute struct element count for serialize/deserialize json buffer here!
	const int DataModel::HttpSize = DataModel::CommonSize + 2;

	/*
	* Summary: json serialize data for rifm client create
	* Parameters:
	*  - mapData: input created data
	*  - strJson: output json serialized data
	* Return: json serialized data
	*/
	void DataModel::SerializeCreateData(
		std::map<std::string, std::string> &mapData,
		std::string &strJson
		)
	{
		if (mapData.size() <= 0)
		{
			strJson.erase();
			return;
		}

		using namespace ArduinoJson;
		//StaticJsonBuffer<DataModel::SignalElementSize> objJsonBuffer;
		ArduinoJson::DynamicJsonBuffer objJsonBuffer(JSON_OBJECT_SIZE(mapData.size() + 1));
		// Note:
		// Using 'StaticJsonBuffer' may not get the serialized json string.
		// And I have no idea of the fuck why.

		// add all elements for creating items except user information
		BOOL bHasUserInfo = FALSE;
		std::string strUserKey;
		std::string strUserValue;
		ArduinoJson::JsonObject &objRoot = objJsonBuffer.createObject();
		for (auto kv : mapData)
		{
			if (kv.first.compare(DataModel::KeyCommon::User) == 0)
			{
				bHasUserInfo = TRUE;
				strUserKey = kv.first;
				strUserValue = kv.second;
			}
			else
			{
				objRoot.set(kv.first, kv.second);
			}
		}
		// do the create user information at last.
		if (bHasUserInfo == TRUE)
		{
			ArduinoJson::JsonObject &objEmb = objRoot.createNestedObject("create");
			objEmb.set(strUserKey, strUserValue);
		}
		objRoot.printTo(strJson);
	}

	/*
	* Summary: json serialize data for rifm client create
	* Parameters:
	*  - vData: input created data
	*  - strJson: output json serialized data
	* Return: json serialized data
	*/
	void DataModel::SerializeCreateData(
		std::vector<std::pair<std::string, std::string>> &vData,
		std::string &strJson
	)
	{
		if (vData.size() <= 0)
		{
			strJson.erase();
			return;
		}

		using namespace ArduinoJson;
		//StaticJsonBuffer<DataModel::SignalElementSize> objJsonBuffer;
		ArduinoJson::DynamicJsonBuffer objJsonBuffer(JSON_OBJECT_SIZE(vData.size() + 1));
		// Note:
		// Using 'StaticJsonBuffer' may not get the serialized json string.
		// And I have no idea of the fuck why.

		ArduinoJson::JsonObject &objRoot = objJsonBuffer.createObject();
		// add all elements for creating items except user information
		BOOL bHasUserInfo = FALSE;
		std::string strUserKey;
		std::string strUserValue;
		for (auto iter = vData.begin(); iter != vData.end(); iter++)
		{
			if (iter->first.compare(DataModel::KeyCommon::User) == 0)
			{
				bHasUserInfo = TRUE;
				strUserKey = iter->first;
				strUserValue = iter->second;
			}
			else
			{
				objRoot.set(iter->first, iter->second);
			}
		}
		// add user information for creating item at last
		if (bHasUserInfo == TRUE)
		{
			ArduinoJson::JsonObject &objEmb = objRoot.createNestedObject("create");
			objEmb.set(strUserKey, strUserValue);
		}
		objRoot.printTo(strJson);
	}

	/*
	* Summary: json serialize data for rifm client update
	* Parameters:
	*  - mapData: input updated data
	*  - strJson: output json serialized data
	* Return: json serialized data
	*/
	void DataModel::SerializeUpdateData(
		std::map<std::string, std::string> &mapData,
		std::string &strJson
		)
	{
		if (mapData.size() <= 0)
		{
			strJson.erase();
			return;
		}
		using namespace ArduinoJson;
		//StaticJsonBuffer<DataModel::SignalElementSize> objJsonBuffer;
		ArduinoJson::DynamicJsonBuffer objJsonBuffer(JSON_OBJECT_SIZE(mapData.size() + 1));
		// Note:
		// Using 'StaticJsonBuffer' may not get the serialized json string.
		// And I have no idea of the fuck why.

		ArduinoJson::JsonObject &objRoot = objJsonBuffer.createObject();
		ArduinoJson::JsonObject &objSet = objRoot.createNestedObject("$set");
		for (auto kv : mapData)
		{
			if (kv.first.compare(DataModel::KeyCommon::User) == 0)
			{
				std::string strNewKey = "modify." + DataModel::KeyCommon::User;
				objSet[strNewKey] = kv.second;
			}
			else
			{
				objSet[kv.first] = kv.second;
			}
		}
		objRoot.printTo(strJson);
	}

	/*
	* Summary: json serialize data for rifm client update
	* Parameters:
	*  - vData: input updated data
	*  - strJson: output json serialized data
	* Return: json serialized data
	*/
	void DataModel::SerializeUpdateData(
		std::vector<std::pair<std::string, std::string>> &vData,
		std::string &strJson
	)
	{
		if (vData.size() <= 0)
		{
			strJson.erase();
			return;
		}
		using namespace ArduinoJson;
		//StaticJsonBuffer<DataModel::SignalElementSize> objJsonBuffer;
		ArduinoJson::DynamicJsonBuffer objJsonBuffer(JSON_OBJECT_SIZE(vData.size() + 1));
		// Note:
		// Using 'StaticJsonBuffer' may not get the serialized json string.
		// And I have no idea of the fuck why.

		ArduinoJson::JsonObject &objRoot = objJsonBuffer.createObject();
		ArduinoJson::JsonObject &objSet = objRoot.createNestedObject("$set");
		for (auto iter = vData.begin(); iter != vData.end(); iter++)
		{
			if (iter->first.compare(DataModel::KeyCommon::User) == 0)
			{
				std::string strNewKey = "modify." + DataModel::KeyCommon::User;
				objSet[strNewKey] = iter->second;
			}
			else
			{
				objSet[iter->first] = iter->second;
			}
		}
		objRoot.printTo(strJson);
	}

	/*
	* Summary: add deserialize data into a variable of map class
	* Parameters:
	*  - mapData: input and output data of map class for deserialized result
	*  - objRoot: json root object of ArduinoJson
	*  - id: id of add data
	* Return: add map element with deserilized data
	*/
	void DataModel::AddDeserializedData(
		std::map<std::string, std::string> &mapData,
		ArduinoJson::JsonObject &objRoot,
		std::string id
		)
	{
		if (objRoot.containsKey(id))
		{
			mapData.insert(std::make_pair(id, objRoot[id].as<std::string>()));
		}
	}

	/*
	* Summary: add deserialize data into a variable of vector class
	* Parameters:
	*  - vData: input and output data of vector class for deserialized result
	*  - objRoot: json root object of ArduinoJson
	*  - id: id of add data
	* Return: add vector element with deserilized data
	*/
	void DataModel::AddDeserializedData(
		std::vector<std::pair<std::string, std::string>> &vData,
		ArduinoJson::JsonObject &objRoot,
		std::string id
	)
	{
		if (objRoot.containsKey(id))
		{
			vData.push_back(std::make_pair(id, objRoot[id].as<std::string>()));
		}
	}

	/*
	* Summary: add deserialize data into a variable of map class
	* Parameters:
	*  - mapData: input and output data of map class for deserialized result
	*  - strJson: input json string to be deserialized
	*  - eJsonClass: class of data
	* Return: add map element with deserilized data
	*/
	void DataModel::DeserializeReadData(
		std::map<std::string, std::string> &mapData,
		const std::string &strJson,
		JsonDataClass eJsonClass
		)
	{
		ArduinoJson::DynamicJsonBuffer objJsonBuffer(DataModel::SignalElementSize);
		// Fuck: StaticJsonBuffer can not use here when I debug it, and I do not know why.

		ArduinoJson::JsonObject &objRoot = objJsonBuffer.parseObject(strJson);
		// ID
		mapData.insert(std::make_pair(DataModel::KeyCommon::ID, objRoot[DataModel::KeyCommon::ID].as<std::string>()));
		// SrcID
		AddDeserializedData(mapData, objRoot, DataModel::KeyCommon::SrcID);
		// Timestamp
		AddDeserializedData(mapData, objRoot, DataModel::KeyCommon::TimeStamp);
		// StoragePath
		AddDeserializedData(mapData, objRoot, DataModel::KeyCommon::StoragePath);
		// CreateUser
		mapData.insert(std::make_pair("create_user", objRoot["create"]["user"].as<std::string>()));
		// CreateTime
		mapData.insert(std::make_pair("create_time", objRoot["create"]["time"].as<std::string>()));
		// ModifyUser
		mapData.insert(std::make_pair("modify_user", objRoot["modify"]["user"].as<std::string>()));
		// ModifyTime
		mapData.insert(std::make_pair("modify_time", objRoot["modify"]["time"].as<std::string>()));
		switch (eJsonClass)
		{
		case RIFMClient::DataModel::SignalElement:
			AddDeserializedData(mapData, objRoot, DataModel::KeySignalElement::Antenna);
			AddDeserializedData(mapData, objRoot, DataModel::KeySignalElement::Demodulator);
			AddDeserializedData(mapData, objRoot, DataModel::KeySignalElement::FrameType);
			AddDeserializedData(mapData, objRoot, DataModel::KeySignalElement::Frequency);
			AddDeserializedData(mapData, objRoot, DataModel::KeySignalElement::ModulateRate);
			AddDeserializedData(mapData, objRoot, DataModel::KeySignalElement::ModulateType);
			AddDeserializedData(mapData, objRoot, DataModel::KeySignalElement::Polarity);
			AddDeserializedData(mapData, objRoot, DataModel::KeySignalElement::Satellite);
			AddDeserializedData(mapData, objRoot, DataModel::KeySignalElement::SourceType);
			break;
		case RIFMClient::DataModel::ControlFrame:
			AddDeserializedData(mapData, objRoot, DataModel::KeyFrameControl::Type);
			break;
		case RIFMClient::DataModel::Ip:
			AddDeserializedData(mapData, objRoot, DataModel::KeyIp::EmbeddedSrcID);
			AddDeserializedData(mapData, objRoot, DataModel::KeyIp::Encrypted);
			AddDeserializedData(mapData, objRoot, DataModel::KeyIp::Protocol);
			AddDeserializedData(mapData, objRoot, DataModel::KeyIp::SrcIp);
			AddDeserializedData(mapData, objRoot, DataModel::KeyIp::SrcPort);
			AddDeserializedData(mapData, objRoot, DataModel::KeyIp::DstIp);
			AddDeserializedData(mapData, objRoot, DataModel::KeyIp::DstPort);
			break;
		case RIFMClient::DataModel::Http:
			AddDeserializedData(mapData, objRoot, DataModel::KeyHttp::Title);
			AddDeserializedData(mapData, objRoot, DataModel::KeyHttp::Type);
			break;
		case RIFMClient::DataModel::Email:
			AddDeserializedData(mapData, objRoot, DataModel::KeyEmail::Title);
			AddDeserializedData(mapData, objRoot, DataModel::KeyEmail::From);
			AddDeserializedData(mapData, objRoot, DataModel::KeyEmail::To);
			AddDeserializedData(mapData, objRoot, DataModel::KeyEmail::AttachmentType);
			break;
		default:
			break;
		}
	}

	/*
	* Summary: add deserialize data into a variable of vector class
	* Parameters:
	*  - vData: input and output data of vector class for deserialized result
	*  - strJson: input json string to be deserialized
	*  - eJsonClass: class of data
	* Return: add vector element with deserilized data
	*/
	void DataModel::DeserializeReadData(
		std::vector<std::pair<std::string, std::string>> &vData,
		const std::string &strJson,
		JsonDataClass eJsonClass
	)
	{
		ArduinoJson::DynamicJsonBuffer objJsonBuffer(DataModel::SignalElementSize);
		// Fuck: StaticJsonBuffer can not use here when I debug it, and I do not know why.

		ArduinoJson::JsonObject &objRoot = objJsonBuffer.parseObject(strJson);
		// ID
		vData.push_back(std::make_pair(DataModel::KeyCommon::ID, objRoot[DataModel::KeyCommon::ID].as<std::string>()));
		// SrcID
		AddDeserializedData(vData, objRoot, DataModel::KeyCommon::SrcID);
		// Timestamp
		AddDeserializedData(vData, objRoot, DataModel::KeyCommon::TimeStamp);
		// StoragePath
		AddDeserializedData(vData, objRoot, DataModel::KeyCommon::StoragePath);
		// CreateUser
		vData.push_back(std::make_pair("create_user", objRoot["create"]["user"].as<std::string>()));
		// CreateTime
		vData.push_back(std::make_pair("create_time", objRoot["create"]["time"].as<std::string>()));
		// ModifyUser
		vData.push_back(std::make_pair("modify_user", objRoot["modify"]["user"].as<std::string>()));
		// ModifyTime
		vData.push_back(std::make_pair("modify_time", objRoot["modify"]["time"].as<std::string>()));
		switch (eJsonClass)
		{
		case RIFMClient::DataModel::SignalElement:
			AddDeserializedData(vData, objRoot, DataModel::KeySignalElement::Antenna);
			AddDeserializedData(vData, objRoot, DataModel::KeySignalElement::Demodulator);
			AddDeserializedData(vData, objRoot, DataModel::KeySignalElement::FrameType);
			AddDeserializedData(vData, objRoot, DataModel::KeySignalElement::Frequency);
			AddDeserializedData(vData, objRoot, DataModel::KeySignalElement::ModulateRate);
			AddDeserializedData(vData, objRoot, DataModel::KeySignalElement::ModulateType);
			AddDeserializedData(vData, objRoot, DataModel::KeySignalElement::Polarity);
			AddDeserializedData(vData, objRoot, DataModel::KeySignalElement::Satellite);
			AddDeserializedData(vData, objRoot, DataModel::KeySignalElement::SourceType);
			break;
		case RIFMClient::DataModel::ControlFrame:
			AddDeserializedData(vData, objRoot, DataModel::KeyFrameControl::Type);
			break;
		case RIFMClient::DataModel::Ip:
			AddDeserializedData(vData, objRoot, DataModel::KeyIp::EmbeddedSrcID);
			AddDeserializedData(vData, objRoot, DataModel::KeyIp::Encrypted);
			AddDeserializedData(vData, objRoot, DataModel::KeyIp::Protocol);
			AddDeserializedData(vData, objRoot, DataModel::KeyIp::SrcIp);
			AddDeserializedData(vData, objRoot, DataModel::KeyIp::SrcPort);
			AddDeserializedData(vData, objRoot, DataModel::KeyIp::DstIp);
			AddDeserializedData(vData, objRoot, DataModel::KeyIp::DstPort);
			break;
		case RIFMClient::DataModel::Http:
			AddDeserializedData(vData, objRoot, DataModel::KeyHttp::Title);
			AddDeserializedData(vData, objRoot, DataModel::KeyHttp::Type);
			break;
		case RIFMClient::DataModel::Email:
			AddDeserializedData(vData, objRoot, DataModel::KeyEmail::Title);
			AddDeserializedData(vData, objRoot, DataModel::KeyEmail::From);
			AddDeserializedData(vData, objRoot, DataModel::KeyEmail::To);
			AddDeserializedData(vData, objRoot, DataModel::KeyEmail::AttachmentType);
			break;
		default:
			break;
		}
	}

	/////////////////
	// DataOperate //
	/////////////////

	DataOperate::DataOperate()
	{
	}


	DataOperate::~DataOperate()
	{
	}

	/*
	* Summary: get last error message
	* Parameters: NULL
	* Return: error message
	*/
	std::wstring DataOperate::GetErrorMessage()
	{
		return m_wstrErrorMessage;
	}

	/*
	* Summary: create one document in mongodb using http
	* Parameters:
	*  - strUrl: url for mongodb restful interface
	*  - strData: json data which is to be created
	*  - tagResult: response of http request
	* Return: TRUE if create success
	*/
	BOOL DataOperate::CreateOne(
		IN CONST std::string &strUrl,
		IN CONST std::string &strData,
		IN OUT WinHttpClient::RESPONSE &tagResult
		)
	{
		// send request using rifm restful interface
		if (m_objHttpClient.Create(strUrl, strData, tagResult) == TRUE)
		if (tagResult.Code == 201)
			return TRUE;
		m_wstrErrorMessage = m_objHttpClient.GetErrorMessage();
		return FALSE;
	}

	/*
	* Summary: read one document in mongodb using http
	* Parameters:
	*  - strUrl: url for mongodb restful interface
	*  - tagResult: response of http request
	* Return: TRUE if read success
	*/
	BOOL DataOperate::ReadOne(
		IN CONST std::string &strUrl,
		IN OUT WinHttpClient::RESPONSE &tagResult
		)
	{
		// send request using rifm restful interface
		if (m_objHttpClient.Read(strUrl, tagResult) == TRUE)
		if (tagResult.Code == 200)
			return TRUE;
		m_wstrErrorMessage = m_objHttpClient.GetErrorMessage();
		return FALSE;
	}

	/*
	* Summary: update one document in mongodb using http
	* Parameters:
	*  - strUrl: url for mongodb restful interface
	*  - strData: json data which is to be updated
	*  - tagResult: response of http request
	* Return: TRUE if update success
	*/
	BOOL DataOperate::UpdateOne(
		IN CONST std::string &strUrl,
		IN CONST std::string &strData,
		IN OUT WinHttpClient::RESPONSE &tagResult
		)
	{
		if (m_objHttpClient.Update(strUrl, strData, tagResult) == TRUE)
		if (tagResult.Code == 200)
			return TRUE;
		m_wstrErrorMessage = m_objHttpClient.GetErrorMessage();
		return FALSE;
	}

	/*
	* Summary: delete one document in mongodb using http
	* Parameters:
	*  - strUrl: url for mongodb restful interface
	*  - tagResult: response of http request
	* Return: TRUE if delete success
	*/
	BOOL DataOperate::DeleteOne(
		IN CONST std::string &strUrl,
		IN OUT WinHttpClient::RESPONSE &tagResult
		)
	{
		if (m_objHttpClient.Delete(strUrl, tagResult) == TRUE)
		if (tagResult.Code == 200)
			return TRUE;
		m_wstrErrorMessage = m_objHttpClient.GetErrorMessage();
		return FALSE;
	}

	////////////
	// Client //
	////////////

	Client::Client()
	{

	}

	Client::~Client()
	{

	}

	VOID Client::SetResult(WinHttpClient::RESPONSE &tagResponse)
	{
		m_dwRestultCode = tagResponse.Code;
		m_strResultBody = tagResponse.Body;
	}

	DWORD Client::GetResultCode()
	{
		return m_dwRestultCode;
	}

	std::string Client::GetResultBody()
	{
		return m_strResultBody;
	}

	VOID Client::SetErrorMessage()
	{
		m_strError = CommonString::UnicodeToANSI(m_objDataOperator.GetErrorMessage());
	}

	std::string Client::GetErrorMessage()
	{
		return m_strError;
	}

	/*
	* Summary: create one document from client
	* Parameters:
	*  - strUrl: url for mongodb restful interface
	*  - mapData: map data which is to be created
	* Return: TRUE if creating action is success
	*/
	BOOL Client::Create(IN std::string &strUrl, IN std::map<std::string, std::string> &mapData)
	{
		// serialize json string
		std::string strJson;
		DataModel::SerializeCreateData(mapData, strJson);
		// send request
		WinHttpClient::RESPONSE tagResult;
		ZeroMemory(&tagResult, sizeof(tagResult));
		BOOL bReturn = m_objDataOperator.CreateOne(strUrl, strJson, tagResult);
		SetResult(tagResult);
		if (bReturn == FALSE)
			SetErrorMessage();
		return bReturn;
	}

	/*
	* Summary: create one document from client
	* Parameters:
	*  - strUrl: url for mongodb restful interface
	*  - vData: vector data which is to be created
	* Return: TRUE if creating action is success
	*/
	BOOL Client::Create(IN std::string &strUrl, IN std::vector<std::pair<std::string, std::string>> &vData)
	{
		// serialize json string
		std::string strJson;
		DataModel::SerializeCreateData(vData, strJson);
		// send request
		WinHttpClient::RESPONSE tagResult;
		ZeroMemory(&tagResult, sizeof(tagResult));
		BOOL bReturn = m_objDataOperator.CreateOne(strUrl, strJson, tagResult);
		SetResult(tagResult);
		if (bReturn == FALSE)
			SetErrorMessage();
		return bReturn;
	}

	/*
	* Summary: read one document from client
	* Parameters:
	*  - strUrl: url for mongodb restful interface
	* Return: TRUE if reading action is success
	*/
	BOOL Client::Read(IN std::string &strUrl)
	{
		WinHttpClient::RESPONSE tagResult;
		ZeroMemory(&tagResult, sizeof(tagResult));
		BOOL bReturn = m_objDataOperator.ReadOne(strUrl, tagResult);
		SetResult(tagResult);
		if (bReturn == FALSE)
			SetErrorMessage();
		return bReturn;
	}

	/*
	* Summary: update one document from client
	* Parameters:
	*  - strUrl: url for mongodb restful interface
	*  - mapData: map data which is to be created
	* Return: TRUE if creating action is success
	*/
	BOOL Client::Update(IN std::string &strUrl, IN std::map<std::string, std::string> &mapData)
	{
		// serialize json string
		std::string strJson;
		DataModel::SerializeUpdateData(mapData, strJson);
		// send request
		WinHttpClient::RESPONSE tagResult;
		ZeroMemory(&tagResult, sizeof(tagResult));
		BOOL bReturn = m_objDataOperator.UpdateOne(strUrl, strJson, tagResult);
		SetResult(tagResult);
		if (bReturn == FALSE)
			SetErrorMessage();
		return bReturn;
	}

	/*
	* Summary: update one document from client
	* Parameters:
	*  - strUrl: url for mongodb restful interface
	*  - vData: vector data which is to be created
	* Return: TRUE if creating action is success
	*/
	BOOL Client::Update(IN std::string &strUrl, IN std::vector<std::pair<std::string, std::string>> &vData)
	{
		// serialize json string
		std::string strJson;
		DataModel::SerializeUpdateData(vData, strJson);
		// send request
		WinHttpClient::RESPONSE tagResult;
		ZeroMemory(&tagResult, sizeof(tagResult));
		BOOL bReturn = m_objDataOperator.UpdateOne(strUrl, strJson, tagResult);
		SetResult(tagResult);
		if (bReturn == FALSE)
			SetErrorMessage();
		return bReturn;
	}

	/*
	* Summary: delete one document from client
	* Parameters:
	*  - strUrl: url for mongodb restful interface
	* Return: TRUE if creating action is success
	*/
	BOOL Client::Delete(IN std::string &strUrl)
	{
		// send request
		WinHttpClient::RESPONSE tagResult;
		ZeroMemory(&tagResult, sizeof(tagResult));
		BOOL bReturn = m_objDataOperator.DeleteOne(strUrl, tagResult);
		SetResult(tagResult);
		if (bReturn == FALSE)
			SetErrorMessage();
		return bReturn;
	}

	/*
	* Summary: get struct struct data
	* Parameters:
	*  - strRawData: http response body
	*  - eJsonClass: document type in mongodb collection
	*  - mapData: output map struct data
	* Return: NULL
	*/
	VOID Client::GetStructDataFromJson(
		IN std::string &strRawData,
		IN DataModel::JsonDataClass eJsonClass,
		IN OUT std::map<std::string, std::string> &mapData
		)
	{
		DataModel::DeserializeReadData(
			mapData,
			strRawData,
			DataModel::JsonDataClass::SignalElement);
	}

	/*
	* Summary: get struct struct data
	* Parameters:
	*  - strRawData: http response body
	*  - eJsonClass: document type in mongodb collection
	*  - vData: output vector struct data
	* Return: NULL
	*/
	VOID Client::GetStructDataFromJson(
		IN std::string &strRawData,
		IN DataModel::JsonDataClass eJsonClass,
		IN OUT std::vector<std::pair<std::string, std::string>> &vData
	)
	{
		DataModel::DeserializeReadData(
			vData,
			strRawData,
			DataModel::JsonDataClass::SignalElement);
	}
}
