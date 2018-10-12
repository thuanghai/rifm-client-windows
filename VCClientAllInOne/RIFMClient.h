#pragma once
#include "../ExternalLib/ArduinoJson.hpp"
#include <string>
#include <map>
#include <sstream>
#include <vector>
#include <codecvt>
#include <Windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp")

//
// Common Class Definition
//
class CommonString
{
public:
	CommonString();
	~CommonString();

	static std::wstring ANSIToUnicode(const std::string &str);
	static std::string UnicodeToANSI(const std::wstring &str);

	static std::wstring UTF8ToUnicode(const std::string &str);
	static std::string UnicodeToUTF8(const std::wstring &str);
};



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
		} RESPONSE;

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
		VOID Recv(IN OUT RESPONSE &tagResult);
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
			IN OUT RESPONSE &tagResult
			);
		BOOL Read(IN std::string strUrl, IN OUT RESPONSE &tagResult);
		BOOL Update(
			IN std::string strUrl,
			IN std::string strData,
			IN OUT RESPONSE &tagResult
			);
		BOOL Delete(IN std::string strUrl, IN OUT RESPONSE &tagResult);

		// Get error message of interact with rifm
		std::wstring GetErrorMessage();
	};

	class DataModel
	{
	public:
		DataModel();
		~DataModel();

		// Key Name For Common
		struct KeyCommon
		{
			static const std::string ID;
			static const std::string SrcID;
			static const std::string TimeStamp;
			static const std::string StoragePath;
			static const std::string User; // creator or modifyer
			//static const std::string CreateUser;
			//static const std::string CreateTime;
			//static const std::string ModifyUser;
			//static const std::string ModifyTime;
		};
		static const int CommonSize;

		// Key Name For SignalElement
		struct KeySignalElement
		{
			static const std::string Satellite;
			static const std::string Antenna;
			static const std::string Polarity;
			static const std::string Frequency;
			static const std::string ModulateType;
			static const std::string ModulateRate;
			static const std::string Demodulator;
			static const std::string FrameType;
			static const std::string SourceType;
		};
		static const int SignalElementSize;

		// Key Name For Frame
		struct KeyFrameControl
		{
			static const std::string Type;
		};
		static const int FrameControlSize;

		// Key Name For IP Package
		struct KeyIp
		{
			static const std::string EmbeddedSrcID;
			static const std::string Encrypted;
			static const std::string Protocol;
			static const std::string SrcIp;
			static const std::string DstIp;
			static const std::string SrcPort;
			static const std::string DstPort;
		};
		static const int IpSize;

		// Key Name For Email
		struct KeyEmail
		{
			static const std::string Title;
			static const std::string From;
			static const std::string To;
			static const std::string AttachmentType;
		};
		static const int EmailSize;

		// Key Name For HTTP
		struct KeyHttp
		{
			static const std::string Title;
			static const std::string Type;
		};
		static const int HttpSize;

		typedef struct _CommonData
		{
			std::string Id;
			std::string SrcID;
			std::string TimeStamp;
			std::string StoragePath;
			std::string CreateUser;
			std::string CreateTime;
			std::string ModifyUser;
			std::string ModifyTime;
		} CommonData;

		typedef struct _SignalElementData
		{
			CommonData CommonDetail;
			std::string Satellite;
			std::string Antenna;
			std::string Polarity;
			std::string Frequency;
			std::string ModulateType;
			std::string ModulateRate;
			std::string Demodulator;
			std::string FrameType;
			std::string SourceType;
		} SignalElementData;

		typedef struct _ControlFrameData
		{
			CommonData CommonDetail;
			std::string Type;
		} ControlFrameData;

		typedef struct _IpData
		{
			CommonData CommonDetail;
			std::string EmbeddedSrcID;
			std::string Encrypted;
			std::string Protocol;
			std::string SrcIp;
			std::string DstIp;
			std::string SrcPort;
			std::string DstPort;
		} IpData;

		typedef struct _EmailData
		{
			CommonData CommonDetail;
			std::string Title;
			std::string From;
			std::string To;
			std::string AttachmentType;
		} EmailData;

		typedef struct _HttpData
		{
			CommonData CommonDetail;
			std::string Title;
			std::string Type;
		} HttpData;

		enum JsonDataClass
		{
			SignalElement,
			ControlFrame,
			Ip,
			Http,
			Email
		};

		static void SerializeCreateData(
			std::map<std::string, std::string> &mapData,
			std::string &strJson
			);
		static void SerializeUpdateData(
			std::map<std::string, std::string> &mapData,
			std::string &strJson
			);
	private:
		static void AddDeserializedData(
			std::map<std::string, std::string> &mapData,
			ArduinoJson::JsonObject &objRoot,
			std::string id
			);
	public:
		static void DeserializeReadData(
			std::map<std::string, std::string> &mapData,
			const std::string &strJson,
			JsonDataClass eJsonClass);
	};

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
			IN CONST std::string &strUrl,
			IN CONST std::string &data,
			IN OUT WinHttpClient::RESPONSE &tagResult
			);
		BOOL ReadOne(
			IN CONST std::string &strUrl,
			IN OUT WinHttpClient::RESPONSE &tagResult
			);
		BOOL UpdateOne(
			IN CONST std::string &strUrl,
			IN CONST std::string &data,
			IN OUT WinHttpClient::RESPONSE &tagResult
			);
		BOOL DeleteOne(
			IN CONST std::string &strUrl,
			IN OUT WinHttpClient::RESPONSE &tagResult
			);
	};

	class Client
	{
	private:
		DataOperate m_objDataOperator;
		
		DWORD m_dwRestultCode;
		std::string m_strResultBody;
		VOID SetResult(WinHttpClient::RESPONSE &tagResponse);
		std::string m_strError;
		VOID SetErrorMessage();
	public:
		Client();
		~Client();
		// interface for create one document
		BOOL Create(
			IN std::string &strUrl,
			IN std::map<std::string, std::string> &mapData
			);
		BOOL Read(IN std::string &strUrl);
		BOOL Update(
			IN std::string &strUrl,
			IN std::map<std::string, std::string> &mapData
			);
		BOOL Delete(IN std::string &strUrl);

		DWORD GetResultCode();
		std::string GetResultBody();
		std::string GetErrorMessage();
		VOID GetStructDataFromJson(
			IN std::string &strJson,
			IN DataModel::JsonDataClass eJsonClass,
			IN OUT std::map<std::string, std::string> &mapData
			);
	};
}

