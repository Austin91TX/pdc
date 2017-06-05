#pragma once

// HubSDKConnector is a class that implements the ISpokesProxy event callbacks used
// to receive events from the Plantronics REST API (Plantronics headset events and call events)

#include "SpokesProxy.h"

class HubSDKConnector : ISpokesProxy
{
public:
	HubSDKConnector();
	~HubSDKConnector();

	// Inherited via ISpokesProxy - event callbacks to receive events from the Plantronics REST API
	virtual void SpokesProxy_SDKError(HubSDKErrorType ErrorType, string ErrorTypeStr, string msg) override;
	virtual void SpokesProxy_SDKInfo(HubSDKInfoType InfoType, string InfoTypeStr, string msg) override;
	virtual void SpokesProxy_CallStateChanged(int callid, HubSDKCallState callstate, string callstatestr) override;
	virtual void SpokesProxy_HeadsetStateChanged(HubSDKHeadsetStateChange headsetstate, string headsetstatestr) override;

	// Function to send commands to Plantronics REST API (incoming call, outgoing call, etc).
	void DoHubSDKAction(HubSDKAction action);

private:
	SpokesProxy* hubSDK;
};

