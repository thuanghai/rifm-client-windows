#pragma once
#include "DataModel.h"
#include "DataOperate.h"
#include <iostream>
#include <algorithm>

class SignalElementDemo
{
public:
	SignalElementDemo();
	~SignalElementDemo();
private:
	//RIFMClient::DataModel m_objDataModel;
	RIFMClient::DataOperate m_objDataOperate;

	std::string m_strDemoUrl;
	std::string m_strDemoId;

	VOID CreateOne();
	VOID ReadOne();
	VOID UpdateOne();
	VOID DeleteOne();
public:
	VOID Run();
};

