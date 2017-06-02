// PLTCPlusPlusSample.cpp : C++ COM sample demonstrating basic call control.
// NOTE: Spokes 3.0 COM SDK is distributed as tlb file. 
// C++ user can use #import directive (see below) that will create all proper C++ types, wrappers and interfaces for 
// communicating with running PLTHub.exe COM server

#include <stdio.h>
#include <tchar.h>
#include <atlbase.h>
#include <atlstr.h>
#include <fstream>
#include <istream>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include "atlcom.h"
#include <SDKDDKVer.h>

#import "Plantronics.tlb" no_namespace, named_guids, raw_interfaces_only

#ifdef _MSC_VER

#if _MSC_VER < 1900
// Version of the compiler before 19 (VS2015) did not properly handle this
#define __func__ __FUNCTION__
#define noexcept
#endif

#endif

#define _ATL_DEBUG_QI
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

using namespace std;
using namespace ATL;

// We need this as project is created as Win console app, and we are instantiating ATL COM objects that are used as Event sink's
CComModule _Module;
extern __declspec(selectany) CAtlModule* _pAtlModule=&_Module;

#define PRINT_ERROR(str, res) std::cout << str << "Error Code: " << std::hex << res << std::endl

ICOMSessionManagerPtr sessionManager;
ICOMSessionPtr session;

// Helper functions to print meaningful string representations of enums
std::string EnumToString(CallState val)
{
	std::string ret;
	switch (val)
	{
	case CallState_Unknown: ret = "CallState_Unknown"; break;
	case CallState_AcceptCall: ret = "CallState_AcceptCall"; break;
	case CallState_TerminateCall: ret = "CallState_TerminateCall"; break;
	case CallState_HoldCall: ret = "CallState_HoldCall"; break;
	case CallState_Resumecall: ret = "CallState_Resumecall"; break;
	case CallState_Flash: ret = "CallState_Flash"; break;
	case CallState_CallInProgress: ret = "CallState_CallInProgress"; break;
	case CallState_CallRinging: ret = "CallState_CallRinging"; break;
	case CallState_CallEnded: ret = "CallState_CallEnded"; break;
	case CallState_TransferToHeadSet: ret = "CallState_TransferToHeadSet"; break;
	case CallState_TransferToSpeaker: ret = "CallState_TransferToSpeaker"; break;
	case CallState_MuteON: ret = "CallState_MuteON"; break;
	case CallState_MuteOFF: ret = "CallState_MuteOFF"; break;
	case CallState_MobileCallRinging: ret = "CallState_MobileCallRinging"; break;
	case CallState_MobileCallInProgress: ret = "CallState_MobileCallInProgress"; break;
	case CallState_MobileCallEnded: ret = "CallState_MobileCallEnded"; break;
	case CallState_Don: ret = "CallState_Don"; break;
	case CallState_Doff: ret = "CallState_Doff"; break;
	case CallState_CallIdle: ret = "CallState_CallIdle"; break;
	case CallState_Play: ret = "CallState_Play"; break;
	case CallState_Pause: ret = "CallState_Pause"; break;
	case CallState_Stop: ret = "CallState_Stop"; break;
	case CallState_DTMFKey: ret = "CallState_DTMFKey"; break;
	case CallState_RejectCall: ret = "CallState_RejectCall"; break;
	}
	return ret;
}
std::string EnumToString(DeviceHeadsetStateChange val)
{
	std::string ret;
	switch (val)
	{
	case HeadsetStateChange_Unknown: ret = "HeadsetStateChange_Unknown"; break;
	case HeadsetStateChange_MonoON: ret = "HeadsetStateChange_MonoON"; break;
	case HeadsetStateChange_MonoOFF: ret = "HeadsetStateChange_MonoOFF"; break;
	case HeadsetStateChange_StereoON: ret = "HeadsetStateChange_StereoON"; break;
	case HeadsetStateChange_StereoOFF: ret = "HeadsetStateChange_StereoOFF"; break;
	case HeadsetStateChange_MuteON: ret = "HeadsetStateChange_MuteON"; break;
	case HeadsetStateChange_MuteOFF: ret = "HeadsetStateChange_MuteOFF"; break;
	case HeadsetStateChange_BatteryLevel: ret = "HeadsetStateChange_BatteryLevel"; break;
	case HeadsetStateChange_InRange: ret = "HeadsetStateChange_InRange"; break;
	case HeadsetStateChange_OutofRange: ret = "HeadsetStateChange_OutofRange"; break;
	case HeadsetStateChange_Docked: ret = "HeadsetStateChange_Docked"; break;
	case HeadsetStateChange_UnDocked: ret = "HeadsetStateChange_UnDocked"; break;
	case HeadsetStateChange_InConference: ret = "HeadsetStateChange_InConference"; break;
	case HeadsetStateChange_Don: ret = "HeadsetStateChange_Don"; break;
	case HeadsetStateChange_Doff: ret = "HeadsetStateChange_Doff"; break;
	case HeadsetStateChange_SerialNumber: ret = "HeadsetStateChange_SerialNumber"; break;
	case HeadsetStateChange_Near: ret = "HeadsetStateChange_Near"; break;
	case HeadsetStateChange_Far: ret = "HeadsetStateChange_Far"; break;
	case HeadsetStateChange_DockedCharging: ret = "HeadsetStateChange_DockedCharging"; break;
	case HeadsetStateChange_ProximityUnknown: ret = "HeadsetStateChange_ProximityUnknown"; break;
	case HeadsetStateChange_ProximityEnabled: ret = "HeadsetStateChange_ProximityEnabled"; break;
	case HeadsetStateChange_ProximityDisabled: ret = "HeadsetStateChange_ProximityDisabled"; break;
	}
	return ret;
}
std::string EnumToString(DeviceChangeState val)
{
	std::string ret;
	switch (val)
	{
	case DeviceState_Unknown: ret = "DeviceState_Unknown"; break;
	case DeviceState_Added: ret = "DeviceState_Added"; break;
	case DeviceState_Removed: ret = "DeviceState_Removed"; break;
	}
	return ret;
}
std::string EnumToString(COMDeviceEventType val)
{
	std::string ret;
	switch (val)
	{
	case DeviceEventType_Unknown: ret = "DeviceEventType_Unknown"; break;
	case DeviceEventType_HeadsetButtonPressed: ret = "DeviceEventType_HeadsetButtonPressed"; break;
	case DeviceEventType_HeadsetStateChanged: ret = "DeviceEventType_HeadsetStateChanged"; break;
	case DeviceEventType_BaseButtonPressed: ret = "DeviceEventType_BaseButtonPressed"; break;
	case DeviceEventType_BaseStateChanged: ret = "DeviceEventType_BaseStateChanged"; break;
	case DeviceEventType_ATDButtonPressed: ret = "DeviceEventType_ATDButtonPressed"; break;
	case DeviceEventType_ATDStateChanged: ret = "DeviceEventType_ATDStateChanged"; break;
	}
	return ret;
}

