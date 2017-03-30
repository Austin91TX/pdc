// PLTSDKConnectDemo.cpp : Defines the entry point for the console application.
//

#include <afxcmn.h>
#include <iostream>
using namespace std;

// Spokes 3.0 COM SDK is distributed as tlb file.
// C++ user can use #import directive that will create all proper C++ types, wrappers and interfaces for communicating with running Spokes3G.exe COM server
#import "Plantronics.tlb" no_namespace, named_guids, raw_interfaces_only

// We need this as project is created as Win console app, and we are instantiating ATL COM objects that are used as Event sink's
CComModule _Module;
extern __declspec(selectany) CAtlModule* _pAtlModule = &_Module;

// The one and only application object
CWinApp theApp;

int main()
{
	int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(nullptr);

	if (hModule != nullptr)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			wprintf(L"Fatal Error: MFC initialization failed\n");
			nRetCode = 1;
		}
		else
		{
			// TODO: code your application's behavior here.
			// Initialize COM
			::CoInitializeEx(NULL, COINIT_MULTITHREADED);
			{
				// Create Spokes session manager object
				ICOMSessionManagerPtr sessionManager;
				if (SUCCEEDED(sessionManager.CreateInstance(CLSID_COMSessionManager)))
				{
					wprintf(L"NOTE: Succeeded to create session manager\n");
					std::cout << "Press a key to quit" << endl;
					std::cin.ignore();

					sessionManager.Release();
				}
			}
			// uninitialize COM
			::CoUninitialize();
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		wprintf(L"Fatal Error: GetModuleHandle failed\n");
		nRetCode = 1;
	}

	return nRetCode;
}