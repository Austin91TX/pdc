// PLTCPlusPlusNativeSample.cpp : C++ Sample demonstrating basic call control. This implementation is linked to Plantronics native Spokes library
// Note: With Plantronics SDK installed, ensure the following project configuration: 
// 1. C/C++ -> General -> Additional Include direcories =>
//		C:/Program Files (x86)/Plantronics/Spokes3G SDK/Include;C:/Program Files/Plantronics/Spokes3G SDK/Include
// 2. Linker -> General -> Additional Library Directories =>
//		C:/Program Files (x86)/Plantronics/Spokes3G SDK;C:/Program Files/Plantronics/Spokes3G SDK
// 3. Linker -> Input -> Additional Dependancies =>
//		%(AdditionalDependencies);Spokes.lib
// **NOTE**: Native Library does not support Manager Pro, Multi-softphone or Multi-device configurations
// Please consider REST Service (PC and Mac) or COM Service (PC only) APIs
// For more information see: http://developer.plantronics.com/api-overview#second
#pragma once

#if (_MSC_VER >= 1915)
#define no_init_all deprecated
#endif

#include <windows.h>
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <regex>
#include <stdio.h>
#include <tchar.h>
#include <SDKDDKVer.h>
#include "Spokes3G.h"
#include "cpp\query_cast.h"

using namespace std;

// global variables
ISession* session = nullptr;
IDevice* activeDevice = nullptr;
bool gGotDevice = false;
	
std::string EnumToString(eCallState val)
{
	std::string str;
	switch (val)
	{

	case CALL_STATE_UNKNOWN: str = "CALL_STATE_UNKNOWN  "; break;
	case CALL_STATE_ACCEPT_CALL: str = "CALL_STATE_ACCEPT_CALL  "; break;
	case CALL_STATE_TERMINATE_CALL: str = "  CALL_STATE_TERMINATE_CALL"; break;
	case CALL_STATE_HOLD_CALL: str = "CALL_STATE_HOLD_CALL  "; break;
	case CALL_STATE_RESUME_CALL: str = " CALL_STATE_RESUME_CALL "; break;
	case CALL_STATE_FLASH: str = "CALL_STATE_FLASH  "; break;
	case CALL_STATE_CALL_IN_PROGRESS: str = "CALL_STATE_CALL_IN_PROGRESS  "; break;
	case CALL_STATE_CALL_RINGING: str = "CALL_STATE_CALL_RINGING  "; break;
	case CALL_STATE_CALL_ENDED: str = "CALL_STATE_CALL_ENDED  "; break;
	case CALL_STATE_TRANSFER_TO_HEADSET: str = " CALL_STATE_TRANSFER_TO_HEADSET "; break;
	case CALL_STATE_TRANSFER_TO_SPEAKER: str = "CALL_STATE_TRANSFER_TO_SPEAKER  "; break;
	case CALL_STATE_MUTEON: str = "CALL_STATE_MUTEON  "; break;
	case CALL_STATE_MUTEOFF: str = "CALL_STATE_MUTEOFF  "; break;
	case CALL_STATE_MOBILE_CALL_RINGING: str = " CALL_STATE_MOBILE_CALL_RINGING "; break;
	case CALL_STATE_MOBILE_CALL_IN_PROGRESS: str = "CALL_STATE_MOBILE_CALL_IN_PROGRESS  "; break;
	case CALL_STATE_MOBILE_CALL_ENDED: str = "  CALL_STATE_MOBILE_CALL_ENDED"; break;
	case CALL_STATE_DON: str = " CALL_STATE_DON "; break;
	case CALL_STATE_DOFF: str = "CALL_STATE_DOFF  "; break;
	case CALL_STATE_CALL_IDLE: str = "  CALL_STATE_CALL_IDLE"; break;
	case CALL_STATE_PLAY: str = "CALL_STATE_PLAY  "; break;
	case CALL_STATE_PAUSE: str = "  CALL_STATE_PAUSE"; break;
	case CALL_STATE_STOP: str = "CALL_STATE_STOP  "; break;
	case CALL_STATE_DTMF_KEY: str = " CALL_STATE_DTMF_KEY "; break;
	case CALL_STATE_REJECT_CALL: str = "CALL_STATE_REJECT_CALL  "; break;
	case CALL_STATE_MAKE_CALL: str = " CALL_STATE_MAKE_CALL "; break; // Information only.  
	case CALL_STATE_HOOK: str = "  CALL_STATE_HOOK"; break;
	case CALL_STATE_HOOK_IDLE: str = " CALL_STATE_HOOK_IDLE "; break;
	case CALL_STATE_HOOK_DOCKED: str = "  CALL_STATE_HOOK_DOCKED"; break;
	case CALL_STATE_HOOK_UNDOCKED: str = " CALL_STATE_HOOK_UNDOCKED"; break;
	case CALL_STATE_BASE_EVENT: str = "CALL_STATE_BASE_EVENT  "; break;
	case CALL_STATE_CALL_ANSWERED_AND_ENDED: str = "CALL_STATE_CALL_ANSWERED_AND_ENDED  "; break;
	case CALL_STATE_CALL_UNANSWERED_AND_ENDED: str = "CALL_STATE_CALL_UNANSWERED_AND_ENDED  "; break;
	case CALL_STATE_DEVICE_ARRIVED: str = "CALL_STATE_DEVICE_ARRIVED  "; break;
	case CALL_STATE_DEVICE_REMOVED: str = "CALL_STATE_DEVICE_REMOVED  "; break;
	case CALL_STATE_DEVICE_CHANGE: str = "CALL_STATE_DEVICE_CHANGE  "; break;
	default:return "CALL_STATE_UNKNOWN";
	};
	return str;
}