// CallObject
class ATL_NO_VTABLE CallObject :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<ICOMCall, &__uuidof(ICOMCall), &LIBID_Spokes3GCOMServerLib, /*wMajor =*/ 3, /*wMinor =*/ 0 >
{
public:

	CallObject() { }
	
	CallObject(long id) { put_Id(id); }

	static CComObject<CallObject>* GetCallObject()
	{
		CComObject<CallObject>* pCall;
		CComObject<CallObject>::CreateInstance(&pCall);
		pCall->AddRef(); // this object is created with ref count 0;
		std::string strCallId;
		std::cout << "Enter Call Id: ";
		std::getline(std::cin, strCallId);

		long id;
		std::stringstream myStream(strCallId);
		myStream >> id;

		pCall->put_Id(id);
		return pCall;
	}

	// Constructor that takes the call id as a parameter
	static CComObject<CallObject>* GetCallObject(long id)
	{
		CComObject<CallObject>* pCall;
		CComObject<CallObject>::CreateInstance(&pCall);
		pCall->AddRef(); // this object is created with ref count 0;
		pCall->put_Id(id);
		return pCall;
	}
	
	BEGIN_COM_MAP(CallObject)
		COM_INTERFACE_ENTRY2(IDispatch, ICOMCall)
		COM_INTERFACE_ENTRY(ICOMCall)
	END_COM_MAP()
	
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct() { return S_OK;	}

