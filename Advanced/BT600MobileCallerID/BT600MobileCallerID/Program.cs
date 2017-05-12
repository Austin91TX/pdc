// NOTE: This extends the basic C# call control sample to show how to receive mobile caller id
// from BT600-based devices. Look for comments starting // Mobile Caller ID: 
// add a reference to: C:\Program Files (x86)\Plantronics\Spokes3G SDK\Interop.Plantronics.dll
using System;
using System.Runtime.InteropServices;
using Interop.Plantronics;
using System.Text;

namespace BT600MobileCallerID
{
    class Program
    {
        // Plantronics API objects and interfaces required
        private static COMSessionManager _sessionManager;
        private static COMSession _session;
        private static ICOMSessionManagerEvents_Event _sessionManagerEvents;
        private static COMDevice _device;
        private static ICOMDeviceListenerEvents_Event _deviceListenerEvents;
        private static COMCallCommand _callCommand;
        private static COMDeviceListener _deviceListener;

        // Mobile Caller ID: event interface to receive BT600 mobile caller id message
        private static ICOMDeviceEvents_Event _deviceComEvents;

        private static int _callid; // variable to track call id between my app and Plantronics

        static void Main()
        {
            Console.WriteLine("C# Plantronics COM API Sample");
            bool quit = false;

            // Connect to the Plantronics COM API
            _sessionManager = new COMSessionManager();
            _sessionManager.Register("My App", out _session);

            _sessionManagerEvents = (ICOMSessionManagerEvents_Event)_sessionManager; // obtain session manager events interface
            _sessionManagerEvents.onDeviceStateChanged += _sessionManagerEvents_onDeviceStateChanged; // register for device state changed events (device arrival / removal)

            _session.onCallStateChanged += _session_onCallStateChanged; // register for call state changed events (headset call control)

            _callCommand = _session.GetCallCommand(); // obtain Plantronics object for controlling call state of Plantronics device
            AttachDevice(); // register for device events and obtain device listener

            while (!quit)
            {
                ShowMenu();
                string cmd = Console.ReadLine();
                switch (cmd)
                {
                    case "1":
                        _callid++;
                        DoIncomingCall(_callid, "Bob%20Smith"); // inform Plantronics my app has an incoming (ringing) call
                        break;
                    case "2":
                        _callid++;
                        DoOutgoingCall(_callid, "Bob%20Smith"); // inform Plantronics my app has an outgoing call
                        break;
                    case "3":
                        DoAnswerCall(_callid); // inform Plantronics my app has now answered an incoming (ringing) call
                        break;
                    case "4":
                        DoHoldCall(_callid); // place call on hold
                        break;
                    case "5":
                        DoResumeCall(_callid); // resume the call
                        break;
                    case "6":
                        DoMuteCall(true); // mute the headset (note for wireless products, audio link must be active)
                        break;
                    case "7":
                        DoMuteCall(false); // unmute the headset (note for wireless products, audio link must be active)
                        break;
                    case "8":
                        DoTerminateCall(_callid); // inform Plantronics my app has now terminated the call
                        break;
                    case "0":
                        quit = true;
                        break;
                    default:
                        Console.WriteLine("Unrecognised menu choice.");
                        break;
                }
            }

            // Cleanup the Plantronics COM API
            DetachDevice();
            _session.onCallStateChanged -= _session_onCallStateChanged;
            _sessionManagerEvents.onDeviceStateChanged -= _sessionManagerEvents_onDeviceStateChanged;
            _sessionManager.Unregister(_session);
            Marshal.ReleaseComObject(_session);
            Marshal.ReleaseComObject(_sessionManager);
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
            Console.Write("> ");
        }

        private static void DoIncomingCall(int callid, string contactname)
        {
            COMCall call = new COMCall() { Id = callid };
            Contact contact = new Contact() { Name = contactname };
            _callCommand.IncomingCall(call, contact, CallRingTone.RingTone_Unknown,
                CallAudioRoute.AudioRoute_ToHeadset);
            Console.WriteLine("Performing incoming call, id = " + callid);
        }

        private static void DoOutgoingCall(int callid, string contactname)
        {
            Console.WriteLine("Performing outgoing call, id = " + callid);
            COMCall call = new COMCall() { Id = callid };
            Contact contact = new Contact() { Name = contactname };
            _callCommand.OutgoingCall(call, contact, CallAudioRoute.AudioRoute_ToHeadset);
        }

        private static void DoAnswerCall(int callid)
        {
            Console.WriteLine("Performing outgoing call, id = " + callid);
            COMCall call = new COMCall() { Id = callid };
            _callCommand.AnsweredCall(call);
        }

        private static void DoHoldCall(int callid)
        {
            Console.WriteLine("Performing outgoing call, id = " + callid);
            COMCall call = new COMCall() { Id = callid };
            _callCommand.HoldCall(call);
        }

        private static void DoResumeCall(int callid)
        {
            Console.WriteLine("Performing outgoing call, id = " + callid);
            COMCall call = new COMCall() { Id = callid };
            _callCommand.ResumeCall(call);
        }

        private static void DoMuteCall(bool mute)
        {
            Console.WriteLine("Setting headset mute = " + mute);
            _deviceListener.mute = mute;
        }

