#include "HubSDKConnector.h"

// HubSDKConnector is a class that implements the ISpokesProxy event callbacks used
// to receive events from the Plantronics REST API (Plantronics headset events and call events)


HubSDKConnector::HubSDKConnector()
{
	hubSDK = new SpokesProxy(this); // Connect to the Plantronics REST API
}


HubSDKConnector::~HubSDKConnector()
{
	delete hubSDK; // Cleanup the Plantronics REST API
}

void HubSDKConnector::SpokesProxy_SDKError(HubSDKErrorType ErrorType, string ErrorTypeStr, string msg)
{
	cout << "SDKError: " << ErrorTypeStr << "(" << ErrorType << "), " << msg << endl;
}

void HubSDKConnector::SpokesProxy_SDKInfo(HubSDKInfoType InfoType, string InfoTypeStr, string msg)
{
	cout << "SDKInfo: " << InfoTypeStr << "(" << InfoType << ") " << msg << endl;
}

void HubSDKConnector::SpokesProxy_CallStateChanged(int callid, HubSDKCallState callstate, string callstatestr)
{
	cout << "CallStateChanged: " << "callid = " << callid << ", state = " << callstatestr << "(" << callstate << ")" << endl;
}

void HubSDKConnector::SpokesProxy_HeadsetStateChanged(HubSDKHeadsetStateChange headsetstate, string headsetstatestr)
{
	cout << "HeadsetStateChanged: " << "headsetstate = " << headsetstatestr << "(" << headsetstate << ")" << endl;
}

// Function to send commands to Plantronics REST API (incoming call, outgoing call, etc).
void HubSDKConnector::DoHubSDKAction(HubSDKAction action)
{
	hubSDK->DoHubSDKAction(action);
}