	void FinalRelease()	{ }

public:

private:
	long m_id;

// ICOMCall Methods
public:
	STDMETHOD(get_Id)(long * pVal)
	{
		*pVal = m_id;
		return S_OK;
	}
	STDMETHOD(put_Id)(long pVal)
	{
		m_id = pVal;
		return S_OK;
	}
	STDMETHOD(get_ConferenceId)(long * pVal) { return E_NOTIMPL; }
	STDMETHOD(put_ConferenceId)(long pVal) { return E_NOTIMPL; }
	STDMETHOD(get_InConference)(VARIANT_BOOL * pVal) { return E_NOTIMPL; }
	STDMETHOD(put_InConference)(VARIANT_BOOL pVal) { return E_NOTIMPL;}
};

// CallContact
class ATL_NO_VTABLE CallContact :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<ICOMContact, &__uuidof(ICOMContact), &LIBID_Spokes3GCOMServerLib, /*wMajor =*/ 3, /*wMinor =*/ 0 >
{
private:
	_bstr_t m_name;
	_bstr_t m_friendName;
	LONG m_id;
	_bstr_t m_sipUri;
	_bstr_t m_phone;
	_bstr_t m_email;
	_bstr_t m_workPhone;
	_bstr_t m_mobilePhone;
	_bstr_t m_homePhone;
public:
	CallContact()
	{
		static LONG id = 0;
		m_id = ++id;
	}

	~CallContact() { }

	static CComObject<CallContact>* GetContactObject()
	{
		CComObject<CallContact>* pContact;
		CComObject<CallContact>::CreateInstance(&pContact);
		pContact->AddRef(); // this object is created with ref count 0;
		std::string name;
		std::cout << "Enter Contact Name: ";
		std::getline(std::cin, name);
		pContact->put_Name(_bstr_t(name.c_str()));
		return pContact;
	}

	// Added a function override taking contact name as a parameter
	static CComObject<CallContact>* GetContactObject(std::string name)
	{
		CComObject<CallContact>* pContact;
		CComObject<CallContact>::CreateInstance(&pContact);
		pContact->AddRef(); // this object is created with ref count 0;
		pContact->put_Name(_bstr_t(name.c_str()));
		return pContact;
	}

	BEGIN_COM_MAP(CallContact)
		COM_INTERFACE_ENTRY2(IDispatch, ICOMContact)
		COM_INTERFACE_ENTRY(ICOMContact)
	END_COM_MAP()
	
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct() { return S_OK;	}

	void FinalRelease()	{ }

public:
// ICOMContact Methods
public:
	STDMETHOD(get_Name)(BSTR * pVal)
	{
		*pVal = SysAllocString(m_name);
		return S_OK;
	}
	STDMETHOD(put_Name)(BSTR pVal)
	{
		m_name = pVal;
		return S_OK;
	}
	STDMETHOD(get_FriendlyName)(BSTR * pVal)
	{
		*pVal = SysAllocString(m_friendName);
		return S_OK;
	}
	STDMETHOD(put_FriendlyName)(BSTR pVal)
	{
		m_friendName = pVal;
		return S_OK;
	}
	STDMETHOD(get_Id)(LONG * pVal)
	{
		*pVal = m_id;
		return S_OK;
	}
	STDMETHOD(put_Id)(LONG pVal)
	{
		m_id = pVal;
		return S_OK;
	}
	STDMETHOD(get_SipUri)(BSTR * pVal)
	{
		*pVal = SysAllocString(m_sipUri);
		return S_OK;
	}
	STDMETHOD(put_SipUri)(BSTR pVal)
	{
		m_sipUri = pVal;
		return S_OK;
	}
	STDMETHOD(get_Phone)(BSTR * pVal)
	{
		*pVal = SysAllocString(m_phone);
		return S_OK;
	}
	STDMETHOD(put_Phone)(BSTR pVal)
	{
		m_phone = pVal;
		return S_OK;
	}
	STDMETHOD(get_Email)(BSTR * pVal)
	{
		*pVal = SysAllocString(m_email);
		return S_OK;
	}
	STDMETHOD(put_Email)(BSTR pVal)
	{
		m_email = pVal;
		return S_OK;
	}
	STDMETHOD(get_WorkPhone)(BSTR * pVal)
	{
		*pVal = SysAllocString(m_workPhone);
		return S_OK;
	}
	STDMETHOD(put_WorkPhone)(BSTR pVal)
	{
		m_workPhone = pVal;
		return S_OK;
	}
	STDMETHOD(get_MobilePhone)(BSTR * pVal)
	{
		*pVal = SysAllocString(m_mobilePhone);
		return S_OK;
	}
	STDMETHOD(put_MobilePhone)(BSTR pVal)
	{
		m_mobilePhone = pVal;
		return S_OK;
	}
	STDMETHOD(get_HomePhone)(BSTR * pVal)
	{
		*pVal = SysAllocString(m_homePhone);
		return S_OK;
	}
	STDMETHOD(put_HomePhone)(BSTR pVal)
	{
		m_homePhone = pVal;
		return S_OK;
	}
};

