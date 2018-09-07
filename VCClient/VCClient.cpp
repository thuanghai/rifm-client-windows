// VCClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "WinHttpClient.h"

#include <iostream>
#include <string>

CONST std::wstring gc_wstrTestUrlPath = L"http://192.168.122.1:27080/dev/l2_ip_data";
CONST std::string gc_strTestDataID = "test_id";
CONST std::string gc_strTestDataKey1 = "test_key1";
CONST std::string gc_strTestDataVal1 = "test_val1";
CONST std::string gc_strTestUpdateVal1 = "update_val1";
CONST std::string gc_strTestDataKey2 = "test_key2";
CONST std::string gc_strTestDataVal2 = "test_val2";
CONST std::string gc_strTestUpdateVal2 = "update_val2";

std::wstring ANSIToUnicode(CONST std::string& str)
{
	int  len = 0;
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

std::string UnicodeToANSI(CONST std::wstring& str)
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
	memset((void*)pElementText, 0, sizeof(char) * (iTextLen + 1));
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

std::wstring UTF8ToUnicode(CONST std::string& str)
{
	int  len = 0;
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

std::string UnicodeToUTF8(CONST std::wstring& str)
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
	memset((void*)pElementText, 0, sizeof(char) * (iTextLen + 1));
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

void print_response_body(std::string data)
{
	printf("Response message:\n %s\n", data.c_str());
}

void test_create(RIFMClient::WinHttpClient objClient)
{
	// TODO: think whether use jsoncpp for constructing for json data
	std::string strCreateData =
		"{\"_id\":\"" + gc_strTestDataID + "\","
		+ "\"" + gc_strTestDataKey1 + "\":\"" + gc_strTestDataVal1 + "\","
		+ "\"" + gc_strTestDataKey2 + "\":\"" + gc_strTestDataVal2 + "\"}";
	if (objClient.Create(gc_wstrTestUrlPath, strCreateData.c_str()) == TRUE)
	{
		if (objClient.GetStatusCode() == 201)
			printf("Create success.");
		else
			printf("Create failed.");
		print_response_body(objClient.GetResponseBody());
	}
	else
	{
		std::wstring error = objClient.GetErrorMessage();
		printf("%s", error.c_str());
	}
	// Note: Do not forget clean handles!
	objClient.Close();
}
void test_read(RIFMClient::WinHttpClient objClient)
{
	std::wstring wstrReadDataID = ANSIToUnicode(gc_strTestDataID);
	std::wstring wstrReadUrlPath = gc_wstrTestUrlPath + L"/" + wstrReadDataID;
	if (objClient.Read(wstrReadUrlPath) == TRUE)
	{
		if (objClient.GetStatusCode() == 200)
			printf("Read success.");
		else
			printf("Read failed.");
		print_response_body(objClient.GetResponseBody());
	}
	else
	{
		std::wstring error = objClient.GetErrorMessage();
		printf("%s", error.c_str());
	}
	objClient.Close();
}

void test_update(RIFMClient::WinHttpClient objClient)
{
	std::wstring wstrUpdateDataID = ANSIToUnicode(gc_strTestDataID);
	std::wstring wstrUpdateUrlPath = gc_wstrTestUrlPath + L"/" + wstrUpdateDataID;
	std::string strUpdateData = "{\"$set\":{\"" + gc_strTestDataKey1 + "\":\"" + gc_strTestUpdateVal1 + "\","
		+ "\"" + gc_strTestDataKey2 + "\":\"" + gc_strTestUpdateVal2 + "\"}}";
	if (objClient.Update(wstrUpdateUrlPath, strUpdateData.c_str()) == TRUE)
	{
		if (objClient.GetStatusCode() == 201)
			printf("Update success.");
		else
			printf("Update failed.");
		print_response_body(objClient.GetResponseBody());
	}
	else
	{
		std::wstring error = objClient.GetErrorMessage();
		printf("%s", error.c_str());
	}
	// Note: Do not forget clean handles!
	objClient.Close();
}

void test_delete(RIFMClient::WinHttpClient objClient)
{
	std::wstring wstrDeleteDataID = ANSIToUnicode(gc_strTestDataID);
	std::wstring wstrDeleteUrlPath = gc_wstrTestUrlPath + L"/" + wstrDeleteDataID;
	if (objClient.Delete(wstrDeleteUrlPath) == TRUE)
	{
		if (objClient.GetStatusCode() == 200)
			printf("Delete success.");
		else
			printf("Delete failed.");
		print_response_body(objClient.GetResponseBody());
	}
	else
	{
		std::wstring error = objClient.GetErrorMessage();
		printf("%s", error.c_str());
	}
	objClient.Close();
}

int main()
{
	RIFMClient::WinHttpClient objClient;
	test_create(objClient);
	test_read(objClient);
	test_update(objClient);
	test_delete(objClient);
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
