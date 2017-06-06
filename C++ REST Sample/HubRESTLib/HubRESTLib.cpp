// HubRESTLib is a class that implements the Plantronics REST API integration in C++
// allowing Native C++ programs to connect to Plantronics Hub via REST API requests

#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include <Poco/Net/HTTPCredentials.h>
#include "Poco/StreamCopier.h"
#include "Poco/NullStream.h"
#include "Poco/Path.h"
#include "Poco/URI.h"
#include "Poco/Exception.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/Object.h"
#include "HubRESTLib.h"

using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPMessage;
using Poco::StreamCopier;
using Poco::Path;
using Poco::URI;
using Poco::Exception; 
using Poco::JSON::Parser;
using Poco::JSON::Object;
using Poco::JSON::Array;
using Poco::Dynamic::Var;
using Poco::Thread;

char * getTextForHubSDKActionType(int enumVal)
{
	switch (enumVal)
	{
	case HubSDKActionType::HubSDKActionType_None:
		return "HubSDKActionType_None";
		break;
	case HubSDKActionType::HubSDKActionType_IncomingCall:
		return "HubSDKActionType_IncomingCall";
		break;
	case HubSDKActionType::HubSDKActionType_OutgoingCall:
		return "HubSDKActionType_OutgoingCall";
		break;
	case HubSDKActionType::HubSDKActionType_AnswerCall:
		return "HubSDKActionType_AnswerCall";
		break;
	case HubSDKActionType::HubSDKActionType_HoldCall:
		return "HubSDKActionType_HoldCall";
		break;
	case HubSDKActionType::HubSDKActionType_ResumeCall:
		return "HubSDKActionType_ResumeCall";
		break;
	case HubSDKActionType::HubSDKActionType_SetMute:
		return "HubSDKActionType_SetMute";
		break;
	case HubSDKActionType::HubSDKActionType_TerminateCall:
		return "HubSDKActionType_TerminateCall";
		break;
	case HubSDKActionType::HubSDKActionType_Shutdown:
		return "HubSDKActionType_Shutdown";
		break;
	case HubSDKActionType::HubSDKActionType_GetDeviceEvents:
		return "HubSDKActionType_GetDeviceEvents";
		break;
	case HubSDKActionType::HubSDKActionType_GetCallEvents:
		return "HubSDKActionType_GetCallEvents";
		break;
	case HubSDKActionType::HubSDKActionType_ConnectToAPI:
		return "HubSDKActionType_ConnectToAPI";
		break;

	default:
		return "Not recognized..";
	}
}

char * getTextForHubSDKInfoType(int enumVal)
{
	switch (enumVal)
	{
	case HubSDKInfoType::HubSDKInfoType_connected:
		return "HubSDKInfoType_connected";
		break;
	case HubSDKInfoType::HubSDKInfoType_disconnected:
		return "HubSDKInfoType_disconnected";
		break;
	case HubSDKInfoType::HubSDKInfoType_device_attached:
		return "HubSDKInfoType_device_attached";
		break;
	case HubSDKInfoType::HubSDKInfoType_device_detached:
		return "HubSDKInfoType_device_detached";
		break;
	case HubSDKInfoType::HubSDKInfoType_notification:
		return "HubSDKInfoType_notification";
		break;
	case HubSDKInfoType::HubSDKInfoType_nodevice:
		return "HubSDKInfoType_nodevice";
		break;
	default:
		return "Not recognized..";
	}
}

char * getTextForHubSDKErrorType(int enumVal)
{
	switch (enumVal)
	{
	case HubSDKErrorType::HubSDKErrorType_connection_failed:
		return "HubSDKErrorType_connection_failed";
		break;
	case HubSDKErrorType::HubSDKErrorType_invalid_action_requested:
		return "HubSDKErrorType_invalid_action_requested";
		break;
	case HubSDKErrorType::HubSDKErrorType_not_connected:
		return "HubSDKErrorType_not_connected";
		break;

	default:
		return "Not recognized..";
	}
}