std::string EnumToString(eDeviceState val)
{
	std::string str;
	switch (val)
	{
	case DEV_STATE_ADDED: str = " DEV_STATE_ADDED"; break;

	case DEV_STATE_REMOVED: str = " DEV_STATE_REMOVED"; break;
	default: str = " DEV_STATE_UNKNOWN";
	}
	return str;
}

std::string EnumToString(eHeadsetStateChange val)
{
	std::string ret;
	switch (val)
	{
	case HS_STATE_CHANGE_UNKNOWN: ret = "HS_STATE_CHANGE_UNKNOWN"; break;
	case HS_STATE_CHANGE_MONO_ON: ret = "HS_STATE_CHANGE_MONO_ON"; break;
	case HS_STATE_CHANGE_MONO_OFF: ret = "HS_STATE_CHANGE_MONO_OFF"; break;
	case HS_STATE_CHANGE_STEREO_ON: ret = "HS_STATE_CHANGE_STEREO_ON"; break;
	case HS_STATE_CHANGE_STEREO_OFF: ret = "HS_STATE_CHANGE_STEREO_OFF"; break;
	case HS_STATE_CHANGE_MUTE_ON: ret = "HS_STATE_CHANGE_MUTE_ON"; break;
	case HS_STATE_CHANGE_MUTE_OFF: ret = "HS_STATE_CHANGE_MUTE_OFF"; break;
	case HS_STATE_CHANGE_BATTERY_LEVEL: ret = "HS_STATE_CHANGE_BATTERY_LEVEL"; break;
	case HS_STATE_CHANGE_IN_RANGE: ret = "HS_STATE_CHANGE_IN_RANGE"; break;
	case HS_STATE_CHANGE_OUT_OF_RANGE: ret = "HS_STATE_CHANGE_OUT_OF_RANGE"; break;
	case HS_STATE_CHANGE_DOCKED: ret = "HS_STATE_CHANGE_DOCKED"; break;
	case HS_STATE_CHANGE_UNDOCKED: ret = "HS_STATE_CHANGE_UNDOCKED"; break;
	case HS_STATE_CHANGE_IN_CONFERENCE: ret = "HS_STATE_CHANGE_IN_CONFERENCE"; break;
	case HS_STATE_CHANGE_DON: ret = "HS_STATE_CHANGE_DON"; break;
	case HS_STATE_CHANGE_DOFF: ret = "HS_STATE_CHANGE_DOFF"; break;
	case HS_STATE_CHANGE_SERIAL_NUMBER: ret = "HS_STATE_CHANGE_SERIAL_NUMBER"; break;
	case HS_STATE_CHANGE_NEAR: ret = "HS_STATE_CHANGE_NEAR"; break;
	case HS_STATE_CHANGE_FAR: ret = "HS_STATE_CHANGE_FAR"; break;
	case HS_STATE_CHANGE_DOCKED_CHARGING: ret = "HS_STATE_CHANGE_DOCKED_CHARGING"; break;
	case HS_STATE_CHANGE_PROXIMITY_UNKNOWN: ret = "HS_STATE_CHANGE_PROXIMITY_UNKNOWN"; break;
	case HS_STATE_CHANGE_PROXIMITY_ENABLED: ret = "HS_STATE_CHANGE_PROXIMITY_ENABLED"; break;
	case HS_STATE_CHANGE_PROXIMITY_DISABLED: ret = "HS_STATE_CHANGE_PROXIMITY_DISABLED"; break;
	case HS_STATE_CHANGE_PRODUCT_NAME: ret = "HS_STATE_CHANGE_PRODUCT_NAME"; break;
	case HS_STATE_CHANGE_AAL_INCIDENT_REPORT: ret = "HS_STATE_CHANGE_AAL_INCIDENT_REPORT"; break;
	case HS_STATE_CHANGE_AAL_TWA_REPORT: ret = "HS_STATE_CHANGE_AAL_TWA_REPORT"; break;
	case HS_STATE_CHANGE_CONVERSATION_DYNAMICS: ret = "HS_STATE_CHANGE_CONVERSATION_DYNAMICS"; break;
	case HS_STATE_CHANGE_PRODUCTION_NUMBER: ret = "HS_STATE_CHANGE_PRODUCTION_NUMBER"; break;
	case HS_STATE_CHANGE_PRODUCTION_BUILD: ret = "HS_STATE_CHANGE_PRODUCTION_BUILD"; break;
	case HS_STATE_CHANGE_PRODUCTION_PART_NUMBER: ret = "HS_STATE_CHANGE_PRODUCTION_PART_NUMBER"; break;
	case HS_STATE_CHANGE_CONNECTED: ret = "HS_STATE_CHANGE_CONNECTED"; break;
	case HS_STATE_CHANGE_DISCONNECTED: ret = "HS_STATE_CHANGE_DISCONNECTED"; break;
	case HS_STATE_CHANGE_QD_CONNECTED: ret = "HS_STATE_CHANGE_QD_CONNECTED"; break;
	case HS_STATE_CHANGE_QD_DISCONNECTED: ret = "HS_STATE_CHANGE_QD_DISCONNECTED"; break;
	}
	return ret;
}

