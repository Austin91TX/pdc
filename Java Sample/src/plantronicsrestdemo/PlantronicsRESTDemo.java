package plantronicsrestdemo;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.StringReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author lcollins
 */
public class PlantronicsRESTDemo {

    static Boolean quit = false;
    static String sessionid = "";
    static Boolean pluginRegistered = false;
    static int callid = 0;
    private static EventsListenerThread eventslistener = null;
    
    private static String BaseURL = "http://127.0.0.1:32017/";
    //private static String BaseURL = "https://127.0.0.1:32018/";
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        System.out.println("Java Plantronics REST API Sample");
        
        DoAttachDevice();
        DoShowDeviceInfo();
        DoStartEventsListener(sessionid);
        
        while (!quit)
        {
            ShowMenu();
            String cmd = ReadCommand();          
            switch (cmd)
            {
                case "1": 
                    DoIncomingCall(++callid, "Bob%20Smith");            
                    break;
                case "2":
                    DoOutgoingCall(++callid, "Bob%20Smith");
                    break;
                case "3":
                    DoAnswerCall(callid);
                    break;
                case "4":
                    DoHoldCall(callid);
                    break;
                case "5":
                    DoResumeCall(callid);
                    break;
                case "6":
                    DoMute(true);
                    break;
                case "7":
                    DoMute(false);
                    break;
                 case "8":
                    DoTerminateCall(callid);
                    break;
                 case "0":
                    quit = true;
                    break;
                 default:
                    System.out.println("Unrecognised menu choice.");
                    break;
            }
        }
        