_ATL_FUNC_INFO OnDeviceEvents = { CC_STDCALL, VT_EMPTY, 1,{ VT_DISPATCH } };

// DeviceListenerEventSink() : COM class : Sink for DeviceListener Events
class DeviceListenerEventSink :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<ICOMDeviceListenerEvents, &__uuidof(ICOMDeviceListenerEvents), &LIBID_Spokes3GCOMServerLib, /*wMajor =*/ 3, /*wMinor =*/ 0>
{
public:

	BEGIN_COM_MAP(DeviceListenerEventSink)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_IID(__uuidof(ICOMDeviceListenerEvents), IDispatch)
	END_COM_MAP()

	// ICOMDeviceListenerEvents Methods
	STDMETHOD(onHeadsetStateChanged)(struct ICOMDeviceListenerEventArgs *args);
	STDMETHOD(onHeadsetButtonPressed)(struct ICOMDeviceListenerEventArgs *args);
	STDMETHOD(onBaseButtonPressed)(struct ICOMDeviceListenerEventArgs *args);
	STDMETHOD(onBaseStateChanged)(struct ICOMDeviceListenerEventArgs *args);
	STDMETHOD(onATDStateChanged)(struct ICOMDeviceListenerEventArgs *args);
	
	DECLARE_PROTECT_FINAL_CONSTRUCT()
	HRESULT FinalConstruct() { return S_OK;	}
};

bool gGotDevice = false;

// DeviceListenerSink() : user defined class : wrapper to Advise/Unadvise on Device Listener events
class DeviceListenerSink
{
public:
	DeviceListenerSink(ICOMSessionPtr session)
	{
        if (session != nullptr)
		{
			ICOMDevicePtr device;
			if (SUCCEEDED(session->GetActiveDevice(&device)) && device != nullptr)
			{
				if (SUCCEEDED(device->get_DeviceListener(&m_deviceListener)) && m_deviceListener != nullptr)
				{
					if (SUCCEEDED(CComObject<DeviceListenerEventSink>::CreateInstance(&m_sink)) && m_sink != nullptr)
					{
						m_sink->AddRef();
						HRESULT hr = AtlAdvise(m_deviceListener, m_sink, __uuidof(ICOMDeviceListenerEvents), &m_cookie);
						if (FAILED(hr)) PRINT_ERROR("Advise Device Listener event Error: ", hr);
                        else
                        {
                            gGotDevice = true;
                        }
					}
				}
			}
		}
	}
	~DeviceListenerSink()
	{
        if (m_deviceListener != nullptr)
        {
            HRESULT hr = AtlUnadvise(m_deviceListener, __uuidof(ICOMDeviceListenerEvents), m_cookie);
            if (FAILED(hr)) PRINT_ERROR("Unadvise Device Listener event Error: ", hr);
        }
        if (m_sink != nullptr)
        {
            m_sink->Release();
        }
        gGotDevice = false;
	}
private:
	CComObject<DeviceListenerEventSink> *m_sink = nullptr;
	ICOMDeviceListenerPtr m_deviceListener = nullptr;
	DWORD m_cookie;
};

// SessionEventSink() : COM class : Sink for Session Call Events
class SessionEventSink :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<ICOMCallEvents, &__uuidof(ICOMCallEvents), &LIBID_Spokes3GCOMServerLib, /*wMajor =*/ 3, /*wMinor =*/ 0 >
{
public:

	BEGIN_COM_MAP(SessionEventSink)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_IID(__uuidof(ICOMCallEvents), IDispatch)
	END_COM_MAP()

