#include "pch.h"
#include "SignalElementDemo.h"


SignalElementDemo::SignalElementDemo()
{
}


SignalElementDemo::~SignalElementDemo()
{
}

VOID SignalElementDemo::CreateOne()
{
	std::cout << "1. Create one document in mongodb." << std::endl;

	std::cout << "> set url path:" << std::endl;
	std::string strUrl = m_strDemoUrl;
	std::cout << "\t" << strUrl << std::endl;

	// set create data
	std::cout << "> set create data value..." << std::endl;
	std::map<std::string, std::string> mapData;
	// Note: 
	// 1. You may not set the create_time.
	// 2. Don't set the modify_user and modify_time
	mapData.insert(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeyCommon::ID , m_strDemoId));
	mapData.insert(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeyCommon::SrcID, "")); // no source data for signalelement
	mapData.insert(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeyCommon::TimeStamp, "time_stamp_value"));
	mapData.insert(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeyCommon::StoragePath, "storeage_full_path"));
	mapData.insert(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeyCommon::CreateUser, "demo"));
	mapData.insert(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeySignalElement::Satellite, "demo_satellite_value"));
	mapData.insert(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeySignalElement::Antenna, "demo_antenna_value"));
	mapData.insert(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeySignalElement::Polarity, "demo_polarity_value"));
	mapData.insert(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeySignalElement::Frequency, "demo_frequency_value"));
	mapData.insert(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeySignalElement::ModulateRate, "demo_modulate_rate"));
	mapData.insert(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeySignalElement::ModulateType, "demo_modulate_type"));
	mapData.insert(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeySignalElement::Demodulator, "demo_demodulator_value"));
	mapData.insert(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeySignalElement::FrameType, "demo_frame_value"));
	mapData.insert(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeySignalElement::SourceType, "demo_source_type"));

	std::cout << "> json serialize create data value..." << std::endl;
	std::string strJson;
	RIFMClient::DataModel::SerializeCreateData(mapData, strJson);

	std::cout << "> send request to rifm for creating..." << std::endl;
	RIFMClient::WinHttpClient::RESULT tagResult;
	ZeroMemory(&tagResult, sizeof(tagResult));
	if (m_objDataOperate.CreateOne(strUrl, strJson, tagResult) == FALSE)
	{
		std::cout << "> create failed." << std::endl;
		std::cout << "> error message:" << std::endl;
		std::wcout << L"\t " << m_objDataOperate.GetErrorMessage() << std::endl;
	}
	else
	{
		std::cout << "> create success." << std::endl;
	}
}

VOID SignalElementDemo::ReadOne()
{
	std::cout << "2. Read one document in mongodb." << std::endl;

	std::cout << "> set url path:" << std::endl;
	std::string strUrl = m_strDemoUrl + "/" + m_strDemoId;
	std::cout << "\t" << strUrl << std::endl;

	std::cout << "> send request to rifm for creating..." << std::endl;
	RIFMClient::WinHttpClient::RESULT tagResult;
	ZeroMemory(&tagResult, sizeof(tagResult));
	if (m_objDataOperate.ReadOne(strUrl, tagResult) == FALSE)
	{
		std::cout << "> read failed." << std::endl;
		std::cout << "> error message:" << std::endl;
		std::wcout << L"\t " << m_objDataOperate.GetErrorMessage() << std::endl;
	}
	else
	{
		std::cout << "> read success." << std::endl;
		std::cout << "code: " << tagResult.Code << std::endl;
		std::cout << "body: \n" << tagResult.Body << std::endl;
		std::cout << "> deserialize data from body..." << std::endl;
		std::map<std::string, std::string> mapData;
		RIFMClient::DataModel::DeserializeReadData(
			mapData, 
			tagResult.Body,
			RIFMClient::DataModel::JsonDataClass::SignalElement);
		if (mapData.size() != 0)
			for (auto kv : mapData)
				std::cout << kv.first << " : " << kv.second << std::endl;
		std::cout << "read finished!" << std::endl;
	}
}

VOID SignalElementDemo::UpdateOne()
{
	std::cout << "3. Update one document in mongodb." << std::endl;

	std::cout << "> set url path:" << std::endl;
	std::string strUrl = m_strDemoUrl + "/" + m_strDemoId;
	std::cout << "\t" << strUrl << std::endl;

	// set create data
	std::cout << "> set update data value..." << std::endl;
	std::map<std::string, std::string> mapData;
	// Note: You can update anyone you want except 'modify_time'
	mapData.insert(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeyCommon::StoragePath, "update_storeage_path"));
	//mapData.insert(std::pair<std::string, std::string>
	//	(RIFMClient::DataModel::KeyCommon::ModifyUser, "update_user"));
	mapData.insert(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeySignalElement::ModulateRate, "update_modulate_rate"));
	mapData.insert(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeySignalElement::ModulateType, "update_modulate_type"));
	

	std::cout << "> json serialize create data value..." << std::endl;
	std::string strJson;
	RIFMClient::DataModel::SerializeUpdateData(mapData, strJson);

	std::cout << "> send request to rifm for updating..." << std::endl;
	RIFMClient::WinHttpClient::RESULT tagResult;
	ZeroMemory(&tagResult, sizeof(tagResult));
	if (m_objDataOperate.UpdateOne(strUrl, strJson, tagResult) == TRUE)
	{
		std::cout << "> update failed." << std::endl;
		std::cout << "> error message:" << std::endl;
		std::wcout << L"\t " << m_objDataOperate.GetErrorMessage() << std::endl;
	}
	else
	{
		std::cout << "> update success." << std::endl;
	}
}

VOID SignalElementDemo::DeleteOne()
{
	std::cout << "4. Delete one document in mongodb." << std::endl;

	std::cout << "> set url path:" << std::endl;
	std::string strUrl = m_strDemoUrl + "/" + m_strDemoId;
	std::cout << "\t" << strUrl << std::endl;

	std::cout << "> send request to rifm for deleting..." << std::endl;
	RIFMClient::WinHttpClient::RESULT tagResult;
	ZeroMemory(&tagResult, sizeof(tagResult));
	if (m_objDataOperate.DeleteOne(strUrl, tagResult) == FALSE)
	{
		std::cout << "> delete failed." << std::endl;
		std::cout << "> error message:" << std::endl;
		std::wcout << L"\t " << m_objDataOperate.GetErrorMessage() << std::endl;
	}
	else
	{
		std::cout << "> delete success." << std::endl;
	}
}

VOID SignalElementDemo::Run()
{
	std::cout << "Demo operation for 'SignalElement' database..." << std::endl;
	// set demo url
	m_strDemoUrl = "http://192.168.122.1:27080/dev/signal_element";
	// set demo id
	m_strDemoId = "demo_id";
	// create one document
	CreateOne();
	// read one document
	ReadOne();
	// update one document
	UpdateOne();
	// delete one document
	DeleteOne();
}
