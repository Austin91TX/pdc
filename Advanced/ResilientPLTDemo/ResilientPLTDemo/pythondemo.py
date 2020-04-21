# Test python script to interact with Hub SDK
# Lewis.Collins@poly.com 21st Apr 2020
#
# Pre-requisites:
# --
# 1. http://www.plantronics.com/software
#    - download Latest Hub for Windows
#
# 2. https://www.python.org/downloads/
#    - download and install Python 3.4.1 for Windows 64-bit or 32-bit
#
# 3. www.lfd.uci.edu/~gohlke/pythonlibs/#pythonnet
#    - download and run installer: 64-bit: pythonnet-2.0.0.win-amd64-py3.4.exe, 32-bit: pythonnet-2.0.0.win32-py3.4.exe
#     (this is the Python for .NET module)
#
# 4. ResilientPLTDemo - HubSDKConnector
#    - Connector class for consuming Hub SDK
#
# **NOTE**: ensure you match system architectures 64-bit or 32-bit!!!
# The "python" and "Python for .NET" must match, also the .NET Assembly must
# target same CPU: for 64-bit should target "x64 or Any", and for 32-bit should target "x86"
#
# More info:
# --
# http://pythonnet.sourceforge.net/readme.html#getting_started
# - this is the reference guide for Python for .NET
#
# How to run:
# --
#   Once you have the above pre-requisites installed:
#   From Command Prompt type:
#
#		python pythondemo.py
#
# pythondemo.py
# 
# Test python script to interact with Hub SDK
#
# Author: Lewis Collins
# 
# Contact: Lewis.Collins@poly.com
# 
# VERSION HISTORY:
# ********************************************************************************
# Version 0.0.0.1:
# Date: 21st April 2020
# Changed by: Lewis Collins
#   Changes:
#     - Initial version, demonstrates monitoring of Call State and Headset State change events
# ********************************************************************************
#
# Have fun! ;-)
#

import msvcrt
import clr
import time
clr.AddReference('System.Collections')
hubsdkconnector = clr.AddReference("ResilientPLTDemo")

from System import String, Int32
from System.Collections.Generic import List
p = String("Welcome to ResilientPLTDemoPythonTest")		# test that .NET is working
print (p)

print (hubsdkconnector.FullName)

from ResilientPLTDemo import HubSDKConnector

MY_APP_NAME = "ResilientPLTDemoPythonTest"

def _hubSDK_SDKError(sender, args):
	print ("_hubSDK_SDKError:")
	print ("Event Type: ", args.EventType, ", Message: ", args.msg, "\r\n")

def _hubSDK_SDKInfo(sender, args):
	print ("_hubSDK_SDKInfo:")
	print ("Event Type: ", args.EventType, ", Message: ", args.msg, "\r\n")

def _hubSDK_CallStateChangedPy(callId, callState, callStateStr, callSource):
	print ("_hubSDK_CallStateChangedPy:")
	print ("callId=", callId)
	print ("callState=", callState)
	print ("callStateStr=", callStateStr)
	print ("callSource=", callSource, "\r\n")

def _hubSDK_HeadsetStateChangedPy( deviceEventType, deviceEventTypeStr, headsetStateChange, headsetStateChangeStr):
	print ("_hubSDK_HeadsetStateChanged:")
	print ("deviceEventType=", deviceEventType)
	print ("deviceEventTypeStr=", deviceEventTypeStr)
	print ("headsetStateChange=", headsetStateChange)
	print ("headsetStateChangeStr=", headsetStateChangeStr, "\r\n")

# 1. Setup Hub SDK
hubsdk = HubSDKConnector(MY_APP_NAME)
hubsdk.SDKError += _hubSDK_SDKError
hubsdk.SDKInfo += _hubSDK_SDKInfo
hubsdk.CallStateChangedPy += _hubSDK_CallStateChangedPy
hubsdk.HeadsetStateChangedPy += _hubSDK_HeadsetStateChangedPy

# 2. Wait for enter
input("Press Enter to quit...\r\n\r\n")

#time.sleep(1)	 #wait for 1 second, *warning* without this Hub will crash!

# 3. Shutdown Hub SDK
hubsdk.SDKError -= _hubSDK_SDKError
hubsdk.SDKInfo -= _hubSDK_SDKInfo
hubsdk.CallStateChangedPy -= _hubSDK_CallStateChangedPy
hubsdk.HeadsetStateChangedPy -= _hubSDK_HeadsetStateChangedPy
print ("Bye")