std::string EnumToString(eHeadsetButton val)
{
	std::string str;
	switch (val)
	{
		case HEADSET_BUTTON_UNKNOWN: str = "HEADSET_BUTTON_UNKNOWN"; break;
		case HEADSET_BUTTON_VOLUME_UP: str = "HEADSET_BUTTON_VOLUME_UP"; break;
		case HEADSET_BUTTON_VOLUME_DOWN: str = "HEADSET_BUTTON_VOLUME_DOWN"; break;
		case HEADSET_BUTTON_VOLUME_UP_HELD: str = "HEADSET_BUTTON_VOLUME_UP_HELD"; break;
		case HEADSET_BUTTON_VOLUME_DOWN_HELD: str = "HEADSET_BUTTON_VOLUME_DOWN_HELD"; break;
		case HEADSET_BUTTON_MUTE: str = "HEADSET_BUTTON_MUTE"; break;
		case HEADSET_BUTTON_MUTE_HELD: str = "HEADSET_BUTTON_MUTE_HELD"; break;
		case HEADSET_BUTTON_TALK: str = "HEADSET_BUTTON_TALK"; break;
		case HEADSET_BUTTON_AUDIO: str = "HEADSET_BUTTON_AUDIO"; break;
		case HEADSET_BUTTON_PLAY: str = "HEADSET_BUTTON_PLAY"; break;
		case HEADSET_BUTTON_PAUSE: str = "HEADSET_BUTTON_PAUSE"; break;
		case HEADSET_BUTTON_NEXT: str = "HEADSET_BUTTON_NEXT"; break;
		case HEADSET_BUTTON_PREVIOUS: str = "HEADSET_BUTTON_PREVIOUS"; break;
		case HEADSET_BUTTON_FAST_FORWARD: str = "HEADSET_BUTTON_FAST_FORWARD"; break;
		case HEADSET_BUTTON_REWIND: str = "HEADSET_BUTTON_REWIND"; break;
		case HEADSET_BUTTON_STOP: str = "HEADSET_BUTTON_STOP"; break;
		case HEADSET_BUTTON_FLASH: str = "HEADSET_BUTTON_FLASH"; break;
		case HEADSET_BUTTON_SMART: str = "HEADSET_BUTTON_SMART"; break;

		//Handset buttons
		case HEADSET_BUTTON_OFF_HOOK: str = "HEADSET_BUTTON_OFF_HOOK"; break;
		case HEADSET_BUTTON_ON_HOOK: str = "HEADSET_BUTTON_ON_HOOK"; break;
		case HEADSET_BUTTON_KEY_0: str = "HEADSET_BUTTON_KEY_0"; break;
		case HEADSET_BUTTON_KEY_1: str = "HEADSET_BUTTON_KEY_1"; break;
		case HEADSET_BUTTON_KEY_2: str = "HEADSET_BUTTON_KEY_2"; break;
		case HEADSET_BUTTON_KEY_3: str = "HEADSET_BUTTON_KEY_3"; break;
		case HEADSET_BUTTON_KEY_4: str = "HEADSET_BUTTON_KEY_4"; break;
		case HEADSET_BUTTON_KEY_5: str = "HEADSET_BUTTON_KEY_5"; break;
		case HEADSET_BUTTON_KEY_6: str = "HEADSET_BUTTON_KEY_6"; break;
		case HEADSET_BUTTON_KEY_7: str = "HEADSET_BUTTON_KEY_7"; break;
		case HEADSET_BUTTON_KEY_8: str = "HEADSET_BUTTON_KEY_8"; break;
		case HEADSET_BUTTON_KEY_9: str = "HEADSET_BUTTON_KEY_9"; break;
		case HEADSET_BUTTON_KEY_STAR: str = "HEADSET_BUTTON_KEY_STAR"; break;
		case HEADSET_BUTTON_KEY_POUND: str = "HEADSET_BUTTON_KEY_POUND"; break;
		case HEADSET_BUTTON_SPEAKER: str = "HEADSET_BUTTON_SPEAKER"; break;
		case HEADSET_BUTTON_REJECT: str = "HEADSET_BUTTON_REJECT"; break;
		case HEADSET_BUTTON_REDIAL: str = "HEADSET_BUTTON_REDIAL"; break;
		default: str = "HEADSET_BUTTON_UNKNOWN";
	}
	return str;
}