char * getTextForHubSDKCallState(int enumVal)
{
	switch (enumVal)
	{
	case HubSDKCallState::CallState_Unknown:
		return "CallState_Unknown";
		break;
	case HubSDKCallState::CallState_AcceptCall:
		return "CallState_AcceptCall";
		break;
	case HubSDKCallState::CallState_TerminateCall:
		return "CallState_TerminateCall";
		break;
	case HubSDKCallState::CallState_HoldCall:
		return "CallState_HoldCall";
		break;
	case HubSDKCallState::CallState_Resumecall:
		return "CallState_Resumecall";
		break;
	case HubSDKCallState::CallState_Flash:
		return "CallState_Flash";
		break;
	case HubSDKCallState::CallState_CallInProgress:
		return "CallState_CallInProgress";
		break;
	case HubSDKCallState::CallState_CallRinging:
		return "CallState_CallRinging";
		break;
	case HubSDKCallState::CallState_CallEnded:
		return "CallState_CallEnded";
		break;
	case HubSDKCallState::CallState_TransferToHeadSet:
		return "CallState_TransferToHeadSet";
		break;
	case HubSDKCallState::CallState_TransferToSpeaker:
		return "CallState_TransferToSpeaker";
		break;
	case HubSDKCallState::CallState_MuteON:
		return "CallState_MuteON";
		break;
	case HubSDKCallState::CallState_MuteOFF:
		return "CallState_MuteOFF";
		break;
	case HubSDKCallState::CallState_MobileCallRinging:
		return "CallState_MobileCallRinging";
		break;
	case HubSDKCallState::CallState_MobileCallInProgress:
		return "CallState_MobileCallInProgress";
		break;
	case HubSDKCallState::CallState_MobileCallEnded:
		return "CallState_MobileCallEnded";
		break;
	case HubSDKCallState::CallState_Don:
		return "CallState_Don";
		break;
	case HubSDKCallState::CallState_Doff:
		return "CallState_Doff";
		break;
	case HubSDKCallState::CallState_CallIdle:
		return "CallState_CallIdle";
		break;
	case HubSDKCallState::CallState_Play:
		return "CallState_Play";
		break;
	case HubSDKCallState::CallState_Pause:
		return "CallState_Pause";
		break;
	case HubSDKCallState::CallState_Stop:
		return "CallState_Stop";
		break;
	case HubSDKCallState::CallState_DTMFKey:
		return "CallState_DTMFKey";
		break;
	case HubSDKCallState::CallState_RejectCall:
		return "CallState_RejectCall";
		break;
	case HubSDKCallState::CallState_Hook:
		return "CallState_Hook";
		break;
	case HubSDKCallState::CallState_HookIdle:
		return "CallState_HookIdle";
		break;
	case HubSDKCallState::CallState_HookDocked:
		return "CallState_HookDocked";
		break;
	case HubSDKCallState::CallState_HookUndocked:
		return "CallState_HookUndocked";
		break;
	case HubSDKCallState::CallState_BaseEvent:
		return "CallState_BaseEvent";
		break;
	case HubSDKCallState::CallState_AnsweredAndEnded:
		return "CallState_AnsweredAndEnded";
		break;
	case HubSDKCallState::CallState_UnansweredAndEnded:
		return "CallState_UnansweredAndEnded";
		break;
	case HubSDKCallState::CallState_DeviceChange:
		return "CallState_DeviceChange";
		break;
	case HubSDKCallState::CallState_DeviceArrived:
		return "CallState_DeviceArrived";
		break;
	case HubSDKCallState::CallState_MakeCall:
		return "CallState_MakeCall";
		break;

	default:
		return "Not recognized..";
	}
}

