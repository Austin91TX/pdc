using System;
using System.Runtime.InteropServices;
using System.Threading;
using Interop.Plantronics;

namespace ResilientPLTDemo
{
    class Program
    {

        private static int _callid; // variable to track call id between my app and Plantronics
        private static HubSDKConnector _hubSDK;

        static void Main()
        {
            Console.WriteLine("C# Plantronics COM API Sample");
            bool quit = false;

            _hubSDK = new HubSDKConnector();
            _hubSDK.SDKError += _hubSDK_SDKError;
            _hubSDK.SDKInfo += _hubSDK_SDKInfo;
            _hubSDK.CallStateChanged += _hubSDK_CallStateChanged;
            _hubSDK.HeadsetStateChanged += _hubSDK_HeadsetStateChanged;

            while (!quit)
            {
                ShowMenu();
                string cmd = Console.ReadLine();
                switch (cmd)
                {
                    case "1":
                        _callid++;
                        // inform Plantronics my app has an incoming (ringing) call
                        Console.WriteLine("Performing incoming call, id = " + _callid);
                        _hubSDK.DoHubSDKAction(new HubSDKAction(HubSDKActionType.IncomingCall, _callid, "Bob%20Smith"));
                        break;
                    case "2":
                        _callid++;
                        // inform Plantronics my app has an outgoing call
                        Console.WriteLine("Performing outgoing call, id = " + _callid);
                        _hubSDK.DoHubSDKAction(new HubSDKAction(HubSDKActionType.OutgoingCall, _callid, "Bob%20Smith")); 
                        break;
                    case "3":
                        // inform Plantronics my app has now answered an incoming (ringing) call
                        Console.WriteLine("Answering call, id = " + _callid);
                        _hubSDK.DoHubSDKAction(new HubSDKAction(HubSDKActionType.AnswerCall, _callid)); 
                        break;
                    case "4":
                        // place call on hold
                        Console.WriteLine("Holding call, id = " + _callid);
                        _hubSDK.DoHubSDKAction(new HubSDKAction(HubSDKActionType.HoldCall, _callid)); 
                        break;
                    case "5":
                        // resume the call
                        Console.WriteLine("Resuming call, id = " + _callid);
                        _hubSDK.DoHubSDKAction(new HubSDKAction(HubSDKActionType.ResumeCall, _callid)); 
                        break;
                    case "6":
                        // mute the headset (note for wireless products, audio link must be active)
                        Console.WriteLine("Setting headset mute = true");
                        _hubSDK.DoHubSDKAction(new HubSDKAction(HubSDKActionType.SetMute, true)); 
                        break;
                    case "7":
                        // unmute the headset (note for wireless products, audio link must be active)
                        Console.WriteLine("Setting headset mute = false");
                        _hubSDK.DoHubSDKAction(new HubSDKAction(HubSDKActionType.SetMute, false)); 
                        break;
                    case "8":
                        // inform Plantronics my app has now terminated the call
                        Console.WriteLine("Terminating call, id = " + _callid);
                        _hubSDK.DoHubSDKAction(new HubSDKAction(HubSDKActionType.TerminateCall, _callid)); 
                        break;
                    case "0":
                        quit = true;
                        break;
                    // ADVANCED OPTIONS
                    case "9":
                        // dial outbound call using Hub SDK
                        Console.Write("Enter phone number >");
                        string num = Console.ReadLine();
                        Console.WriteLine("Dialling outbound call to "+num+" with Hub default softphone...");
                        if (num.Length > 0) _hubSDK.DoHubSDKAction(new HubSDKAction(HubSDKActionType.DialOutbound, 0, num));
                        else Console.WriteLine("You must enter a contact number or name to dial. Please invoke operation again to retry.");
                        break;
                    case "10":
                        _callid++;
                        // inform Plantronics my app has an already ongoing call, for instance in auto-answer scenario
                        Console.WriteLine("Performing insert call, id = " + _callid);
                        _hubSDK.DoHubSDKAction(new HubSDKAction(HubSDKActionType.InsertCall, _callid, "Bob%20Smith"));
                        Console.WriteLine("Resuming call, id = " + _callid);
                        _hubSDK.DoHubSDKAction(new HubSDKAction(HubSDKActionType.ResumeCall, _callid));
                        break;
                    default:
                        Console.WriteLine("Unrecognised menu choice.");
                        break;
                }
            }

        }

        private static void _hubSDK_SDKInfo(object sender, HubSDKConnector.SDKInfoArgs e)
        {
            Console.WriteLine("SDK INFO: " + e.EventType + ": " + e.msg);
        }

        private static void _hubSDK_CallStateChanged(Interop.Plantronics.COMCallEventArgs callEventArgs)
        {
            // informs us the calling state has changed, for example user as answered/terminated a call
            // using headset buttons - this event should be used in my app to actually connect/terminate the call!
            Console.WriteLine("Call State Changed: callid=" + callEventArgs.call.Id + " new state=" + callEventArgs.CallState + " call source = " + callEventArgs.CallSource);
        }

        private static void _hubSDK_HeadsetStateChanged(Interop.Plantronics.COMDeviceListenerEventArgs args)
        {
            // informs us of a variety of Plantronics device state changes
            Console.Write(args.DeviceEventType + " - ");
            if (args.DeviceEventType == COMDeviceEventType.DeviceEventType_HeadsetStateChanged)
            {
                Console.Write(args.HeadsetStateChange);
            }
            Console.WriteLine();

        }

        private static void _hubSDK_SDKError(object sender, HubSDKConnector.SDKErrorArgs e)
        {
            Console.WriteLine("SDK ERROR: "+e.EventType+": "+e.msg);
        }

        private static void ShowMenu()
        {
            Console.WriteLine();
            Console.WriteLine("plt sample menu");
            Console.WriteLine("--");
            Console.WriteLine("1 - ring/incoming call");
            Console.WriteLine("2 - outgoing call");
            Console.WriteLine("3 - answer call");
            Console.WriteLine("4 - hold call");
            Console.WriteLine("5 - resume call");
            Console.WriteLine("6 - mute call");
            Console.WriteLine("7 - unmute call");
            Console.WriteLine("8 - end call");
            Console.WriteLine("0 - quit");
            Console.WriteLine();
            Console.WriteLine("-- ADVANCED --");
            Console.WriteLine("9 - dial outbound");
            Console.WriteLine("10 - insert call");
            Console.WriteLine();
            Console.Write("> ");
        }

    }
}