void PrintDeviceDetails(IDevice* device)
{
	bool muted;
	IDeviceListener* myDeviceListener = nullptr;

	DMResult res = device->getDeviceListener(&myDeviceListener);
	if (res != DMResult::DM_RESULT_SUCCESS)
	{
		std::cout << "Device is not attached / unable to hook up to device listener events" << std::endl;
		return;
	}
	std::cout << "Successfully hooked to device listener events" << std::endl;

	wchar_t buffer1[1024];
	device->getProductName(buffer1, 1024);
	int32_t pid = device->getProductID();

	std::wcout << "Device attached: " << buffer1;
	std::wcout << L", Product ID = " << std::uppercase << std::hex << std::setfill(L'0') << std::setw(4) << pid << std::endl;

	// Obtain initial device microphone mute state
	res = myDeviceListener->getMute(muted);
	std::cout << "Device mute state: muted = ";
	(muted == true) ? std::cout << "True" : std::cout << "False"; std::cout << std::endl;

	return;
}
	
// Call Object
class Call : public ICall
{
private:
	int32_t m_id = 0;
public:
	virtual int32_t getID() { return m_id; }
	virtual void setID(int32_t id) { m_id = id; }
	virtual int32_t getConferenceID() { return 0; }
	virtual void setConferenceID(int32_t id) { /*no op*/ }
	virtual bool getInConference() { return false; }
	virtual void setInConference(bool bConference) { /*no op*/ }

	void operator delete(void *p) { ::operator delete(p); }

	~Call() { }
};

ICall* GetCall(Call& call)
{
	std::string strCallId;
	std::cout << "Enter Call Id: ";
	std::getline(std::cin, strCallId);

	int32_t id;
	std::stringstream myStream(strCallId);
	myStream >> id;
	call.setID(id);
	return &call;
}

ICall* GetCall(Call& call, int id)
{
	call.setID(id);
	return &call;
}