char * getTextForHubSDKHeadsetStateChange(int enumVal)
{
	switch (enumVal)
	{
	case HubSDKHeadsetStateChange::HeadsetStateChange_Unknown:
		return "CallState_Unknown";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_MonoON:
		return "HeadsetStateChange_MonoON";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_MonoOFF:
		return "HeadsetStateChange_MonoOFF";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_StereoON:
		return "HeadsetStateChange_StereoON";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_StereoOFF:
		return "HeadsetStateChange_StereoOFF";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_MuteON:
		return "HeadsetStateChange_MuteON";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_MuteOFF:
		return "HeadsetStateChange_MuteOFF";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_BatteryLevel:
		return "HeadsetStateChange_BatteryLevel";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_InRange:
		return "HeadsetStateChange_InRange";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_OutofRange:
		return "HeadsetStateChange_OutofRange";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_Docked:
		return "HeadsetStateChange_Docked";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_UnDocked:
		return "HeadsetStateChange_UnDocked";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_InConference:
		return "HeadsetStateChange_InConference";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_Don:
		return "HeadsetStateChange_Don";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_Doff:
		return "HeadsetStateChange_Doff";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_SerialNumber:
		return "HeadsetStateChange_SerialNumber";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_Near:
		return "HeadsetStateChange_Near";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_Far:
		return "HeadsetStateChange_Far";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_DockedCharging:
		return "HeadsetStateChange_DockedCharging";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_ProximityUnknown:
		return "HeadsetStateChange_ProximityUnknown";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_ProximityEnabled:
		return "HeadsetStateChange_ProximityEnabled";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_ProximityDisabled:
		return "HeadsetStateChange_ProximityDisabled";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_ProductName:
		return "HeadsetStateChange_ProductName";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_AALIncidentReport:
		return "HeadsetStateChange_AALIncidentReport";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_AALTWAReport:
		return "HeadsetStateChange_AALTWAReport";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_ConversationDynamics:
		return "HeadsetStateChange_ConversationDynamics";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_ProductionNumber:
		return "HeadsetStateChange_ProductionNumber";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_ProductionBuild:
		return "HeadsetStateChange_ProductionBuild";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_ProductionPartNumber:
		return "HeadsetStateChange_ProductionPartNumber";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_Connected:
		return "HeadsetStateChange_Connected";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_Disconnected:
		return "HeadsetStateChange_Disconnected";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_QDConnected:
		return "HeadsetStateChange_QDConnected";
		break;
	case HubSDKHeadsetStateChange::HeadsetStateChange_QDDisconnected:
		return "HeadsetStateChange_QDDisconnected";
		break;

	default:
		return "Not recognized..";
	}
}

HubRESTLib::HubRESTLib(IHubRESTLib* eventListener)
{
	this->eventListener = eventListener;

	// start worker thread to handle Hub API interaction
	hubSDKThread = new thread(&HubRESTLib::HubSDKThreadFunction, this);
}

HubRESTLib::~HubRESTLib()
{
	HubSDKAction action(HubSDKActionType_Shutdown);
	DoHubSDKAction(action); // Cleanup the Plantronics REST API
	hubSDKThread->join();
	delete hubSDKThread;
}

string HubRESTLib::GetJSONValue(string json)
{
	string result = "";
	if (json.length() > 0)
	{
		try
		{
			Parser parser;
			Var resultobj = parser.parse(json);
			Object::Ptr object = resultobj.extract<Object::Ptr>();
			Var test = object->get("Result");
			if (!test.isEmpty())
			{
				std::string value = test.convert<std::string>();
				if (value.length() > 0)
				{
					result = value;
				}
			}
		}
		catch (exception const & e)
		{
			if (eventListener != NULL)
			{
				string msg("error extracting GetJSONValue: ");
				msg.append(e.what());
				eventListener->HubRESTLib_SDKInfo(HubSDKInfoType_notification,
					getTextForHubSDKInfoType(HubSDKInfoType_notification), msg);
			}
		}
	}
	return result;
}

void HubRESTLib::ParseDeviceEvents(string json)
{
	string result = "";
	if (json.length() > 0)
	{
		try
		{
			Parser parser;
			Var resultobj = parser.parse(json);
			Object::Ptr object = resultobj.extract<Object::Ptr>();
			Array::Ptr test = object->getArray("Result");
			if (!test.isNull())
			{
				Poco::Dynamic::Array da = *test;
				for (int i = 0; i < da.size(); i++)
				{
					int eventId = da[i]["Event_Id"];
					if (eventListener != NULL)
					{
						eventListener->HubRESTLib_HeadsetStateChanged(
							(HubSDKHeadsetStateChange)eventId, getTextForHubSDKHeadsetStateChange(eventId));
					}
				}
			}
		}
		catch (exception const & e)
		{
			if (eventListener != NULL)
			{
				string msg("error extracting ParseDeviceEvents: ");
				msg.append(e.what());
				eventListener->HubRESTLib_SDKInfo(HubSDKInfoType_notification,
					getTextForHubSDKInfoType(HubSDKInfoType_notification), msg);
			}
		}
	}
}