        // Cleanup the Plantronics REST API
        DoStopEventsListener();
        DoReleaseDevice();
    }

    private static void ShowMenu() {
        System.out.println();
        System.out.println("plt sample menu");
        System.out.println("--");
        System.out.println("1 - ring/incoming call");
        System.out.println("2 - outgoing call");
        System.out.println("3 - answer call");
        System.out.println("4 - hold call");
        System.out.println("5 - resume call");
        System.out.println("6 - mute call");
        System.out.println("7 - unmute call");
        System.out.println("8 - end call");
        System.out.println("0 - quit");
        System.out.println();
        System.out.print("Type a command> ");
    }

    private static String ReadCommand() {
        BufferedReader buffer=new BufferedReader(new InputStreamReader(System.in));
        String line = "";
        try {
            line = buffer.readLine();
        } catch (IOException ex) {
            Logger.getLogger(PlantronicsRESTDemo.class.getName()).log(Level.SEVERE, null, ex);
        }
        return line;
    }

    public static void DoShowDeviceInfo() {
        RESTConvenienceClass.SendRESTCommand(
            BaseURL + "Spokes/DeviceServices/Info"
        );
    }

    public static void DoAttachDevice() {
        String tmpResult = 
            RESTConvenienceClass.SendRESTCommand(
                BaseURL + "Spokes/DeviceServices/Attach?uid=0123456789"
            );
        int pos = tmpResult.indexOf("\"Result\":\"");
        if (pos>-1)
        {
            tmpResult = tmpResult.substring(pos+10);
            pos = tmpResult.indexOf("\"");
            if (pos>-1)
            {
                tmpResult = tmpResult.substring(0, pos);
                System.out.println("Session id is: " + tmpResult);
                sessionid = tmpResult;
            }
            try 
            {
                Thread.sleep(250);
            } 
            catch (InterruptedException ex) 
            {
                Logger.getLogger(PlantronicsRESTDemo.class.getName()).log(Level.SEVERE, null, ex);
            }                                  
                                        
            if (sessionid.length()>0)
            {
                RESTConvenienceClass.SendRESTCommand(
                    BaseURL + "Spokes/SessionManager/Register?name=My%20Java%20Plugin"
                );               
                pluginRegistered = true;
            }
        }
        else     
        {
            System.out.println("Error: Connecting to Device failed, no Result/session detected in response");
        }
    }

    public static void DoReleaseDevice() {
        RESTConvenienceClass.SendRESTCommand(
            BaseURL + "Spokes/SessionManager/UnRegister?name=My%20Java%20Plugin"
        );                
        pluginRegistered = false;

        try 
        {
            Thread.sleep(250);
        } 
        catch (InterruptedException ex) 
        {
            Logger.getLogger(PlantronicsRESTDemo.class.getName()).log(Level.SEVERE, null, ex);
        }      
            
        if (sessionid.length()>0)
        {
            RESTConvenienceClass.SendRESTCommand(
                BaseURL + "Spokes/DeviceServices/Release?sess="
                    + sessionid
            );
            sessionid = "";            
        }
    }

    private static void DoIncomingCall(int callid, String caller_name) {
        if (sessionid.length()>0 && pluginRegistered)
        {
            RESTConvenienceClass.SendRESTCommand(
                BaseURL + "Spokes/CallServices/IncomingCall?name=My%20Java%20Plugin&callID=%7B%22Id%22%3A%22"
                    + callid 
                    + "%22%7D&contact=%7B%22Name%22%3A%22"
                    + caller_name 
                    + "%22%7D&tones=Unknown&route=ToHeadset"
            );
        }
        else
        {
            System.out.println("Error: You need to Attach first (command 2), and register plugin (command 3)");
        }
    }
    
    private static void DoOutgoingCall(int callid, String caller_name) {
        if (sessionid.length()>0 && pluginRegistered)
        {
            RESTConvenienceClass.SendRESTCommand(
                BaseURL + "Spokes/CallServices/OutgoingCall?name=My%20Java%20Plugin&callID=%7B%22Id%22%3A%22"
                    + callid 
                    + "%22%7D&contact=%7B%22Name%22%3A%22"
                    + caller_name 
                    + "%22%7D&route=ToHeadset"
            );
        }
        else
        {
            System.out.println("Error: You need to Attach first (command 2), and register plugin (command 3)");
        }
    }

        private static void DoTerminateCall(int callid) {
        if (sessionid.length()>0 && pluginRegistered)
        {
            RESTConvenienceClass.SendRESTCommand(
                BaseURL + "Spokes/CallServices/TerminateCall?name=My%20Java%20Plugin&callID=%7B%22Id%22%3A%22"
                    + callid 
                    + "%22%7D"
            );
        }
        else
        {
            System.out.println("Error: You need to Attach first (command 2), and register plugin (command 3)");
        }
    }

    private static void DoAnswerCall(int callid) {
        if (sessionid.length()>0 && pluginRegistered)
        {
            RESTConvenienceClass.SendRESTCommand(
                BaseURL + "Spokes/CallServices/AnswerCall?name=My%20Java%20Plugin&callID=%7B%22Id%22%3A%22"
                    + callid 
                    + "%22%7D"
            );
        }
        else
        {
            System.out.println("Error: You need to Attach first (command 2), and register plugin (command 3)");
        }
    }

    private static void DoHoldCall(int callid) {
        if (sessionid.length()>0 && pluginRegistered)
        {
            RESTConvenienceClass.SendRESTCommand(
                BaseURL + "Spokes/CallServices/HoldCall?name=My%20Java%20Plugin&callID=%7B%22Id%22%3A%22"
                    + callid 
                    + "%22%7D"
            );
        }
        else
        {
            System.out.println("Error: You need to Attach first (command 2), and register plugin (command 3)");
        }
    }

    private static void DoResumeCall(int callid) {
        if (sessionid.length()>0 && pluginRegistered)
        {
            RESTConvenienceClass.SendRESTCommand(
                BaseURL + "Spokes/CallServices/ResumeCall?name=My%20Java%20Plugin&callID=%7B%22Id%22%3A%22"
                    + callid 
                    + "%22%7D"
            );
        }
        else
        {
            System.out.println("Error: You need to Attach first (command 2), and register plugin (command 3)");
        }
    }

    private static void DoMute(boolean muted) {
        if (sessionid.length()>0 && pluginRegistered)
        {
            RESTConvenienceClass.SendRESTCommand(
                BaseURL + "Spokes/CallServices/MuteCall?name=My%20Java%20Plugin&muted="
                    + muted 
            );
        }
        else
        {
            System.out.println("Error: You need to Attach first (command 2), and register plugin (command 3)");
        }
    }    

    private static void DoStartEventsListener(String sessid) {
        if (eventslistener==null)
        {
            eventslistener = new EventsListenerThread(sessid, "My%20Java%20Plugin");
            eventslistener.start();
        }
    }
    
    private static void DoStopEventsListener() {
        if (eventslistener!=null)
        {
            eventslistener.shutdown();
            eventslistener = null;
        }
    }

    private static void DoGetCallManagerState() {
        if (sessionid.length()>0 && pluginRegistered)
        {
            RESTConvenienceClass.SendRESTCommand(
                BaseURL + "Spokes/CallServices/CallManagerState"
           );
        }
        else
        {
            System.out.println("Error: You need to Attach first (command 2), and register plugin (command 3)");
        }
    }
}
class EventsListenerThread extends Thread
{
    private Boolean quit = false;
    private Boolean deviceAttached = true;
    private String sessid = "";
    private String plugin_name;
    private static String BaseURL = "http://127.0.0.1:32017/";
    //private static String BaseURL = "https://127.0.0.1:32018/";
    
