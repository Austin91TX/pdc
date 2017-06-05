#pragma once

// SpokesProxy is a class that implements the Plantronics REST API integration in C++
// allowing Native C++ programs to connect to Plantronics Hub via REST API requests

#include <iostream>
#include <thread>
#include <sstream>
#include <queue>
#include <mutex>
using namespace std;

enum HubSDKActionType
{
	HubSDKActionType_None,
	HubSDKActionType_IncomingCall,
	HubSDKActionType_OutgoingCall,
	HubSDKActionType_AnswerCall,
	HubSDKActionType_HoldCall,
	HubSDKActionType_ResumeCall,
	HubSDKActionType_SetMute,
	HubSDKActionType_TerminateCall,
	HubSDKActionType_Shutdown,
	HubSDKActionType_GetDeviceEvents,
	HubSDKActionType_GetCallEvents,
	HubSDKActionType_ConnectToAPI
};

enum HubSDKErrorType
{
	HubSDKErrorType_connection_failed,
	HubSDKErrorType_invalid_action_requested,
	HubSDKErrorType_not_connected
};

enum HubSDKInfoType
{
	HubSDKInfoType_connected,
	HubSDKInfoType_disconnected,
	HubSDKInfoType_device_attached,
	HubSDKInfoType_device_detached,
	HubSDKInfoType_notification,
	HubSDKInfoType_nodevice
};

enum HubSDKCallState
{
	CallState_Unknown = 0,
	CallState_AcceptCall = 1,
	CallState_TerminateCall = 2,
	CallState_HoldCall = 3,
	CallState_Resumecall = 4,
	CallState_Flash = 5,
	CallState_CallInProgress = 6,
	CallState_CallRinging = 7,
	CallState_CallEnded = 8,
	CallState_TransferToHeadSet = 9,
	CallState_TransferToSpeaker = 10,
	CallState_MuteON = 11,
	CallState_MuteOFF = 12,
	CallState_MobileCallRinging = 13,
	CallState_MobileCallInProgress = 14,
	CallState_MobileCallEnded = 15,
	CallState_Don = 16,
	CallState_Doff = 17,
	CallState_CallIdle = 18,
	CallState_Play = 19,
	CallState_Pause = 20,
	CallState_Stop = 21,
	CallState_DTMFKey = 22,
	CallState_RejectCall = 23,
	CallState_Hook = 24,
	CallState_HookIdle = 25,
	CallState_HookDocked = 26,
	CallState_HookUndocked = 27,
	CallState_BaseEvent = 28,
	CallState_AnsweredAndEnded = 29,
	CallState_UnansweredAndEnded = 30,
	CallState_DeviceChange = 31,
	CallState_DeviceArrived = 32,
	CallState_MakeCall = 33
};

enum HubSDKHeadsetStateChange
{
	HeadsetStateChange_Unknown = 0,
	HeadsetStateChange_MonoON = 1,
	HeadsetStateChange_MonoOFF = 2,
	HeadsetStateChange_StereoON = 3,
	HeadsetStateChange_StereoOFF = 4,
	HeadsetStateChange_MuteON = 5,
	HeadsetStateChange_MuteOFF = 6,
	HeadsetStateChange_BatteryLevel = 7,
	HeadsetStateChange_InRange = 8,
	HeadsetStateChange_OutofRange = 9,
	HeadsetStateChange_Docked = 10,
	HeadsetStateChange_UnDocked = 11,
	HeadsetStateChange_InConference = 12,
	HeadsetStateChange_Don = 13,
	HeadsetStateChange_Doff = 14,
	HeadsetStateChange_SerialNumber = 15,
	HeadsetStateChange_Near = 16,
	HeadsetStateChange_Far = 17,
	HeadsetStateChange_DockedCharging = 18,
	HeadsetStateChange_ProximityUnknown = 19,
	HeadsetStateChange_ProximityEnabled = 20,
	HeadsetStateChange_ProximityDisabled = 21,
	HeadsetStateChange_ProductName = 22,
	HeadsetStateChange_AALIncidentReport = 23,
	HeadsetStateChange_AALTWAReport = 24,
	HeadsetStateChange_ConversationDynamics = 25,
	HeadsetStateChange_ProductionNumber = 26,
	HeadsetStateChange_ProductionBuild = 27,
	HeadsetStateChange_ProductionPartNumber = 28,
	HeadsetStateChange_Connected = 29,
	HeadsetStateChange_Disconnected = 30,
	HeadsetStateChange_QDConnected = 31,
	HeadsetStateChange_QDDisconnected = 32
};