void HubRESTLib::ParseCallEvents(string json)
{
	string result = "";
	if (json.length() > 0)
	{
		try
		{
			Parser parser;
			Var resultobj = parser.parse(json);
			Object::Ptr object = resultobj.extract<Object::Ptr>();
			Array::Ptr test = object->getArray("Result");
			if (!test.isNull())
			{
				Poco::Dynamic::Array da = *test;
				for (int i = 0; i < da.size(); i++)
				{
					int callState = da[i]["Action"];
					int callId = da[i]["CallId"]["Id"];
					if (eventListener != NULL)
					{
						eventListener->HubRESTLib_CallStateChanged(
							callId,	(HubSDKCallState)callState, getTextForHubSDKCallState(callState));
					}
				}
			}
		}
		catch (exception const & e)
		{
			if (eventListener != NULL)
			{
				if (eventListener != NULL)
				{
					string msg("error extracting ParseCallEvents: ");
					msg.append(e.what());
					eventListener->HubRESTLib_SDKInfo(HubSDKInfoType_notification,
						getTextForHubSDKInfoType(HubSDKInfoType_notification), msg);
				}
			}
		}
	}
}

void HubRESTLib::ParseDeviceInfo(string json)
{
	string result = "";
	if (json.length() > 0)
	{
		try
		{
			Parser parser;
			Var resultobj = parser.parse(json);
			Object::Ptr object = resultobj.extract<Object::Ptr>();
			Object::Ptr test = object->getObject("Result");
			if (!test.isNull())
			{
				Var test2 = test->get("ProductName");
				if (eventListener != NULL)
				{
					string msg("Device Attached: Product Name = ");
					msg.append(test2.convert<string>());
					eventListener->HubRESTLib_SDKInfo(HubSDKInfoType_device_attached,
						getTextForHubSDKInfoType(HubSDKInfoType_notification), msg);
				}
			}
		}
		catch (exception const & e)
		{
			if (eventListener != NULL)
			{
				if (eventListener != NULL)
				{
					string msg("error extracting ParseDeviceInfo: ");
					msg.append(e.what());
					eventListener->HubRESTLib_SDKInfo(HubSDKInfoType_notification,
						getTextForHubSDKInfoType(HubSDKInfoType_notification), msg);
				}
			}
		}
	}
}

string HubRESTLib::GetJSONSubValue(string json, string keyname, string valuename)
{
	string result = "";
	if (json.length() > 0)
	{
		try
		{
			Parser parser;
			Var resultobj = parser.parse(json);
			Object::Ptr object = resultobj.extract<Object::Ptr>();
			Var test = object->get(keyname);
			if (!test.isEmpty())
			{
				std::string value = test.convert<std::string>();
				if (value.length() > 0)
				{
					result = value;
				}
			}
		}
		catch (exception const & e)
		{
			if (eventListener != NULL)
			{
				string msg("error extracting GetJSONSubValue: ");
				msg.append(e.what());
				eventListener->HubRESTLib_SDKInfo(HubSDKInfoType_notification,
					getTextForHubSDKInfoType(HubSDKInfoType_notification), msg);
			}
		}
	}
	return result;
}

void HubRESTLib::DoAttachDevice()
{
	// Connect to the Plantronics REST API (attach device session)
	string tmpResult = SendRESTCommand("/Spokes/DeviceServices/Attach?uid=0123456789");
	if (tmpResult.length() > 0)
	{
		sessionid = GetJSONValue(tmpResult);
		if (sessionid.length() > 0)
		{
			if (eventListener != NULL)
			{
				string notice("Hub SDK connection successful, session id = ");
				notice.append(sessionid);
				eventListener->HubRESTLib_SDKInfo(HubSDKInfoType_notification,
					getTextForHubSDKInfoType(HubSDKInfoType_notification), notice);
			}
			Thread::sleep(250);
			// Register Spokes plugin (Plantronics API application session)
			string url("/Spokes/SessionManager/Register?name=");
			url.append(pluginid);
			SendRESTCommand(url);
			pluginRegistered = true;
			if (eventListener != NULL)
				eventListener->HubRESTLib_SDKInfo(HubSDKInfoType_connected, getTextForHubSDKInfoType(HubSDKInfoType_connected), "SDK now connected");
		}
		else
		{
			if (eventListener != NULL)
				eventListener->HubRESTLib_SDKInfo(HubSDKInfoType_nodevice,
					getTextForHubSDKInfoType(HubSDKInfoType_nodevice),
					"There are no supported devices. Please attach a Plantronics headset to the PC.");
		}
	}
	else
	{
		if (eventListener != NULL)
		{
			eventListener->HubRESTLib_SDKError(HubSDKErrorType_connection_failed,
				getTextForHubSDKErrorType(HubSDKErrorType_connection_failed),
				"Error connecting to Plantronics Hub. (Have you installed and run Plantronics Hub from http://www.plantronics.com/software");
			eventListener->HubRESTLib_SDKInfo(HubSDKInfoType_disconnected,
				getTextForHubSDKInfoType(HubSDKInfoType_disconnected),
				"SDK not connected.");
		}
	}
}

