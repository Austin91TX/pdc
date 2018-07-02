#include "HubSDKConnector.h"

// thanks: https://stackoverflow.com/questions/23594244/is-there-a-safe-way-to-have-a-stdthread-as-a-member-of-a-class
HubSDKConnector::HubSDKConnector() : hubSDKThread{}, watchdogThread{} // default constructor
{
	// move assignment
	hubSDKThread = thread{ &HubSDKConnector::HubSDKThreadFunction, this };

	// start watch dog task to periodically check COM API link is still alive
	// (check that Plantronics Hub PLTHub.exe has not exitted)
	watchdogThread = thread{ &HubSDKConnector::DoWatchdogTask, this };
}

HubSDKConnector::~HubSDKConnector()
{
	_quit = true;
	hubSDKThread.join();
	watchdogThread.join();
}

void HubSDKConnector::HubSDKThreadFunction()
{
	bool quit = false;

	//DoHubSDKAction(new HubSDKAction(HubSDKActionType.ConnectToAPI));

	while (!quit && !_quit)
	{
	}
}

void HubSDKConnector::DoHubSDKAction(HubSDKAction* action)
{
	if (_apiConnected || action->ActionType == HubSDKActionType_ConnectToAPI)
	{
		std::lock_guard<std::mutex> lock(actionListLock);
		actionList.push(action);
	}
	else
	{
		if (action->ActionType != HubSDKActionType_CheckCOMAPIActive)
		{
			//OnSDKError(new SDKErrorArgs(SDKErrorType.sdk_not_connected,
			//	"Cannot perform SDK action: " + action.ActionType.ToString() +
			//	", SDK is not connected (is Plantronics Hub running?)"));
		}
		delete action;
	}
}

void HubSDKConnector::DoWatchdogTask()
{
	while (!_quit)
	{
		std::cout << "HubSDKActionType_CheckCOMAPIActive...\n";
		std::this_thread::sleep_for(1s);
		if (!_quit)
			DoHubSDKAction(new HubSDKAction(HubSDKActionType_CheckCOMAPIActive));
	}
}
