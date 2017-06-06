#pragma once

// HubSDKConnector is a class that implements the IHubRESTLib event callbacks used
// to receive events from the Plantronics REST API (Plantronics headset events and call events)

#include "HubRESTLib.h"

class HubSDKConnector : IHubRESTLib
{
public:
	HubSDKConnector();
	~HubSDKConnector();

	// Inherited via IHubRESTLib - event callbacks to receive events from the Plantronics REST API
	virtual void HubRESTLib_SDKError(HubSDKErrorType ErrorType, string ErrorTypeStr, string msg) override;
	virtual void HubRESTLib_SDKInfo(HubSDKInfoType InfoType, string InfoTypeStr, string msg) override;
	virtual void HubRESTLib_CallStateChanged(int callid, HubSDKCallState callstate, string callstatestr) override;
	virtual void HubRESTLib_HeadsetStateChanged(HubSDKHeadsetStateChange headsetstate, string headsetstatestr) override;

	// Function to send commands to Plantronics REST API (incoming call, outgoing call, etc).
	void DoHubSDKAction(HubSDKAction action);

private:
	HubRESTLib* hubSDK;
};