// Contact Object
class Contact : public IContact
{
private:
	int32_t m_id = 0;
	wstring m_name;
	wstring m_friendlyName;
	wstring m_sipUri;
	wstring m_email;
	wstring m_phone;
	wstring m_workPhone;
	wstring m_mobilePhone;
	wstring m_homePhone;

public:
	virtual const wchar_t* getName() const { return m_name.c_str(); }
	virtual void setName(const wchar_t* pName) { m_name = pName; }
	virtual const wchar_t* getFriendlyName() const { return m_friendlyName.c_str(); }
	virtual void setFriendlyName(const wchar_t* pFName) { m_friendlyName = pFName; }
	virtual int32_t getID() const { return m_id; }
	virtual void setID(int32_t id) { m_id = id; }
	virtual const wchar_t* getSipUri() const { return m_sipUri.c_str(); }
	virtual void setSipUri(const wchar_t* pSip) { m_sipUri = pSip; }
	virtual const wchar_t* getPhone() const { return m_phone.c_str(); }
	virtual void setPhone(const wchar_t* pPhone) { m_phone = pPhone; }
	virtual const wchar_t* getEmail() const { return m_email.c_str(); }
	virtual void setEmail(const wchar_t* pEmail) { m_email = pEmail; }
	virtual const wchar_t* getWorkPhone() const { return m_workPhone.c_str(); }
	virtual void setWorkPhone(const wchar_t* pWPhone) { m_workPhone = pWPhone; }
	virtual const wchar_t* getMobilePhone() const { return m_mobilePhone.c_str(); }
	virtual void setMobilePhone(const wchar_t* pMPhone) { m_mobilePhone = pMPhone; }
	virtual const wchar_t* getHomePhone() const { return m_homePhone.c_str(); }
	virtual void setHomePhone(const wchar_t* pHPhone) { m_homePhone = pHPhone; }

	void operator delete(void *p) { ::operator delete(p); }

	~Contact() { }
};

IContact* GetContact()
{
	std::string strContactName;
	std::cout << "Enter contact name: ";
	std::getline(std::cin, strContactName);

	wstring wName(strContactName.begin(), strContactName.end());
	std::cout << "Name:" << strContactName << std::endl;
	IContact* contact = nullptr;
	//	getContact( &contact );
	//	contact->setName( wName.c_str() );
	return contact;
}

IContact* GetContact(Contact& contact, std::string name)
{
	std::wstring wsTmp(name.begin(), name.end());
	contact.setName(wsTmp.c_str());
	return &contact;
}

Call call;
Contact contact;

// create sink for receiving session events
class CallEventSink : public ICallEvents
{
private:
	ISession* m_session = nullptr;
public:
	CallEventSink() {}
	CallEventSink(ISession* session) : m_session(session) { m_session->registerCallback(this); }
	virtual ~CallEventSink() { m_session->unregisterCallback(this); }

	virtual bool OnCallStateChanged(CallStateEventArgs const& pcscArgs)
	{
		std::string state = EnumToString(pcscArgs.callState);
		//cout << "OnCallStateChanged Session event " << state.c_str() << endl; 
		return true;
	}
	virtual bool OnCallRequest(CallRequestEventArgs const& pcscArgs) { cout << "OnCallRequest Session event" << endl; return true; }
	void operator delete(void *p) { ::operator delete(p); }
};

// create sink for receiving device listener events
class DeviceListenerEventSink : public IDeviceListenerCallback
{
public:
	DeviceListenerEventSink() { cout << "DeviceListenerEventSink constructor" << endl; }
	// IDeviceListenerCallback implementations
	virtual void onHeadsetButtonPressed(DeviceListenerEventArgs const &args) { cout << "onHeadsetButtonPressed Device Listener event: " + EnumToString(
		args.headsetButton) << endl; }
	virtual void onHeadsetStateChanged(DeviceListenerEventArgs const &args)
	{
		std::string state = EnumToString(args.headsetStateChange);
		cout << "onHeadsetStateChanged Device Listener event: " << state << endl;
	}
	virtual void onBaseButtonPressed(DeviceListenerEventArgs const &args) { cout << "onBaseButtonPressed Device Listener event" << endl; }
	virtual void onBaseStateChanged(DeviceListenerEventArgs const &args) { cout << "onBaseStateChanged Device Listener event" << endl; }
	virtual void onATDStateChanged(DeviceListenerEventArgs const &args) { cout << "onATDStateChanged Device Listener event" << endl; }
};