    public EventsListenerThread(String sessid, String plugin_name)
    {
        this.sessid = sessid;
        this.plugin_name = plugin_name;
        if (sessid.length()==0)
        {
            deviceAttached = false;
        }
    }
    
    public void run ()
    {
        System.out.println("Events Listener Starting");
        while (!quit) {
            Sleep(1800);
            if (!quit)
            {
                System.out.println();
                System.out.println();
                if (!deviceAttached)
                {
                    System.out.println("-- POLLING FOR DEVICE RE-ATTACH --");
                    PlantronicsRESTDemo.DoAttachDevice();
                    if (PlantronicsRESTDemo.sessionid.length()>0)
                    {
                        sessid = PlantronicsRESTDemo.sessionid;
                        deviceAttached = true;
                        PlantronicsRESTDemo.DoShowDeviceInfo();
                    }
                }
                else
                {
                    System.out.println("-- POLLING FOR EVENTS --");
                    // deviceEvents informs us of a variety of Plantronics device state changes
                    // SPECIAL CASE device removal - with the REST API, when device is removed
                    // polling for device events will return error Error of type Invalid session id
                    String deviceEvents = RESTConvenienceClass.SendRESTCommandWithDebugPrompt(
                        BaseURL + "Spokes/DeviceServices/Events?sess="
                            + sessid
                            + "&queue=0"
                        , "DEVICE EVENTS", true
                    );
                    if(deviceEvents.contains("Invalid session id") || deviceEvents.contains("Empty session id"))
                    {
                        System.out.println("-- ** DEVICE DETACHED / SESSION INVALID ** --");                    
                        deviceAttached = false;
                        PlantronicsRESTDemo.sessionid = "";
                        PlantronicsRESTDemo.DoReleaseDevice();
                    }
                    Sleep(200);            
                    if (!quit && deviceAttached)
                    {
                        // session Call Events informs us the calling state has changed, for example user as answered/terminated a call
                        // using headset buttons - this event should be used in my app to actually connect/terminate the call!
                        String callEvents = RESTConvenienceClass.SendRESTCommandWithDebugPrompt(
                            BaseURL + "Spokes/CallServices/CallEvents?name=My%20Java%20Plugin"
                            , "CALL EVENTS", true
                        );
                        ParseCallEvents(callEvents);
                    }
                }
            }
        }
        System.out.println("Events Listener Stopping");
    }
    
    private void ParseCallEvents(String callEvents)
    {
        if (callEvents.contains("[") || callEvents.contains("\"Action\":"))
        {
            callEvents = callEvents.substring(callEvents.indexOf("["));
            callEvents = callEvents.substring(0, callEvents.indexOf("]"));
            List<String> actions = Arrays.asList(callEvents.split("\"Action\":"));
            String callId;
            for(Iterator<String> i = actions.iterator(); i.hasNext(); ) {
                String item = i.next();
                int pos = item.indexOf(",");
                if (pos>-1)
                {
                    callId = item.substring(item.indexOf("\"Id\":") + 5);
                    callId = callId.substring(0, callId.indexOf(","));
                    System.out.println("CallState: " + RESTConvenienceClass.eCallState.get(item.substring(0, pos)) +
                            ", " + "Call ID: " + callId);
                }
            }
        }
    }
    