// Cleanup the Plantronics REST API
void HubRESTLib::DoReleaseDevice() 
{
	// Unregister Spokes plugin (Plantronics API application session)
	string url("/Spokes/SessionManager/UnRegister?name=");
	url.append(pluginid);
	SendRESTCommand(url);
	pluginRegistered = false;
	Thread::sleep(250);
	if (sessionid.length()>0)
	{
		// Disconnect from the Plantronics REST API (release device session)
		string url = "/Spokes/DeviceServices/Release?sess=";
		url.append(sessionid);
		SendRESTCommand(url);
		sessionid = "";
	}
}

void HubRESTLib::DoShowDeviceInfo()
{
	// get info about attached device (if any)
	string url("/Spokes/DeviceServices/Info");
	string deviceInfo = SendRESTCommand(url);
	ParseDeviceInfo(deviceInfo);
}

void HubRESTLib::DoHubSDKAction(HubSDKAction action)
{
	if (pluginRegistered || action.ActionType == HubSDKActionType_ConnectToAPI
		|| action.ActionType == HubSDKActionType_Shutdown)
	{
		{
			lock_guard<mutex> lg(actionListLock);
			actionList.push(action);
		} // release lock
		actionListLockCV.notify_one();
	}
	else
	{
		if (eventListener != NULL)
		{
			string msg("Cannot perform SDK action: ");
			msg.append(getTextForHubSDKActionType(action.ActionType));
			msg.append(", SDK is not connected (is Plantronics Hub running?)");
			eventListener->HubRESTLib_SDKError(
				HubSDKErrorType_not_connected, getTextForHubSDKErrorType(HubSDKErrorType_not_connected), msg);
		}
	}
}

void HubRESTLib::DoGetDeviceEvents()
{
	string url = "/Spokes/DeviceServices/Events?sess=";
	url.append(sessionid);
	url.append("&queue=0");
	string deviceEvents = SendRESTCommand(url);
	if (deviceEvents.length() > 0)
	{
		// first check if device was unplugged
		string err = GetJSONSubValue(deviceEvents, "Err", "Description");
		std::size_t found = err.find("Invalid session id");
		if (found != std::string::npos)
		{
			if (eventListener != NULL)
			{
				eventListener->HubRESTLib_SDKInfo(HubSDKInfoType_device_detached,
					getTextForHubSDKInfoType(HubSDKInfoType_device_detached), "Device detached.");
			}
			// device has been unplugged
			DoReleaseDevice();
		}
		else
		{
			// if not, parse device events
			ParseDeviceEvents(deviceEvents);
		}
	}
	else
	{
		// Hub has been shutdown
		DoReleaseDevice();
	}
}

void HubRESTLib::DoGetCallEvents()
{
	string url = "/Spokes/CallServices/CallEvents?name=";
	url.append(pluginid);
	string callEvents = SendRESTCommand(url);
	// parse call events
	ParseCallEvents(callEvents);
}

