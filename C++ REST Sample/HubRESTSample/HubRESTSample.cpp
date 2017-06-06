// HubRESTSample.cpp : This is a Sample C++ integration to Plantronics Hub REST Service API
// illustrating how to add Plantronics call control features to a Native C++ application
// http://developer.plantronics.com
//
// Pre-requisites (compile time):
// - POCO C++ library, download and build from: https://pocoproject.org/ 
//   - THE POCO headers and libraries are referred to by this solution, adjust project settings/paths if required
// Pre-requisites (run time):
// - Install Plantronics Hub from: http://www.plantronics.com/software
// - POCO library DLLs: PocoFoundation.dll / PocoJSON.dll / PocoNet.dll

#include <iostream>
#include <string>
#include "HubSDKConnector.h"

using namespace std;

void ShowMenu()
{
	cout << endl;
	cout << "plt sample menu" << endl;
	cout << "--" << endl;
	cout << "1 - ring/incoming call" << endl;
	cout << "2 - outgoing call" << endl;
	cout << "3 - answer call" << endl;
	cout << "4 - hold call" << endl;
	cout << "5 - resume call" << endl;
	cout << "6 - mute call" << endl;
	cout << "7 - unmute call" << endl;
	cout << "8 - end call" << endl;
	cout << "0 - quit" << endl;
	cout << endl;
	cout << "> ";
}

int GetCommand()
{
	string cmdstr;
	int cmd;
	getline(std::cin, cmdstr);
	if (cmdstr.empty()) cmd = -1;
	else
	{
		try
		{
			cmd = atoi(cmdstr.c_str());
		}
		catch (exception const & e)
		{
			cout << "error : " << e.what() << endl;
			cmd = -1;
		}
	}
	return cmd;
}

int main()
{
	cout << "C++ Plantronics Hub REST Sample" <<  endl;
	bool quit = false;
	int callid = 0; 

	HubSDKConnector * hubSDK = new HubSDKConnector(); // Connect to the Plantronics REST API

	while (!quit)
	{
		ShowMenu();
		int cmd = GetCommand();
		HubSDKAction action;
		switch (cmd)
		{
			case 1:
				callid++;
				// inform Plantronics my app has an incoming (ringing) call
				cout << "Performing incoming call, id = " << callid << endl;
				action.ActionType = HubSDKActionType_IncomingCall;
				action.callid = callid; // numeric id to uniquely identify each call
				action.contactname = "Bob%20Smith"; // optional contact name for display devices, e.g. Calisto 240, note: needs url encoding e.g. spaces are %20, etc.
				hubSDK->DoHubSDKAction(action);
				break;
			case 2:
				callid++;
				// inform Plantronics my app has an outgoing call
				cout << "Performing outgoing call, id = " << callid << endl;
				action.ActionType = HubSDKActionType_OutgoingCall;
				action.callid = callid;
				action.contactname = "Bob%20Smith";
				hubSDK->DoHubSDKAction(action);
				break;
			case 3:
				// inform Plantronics my app has now answered an incoming (ringing) call
				cout << "Answering call, id = " << callid << endl;
				action.ActionType = HubSDKActionType_AnswerCall;
				action.callid = callid;
				hubSDK->DoHubSDKAction(action);
				break;
			case 4:
				// place call on hold
				cout << "Holding call, id = " << callid << endl;
				action.ActionType = HubSDKActionType_HoldCall;
				action.callid = callid;
				hubSDK->DoHubSDKAction(action);
				break;
			case 5:
				// resume the call
				cout << "Resuming call, id = " << callid << endl;
				action.ActionType = HubSDKActionType_ResumeCall;
				action.callid = callid;
				hubSDK->DoHubSDKAction(action);
				break;
			case 6:
				// mute the headset (note for wireless products, audio link must be active)
				cout << "Setting headset mute = true";
				action.ActionType = HubSDKActionType_SetMute;
				action.mutestate = true;
				hubSDK->DoHubSDKAction(action);
				break;
			case 7:
				// unmute the headset (note for wireless products, audio link must be active)
				cout << "Setting headset mute = false";
				action.ActionType = HubSDKActionType_SetMute;
				action.mutestate = false;
				hubSDK->DoHubSDKAction(action);
				break;
			case 8:
				// inform Plantronics my app has now terminated the call
				cout << "Terminating call, id = " << callid << endl;
				action.ActionType = HubSDKActionType_TerminateCall;
				action.callid = callid;
				hubSDK->DoHubSDKAction(action);
				break;
			case 0:
				quit = true;
				break;
			default:
				cout << "Unrecognised menu choice." << endl;
				break;
		}
	}

	delete hubSDK; // Cleanup the Plantronics REST API

	return 0;
}