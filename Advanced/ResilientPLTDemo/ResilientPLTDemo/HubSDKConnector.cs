// NOTE:
// add a reference to: C:\Program Files (x86)\Plantronics\Spokes3G SDK\Interop.Plantronics.dll
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Interop.Plantronics;

namespace ResilientPLTDemo
{
    /// <summary>
    /// This is an "active" class used to connect to and communicate with the Hub SDK
    /// The reason for putting this in it's own thread is to avoid impacting the applications
    /// GUI, for example if Hub isn't running when you try to connect to it there is a very
    /// long startup time, especially for full Plantronics Hub product (3.8 etc)
    /// </summary>
    public class HubSDKConnector
    {
        Thread hubSDKThread;
        Queue<HubSDKAction> actionList = new Queue<HubSDKAction>();
        object actionListLock = new object();
        private bool _quit;
        private bool _apiConnected;

        // Plantronics API objects and interfaces required
        private COMSessionManager _sessionManager;
        private COMSession _session;
        private ICOMSessionManagerEvents_Event _sessionManagerEvents;
        private COMDevice _device;
        private ICOMDeviceListenerEvents_Event _deviceListenerEvents;
        private COMCallCommand _callCommand;
        private COMDeviceListener _deviceListener;

        // some delegates to let application know of SDK events and any errors
        public delegate void SDKErrorHandler(object sender, SDKErrorArgs e);
        public event SDKErrorHandler SDKError;
        private void OnSDKError(SDKErrorArgs e)
        {
            if (SDKError != null)
                SDKError(this, e);
        }
        public class SDKErrorArgs : EventArgs
        {
            public SDKErrorType EventType { get; set; }
            public string EventTypeStr { get; set; }
            public string msg { get; set; }

            public SDKErrorArgs(SDKErrorType eventType, string msg)
            {
                EventType = eventType;
                EventTypeStr = eventType.ToString();
                this.msg = msg;
            }
        }
        public enum SDKErrorType
        {
            sdk_connection_failed,
            sdk_invalid_action_requested,
            sdk_not_connected,
        }
        public delegate void SDKInfoHandler(object sender, SDKInfoArgs e);
        public event SDKInfoHandler SDKInfo;
        private void OnSDKInfo(SDKInfoArgs e)
        {
            if (SDKInfo != null)
                SDKInfo(this, e);
        }
        public class SDKInfoArgs : EventArgs
        {
            public SDKInfoType EventType { get; set; }
            public string EventTypeStr { get; set; }
            public string msg { get; set; }

            public SDKInfoArgs(SDKInfoType eventType, string msg)
            {
                EventType = eventType;
                EventTypeStr = eventType.ToString();
                this.msg = msg;
            }
        }
        public enum SDKInfoType
        {
            sdk_connected,
            sdk_disconnected,
            sdk_device_attached,
            sdk_device_detached,
            sdk_notification
        }
        // pass through delegate for Hub SDK:
        public delegate void CallStateChangedHandler(COMCallEventArgs e);
        public event CallStateChangedHandler CallStateChanged;
        private void OnCallStateChanged(COMCallEventArgs e)
        {
            if (CallStateChanged != null)
                CallStateChanged(e);
        }
        // pass through delegate for Hub SDK:
        public delegate void HeadsetStateChangedHandler(COMDeviceListenerEventArgs e);
        public event HeadsetStateChangedHandler HeadsetStateChanged;
        private void OnHeadsetStateChanged(COMDeviceListenerEventArgs e)
        {
            if (HeadsetStateChanged != null)
                HeadsetStateChanged(e);
        }

        public HubSDKConnector()
        {
            hubSDKThread = new Thread(new ThreadStart(HubSDKThreadFunction));
            hubSDKThread.IsBackground = true;
            hubSDKThread.Start();

            // start watch dog task to periodically check COM API link is still alive
            // (check that Plantronics Hub PLTHub.exe has not exitted)
            Task.Run(() => {
                DoWatchdogTask();
            });

        }

