#define _WIN32_DCOM
#include <iostream>
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>

#pragma comment(lib, "wbemuuid.lib")

#include "SysInfo.h"
using namespace omen;

SysInfo::SysInfo()
{
	getValues();
}

int SysInfo::getValues()
{
	HRESULT hres;

	// Step 1: --------------------------------------------------
	// Initialize COM. ------------------------------------------

	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres))
	{
		cout << "Failed to initialize COM library. Error code = 0x"
			<< hex << hres << endl;
		return 1;                  // Program has failed.
	}

	// Step 2: --------------------------------------------------
	// Set general COM security levels --------------------------

	hres = CoInitializeSecurity(
		NULL,
		-1,                          // COM authentication
		NULL,                        // Authentication services
		NULL,                        // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
		RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
		NULL,                        // Authentication info
		EOAC_NONE,                   // Additional capabilities 
		NULL                         // Reserved
	);


	if (FAILED(hres))
	{
		cout << "Failed to initialize security. Error code = 0x"
			<< hex << hres << endl;
		CoUninitialize();
		return 1;                    // Program has failed.
	}

	// Step 3: ---------------------------------------------------
	// Obtain the initial locator to WMI -------------------------

	IWbemLocator *pLoc = NULL;

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID *)&pLoc);

	if (FAILED(hres))
	{
		cout << "Failed to create IWbemLocator object."
			<< " Err code = 0x"
			<< hex << hres << endl;
		CoUninitialize();
		return 1;                 // Program has failed.
	}

	// Step 4: -----------------------------------------------------
	// Connect to WMI through the IWbemLocator::ConnectServer method

	IWbemServices *pSvc = NULL;

	// Connect to the root\cimv2 namespace with
	// the current user and obtain pointer pSvc
	// to make IWbemServices calls.
	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		NULL,                    // User name. NULL = current user
		NULL,                    // User password. NULL = current
		0,                       // Locale. NULL indicates current
		NULL,                    // Security flags.
		0,                       // Authority (for example, Kerberos)
		0,                       // Context object 
		&pSvc                    // pointer to IWbemServices proxy
	);

	if (FAILED(hres))
	{
		cout << "Could not connect. Error code = 0x"
			<< hex << hres << endl;
		pLoc->Release();
		CoUninitialize();
		return 1;                // Program has failed.
	}

	cout << "Connected to ROOT\\CIMV2 WMI namespace" << endl;


	// Step 5: --------------------------------------------------
	// Set security levels on the proxy -------------------------

	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
	);

	if (FAILED(hres))
	{
		cout << "Could not set proxy blanket. Error code = 0x"
			<< hex << hres << endl;
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return 1;               // Program has failed.
	}

	// Step 6: --------------------------------------------------
	// Use the IWbemServices pointer to make requests of WMI ----

	// For example, get the name of the operating system
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT * FROM Win32_Processor"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);

	if (FAILED(hres))
	{
		cout << "Query for operating system name failed."
			<< " Error code = 0x"
			<< hex << hres << endl;
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return 1;               // Program has failed.
	}

	// Step 7: -------------------------------------------------
	// Get the data from the query in step 6 -------------------

	IWbemClassObject *pclsObj = NULL;
	ULONG uReturn = 0;

	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

		if (0 == uReturn)
		{
			break;
		}

		VARIANT vtProp;

		// Get the value of the Name property
		hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
		wcout << " Processor Name : " << vtProp.bstrVal << endl;

		hr = pclsObj->Get(L"Family", 0, &vtProp, 0, 0);
		wcout << " Family: " << vtProp.uiVal << endl;

		if (vtProp.uiVal == 1) {
			hr = pclsObj->Get(L"OtherFamilyDescription", 0, &vtProp, 0, 0);
			wcout << " Family Description : " << vtProp.bstrVal << endl;
		}

		hr = pclsObj->Get(L"CurrentClockSpeed", 0, &vtProp, 0, 0);
		wcout << "CurrentClockSpee (MHz): " << vtProp.ulVal << endl;

		hr = pclsObj->Get(L"L2CacheSize", 0, &vtProp, 0, 0);
		wcout << "L2 Cache size (kb): " << vtProp.ulVal << endl;

		CIMTYPE t;
		long flavor;
		hr = pclsObj->Get(L"L2CacheSpeed", 0, &vtProp, &t, &flavor);
		wcout << "L2 Cache Speed (MHz): " << vtProp.ulVal << endl;

		hr = pclsObj->Get(L"L3CacheSize", 0, &vtProp, 0, 0);
		wcout << "L3 Cache size (kb): " << vtProp.ulVal << endl;

		hr = pclsObj->Get(L"L3CacheSpeed", 0, &vtProp, 0, 0);
		wcout << "L3 Cache Speed (MHz): " << vtProp.ulVal << endl;

		hr = pclsObj->Get(L"LoadPercentage", 0, &vtProp, 0, 0);
		wcout << "Load (%): " << vtProp.uiVal << endl;

		hr = pclsObj->Get(L"MaxClockSpeed", 0, &vtProp, 0, 0);
		wcout << "Max Clock Speed (MHz): " << vtProp.ulVal << endl;

		hr = pclsObj->Get(L"NumberOfCores", 0, &vtProp, 0, 0);
		wcout << "Number of cores (num): " << vtProp.ulVal << endl;

		hr = pclsObj->Get(L"NumberOfEnabledCores", 0, &vtProp, 0, 0);
		wcout << "Number of Enabled cores (num): " << vtProp.ulVal << endl;

		hr = pclsObj->Get(L"NumberOfLogicalProcessors", 0, &vtProp, 0, 0);
		wcout << "Number of Logical Processors (num): " << vtProp.ulVal << endl;

		hr = pclsObj->Get(L"ThreadCount", 0, &vtProp, 0, 0);
		wcout << "Number of Threads (num): " << vtProp.ulVal << endl;

		VariantClear(&vtProp);

		pclsObj->Release();
	}


	/// Video controller values
	pEnumerator = NULL;
	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t("SELECT * FROM Win32_VideoController"),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);

	if (FAILED(hres))
	{
		cout << "Query for operating system name failed."
			<< " Error code = 0x"
			<< hex << hres << endl;
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return 1;               // Program has failed.
	}

	// Step 7: -------------------------------------------------
	// Get the data from the query in step 6 -------------------

	pclsObj = NULL;
	uReturn = 0;

	while (pEnumerator)
	{
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

		if (0 == uReturn)
		{
			break;
		}

		VARIANT vtProp;

		// Get the value of the Name property
		hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0);
		wcout << " Video Controller Name : " << vtProp.bstrVal << endl;

		hr = pclsObj->Get(L"Description", 0, &vtProp, 0, 0);
		wcout << " Video Controller Description : " << vtProp.bstrVal << endl;

		hr = pclsObj->Get(L"DriverVersion", 0, &vtProp, 0, 0);
		wcout << " Video Controller DriverVersion : " << vtProp.bstrVal << endl;

		hr = pclsObj->Get(L"AdapterCompatibility", 0, &vtProp, 0, 0);
		wcout << " AdapterCompatibility: " << vtProp.bstrVal << endl;

		hr = pclsObj->Get(L"AdapterDACType", 0, &vtProp, 0, 0);
		wcout << "DACType: " << vtProp.bstrVal << endl;

		hr = pclsObj->Get(L"AdapterRAM", 0, &vtProp, 0, 0);
		wcout << "Adapter RAM (MegaBytes): " << static_cast<double>(vtProp.ulVal/1024/1024) << endl;

		hr = pclsObj->Get(L"Caption", 0, &vtProp, 0, 0);
		wcout << "Caption: " << vtProp.bstrVal << endl;

		hr = pclsObj->Get(L"CurrentBitsPerPixel", 0, &vtProp, 0, 0);
		wcout << "CurrentBitsPerPixel: " << vtProp.ulVal << endl;

		hr = pclsObj->Get(L"CurrentHorizontalResolution", 0, &vtProp, 0, 0);
		wcout << "CurrentHorizontalResolution: " << vtProp.ulVal << endl;

		hr = pclsObj->Get(L"CurrentVerticalResolution", 0, &vtProp, 0, 0);
		wcout << "CurrentVerticalResolution: " << vtProp.ulVal << endl;

		hr = pclsObj->Get(L"CurrentNumberOfColors", 0, &vtProp, 0, 0);
		wcout << "CurrentNumberOfColors: " << vtProp.ullVal << endl;

		VariantClear(&vtProp);

		pclsObj->Release();
	}

	// Cleanup
	// ========

	pSvc->Release();
	pLoc->Release();
	pEnumerator->Release();
	CoUninitialize();

	return 0;
}
