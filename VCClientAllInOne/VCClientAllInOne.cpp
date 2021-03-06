// VCClientAllInOne.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "RIFMClient.h"
#include <iostream>

const std::string gc_strDemoCollectionUrl = "http://192.168.122.1:27080/dev/signal_element";
const std::string gc_strDemoDocumentId = "demo_id";

void demo_create()
{
	RIFMClient::Client clt;
	// demo for create one document
	//
	std::cout << "\n1. Create one document in mongodb." << std::endl;
	system("pause");
	// prepair create url
	std::string strCreateUrl = gc_strDemoCollectionUrl;
	// set create data
	std::cout << "> set create data value..." << std::endl;
	// Note: 
	// 1. You may not set the create_time.
	// 2. Don't set the modify_user and modify_time	at create function here.

	//// using map for create information.
	//std::map<std::string, std::string> mapData;
	//mapData.insert(std::pair<std::string, std::string>
	//	(RIFMClient::DataModel::KeyCommon::ID, gc_strDemoDocumentId));
	//mapData.insert(std::pair<std::string, std::string>
	//	(RIFMClient::DataModel::KeyCommon::SrcID, "")); // no source data for signalelement
	//mapData.insert(std::pair<std::string, std::string>
	//	(RIFMClient::DataModel::KeyCommon::TimeStamp, "time_stamp_value"));
	//mapData.insert(std::pair<std::string, std::string>
	//	(RIFMClient::DataModel::KeyCommon::StoragePath, "storeage_full_path"));
	//mapData.insert(std::pair<std::string, std::string>
	//	(RIFMClient::DataModel::KeyCommon::User, "demo"));
	//mapData.insert(std::pair<std::string, std::string>
	//	(RIFMClient::DataModel::KeySignalElement::Satellite, "demo_satellite_value"));
	//mapData.insert(std::pair<std::string, std::string>
	//	(RIFMClient::DataModel::KeySignalElement::Antenna, "demo_antenna_value"));
	//mapData.insert(std::pair<std::string, std::string>
	//	(RIFMClient::DataModel::KeySignalElement::Polarity, "demo_polarity_value"));
	//mapData.insert(std::pair<std::string, std::string>
	//	(RIFMClient::DataModel::KeySignalElement::Frequency, "demo_frequency_value"));
	//mapData.insert(std::pair<std::string, std::string>
	//	(RIFMClient::DataModel::KeySignalElement::ModulateRate, "demo_modulate_rate"));
	//mapData.insert(std::pair<std::string, std::string>
	//	(RIFMClient::DataModel::KeySignalElement::ModulateType, "demo_modulate_type"));
	//mapData.insert(std::pair<std::string, std::string>
	//	(RIFMClient::DataModel::KeySignalElement::Demodulator, "demo_demodulator_value"));
	//mapData.insert(std::pair<std::string, std::string>
	//	(RIFMClient::DataModel::KeySignalElement::FrameType, "demo_frame_value"));
	//mapData.insert(std::pair<std::string, std::string>
	//	(RIFMClient::DataModel::KeySignalElement::SourceType, "demo_source_type"));

	// using vector for create information
	std::vector<std::pair<std::string, std::string>> vData;
	vData.push_back(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeyCommon::ID, gc_strDemoDocumentId));
	vData.push_back(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeyCommon::SrcID, "")); // no source data for signalelement
	vData.push_back(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeyCommon::TimeStamp, "time_stamp_value"));
	vData.push_back(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeyCommon::StoragePath, "storeage_full_path"));
	vData.push_back(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeyCommon::User, "demo"));
	vData.push_back(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeySignalElement::Satellite, "demo_satellite_value"));
	vData.push_back(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeySignalElement::Antenna, "demo_antenna_value"));
	vData.push_back(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeySignalElement::Polarity, "demo_polarity_value"));
	vData.push_back(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeySignalElement::Frequency, "demo_frequency_value"));
	vData.push_back(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeySignalElement::ModulateRate, "demo_modulate_rate"));
	vData.push_back(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeySignalElement::ModulateType, "demo_modulate_type"));
	vData.push_back(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeySignalElement::Demodulator, "demo_demodulator_value"));
	vData.push_back(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeySignalElement::FrameType, "demo_frame_value"));
	vData.push_back(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeySignalElement::SourceType, "demo_source_type"));

	//// send request to rifm using map
	//BOOL bResult = clt.Create(strCreateUrl, mapData);
	
	// send request to rifm using vector
	BOOL bResult = clt.Create(strCreateUrl, vData);
	
	std::cout << "> http response:" << std::endl;
	std::cout << "code: " << clt.GetResultCode() << std::endl;
	std::cout << "body: " << std::endl;
	std::cout << clt.GetResultBody() << std::endl;
	if (bResult == FALSE)
	{
		std::cout << "> create failed." << std::endl;
		std::cout << "> error message:" << std::endl;
		std::cout << "\t " << clt.GetErrorMessage() << std::endl;
	}
	else
	{
		std::cout << "> create success." << std::endl;
	}
}