        private static void DoTerminateCall(int callid)
        {
            Console.WriteLine("Performing outgoing call, id = " + callid);
            COMCall call = new COMCall() { Id = callid };
            _callCommand.TerminateCall(call);
        }

        private static void _session_onCallStateChanged(COMCallEventArgs callEventArgs)
        {
            // informs us the calling state has changed, for example user as answered/terminated a call
            // using headset buttons - this event should be used in my app to actually connect/terminate the call!
            Console.WriteLine("Call State Changed: callid=" + callEventArgs.call.Id + " new state=" + callEventArgs.CallState);
        }

        private static void _sessionManagerEvents_onDeviceStateChanged(COMStateDeviceEventArgs deviceEventArgs)
        {
            // device may have arrived or been removed. Either way detach device event handlers, then try to re-attach them
            DetachDevice();
            AttachDevice();
        }

        private static void AttachDevice()
        {
            // Lets get the primary Plantronics device (if any) and then register
            // for the device event handlers
            try
            {
                _device = _session.GetActiveDevice();
                if (_device != null)
                {
                    // display device information:
                    Console.WriteLine("Device attached: " + _device.ProductName + ", Product ID = " + _device.ProductId.ToString("X"));

                    _deviceListenerEvents = (ICOMDeviceListenerEvents_Event)_device.DeviceListener;
                    if (_deviceListenerEvents != null)
                    {
                        _deviceListenerEvents.onHeadsetStateChanged += _deviceListenerEvents_onHeadsetStateChanged;

                        Console.WriteLine("Successfully hooked to device listener events");

                        // Mobile Caller ID: onDataReceived event handler to receive BT600 mobile caller id
                        _deviceComEvents = (ICOMDeviceEvents_Event)_session.GetActiveDevice();
                        if (_deviceComEvents != null)
                        {
                            _deviceComEvents.onDataReceived += _deviceComEvents_onDataReceived;
                        }
                    }
                    else
                        Console.WriteLine("Unable to hook to device listener events");

                    _deviceListener = _device.DeviceListener; // Obtain a DeviceListener object for later use

                    Console.WriteLine("Device mute state: muted = " + _deviceListener.mute); // Obtain initial device microphone mute state
                }
                else
                    Console.WriteLine("Unable to retrieve active device");
            }
            catch (Exception)
            {
                Console.WriteLine("Unable to retrieve/hook to active device");
            }
        }

        private static void _deviceListenerEvents_onHeadsetStateChanged(COMDeviceListenerEventArgs args)
        {
            // informs us of a variety of Plantronics device state changes
            Console.Write(args.DeviceEventType + " - ");
            if (args.DeviceEventType == COMDeviceEventType.DeviceEventType_HeadsetStateChanged)
            {
                Console.Write(args.HeadsetStateChange);
            }
            Console.WriteLine();
        }

        private static string byteArrayToString(byte[] p)
        {
            StringBuilder b = new StringBuilder();
            foreach (byte x in p)
                b.Append(x.ToString("X2"));
            return b.ToString();
        }

        // Mobile Caller ID: On receipt of Call status event, extract the mobile caller id number digits
        private static void _deviceComEvents_onDataReceived(ref object report)
        {
            byte[] reportbuf = (byte[])report;
            string stringbuf = byteArrayToString(reportbuf);
            string eventId = stringbuf.Substring(18, 4);
            if (eventId == "0E32") // Call status event
            {
                string mobileCallerId = "";
                try
                {
                    int numDigits = short.Parse(stringbuf.Substring(24, 4), System.Globalization.NumberStyles.HexNumber);
                    if (numDigits > 0)
                    {
                        string digitsBuf = stringbuf.Substring(28, 4 * numDigits);
                        StringBuilder sb = new StringBuilder();
                        for (int i = 0; i < numDigits; i++)
                        {
                            sb.Append((char)short.Parse(digitsBuf.Substring(i * 4, 4), System.Globalization.NumberStyles.HexNumber));
                        }
                        mobileCallerId = sb.ToString();
                    }
                }
                catch (Exception e)
                {
                    Console.WriteLine("Exception: " + e.ToString());
                }
                Console.WriteLine("Mobile Caller ID = " + mobileCallerId);
            }
        }

        private static void DetachDevice()
        {
            // Lets un-register the Plantronics device event handlers
            if (_device != null)
            {
                Marshal.ReleaseComObject(_device);
                _device = null;
            }
            if (_deviceListenerEvents != null)
            {
                _deviceListenerEvents.onHeadsetStateChanged -= _deviceListenerEvents_onHeadsetStateChanged;
                Marshal.ReleaseComObject(_deviceListenerEvents);
                _deviceListenerEvents = null;
            }
        }

        // Convenience class for passing a COMContact to CallCommand IncomingCall/OutgoingCall methods
        class Contact : COMContact
        {
            public string Email { get; set; }
            public string FriendlyName { get; set; }
            public string HomePhone { get; set; }
            public int Id { get; set; }
            public string MobilePhone { get; set; }
            public string Name { get; set; }
            public string Phone { get; set; }
            public string SipUri { get; set; }
            public string WorkPhone { get; set; }
        }
    }
}