	// ICOMCallEvents Methods
	STDMETHOD(onCallRequested)(struct ICOMCallRequestEventArgs *args);
	STDMETHOD(onCallStateChanged)(struct ICOMCallEventArgs *args);
	
	DECLARE_PROTECT_FINAL_CONSTRUCT()
	HRESULT FinalConstruct() { return S_OK;	}
};

// SessionManagerEventSink() : COM class : Sink for Session Manager events
class SessionManagerEventSink :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<ICOMSessionManagerEvents, &__uuidof(ICOMSessionManagerEvents), &LIBID_Spokes3GCOMServerLib, /*wMajor =*/ 3, /*wMinor =*/ 0 >
{
public:

	BEGIN_COM_MAP(SessionManagerEventSink)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_IID(__uuidof(ICOMSessionManagerEvents), IDispatch)
	END_COM_MAP()

	// ICOMSessionManagerEvents Methods
	STDMETHOD(onDeviceStateChanged)(struct ICOMStateDeviceEventArgs *args);
	STDMETHOD(onCallStateChanged)(struct ICOMCallEventArgs *args);
	
	DECLARE_PROTECT_FINAL_CONSTRUCT()
	HRESULT FinalConstruct() { return S_OK;	}
};

// SessionSink() : user defined class : wrapper to Advise/Unadvise on Session Call events
class SessionSink
{
public:
	SessionSink(ICOMSessionPtr session)
	{
		if (session != nullptr)
		{
			m_session = session;
			if (SUCCEEDED(CComObject<SessionEventSink>::CreateInstance(&m_sink)) && m_sink != nullptr)
			{
				m_sink->AddRef();
				HRESULT hr = AtlAdvise(m_session, m_sink, __uuidof(ICOMCallEvents), &m_cookie);
				if (FAILED(hr)) PRINT_ERROR("Advise Session Call event Error", hr);
			}
		}
	}
	~SessionSink()
	{
		if (m_session != nullptr)
        {
            HRESULT hr = AtlUnadvise(m_session, __uuidof(ICOMCallEvents), m_cookie);
            if (FAILED(hr)) PRINT_ERROR("Unadvise Session Call event Error", hr);
        }
        if (m_sink != nullptr)
        {
            m_sink->Release();
        }
	}
private:
	CComObject<SessionEventSink> *m_sink = nullptr;
	ICOMSessionPtr m_session = nullptr;
	DWORD m_cookie;
};

// SessionManagerSink() : user defined class : wrapper to Advise/Unadvise on Session Manager events
class SessionManagerSink
{
public:
	SessionManagerSink(ICOMSessionManagerPtr sessionManager)
	{
		m_sessionManager = sessionManager;
		CComObject<SessionManagerEventSink>::CreateInstance(&m_sink);
		if (m_sessionManager != nullptr && m_sink != nullptr)
		{
			m_sink->AddRef();
			HRESULT hr = AtlAdvise(m_sessionManager, m_sink, __uuidof(ICOMSessionManagerEvents), &m_cookie);
			if (FAILED(hr)) PRINT_ERROR("Advise Session Manager events Error", hr);
		}
	}
	~SessionManagerSink()
	{
		if (m_sessionManager != nullptr)
        {
            HRESULT hr = AtlUnadvise(m_sessionManager, __uuidof(ICOMSessionManagerEvents), m_cookie);
            if (FAILED(hr)) PRINT_ERROR("Unadvise Session Manager events Error", hr);
        }
        if (m_sink != nullptr)
        {
            m_sink->Release();
        }
	}
private:
	CComObject<SessionManagerEventSink> *m_sink = nullptr;
	ICOMSessionManagerPtr m_sessionManager = nullptr;
	DWORD m_cookie;
};

// DeviceSink() : user defined class : wrapper to Advise/Unadvise on Device events
class DeviceSink
{
public:
	DeviceSink(ICOMSessionPtr session)
	{
		if (session != nullptr)
		{
			if (SUCCEEDED(session->GetActiveDevice(&m_device)) && m_device != nullptr)
			{
				// hook to device listener event
				m_devListSink = new DeviceListenerSink(session);
			}
		}
	}
	~DeviceSink()
	{
		delete m_devListSink;
	}
private:
	DeviceListenerSink* m_devListSink = nullptr;
	ICOMDevicePtr m_device = nullptr;
};