    public void shutdown()
    {
        quit = true;
    }

    private void Sleep(int millis) {
        try {        
            Thread.sleep(millis);
        } catch (InterruptedException ex) {
            Logger.getLogger(EventsListenerThread.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
}

class RESTConvenienceClass {
    public static String SendRESTCommand(String restcommand) {
        return SendRESTCommandWithDebugPrompt(restcommand, "RESULT", true);
    }
     
    public static String SendRESTCommandWithDebugPrompt(String restcommand, String debugprompt, Boolean showsent) {
        String result = "";
        if (showsent) System.out.println("Sending REST Command: " + restcommand);
        try {
            result = getHTML(restcommand);
            System.out.println(debugprompt + " = " + result);            
        } catch (Exception ex) {
            Logger.getLogger(PlantronicsRESTDemo.class.getName()).log(Level.SEVERE, null, ex);
        }
        return result;
    }
    
    // thanks: http://stackoverflow.com/questions/1485708/how-do-i-do-a-http-get-in-java
    public static String getHTML(String urlToRead) throws Exception {
        StringBuilder result = new StringBuilder();
        URL url = new URL(urlToRead);
        HttpURLConnection conn = (HttpURLConnection) url.openConnection();
        conn.setRequestMethod("GET");
        BufferedReader rd = new BufferedReader(new InputStreamReader(conn.getInputStream()));
        String line;
        while ((line = rd.readLine()) != null) {
            result.append(line);
        }
        rd.close();
        return result.toString();
    }    

    public static Map<String, String> eCallState = new HashMap<String, String>() {{
        put("0", "CALL_STATE_UNKNOWN");
        put("1", "CALL_STATE_ACCEPT_CALL");
        put("2", "CALL_STATE_TERMINATE_CALL");
        put("3", "CALL_STATE_HOLD_CALL");
        put("4", "CALL_STATE_RESUME_CALL");
        put("5", "CALL_STATE_FLASH");
        put("6", "CALL_STATE_CALL_IN_PROGRESS");
        put("7", "CALL_STATE_CALL_RINGING");
        put("8", "CALL_STATE_CALL_ENDED");
        put("9", "CALL_STATE_TRANSFER_TO_HEADSET");
        put("10", "CALL_STATE_TRANSFER_TO_SPEAKER");
        put("11", "CALL_STATE_MUTEON");
        put("12", "CALL_STATE_MUTEOFF");
        put("13", "CALL_STATE_MOBILE_CALL_RINGING");
        put("14", "CALL_STATE_MOBILE_CALL_IN_PROGRESS");
        put("15", "CALL_STATE_MOBILE_CALL_ENDED");
        put("16", "CALL_STATE_DON");
        put("17", "CALL_STATE_DOFF");
        put("18", "CALL_STATE_CALL_IDLE");
        put("19", "CALL_STATE_PLAY");
        put("20", "CALL_STATE_PAUSE");
        put("21", "CALL_STATE_STOP");
        put("22", "CALL_STATE_DTMF_KEY");
        put("23", "CALL_STATE_REJECT_CALL");
	put("24", "CALL_STATE_MAKE_CALL");
	put("25", "CALL_STATE_HOOK");
	put("26", "CALL_STATE_HOOK_IDLE");
	put("27", "CALL_STATE_HOOK_DOCKED");
	put("28", "CALL_STATE_HOOK_UNDOCKED");
	put("29", "CALL_STATE_BASE_EVENT");
        put("30", "CALL_STATE_CALL_ANSWERED_AND_ENDED");
        put("31", "CALL_STATE_CALL_UNANSWERED_AND_ENDED");
        put("32", "CALL_STATE_DEVICE_CHANGE");
        put("33", "CALL_STATE_DEVICE_ARRIVED");
	put("34", "CALL_STATE_DEVICE_REMOVED");        
    }};
}
