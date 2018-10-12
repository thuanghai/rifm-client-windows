#include "pch.h"
#include "DataModel.h"

namespace RIFMClient
{
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
	const std::string DataModel::KeyCommon::CreateUser = "create_user";
	const std::string DataModel::KeyCommon::CreateTime = "create_time";
	const std::string DataModel::KeyCommon::ModifyUser = "modify_user";
	const std::string DataModel::KeyCommon::ModifyTime = "modify_time";
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
		const std::map<std::string, std::string> &mapData,
		std::string &strJson
	)
	{
		if (mapData.size() <= 0)
		{
			strJson.erase();
			return;
		}

		//StaticJsonBuffer<DataModel::SignalElementSize> objJsonBuffer;
		DynamicJsonBuffer objJsonBuffer(JSON_OBJECT_SIZE(mapData.size() + 1));
		// Note:
		// Using 'StaticJsonBuffer' may not get the serialized json string, and I do not know fuck why?
		
		JsonObject &objRoot = objJsonBuffer.createObject();
		for (auto kv : mapData)
			objRoot.set(kv.first, kv.second);
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
		const std::map<std::string, std::string> &mapData,
		std::string &strJson
	)
	{
		if (mapData.size() <= 0)
		{
			strJson.erase();
			return;
		}

		//StaticJsonBuffer<DataModel::SignalElementSize> objJsonBuffer;
		DynamicJsonBuffer objJsonBuffer(JSON_OBJECT_SIZE(mapData.size() + 1));

		JsonObject &objRoot = objJsonBuffer.createObject();
		JsonObject &objSet = objRoot.createNestedObject("$set");
		for (auto kv : mapData)
			objSet.set(kv.first, kv.second);
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
		JsonObject &objRoot,
		std::string id
	)
	{
		if (objRoot.containsKey(id))
			mapData.insert(std::make_pair(id, objRoot[id].as<std::string>()));
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
		DynamicJsonBuffer objJsonBuffer(DataModel::SignalElementSize);
		// Fuck: StaticJsonBuffer can not use here when I debug it, and I do not know why.
		
		JsonObject &objRoot = objJsonBuffer.parseObject(strJson);
		// ID
		mapData.insert(std::make_pair(DataModel::KeyCommon::ID, objRoot[DataModel::KeyCommon::ID].as<std::string>()));
		// SrcID
		AddDeserializedData(mapData, objRoot, DataModel::KeyCommon::SrcID);
		// Timestamp
		AddDeserializedData(mapData, objRoot, DataModel::KeyCommon::TimeStamp);
		// StoragePath
		AddDeserializedData(mapData, objRoot, DataModel::KeyCommon::StoragePath);
		// CreateUser
		AddDeserializedData(mapData, objRoot, DataModel::KeyCommon::CreateUser);
		// CreateTime
		AddDeserializedData(mapData, objRoot, DataModel::KeyCommon::CreateTime);
		// ModifyUser
		AddDeserializedData(mapData, objRoot, DataModel::KeyCommon::ModifyUser);
		// ModifyTime
		AddDeserializedData(mapData, objRoot, DataModel::KeyCommon::ModifyTime);
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
}