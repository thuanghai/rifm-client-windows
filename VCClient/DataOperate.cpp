#include "pch.h"
#include "DataOperate.h"

namespace RIFMClient
{
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
		IN std::string strUrl, 
		IN std::string strData,
		IN OUT WinHttpClient::RESULT &tagResult
	)
	{
		// check strData
		if (strData.empty())
			return FALSE;
		// send request using rifm restful interface
		if (m_objHttpClient.Create(strUrl, strData.c_str(), tagResult) == TRUE)
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
		IN std::string strUrl, 
		IN OUT WinHttpClient::RESULT &tagResult
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
		IN std::string strUrl, 
		IN std::string strData,
		IN OUT WinHttpClient::RESULT &tagResult
	)
	{
		// check update data
		if (strData.empty())
			return FALSE;
		if (m_objHttpClient.Update(strUrl, strData.c_str(), tagResult) == TRUE)
			if (tagResult.Code == 201)
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
		IN std::string strUrl,
		IN OUT WinHttpClient::RESULT &tagResult
	)
	{
		if (m_objHttpClient.Delete(strUrl, tagResult) == TRUE)
			if (tagResult.Code == 200)
				return TRUE;
		m_wstrErrorMessage = m_objHttpClient.GetErrorMessage();
		return FALSE;
	}
}