// create sink for receiving data events
class DeviceEventSink : public IDeviceCallback
{
public:
	virtual void onDataReceived(ReportDataEventArgs const& args) 
	{ 
		// illustration of receiving/surfacing audio status events:
		std::stringstream s;
		for (int i = 0; i < 16; ++i)
			s << std::hex << std::setfill('0') << std::setw(2) << (unsigned short)(unsigned char)args.report[i];
		string reportstr = s.str();
		// convert string to upper case
		std::for_each(reportstr.begin(), reportstr.end(), [](char & c) {
			c = ::toupper(c);
		});
		std::regex pattern1(".*0E1E(01|02|03|04|06|09).*", std::regex_constants::icase);
		std::regex pattern2(".*0E1E(05|07|08|0A|0B).*", std::regex_constants::icase);
		string pattern3 = "0E1E00";
		if (std::regex_match(reportstr, std::regex(pattern1)))
			std::cout << "monoon" << endl;
		if (std::regex_match(reportstr, std::regex(pattern2)))
			std::cout << "media on" << endl;
		if (reportstr.find(pattern3) != string::npos)
			std::cout << "mono off / media off" << endl;
	}
};

// global variable
DeviceListenerEventSink* deviceListenerCallback = nullptr;
DeviceEventSink deviceCallback;

// function to attach device listener events callback to attached headset (if any)
void AttachDeviceListener()
{
	if (SM_RESULT_SUCCESS != session->getActiveDevice(&activeDevice))
	{
		cout << "there is no active devices, please attach one then run the app again" << endl;
		activeDevice = nullptr;
	}
	else
	{
		// Get device listener
		pDeviceListener dev_listener = nullptr;

		auto dm_result = activeDevice->getDeviceListener(&dev_listener);

		if (dm_result != DM_RESULT_SUCCESS)
		{
			cout << "failed to get device listener interface" << endl;
		}
		else
		{
			// Register device listener callbacks
			deviceListenerCallback = new DeviceListenerEventSink();
			dm_result = dev_listener->registerCallback(deviceListenerCallback);

			if (dm_result != DM_RESULT_SUCCESS)
			{
				cout << "failed to register device listener callback" << endl;
			}
		}

		// Get device sink
		cout << "Registering for DeviceManager callbacks" << endl;

		DMResult result = activeDevice->registerCallback(&deviceCallback);

		if (result == DM_RESULT_SUCCESS)
			cout << "Successfully registered for Device callbacks" << endl;
		else
			cout << "Failed to register for Device callbacks" << endl;
	}
}

// function to remove device listener events callback
void RemoveDeviceListener()
{
	// Cleanup
	if (activeDevice != nullptr)
	{
		pDeviceListener dev_listener = nullptr;
		auto dm_result = activeDevice->getDeviceListener(&dev_listener);
		if (dm_result == DM_RESULT_SUCCESS && deviceListenerCallback != nullptr)
		{
			dev_listener->unregisterCallback(deviceListenerCallback);
			delete deviceListenerCallback;
		}

		// clean device sink
		cout << "Unregistering for Device callbacks" << endl;

		DMResult result = activeDevice->unregisterCallback(&deviceCallback);

		if (result == DM_RESULT_SUCCESS)
			cout << "Successfully unregistered for Device callbacks" << endl;
		else
			cout << "Failed to unregister for Device callbacks" << endl;
	}
}

// create sink for receiving session manager events
class SessionManagerEventSink : public ISessionManagerEvents
{
private:
	ISessionManager* m_sessionManager = nullptr;
public:
	SessionManagerEventSink() {}
	SessionManagerEventSink(ISessionManager* sessionManager) : m_sessionManager(sessionManager) { m_sessionManager->registerCallback(this); }
	virtual ~SessionManagerEventSink() { m_sessionManager->unregisterCallback(this); gGotDevice = false; }
	virtual bool OnCallStateChanged(CallStateEventArgs const& cseArgs)
	{
		int32_t callId = call.getID();
		std::string state = EnumToString(cseArgs.callState);
		std::cout << "Call State Changed:  callid=" << callId << " new state=" << state.c_str() << endl;
			
		return true;
	}
	virtual bool OnDeviceStateChanged(DeviceStateEventArgs const& devArgs)
	{
		eDeviceState devState = devArgs.deviceState;
		std::string state = EnumToString(devState);
		cout << "OnDeviceStateChanged: " << state.c_str() << endl;

		// Remove device listener
		RemoveDeviceListener();

		gGotDevice = false; // schedule a device re-attach attempt next time around the main menu loop

		if (devState == eDeviceState::DEV_STATE_ADDED)
		{
			std::cout << "Press Enter to attach this device." << std::endl;
		}

		return true;
	}
	void operator delete(void *p) { ::operator delete(p); }
};

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