STDMETHODIMP SessionEventSink::onCallRequested(struct ICOMCallRequestEventArgs *requestArgs)
{
	std::string contactName;
	ICOMContact* callContact = nullptr;
	requestArgs->get_contact(&callContact);
	if (callContact != nullptr)
	{
		BSTR name;
		callContact->get_Name(&name);
		contactName = (name != nullptr ? _bstr_t(name) : "none");
	}
	std::cout << std::endl << "(Session) SessionEventSink onCallRequested contact: " << contactName;
	return S_OK;
}
STDMETHODIMP SessionEventSink::onCallStateChanged(struct ICOMCallEventArgs *callEventArgs)
{
	CallState callState;
	callEventArgs->get_CallState(&callState);

	long callId(0);
	ICOMCall* call = nullptr;
	callEventArgs->get_call(&call);
	call->get_Id(&callId);

	std::cout << "Call State Changed: callid=" << callId << " new state=" << EnumToString(callState) << std::endl;
	return S_OK;
}

STDMETHODIMP SessionManagerEventSink::onDeviceStateChanged(struct ICOMStateDeviceEventArgs *args)
{
	DeviceChangeState devState;
	args->get_DeviceState(&devState);
	std::cout << "onDeviceStateChanged: " << EnumToString(devState) << std::endl;

    gGotDevice = false; // schedule a device re-attach attempt next time around the main menu loop

    if (devState == DeviceState_Added)
    {
        std::cout << "Press Enter to attach this device." << std::endl;
    }

	return S_OK;
}
STDMETHODIMP SessionManagerEventSink::onCallStateChanged(struct ICOMCallEventArgs *callEventArgs) { return S_OK; }

STDMETHODIMP DeviceListenerEventSink::onHeadsetStateChanged(struct ICOMDeviceListenerEventArgs *args)
{
	DeviceHeadsetStateChange state;
	COMDeviceEventType evType;
	args->get_HeadsetStateChange(&state);
	args->get_DeviceEventType(&evType);

	if (evType == COMDeviceEventType::DeviceEventType_HeadsetStateChanged)
	{
		std::cout << EnumToString(evType) << ", state = " << EnumToString(state) << std::endl;
	}


	return S_OK;
}
STDMETHODIMP DeviceListenerEventSink::onHeadsetButtonPressed(struct ICOMDeviceListenerEventArgs *args)
{
	DeviceHeadsetButton btn;
	COMDeviceEventType evType;
	args->get_HeadsetButton(&btn);
	args->get_DeviceEventType(&evType);

	if ((evType == COMDeviceEventType::DeviceEventType_HeadsetButtonPressed) && (btn == DeviceHeadsetButton::HeadsetButton_Mute))
	{
		ICOMDevicePtr device;
		ICOMDeviceListenerPtr deviceListener;
		VARIANT_BOOL valBool;
		session->GetActiveDevice(&device);
		device->get_DeviceListener(&deviceListener);
		deviceListener->get_mute(&valBool);
		std::cout << "DeviceEventType_HeadsetStateChanged - ";
		(valBool == VARIANT_TRUE) ? std::cout << EnumToString(DeviceHeadsetStateChange::HeadsetStateChange_MuteON) : std::cout << EnumToString(DeviceHeadsetStateChange::HeadsetStateChange_MuteOFF); std::cout << std::endl;
	}
	return S_OK;
}
STDMETHODIMP DeviceListenerEventSink::onBaseButtonPressed(struct ICOMDeviceListenerEventArgs *args) { return S_OK; }
STDMETHODIMP DeviceListenerEventSink::onBaseStateChanged(struct ICOMDeviceListenerEventArgs *args) { return S_OK; }
STDMETHODIMP DeviceListenerEventSink::onATDStateChanged(struct ICOMDeviceListenerEventArgs *args) { return S_OK; }

// Implementation of C++ COM sample
DeviceSink* gDeviceSink = nullptr;