        private void HubSDKThreadFunction()
        {
            bool quit = false;

            DoHubSDKAction(new HubSDKAction(HubSDKActionType.ConnectToAPI));

            HubSDKAction nextAction;
            while (!quit)
            {
                nextAction = null;
                lock (actionListLock)
                {
                    if (actionList.Count > 0)
                        nextAction = actionList.Dequeue();
                }
                if (nextAction != null)
                {
                    switch (nextAction.ActionType)
                    {
                        case HubSDKActionType.Shutdown:
                            quit = true;
                            break;
                        case HubSDKActionType.IncomingCall:
                            DoIncomingCall(nextAction.callid, nextAction.contactname);
                            break;
                        case HubSDKActionType.OutgoingCall:
                            DoOutgoingCall(nextAction.callid, nextAction.contactname);
                            break;
                        case HubSDKActionType.AnswerCall:
                            DoAnswerCall(nextAction.callid);
                            break;
                        case HubSDKActionType.HoldCall:
                            DoHoldCall(nextAction.callid);
                            break;
                        case HubSDKActionType.ResumeCall:
                            DoResumeCall(nextAction.callid);
                            break;
                        case HubSDKActionType.SetMute:
                            DoMuteCall(nextAction.mutestate);
                            break;
                        case HubSDKActionType.TerminateCall:
                            DoTerminateCall(nextAction.callid);
                            break;
                        case HubSDKActionType.CheckCOMAPIActive:
                            // try the com call to see if COM API is still live
                            DoCheckIfCOMSDKIsActive();
                            break;
                        case HubSDKActionType.ConnectToAPI:
                            DoConnectCOMAPI();
                            break;
                        default:
                            OnSDKError(new SDKErrorArgs(SDKErrorType.sdk_invalid_action_requested, "The action requested is not implemented: "+ nextAction.ActionType));
                            break;
                    }
                }
                Thread.Sleep(50); // sleep for a while
            }
            if (_apiConnected)
            {
                // Cleanup the Plantronics COM API
                DoTidyUpSDK();
            }
        }

        private void DoConnectCOMAPI()
        {
            try
            {
                // Connect to the Plantronics COM API
                _sessionManager = new COMSessionManager();
                _sessionManager.Register("My App", out _session);

                _sessionManagerEvents = (ICOMSessionManagerEvents_Event) _sessionManager;
                    // obtain session manager events interface
                _sessionManagerEvents.onDeviceStateChanged += _sessionManagerEvents_onDeviceStateChanged;
                    // register for device state changed events (device arrival / removal)

                _session.onCallStateChanged += _session_onCallStateChanged;
                    // register for call state changed events (headset call control)

                _callCommand = _session.GetCallCommand();
                    // obtain Plantronics object for controlling call state of Plantronics device
                AttachDevice(); // register for device events and obtain device listener
                _apiConnected = true;
                OnSDKInfo(new SDKInfoArgs(SDKInfoType.sdk_connected, "SDK now connected"));
            }
            catch (Exception e)
            {
                DoTidyUpSDK(_session!=null);
                OnSDKError(new SDKErrorArgs(SDKErrorType.sdk_connection_failed, e.ToString()));
                DoAttemptReconnectAfterDelay();
            }
        }

        private void DoCheckIfCOMSDKIsActive()
        {
            bool attemptReconnect = _sessionManager == null;
            if (_sessionManager != null)
            {
                // Is COM Live?
                try
                {
                    COMCallInfo[] calls = _sessionManager.CallManagerState.Calls;
                }
                catch (Exception e)
                {
                    // LC New 29/06/2018 tidy up exception handling
                    if (e is COMException
                        || e is InvalidComObjectException
                        || e is InvalidCastException)
                    {
                        attemptReconnect = true;
                        OnSDKError(new SDKErrorArgs(SDKErrorType.sdk_connection_failed,
                            "Information: Plantronics COM SDK connection failed. This could be due to PLTHub.exe exitting, or if user quit Plantronics Hub or is in process of upgrading Plantronics Hub. NOTE: re-connect attempts will be made."));
                        _apiConnected = false;
                        OnSDKInfo(new SDKInfoArgs(SDKInfoType.sdk_disconnected, "SDK not connected."));
                    }
                    else
                    {
                        attemptReconnect = false;
                        OnSDKError(new SDKErrorArgs(SDKErrorType.sdk_connection_failed,
                            "Information: Plantronics COM SDK connection failed. Unexpected error, please inform the app developer! NOTE: re-connect attempt will NOT be made."));
                        _apiConnected = false;
                        OnSDKInfo(new SDKInfoArgs(SDKInfoType.sdk_disconnected, "SDK not connected."));
                    }
                }
            }
            if (attemptReconnect)
            {
                DoTidyUpSDK(false);
                DoAttemptReconnectAfterDelay(); // introduce a delay so we support the Hub uninstall flow without relaunching the Plthub.exe during uninstall, requiring a reboot!                
            }
        }

