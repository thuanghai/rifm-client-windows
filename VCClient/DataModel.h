#pragma once
#include "ArduinoJson.hpp"
#include <map>

using namespace ArduinoJson;

namespace RIFMClient
{
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
			static const std::string CreateUser;
			static const std::string CreateTime;
			static const std::string ModifyUser;
			static const std::string ModifyTime;
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

		enum JsonDataClass
		{
			SignalElement,
			ControlFrame,
			Ip,
			Http,
			Email
		};

		static void SerializeCreateData(
			const std::map<std::string, std::string> &mapData, 
			std::string &strJson
		);
		static void SerializeUpdateData(
			const std::map<std::string, std::string> &mapData, 
			std::string &strJson
		);
	private:
		static void AddDeserializedData(
			std::map<std::string, std::string> &mapData,
			JsonObject &objRoot,
			std::string id
		);
	public:	
		static void DeserializeReadData(
			std::map<std::string, std::string> &mapData, 
			const std::string &strJson,
			JsonDataClass eJsonClass);
	};
}