void demo_read()
{
	RIFMClient::Client clt;
	// demo for read one document
	//
	std::cout << "\n2. Read one document in mongodb." << std::endl;
	system("pause");
	// prepair read url
	std::string strReadUrl = gc_strDemoCollectionUrl + "/" + gc_strDemoDocumentId;
	// send request to rifm
	std::cout << "> send read request to rifm..." << std::endl;
	BOOL bResult = clt.Read(strReadUrl);
	std::cout << "> http response:" << std::endl;
	std::cout << "code: " << clt.GetResultCode() << std::endl;
	std::cout << "body: " << std::endl;
	std::cout << clt.GetResultBody() << std::endl;
	if (bResult == FALSE)
	{
		std::cout << "> read failed." << std::endl;
		std::cout << "> error message:" << std::endl;
		std::cout << clt.GetErrorMessage() << std::endl;
	}
	else
	{
		std::cout << "> read success!" << std::endl;
		std::cout << "> deserialize data from body..." << std::endl;
		
		std::string strBody = clt.GetResultBody();
		// prepair read buffer
		//
		//// using map
		//std::map<std::string, std::string> mapResult;
		//clt.GetStructDataFromJson(
		//	strBody,
		//	RIFMClient::DataModel::JsonDataClass::SignalElement,
		//	mapResult
		//);
		// using vector
		std::vector<std::pair<std::string, std::string>> vResult;
		clt.GetStructDataFromJson(
			strBody,
			RIFMClient::DataModel::JsonDataClass::SignalElement,
			vResult
		);
		std::cout << "> deserialize finished." << std::endl;
		std::cout << "> check deserial data:" << std::endl;
		
		//// using map
		//if (mapResult.size() != 0)
		//	for (auto kv : mapResult)
		//		std::cout << kv.first << " : " << kv.second << std::endl;
		// using vector
		if (vResult.size() != 0)
			for (auto iter = vResult.begin(); iter != vResult.end(); iter++)
				std::cout << iter->first << " : " << iter->second << std::endl;

		std::cout << ">> read finished!" << std::endl;
	}
}

void demo_update()
{
	RIFMClient::Client clt;
	// demo for read one document
	//
	std::cout << "\n3. Update one document in mongodb." << std::endl;
	system("pause");
	// prepair update url
	std::string strUpdateUrl = gc_strDemoCollectionUrl + "/" + gc_strDemoDocumentId;
	// set update data
	std::cout << "> set update data value..." << std::endl;
	std::map<std::string, std::string> mapData;
	// Note: You can update anyone you want except 'modify_time'
	mapData.insert(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeyCommon::StoragePath, "update_storeage_path"));
	mapData.insert(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeyCommon::User, "update_user"));
	mapData.insert(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeySignalElement::ModulateRate, "update_modulate_rate"));
	mapData.insert(std::pair<std::string, std::string>
		(RIFMClient::DataModel::KeySignalElement::ModulateType, "update_modulate_type"));
	// send request to rifm
	std::cout << "> send update request to rifm..." << std::endl;
	BOOL bResult = clt.Update(strUpdateUrl, mapData);
	std::cout << "> http response:" << std::endl;
	std::cout << "code: " << clt.GetResultCode() << std::endl;
	std::cout << "body: " << std::endl;
	std::cout << clt.GetResultBody() << std::endl;
	if (bResult == FALSE)
	{
		std::cout << "> update failed." << std::endl;
		std::cout << "> error message:" << std::endl;
		std::cout << clt.GetErrorMessage() << std::endl;
	}
	else
	{
		std::cout << "> update success." << std::endl;
	}
}

void demo_delete()
{
	RIFMClient::Client clt;
	// demo for read one document
	//
	std::cout << "\n4. Delete one document in mongodb." << std::endl;
	system("pause");
	// prepair delete url
	std::string strDeleteUrl = gc_strDemoCollectionUrl + "/" + gc_strDemoDocumentId;
	// send request to rifm
	BOOL bResult = clt.Delete(strDeleteUrl);
	std::cout << "> http response:" << std::endl;
	std::cout << "code: " << clt.GetResultCode() << std::endl;
	std::cout << "body: " << std::endl;
	std::cout << clt.GetResultBody() << std::endl;
	if (bResult == FALSE)
	{
		std::cout << "> delete failed." << std::endl;
		std::cout << "> error message:" << std::endl;
		std::cout << clt.GetErrorMessage() << std::endl;
	}
	else
	{
		std::cout << "> delete success." << std::endl;
	}
}

int main()
{
	std::cout << "###############################" << std::endl;
	std::cout << "# rifm client demo on windows #" << std::endl;
	std::cout << "###############################" << std::endl;
	std::cout << "* DataBase: 'dev'" << std::endl;
	std::cout << "* Collection: 'signal_element'" << std::endl;
	demo_create();
	demo_read();
	demo_update();
	demo_delete();
	system("pause");
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