//  helper function that:
//	- gets the handle to the currently active device, 
//	- gets a handle to deviceListner object
//	- prints its ProductName, ProductID & mute status of the device
void PrintDeviceDetails()
{
	HRESULT res = S_OK;
	BSTR productName;
	USHORT productId;
	VARIANT_BOOL valBool;

	ICOMDevicePtr myDevice;
	ICOMDeviceListenerPtr myDeviceListener;
	
	// get current active device from active session
	if (FAILED(res = session->GetActiveDevice(&myDevice)))
	{
		std::cout << "Unable to retrieve/hook to active device" << std::endl;
		return; 
	}

	// get device listener interface
	if (FAILED(res = myDevice->get_DeviceListener(&myDeviceListener)))
	{
		std::cout << "Device is not attached"; 
		return;
	}
	std::cout << "Successfully hooked to device listener events" << std::endl;

	// get & display device information
	myDevice->get_ProductName(&productName);
	myDevice->get_ProductId(&productId);
	_bstr_t bproductName(productName);
	std::cout << "Device attached: " << bproductName;
	std::wcout << L", Product ID = " << std::uppercase << std::hex << std::setfill(L'0') << std::setw(4) << productId << std::endl;
	
	// Obtain initial device microphone mute state
	res = myDeviceListener->get_mute(&valBool);
	std::cout << "Device mute state: muted = "; 
	(valBool == VARIANT_TRUE) ? std::cout << "True" : std::cout << "False"; std::cout << std::endl;
	
	return;
}

void ShowMenu()
{
	std::cout << std::endl;
	std::cout << "plt sample menu" << std::endl;
	std::cout << "--" << std::endl;
	std::cout << "1 - ring/incoming call" << std::endl;
	std::cout << "2 - outgoing call" << std::endl;
	std::cout << "3 - answer call" << std::endl;
	std::cout << "4 - hold call" << std::endl;
	std::cout << "5 - resume call" << std::endl;
	std::cout << "6 - mute call" << std::endl;
	std::cout << "7 - unmute call" << std::endl;
	std::cout << "8 - end call" << std::endl;
	std::cout << "0 - quit" << std::endl;
	std::cout << std::endl;
	std::cout << "> ";
}

void DoIncomingCall(int callid, string contactname)
{
	std::cout << "Performing IncomingCall, id = " << callid << std::endl;
	HRESULT res = S_OK;
	ICOMCallCommandPtr callCommand;

	if (FAILED(res = session->GetCallCommand(&callCommand)))
	{
		PRINT_ERROR("Error gettting interface from Call Command", res); return;
	}

	res = callCommand->IncomingCall(CallObject::GetCallObject(callid), CallContact::GetContactObject(contactname), RingTone_Unknown, AudioRoute_ToHeadset);
}

void DoOutgoingCall(int callid, string contactname)
{
	std::cout << "Performing OutgoingCall, id = " << callid << std::endl;
	HRESULT res = S_OK;
	ICOMCallCommandPtr callCommand;

	if (FAILED(res = session->GetCallCommand(&callCommand)))
	{
		PRINT_ERROR("Error gettting interface from Call Command", res); return;
	}

	res = callCommand->OutgoingCall(CallObject::GetCallObject(callid), CallContact::GetContactObject(contactname), AudioRoute_ToHeadset);
}

void DoAnswerCall(int callid)
{
	std::cout << "Performing AnswerCall, id = " << callid << std::endl;
	HRESULT res = S_OK;
	ICOMCallCommandPtr callCommand;

	if (FAILED(res = session->GetCallCommand(&callCommand)))
	{
		PRINT_ERROR("Error gettting interface from Call Command", res); return;
	}

	res = callCommand->AnsweredCall(CallObject::GetCallObject(callid));
}

void DoTerminateCall(int callid)
{
	std::cout << "Performing TerminateCall, id = " << callid << std::endl;
	HRESULT res = S_OK;
	ICOMCallCommandPtr callCommand;

	if (FAILED(res = session->GetCallCommand(&callCommand)))
	{
		PRINT_ERROR("Error gettting interface from Call Command", res); return;
	}

	res = callCommand->TerminateCall(CallObject::GetCallObject(callid));
}

void DoHoldCall(int callid)
{
	std::cout << "Performing HoldCall, id = " << callid << std::endl;
	HRESULT res = S_OK;
	ICOMCallCommandPtr callCommand;

	if (FAILED(res = session->GetCallCommand(&callCommand)))
	{
		PRINT_ERROR("Error gettting interface from Call Command", res); return;
	}

	res = callCommand->HoldCall(CallObject::GetCallObject(callid));
}