        private void DoAttemptReconnectAfterDelay()
        {
            int delaytime = GetReconnectDelayTimeMilliseconds();
            OnSDKInfo(new SDKInfoArgs(SDKInfoType.sdk_notification, "SDK re-connect attempt will occur in: "+delaytime/1000+" seconds."));
            Task.Run(async () =>
            {
                await Task.Delay(delaytime);
                DoHubSDKAction(new HubSDKAction(HubSDKActionType.ConnectToAPI)); // LC 28/06/2018 fixed bug, now do this via queue!
            });
        }

        /// <summary>
        /// Internal function that checks to see if a Plantronics installer process is running
        /// PlantronicsHub*.exe (PlantronicsHubInstaller.exe or PlantronicsHubSDKInstaller.exe)
        /// If yes returns 60000 (delay of 1 minute), if no returns 5000 (delay of only 5 seconds)
        /// </summary>
        /// <returns></returns>
        private int GetReconnectDelayTimeMilliseconds()
        {
            return ProcessRunningStartingWith("PlantronicsHub", "Plantronics Hub");
        }

        private int ProcessRunningStartingWith(string processNamePrefix, string msiExecWindowTitle)
        {
            processNamePrefix = processNamePrefix.ToUpper();
            bool found = false;
            Process[] processlist = Process.GetProcesses();
            foreach (Process theprocess in processlist)
            {
                if (theprocess.ProcessName.ToUpper().StartsWith(processNamePrefix))
                {
                    found = true;
                    break;
                }
                // New LC 29/06/2018 - also check for Plantronics Hub msiexec processes...
                if (theprocess.ProcessName.ToUpper().StartsWith("MSIEXEC"))
                {
                    if (theprocess.MainWindowTitle.ToUpper().StartsWith(msiExecWindowTitle)
                        || theprocess.MainWindowTitle.ToUpper().Contains("PLANTRONICS"))
                    {
                        found = true;
                        break;
                    }
                }
            }
            return found ? 60000 : 5000;
        }

        private void DoTidyUpSDK(bool funcCalls = true)
        {
            if (_sessionManager != null)
            {
                DetachDevice();
                if (_sessionManagerEvents != null)
                {
                    _sessionManagerEvents.onDeviceStateChanged -= _sessionManagerEvents_onDeviceStateChanged;
                }
                if (_session != null)
                {
                    _session.onCallStateChanged -= _session_onCallStateChanged;
                    try
                    {
                        if (funcCalls) _sessionManager.Unregister(_session);
                    }
                    catch (InvalidComObjectException)
                    {
                        // no action, probably means Hub was being uninstalled
                    }
                    catch (COMException)
                    {
                        // no action, probably means Hub was being uninstalled
                    }
                    Marshal.ReleaseComObject(_sessionManager);
                }
                Marshal.ReleaseComObject(_sessionManager);
                OnSDKInfo(new SDKInfoArgs(SDKInfoType.sdk_disconnected, "SDK has been cleaned up"));
            }
            _apiConnected = false;
            // purge pending actions
            lock (actionListLock)
            {
                actionList.Clear();
            }
        }

        private void DoIncomingCall(int callid, string contactname)
        {
            COMCall call = new COMCall() { Id = callid };
            Contact contact = new Contact() { Name = contactname };
            _callCommand.IncomingCall(call, contact, CallRingTone.RingTone_Unknown,
                CallAudioRoute.AudioRoute_ToHeadset);
        }

        private void DoOutgoingCall(int callid, string contactname)
        {
            COMCall call = new COMCall() { Id = callid };
            Contact contact = new Contact() { Name = contactname };
            _callCommand.OutgoingCall(call, contact, CallAudioRoute.AudioRoute_ToHeadset);
        }

        private void DoAnswerCall(int callid)
        {
            COMCall call = new COMCall() { Id = callid };
            _callCommand.AnsweredCall(call);
        }

        private void DoHoldCall(int callid)
        {
            COMCall call = new COMCall() { Id = callid };
            _callCommand.HoldCall(call);
        }

        private void DoResumeCall(int callid)
        {
            COMCall call = new COMCall() { Id = callid };
            _callCommand.ResumeCall(call);
        }

        private void DoMuteCall(bool mute)
        {
            _deviceListener.mute = mute;
        }

        private void DoTerminateCall(int callid)
        {
            COMCall call = new COMCall() { Id = callid };
            _callCommand.TerminateCall(call);
        }

        private void _session_onCallStateChanged(COMCallEventArgs callEventArgs)
        {
            // informs us the calling state has changed, for example user as answered/terminated a call
            // using headset buttons - this event should be used in my app to actually connect/terminate the call!
            OnCallStateChanged(callEventArgs);
        }

