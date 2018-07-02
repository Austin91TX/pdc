#pragma once
#include <thread>         // std::thread
#include <iostream>       // std::cout
#include <queue>
#include <mutex>
#include <condition_variable>

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
	HubSDKActionType_CheckCOMAPIActive,
	HubSDKActionType_ConnectToAPI
};

enum SDKErrorType
{
	SDKErrorType_sdk_connection_failed,
	SDKErrorType_sdk_invalid_action_requested,
	SDKErrorType_sdk_not_connected,
};

enum SDKInfoType
{
	SDKInfoType_sdk_connected,
	SDKInfoType_sdk_disconnected,
	SDKInfoType_sdk_device_attached,
	SDKInfoType_sdk_device_detached,
	SDKInfoType_sdk_notification
};

class HubSDKAction
{
public:
	HubSDKActionType ActionType;
	int callid;
	string contactname;
	bool mutestate;

	// constructor taking only action type, no extra parameters
	HubSDKAction(HubSDKActionType actionType)
	{
		ActionType = actionType;
		this->callid = 0;
		this->contactname = "";
		this->mutestate = false;
	}
	// constructor for call related actions requiring callid and optionally contactname
	HubSDKAction(HubSDKActionType actionType, int callid, string contactname = "")
	{
		ActionType = actionType;
		this->callid = callid;
		this->contactname = contactname;
		this->mutestate = false;
	}

	// constructor for mute related actions (mute on / off)
	HubSDKAction(HubSDKActionType actionType, bool mutestate)
	{
		ActionType = actionType;
		this->callid = 0;
		this->contactname = "";
		this->mutestate = mutestate;
	}
};

/// <summary>
/// This is an "active" class used to connect to and communicate with the Hub SDK
/// The reason for putting this in it's own thread is to avoid impacting the applications
/// GUI, for example if Hub isn't running when you try to connect to it there is a very
/// long startup time, especially for full Plantronics Hub product (3.8 etc)
/// </summary>
class HubSDKConnector final
{
private:
	thread hubSDKThread, watchdogThread;
	queue<HubSDKAction*> actionList;
	mutable std::mutex actionListLock;
	std::condition_variable actionListLockCondition;
	bool _quit = false;
	bool _apiConnected = false;

	// Plantronics API objects and interfaces required

	void HubSDKThreadFunction();
	void DoHubSDKAction(HubSDKAction* action);
	void DoWatchdogTask();
public:
	HubSDKConnector();
	~HubSDKConnector();
};