void DoResumeCall(int callid)
{
	std::cout << "Performing ResumeCall, id = " << callid << std::endl;
	HRESULT res = S_OK;
	ICOMCallCommandPtr callCommand;

	if (FAILED(res = session->GetCallCommand(&callCommand)))
	{
		PRINT_ERROR("Error gettting interface from Call Command", res); return;
	}

	res = callCommand->ResumeCall(CallObject::GetCallObject(callid));
}

void DoMuteCall(int callid, bool mute)
{
	std::cout << "Setting headset mute = ";
	(mute == true) ? std::cout << "True" : std::cout << "False"; std::cout << std::endl;
	
	HRESULT res = S_OK;
	ICOMCallCommandPtr callCommand;

	if (FAILED(res = session->GetCallCommand(&callCommand)))
	{
		PRINT_ERROR("Error gettting interface from Call Command", res); return;
	}

	(mute == true) ? res = callCommand->MuteCall(CallObject::GetCallObject(callid), VARIANT_TRUE) : res = callCommand->MuteCall(CallObject::GetCallObject(callid), VARIANT_FALSE);
}

// sample entry point

int _tmain(int argc, _TCHAR* argv[])
{
	bool quit = false;
	std::string input;
	int cmd = -1;
	int _callid = 0; // variable to track call id between my app and Plantronics

	// Initialize COM
	{
		::CoInitializeEx(NULL, COINIT_MULTITHREADED);

		std::cout << "C++ Plantronics COM API Sample" << std::endl;

		// Create Spokes session manager object
		if (SUCCEEDED(sessionManager.CreateInstance(CLSID_COMSessionManager)))
		{
			// register Session Manager for device attached/detached events
			SessionManagerSink sessionManagerSink(sessionManager);

			// Register new session in Spokes.
			if (SUCCEEDED(sessionManager->Register(_bstr_t("COM Plugin"), &session)))
			{
				// register Session for call state change events
				SessionSink sessionSink(session);

				ICOMUserPreference *userPref;
				sessionManager->get_UserPreference(&userPref);
				userPref->put_DefaultSoftphone(_bstr_t("COM Plugin"));

				while (!quit)
				{
                    // If we have not attached to device (or device was removed/added), then attempt to attach to a device
                    if (!gGotDevice)
                    {
                        if (gDeviceSink != nullptr)
                        {
                            delete gDeviceSink;
                        }
                        std::cout << "Attempting device attach" << std::endl;
                        // Instantiate Device Sink object and hook to DeviceListner events
                        gDeviceSink = new DeviceSink(session);

                        // call to AttachDevice()
                        PrintDeviceDetails();
                    }

					ShowMenu();
					std::getline(std::cin, input);

					// Convert from string to number safely.
					std::stringstream inputStringStream(input);
					if (!(inputStringStream >> cmd))
					{
						cmd = -1;
						// check if device is attached and prompt error if it is
						if (gGotDevice == true)
							std::cout << "Invalid input, please try again" << std::endl;
					}

					switch (cmd)
					{
						case 1:
							_callid++;
							DoIncomingCall(_callid, "Bob%20Smith"); // inform Plantronics my app has an incoming (ringing) call
							break;
						case 2:
							_callid++;
							DoOutgoingCall(_callid, "Bob%20Smith"); // inform Plantronics my app has an outgoing call
							break;
						case 3:
							DoAnswerCall(_callid); // inform Plantronics my app has now answered an incoming (ringing) call
							break;
						case 4:
							DoHoldCall(_callid); // place call on hold
							break;
						case 5:
							DoResumeCall(_callid); // resume the call
							break;
						case 6:
							DoMuteCall(_callid, true); // mute the headset (note for wireless products, audio link must be active)
							break;
						case 7:
							DoMuteCall(_callid, false); // unmute the headset (note for wireless products, audio link must be active)
							break;
						case 8:
							DoTerminateCall(_callid); // inform Plantronics my app has now terminated the call
							break;
						case 0:
							quit = true;
							break;
						default:
							if (gGotDevice == true)
								std::cout << "Unrecognised menu choice, please try again." << std::endl;
							break;
					}
					Sleep(250);
				}
				// cleanup unregister session
                if (gDeviceSink != nullptr)
                {
                    delete gDeviceSink;
                }
                sessionManager->Unregister(session);
			}
		}
	}

	// uninitialize COM
	::CoUninitialize();

	return 0;
}