int _tmain(int argc, _TCHAR* argv[])
{
	bool quit = false;
	std::string input;
	int cmd = -1;
	int _callid = 0; // variable to track call id between my app and Plantronics

	InitLogger(".\\Spokes.log4cxx"); // Note: use a local copy of Spokes.log4cxx alongside your EXE to surpress unwanted logging in case Hub is not installed!
	InitSpokesRuntime();

	cout << "C++ Plantronics Native API Sample" << std::endl;
		
	// create session manager
	ISessionManager *sessionManager = nullptr;
	if (SM_RESULT_SUCCESS == getSessionManager(&sessionManager))
	{
		// create session
		session = nullptr;
		if (SM_RESULT_SUCCESS == sessionManager->registerSession(L"Sample client", &session))
		{
			activeDevice = nullptr;
			SMResult res = session->getActiveDevice(&activeDevice); // always succeeds (since getActiveDevice() gets a proxy & retruns SM_RESULT_SUCCESS)
				
			IUserPreference* userPreference = nullptr;
			if (SM_RESULT_SUCCESS == sessionManager->getUserPreference(&userPreference))
			{
				// set this session as default one for receiving CallRequest events
				int32_t pluginId;
				session->getPluginId(&pluginId);
				userPreference->setDefaultSoftphone(pluginId);
			}

			// get call command
			ICallCommand* callCommand = nullptr;
			if (SM_RESULT_SUCCESS == session->getCallCommand(&callCommand))
			{
				// sink to all call events
				auto callEvents = std::make_unique<CallEventSink>(session);
				auto sessionManagerEvents = std::make_unique<SessionManagerEventSink>(sessionManager); 
					
				// enter in command loop
				while (!quit)
				{
					// If we have not attached to device (or device was removed/added), then attempt to attach to a device
					if (!gGotDevice)
					{
						std::cout << "Attempting device attach" << std::endl;
						if (true == activeDevice->isAttached())
						{
							// Attach device listener (if device is attached to PC)
							AttachDeviceListener();

							gGotDevice = true;
							PrintDeviceDetails(activeDevice);
						}
						else
						{
							std::cout << "Unable to retrieve/hook to active device" << std::endl;
						}
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
						callCommand->incomingCall(GetCall(call, _callid), // inform Plantronics my app has an incoming (ringing) call
							GetContact(contact, "Bob%20Smith"), RING_TONE_UNKNOWN, eAudioRoute::AUDIO_ROUTE_TO_HEADSET);
						break;
					case 2:
						_callid++;
						callCommand->outgoingCall(GetCall(call, _callid),  // inform Plantronics my app has an outgoing call
							GetContact(contact, "Bob%20Smith"), eAudioRoute::AUDIO_ROUTE_TO_HEADSET);
						break;
					case 3:
						callCommand->answeredCall(GetCall(call, _callid)); // inform Plantronics my app has now answered an incoming (ringing) call
						break;
					case 4:
						callCommand->holdCall(GetCall(call, _callid)); // place call on hold
						break;
					case 5:
						callCommand->resumeCall(GetCall(call, _callid)); // resume the call
						break;
					case 6:
						callCommand->muteCall(true); // mute the headset (note for wireless products, audio link must be active)
						break;
					case 7:
						callCommand->muteCall(false); // unmute the headset (note for wireless products, audio link must be active)
						break;
					case 8:
						callCommand->terminateCall(GetCall(call, _callid)); // inform Plantronics my app has now terminated the call
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

				input = "";
				activeDevice->Release();
			}
		}

		// Remove device listener
		RemoveDeviceListener();

		sessionManager->unregisterSession(session);
		session->Release();
	}

	ShutDownSpokesRuntime();

	return 0;
}