void HubRESTLib::HubSDKThreadFunction()
{
	HubSDKAction action(HubSDKActionType_ConnectToAPI);
	DoHubSDKAction(action);

	HubSDKAction nextAction;
	while (!quit)
	{
		nextAction.ActionType = HubSDKActionType_None;
		{
			unique_lock<mutex> guard(actionListLock);
			if (!actionList.empty())
				//actionListLockCV.wait(guard);
			{
				nextAction = actionList.front();
				actionList.pop();
			}
		}  // release lock

		switch (nextAction.ActionType)
		{
		case HubSDKActionType_None:
			// pass
			break;
		case HubSDKActionType_Shutdown:
			if (pluginRegistered) DoReleaseDevice();
			quit = true;
			break;
		case HubSDKActionType_IncomingCall:
			DoIncomingCall(nextAction.callid, nextAction.contactname);
			break;
		case HubSDKActionType_OutgoingCall:
			DoOutgoingCall(nextAction.callid, nextAction.contactname);
			break;
		case HubSDKActionType_AnswerCall:
			DoAnswerCall(nextAction.callid);
			break;
		case HubSDKActionType_HoldCall:
			DoHoldCall(nextAction.callid);
			break;
		case HubSDKActionType_ResumeCall:
			DoResumeCall(nextAction.callid);
			break;
		case HubSDKActionType_SetMute:
			DoMuteCall(nextAction.mutestate);
			break;
		case HubSDKActionType_TerminateCall:
			DoTerminateCall(nextAction.callid);
			break;
		case HubSDKActionType_GetDeviceEvents:
			DoGetDeviceEvents();
			break;
		case HubSDKActionType_GetCallEvents:
			DoGetCallEvents();
			break;
		case HubSDKActionType_ConnectToAPI:
			DoAttachDevice();
			if (pluginRegistered) DoShowDeviceInfo();
			break;
		default:
			if (eventListener != NULL)
				eventListener->HubRESTLib_SDKError(HubSDKErrorType_invalid_action_requested,
					getTextForHubSDKErrorType(HubSDKErrorType_invalid_action_requested), "The action requested is not implemented: " + nextAction.ActionType);
			break;
		}

		if (!quit)
		{
			Thread::sleep(50); // sleep for a while
			eventPollCounter++;
			if (eventPollCounter == 36)
			{
				// every 36*50 = 1800 seconds, read hub sdk device events (Device state changes)
				if (pluginRegistered)
				{
					HubSDKAction action(HubSDKActionType_GetDeviceEvents);
					DoHubSDKAction(action);
				}
			}
			if (eventPollCounter == 40)
			{
				// every 40*50 = 2000 seconds, read hub sdk call state events (Call control state changes)
				if (pluginRegistered)
				{
					HubSDKAction action(HubSDKActionType_GetCallEvents);
					DoHubSDKAction(action);
				}
				else
				{
					// retry the SDK connection!
					if (eventListener != NULL)
					{
						eventListener->HubRESTLib_SDKInfo(HubSDKInfoType_notification,
							getTextForHubSDKInfoType(HubSDKInfoType_notification),
							"-- POLLING FOR HUB / DEVICE RE-ATTACH --");
					}
					HubSDKAction action(HubSDKActionType_ConnectToAPI);
					DoHubSDKAction(action);
				}
				eventPollCounter = 0;
			}
		}
	}
}

string HubRESTLib::SendRESTCommand(string path)
{
	string responsestr = "";
	try
	{
		HTTPClientSession s(host, port);
		HTTPRequest request(HTTPRequest::HTTP_GET, path);
		s.sendRequest(request);
		HTTPResponse response;
		istream& rs = s.receiveResponse(response);
		std::ostringstream oss;
		StreamCopier::copyStream(rs, oss);
		responsestr = oss.str();
	}
	catch (exception const & e)
	{
		string msg("error sending REST request: ");
		msg.append(e.what());
		if (eventListener != NULL)
		{
			eventListener->HubRESTLib_SDKError(HubSDKErrorType_connection_failed,
				getTextForHubSDKErrorType(HubSDKErrorType_connection_failed), msg);
		}
	}
	return responsestr;
}

void HubRESTLib::DoIncomingCall(int callid, string contactname)
{
	if (sessionid.length()>0 && pluginRegistered)
	{
		// inform Plantronics my app has an incoming (ringing) call
		string url = "/Spokes/CallServices/IncomingCall?name=";
		url.append(pluginid);
		url.append("&callID=%7B%22Id%22%3A%22");
		std::string strcallid = std::to_string(callid);
		url.append(strcallid);
		url.append("%22%7D&contact=%7B%22Name%22%3A%22");
		url.append(contactname);
		url.append("%22%7D&tones=Unknown&route=ToHeadset");
		SendRESTCommand(url);
	}
	else
	{
		if (eventListener != NULL)
		{
			eventListener->HubRESTLib_SDKInfo(HubSDKInfoType_nodevice,
				getTextForHubSDKInfoType(HubSDKInfoType_nodevice), "Error: You need to Attach device first");
		}
	}
}