class HubSDKAction
{
public:
	HubSDKActionType ActionType;
	int callid;
	string contactname;
	bool mutestate;

	// default constructor
	HubSDKAction::HubSDKAction()
	{
		ActionType = HubSDKActionType_None;
		callid = 0;
		contactname = "";
		mutestate = false;
	}

	// constructor taking only action type, no extra parameters
	HubSDKAction::HubSDKAction(HubSDKActionType actionType)
	{
		ActionType = actionType;
		callid = 0;
		contactname = "";
		mutestate = false;
	}

	// constructor for call related actions requiring callid and optionally contactname
	HubSDKAction::HubSDKAction(HubSDKActionType actionType, int callid, string contactname = "")
	{
		ActionType = actionType;
		callid = callid;
		contactname = contactname;
		mutestate = false;
	}

	// constructor for mute related actions (mute on / off)
	HubSDKAction::HubSDKAction(HubSDKActionType actionType, bool mutestate)
	{
		ActionType = actionType;
		callid = 0;
		contactname = "";
		mutestate = mutestate;
	}
};

class ISpokesProxy
{
public:
	virtual void SpokesProxy_SDKError(HubSDKErrorType ErrorType, string ErrorTypeStr, string msg) = 0;
	virtual void SpokesProxy_SDKInfo(HubSDKInfoType InfoType, string InfoTypeStr, string msg) = 0;
	virtual void SpokesProxy_CallStateChanged(int callid, HubSDKCallState callstate, string callstatestr) = 0;
	virtual void SpokesProxy_HeadsetStateChanged(HubSDKHeadsetStateChange headsetstate, string headsetstatestr) = 0;
};

class SpokesProxy
{
public:
	SpokesProxy(ISpokesProxy* eventListener);
	~SpokesProxy();
	void DoHubSDKAction(HubSDKAction action);
private:
	string SendRESTCommand(string path);
	void DoAttachDevice();
	void DoReleaseDevice();
	void DoShowDeviceInfo();
	string GetJSONValue(string json);
	string GetJSONSubValue(string json, string keyname, string valuename);
	void ParseDeviceEvents(string json);
	void ParseCallEvents(string json);
	void ParseDeviceInfo(string json);
	void HubSDKThreadFunction();
	void DoGetDeviceEvents();
	void DoGetCallEvents();
	void DoIncomingCall(int callid, string contactname);
	void DoOutgoingCall(int callid, string contactname);
	void DoTerminateCall(int callid);
	void DoAnswerCall(int callid);
	void DoHoldCall(int callid);
	void DoResumeCall(int callid);
	void DoMuteCall(bool muted);
	string sessionid = "";
	string pluginid = "Spokes%20Proxy%20Demo"; // hard-coded Hub sdk session name, note: needs url encoding
		// e.g. spaces are %20, etc.
	bool pluginRegistered = false;
	int callid = 0;
	thread* hubSDKThread;
	queue<HubSDKAction> actionList;
	mutex actionListLock;
	condition_variable actionListLockCV;
	string host = "127.0.0.1";
	int port = 32017;
	bool quit = false;
	ISpokesProxy* eventListener = NULL;
	int eventPollCounter = 0;
};