        private void _sessionManagerEvents_onDeviceStateChanged(COMStateDeviceEventArgs deviceEventArgs)
        {
            if (deviceEventArgs.DeviceState == DeviceChangeState.DeviceState_Removed)
                OnSDKInfo(new SDKInfoArgs(SDKInfoType.sdk_device_detached, "Device detached."));
            // device may have arrived or been removed. Either way detach device event handlers, then try to re-attach them
            DetachDevice();
            AttachDevice();
        }

        private void AttachDevice()
        {
            // Lets get the primary Plantronics device (if any) and then register
            // for the device event handlers
            try
            {
                _device = _session.GetActiveDevice();
                if (_device != null)
                {
                    // display device information:
                    OnSDKInfo(new SDKInfoArgs(SDKInfoType.sdk_device_attached, "Device attached: " + _device.ProductName + ", Product ID = " + _device.ProductId.ToString("X")));

                    _deviceListenerEvents = (ICOMDeviceListenerEvents_Event)_device.DeviceListener;
                    if (_deviceListenerEvents != null)
                    {
                        _deviceListenerEvents.onHeadsetStateChanged += _deviceListenerEvents_onHeadsetStateChanged;

                        OnSDKInfo(new SDKInfoArgs(SDKInfoType.sdk_notification, "Successfully hooked to device listener events"));
                    }
                    else
                        OnSDKInfo(new SDKInfoArgs(SDKInfoType.sdk_notification, "Unable to hook to device listener events"));

                    _deviceListener = _device.DeviceListener; // Obtain a DeviceListener object for later use

                    OnSDKInfo(new SDKInfoArgs(SDKInfoType.sdk_notification, "Device mute state: muted = " + _deviceListener.mute)); // Obtain initial device microphone mute state
                }
                else
                    OnSDKInfo(new SDKInfoArgs(SDKInfoType.sdk_notification, "Unable to retrieve active device"));
            }
            catch (Exception)
            {
                OnSDKInfo(new SDKInfoArgs(SDKInfoType.sdk_notification, "Unable to retrieve/hook to active device"));
            }
        }

        private void _deviceListenerEvents_onHeadsetStateChanged(COMDeviceListenerEventArgs args)
        {
            // informs us of a variety of Plantronics device state changes
            OnHeadsetStateChanged(args);
        }

        private void DetachDevice()
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

        public void DoHubSDKAction(HubSDKAction action)
        {
            if (_apiConnected || action.ActionType == HubSDKActionType.ConnectToAPI)
            {
                lock (actionListLock)
                {
                    actionList.Enqueue(action);
                }
            }
            else 
            {
                if (action.ActionType != HubSDKActionType.CheckCOMAPIActive)
                {
                    OnSDKError(new SDKErrorArgs(SDKErrorType.sdk_not_connected,
                        "Cannot perform SDK action: " + action.ActionType.ToString() +
                        ", SDK is not connected (is Plantronics Hub running?)"));
                }
            }
        }

        private async Task DoWatchdogTask()
        {
            while (!_quit)
            {
                await Task.Delay(1000);
                DoHubSDKAction(new HubSDKAction(HubSDKActionType.CheckCOMAPIActive));
            }
        }
    }

    public class HubSDKAction
    {
        public HubSDKActionType ActionType { get; set; }
        public int callid { get; set; }
        public string contactname { get; set; }
        public bool mutestate { get; set; }

        // constructor taking only action type, no extra parameters
        public HubSDKAction(HubSDKActionType actionType)
        {
            ActionType = actionType;
            this.callid = 0;
            this.contactname = "";
            this.mutestate = mutestate;
        }
        // constructor for call related actions requiring callid and optionally contactname
        public HubSDKAction(HubSDKActionType actionType, int callid, string contactname="")
        {
            ActionType = actionType;
            this.callid = callid;
            this.contactname = contactname;
            this.mutestate = false;
        }

        // constructor for mute related actions (mute on / off)
        public HubSDKAction(HubSDKActionType actionType, bool mutestate)
        {
            ActionType = actionType;
            this.callid = 0;
            this.contactname = "";
            this.mutestate = mutestate;
        }
    }

    public enum HubSDKActionType
    {
        None,
        IncomingCall,
        OutgoingCall,
        AnswerCall,
        HoldCall,
        ResumeCall,
        SetMute,
        TerminateCall,
        Shutdown,
        CheckCOMAPIActive,
        ConnectToAPI
    }
}