void HubRESTLib::DoOutgoingCall(int callid, string contactname)
{
	if (sessionid.length()>0 && pluginRegistered)
	{
		// inform Plantronics my app has an outgoing call
		string url = "/Spokes/CallServices/OutgoingCall?name=";
		url.append(pluginid);
		url.append("&callID=%7B%22Id%22%3A%22");
		std::string strcallid = std::to_string(callid);
		url.append(strcallid);
		url.append("%22%7D&contact=%7B%22Name%22%3A%22");
		url.append(contactname);
		url.append("%22%7D&route=ToHeadset");
		SendRESTCommand(url);
	}
	else
	{
		if (eventListener != NULL)
		{
			eventListener->HubRESTLib_SDKInfo(HubSDKInfoType_nodevice,
				getTextForHubSDKInfoType(HubSDKInfoType_nodevice), "Error: You need to Attach device first");
		}
	}
}

void HubRESTLib::DoTerminateCall(int callid)
{
	if (sessionid.length()>0 && pluginRegistered)
	{
		// inform Plantronics my app has now terminated the call
		string url = "/Spokes/CallServices/TerminateCall?name=";
		url.append(pluginid);
		url.append("&callID=%7B%22Id%22%3A%22");
		std::string strcallid = std::to_string(callid);
		url.append(strcallid);
		url.append("%22%7D");
		SendRESTCommand(url);
	}
	else
	{
		if (eventListener != NULL)
		{
			eventListener->HubRESTLib_SDKInfo(HubSDKInfoType_nodevice,
				getTextForHubSDKInfoType(HubSDKInfoType_nodevice), "Error: You need to Attach device first");
		}
	}
}

void HubRESTLib::DoAnswerCall(int callid)
{
	if (sessionid.length()>0 && pluginRegistered)
	{
		// inform Plantronics my app has now answered an incoming (ringing) call
		string url = "/Spokes/CallServices/AnswerCall?name=";
		url.append(pluginid);
		url.append("&callID=%7B%22Id%22%3A%22");
		std::string strcallid = std::to_string(callid);
		url.append(strcallid);
		url.append("%22%7D");
		SendRESTCommand(url);
	}
	else
	{
		if (eventListener != NULL)
		{
			eventListener->HubRESTLib_SDKInfo(HubSDKInfoType_nodevice,
				getTextForHubSDKInfoType(HubSDKInfoType_nodevice), "Error: You need to Attach device first");
		}
	}
}

void HubRESTLib::DoHoldCall(int callid)
{
	if (sessionid.length()>0 && pluginRegistered)
	{
		// place call on hold
		string url = "/Spokes/CallServices/HoldCall?name=";
		url.append(pluginid);
		url.append("&callID=%7B%22Id%22%3A%22");
		std::string strcallid = std::to_string(callid);
		url.append(strcallid);
		url.append("%22%7D");
		SendRESTCommand(url);
	}
	else
	{
		if (eventListener != NULL)
		{
			eventListener->HubRESTLib_SDKInfo(HubSDKInfoType_nodevice,
				getTextForHubSDKInfoType(HubSDKInfoType_nodevice), "Error: You need to Attach device first");
		}
	}
}

void HubRESTLib::DoResumeCall(int callid)
{
	if (sessionid.length()>0 && pluginRegistered)
	{
		// resume the call
		string url = "/Spokes/CallServices/ResumeCall?name=";
		url.append(pluginid);
		url.append("&callID=%7B%22Id%22%3A%22");
		std::string strcallid = std::to_string(callid);
		url.append(strcallid);
		url.append("%22%7D");
		SendRESTCommand(url);
	}
	else
	{
		if (eventListener != NULL)
		{
			eventListener->HubRESTLib_SDKInfo(HubSDKInfoType_nodevice,
				getTextForHubSDKInfoType(HubSDKInfoType_nodevice), "Error: You need to Attach device first");
		}
	}
}

void HubRESTLib::DoMuteCall(bool muted)
{
	if (sessionid.length()>0 && pluginRegistered)
	{
		// mute/un-mute the headset (note for legacy wireless products, audio link must be active)
		string url = "/Spokes/CallServices/MuteCall?name=";
		url.append(pluginid);
		url.append("&muted=");
		url.append(muted ? "true" : "false");
		SendRESTCommand(url);
	}
	else
	{
		if (eventListener != NULL)
		{
			eventListener->HubRESTLib_SDKInfo(HubSDKInfoType_nodevice,
				getTextForHubSDKInfoType(HubSDKInfoType_nodevice), "Error: You need to Attach device first");
		}
	}
}
