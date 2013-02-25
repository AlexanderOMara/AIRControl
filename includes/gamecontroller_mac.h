/*
Copyright (c) 2012-2013 Alexander O'Mara alexomara.com

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*/

//The following frameworks must be added.
//IOKit.framework
//CoreFoundation.framework
#include <sstream>
#include <vector>
#include <IOKit/hid/IOHIDLib.h>
#include <iostream>//TODO: Remove this.

#include <limits.h>
#include <mach/mach.h>
#include <mach/mach_time.h>

//Set the total supported controller ID's (16 usually).
static unsigned int controllers = 0;
//Vector of cached controller names read from the registry.
static std::vector<std::string> controllerNames;//(controllers);
//The HID manager.
static IOHIDManagerRef hidManager = NULL;
//The devices to match againts.
static CFArrayRef deviceMatch = NULL;
//The hypothetical axes names and counters to keep track of them.
static std::string axesNames[] = {"x", "y", "z", "r", "u", "v", "?"};
static unsigned int axesNamesLength = 7;
static unsigned int axesNamesIndex = 0;
static unsigned int povNamesIndex = 0;

//Converts uint to string.
static std::string uintToString(unsigned int i)
{
	std::stringstream ss;
	ss << i;
	return ss.str();
}

//Converts int to string.
static std::string intToString(int i)
{
	std::stringstream ss;
	ss << i;
	return ss.str();
}

//Converts float to string.
static std::string floatToString(float i)
{
	std::stringstream ss;
	ss << i;
	return ss.str();
}

//Replaces " with '.
static std::string replaceQuotes(std::string str)
{
	unsigned int i,
		str_size = str.size();
	for(i = 0; i < str_size; i++)
	{
		if(str[i] == '"')
		{
			str[i] = '\'';
		}
	}
	return str;
}

//Returns an XML string of all the game controller states.
static std::string controllerName(IOHIDDeviceRef device, std::string fallbackName)
{
	CFStringRef productKey;
	std::string controllerDescription;
	
	//Get the reference to the name.
	productKey = (CFStringRef)IOHIDDeviceGetProperty(device, CFSTR(kIOHIDProductKey));
	//Check the reference.
	if(productKey == NULL || CFGetTypeID(productKey) != CFStringGetTypeID())
	{
		controllerDescription = fallbackName;
	}
	else
	{
		char * controllerName;
		CFIndex length;
		
		//Copy the name from the reference into an string.
		CFStringGetBytes(productKey, CFRangeMake(0, CFStringGetLength(productKey)), kCFStringEncodingUTF8, '?', false, NULL, 100, &length);
		controllerName = (char *)malloc(length + 1);
		CFStringGetBytes(productKey, CFRangeMake(0, CFStringGetLength(productKey)), kCFStringEncodingUTF8, '?', false, (UInt8 *) controllerName, length + 1, NULL);
		//Null terminate the string.
		controllerName[length] = '\x00';
		//Set it and free it.
		controllerDescription = replaceQuotes(controllerName);
		free(controllerName);
	}
	
	//If the name is still blank, use the fallback.
	if(controllerDescription == "")
	{
		controllerDescription = fallbackName;
	}
	
	return controllerDescription;
}

//Returns an XML string of all the game controller states.
std::string ControlStates()
{
	std::string state;
	
	//The string we will output the states as.
	state = "<r>";
	
	//If this is the first run, create the hidManager and the deviceMatch.
	if(hidManager == NULL)
	{
		CFStringRef keys[2];
		int value;
		CFNumberRef values[2];
		CFDictionaryRef dictionaries[3];
		//CFArrayRef array;
		
		//Create HID Manager reference with default selector and blank options.
		hidManager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
		
		//Create keys.
		keys[0] = CFSTR(kIOHIDDeviceUsagePageKey);
		keys[1] = CFSTR(kIOHIDDeviceUsageKey);
		
		//Create joystick values.
		value = kHIDPage_GenericDesktop;
		values[0] = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &value);
		value = kHIDUsage_GD_Joystick;
		values[1] = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &value);
		//Add the joystick dictionary to the list of dictionaries.
		dictionaries[0] = CFDictionaryCreate(kCFAllocatorDefault, (const void **) keys, (const void **) values, 2, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
		//Free the joystick values.
		CFRelease(values[0]);
		CFRelease(values[1]);
		
		//Create gamepad values.
		value = kHIDPage_GenericDesktop;
		values[0] = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &value);
		value = kHIDUsage_GD_GamePad;
		values[1] = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &value);
		//Add the gamepad dictionary to the list of dictionaries.
		dictionaries[1] = CFDictionaryCreate(kCFAllocatorDefault, (const void **) keys, (const void **) values, 2, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
		//Free the gamepad values.
		CFRelease(values[0]);
		CFRelease(values[1]);
		
		//Create multi-axis controller values.
		value = kHIDPage_GenericDesktop;
		values[0] = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &value);
		value = kHIDUsage_GD_MultiAxisController;
		values[1] = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &value);
		//Add the multi-axis controller dictionary to the list of dictionaries.
		dictionaries[2] = CFDictionaryCreate(kCFAllocatorDefault, (const void **) keys, (const void **) values, 2, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
		//Free the multi-axis controller values.
		CFRelease(values[0]);
		CFRelease(values[1]);
		
		//Create CFArrayRef from the dictionaries.
		deviceMatch = CFArrayCreate(kCFAllocatorDefault, (const void **) dictionaries, 3, &kCFTypeArrayCallBacks);
		//Free the dictionaries.
		CFRelease(dictionaries[0]);
		CFRelease(dictionaries[1]);
		CFRelease(dictionaries[2]);
		
		//Open the HID manager reference with blank options (Apple thinks this is necessary, but I'm not so sure).
		IOHIDManagerOpen(hidManager, kIOHIDOptionsTypeNone);
	}
	
	//Match HID devices from the matching array.
	IOHIDManagerSetDeviceMatchingMultiple(hidManager, deviceMatch);
	
	//Copy out devices.
	CFSetRef devices = IOHIDManagerCopyDevices(hidManager);
	
	//Make sure devices match.
	if(devices != NULL)
	{
		//How many devices?
		CFIndex controller,
			deviceCount = CFSetGetCount(devices);
		
		//Check if the controllers have changed, erase the name cache if they have.
		if(controllers != deviceCount)
		{
			controllerNames = std::vector<std::string>(deviceCount);
			controllers = deviceCount;
		}
		
		//Memory to extract device refs to.
		IOHIDDeviceRef * deviceRefs = (IOHIDDeviceRef*)malloc(sizeof(IOHIDDeviceRef) * deviceCount);
		
		//Extract device refs.
		CFSetGetValues(devices, (const void **)deviceRefs);
		
		//Loop over the devices.
		for(controller = 0; controller < deviceCount; controller++)
		{
			IOHIDDeviceRef device;
			IOHIDElementRef element;
			IOHIDElementType type;
			IOHIDValueRef value;
			CFArrayRef elements;
			CFIndex elementIndex,
				elementCount;
			std::string axesStates,
				buttonStates;
			
			axesStates = "";
			buttonStates = "";
			axesNamesIndex = 0;
			povNamesIndex = 0;
			
			//Get the current device.
			device = deviceRefs[controller];
			
			//If the controller name hasn't been set, look it up and cache it.
			if(controllerNames[controller] == "")
			{
				controllerNames[controller] = controllerName(device, "Controller " + uintToString(controller));
			}
			//Add controller to the list.
			state += "<c id=\"" + uintToString(controller) + "\" name=\"" + controllerNames[controller] + "\">";
			
			//Get the input elements from the controller.
			elements = IOHIDDeviceCopyMatchingElements(device, NULL, kIOHIDOptionsTypeNone);
			elementCount = CFArrayGetCount(elements);
			
			//Loop over the elements.
			for(elementIndex = 0; elementIndex < elementCount; elementIndex++)
			{
				//Get the current element.
				element = (IOHIDElementRef)CFArrayGetValueAtIndex(elements, elementIndex);
				//Get the type of element.
				type = IOHIDElementGetType(element);
				
				//Check the type, axes are more likely to be a Misc than an Axis for some reason.
				if(type == kIOHIDElementTypeInput_Misc || type == kIOHIDElementTypeInput_Axis)
				{
					//Read the value fron the element of the device.
					IOHIDDeviceGetValue(device, element, &value);
					
					//Workaround for a supposed bug where this value is impossibly high.
					if(IOHIDValueGetLength(value) > 4)
					{
						continue;
					}
					
					CFIndex integerValue;
					
					//Get the value from the controller element.
					integerValue = IOHIDValueGetIntegerValue(value);
					
					//Check if POV element, which are 2 axes in one.
					if(IOHIDElementGetUsage(element) == kHIDUsage_GD_Hatswitch)
					{
						//Workaround for POV hat switches that do not have null states.
						if(!IOHIDElementHasNullState(element))
						{
							CFIndex logicalMin,
								logicalMax;
							
							//Get the minimum and maximum values.
							logicalMin = IOHIDElementGetLogicalMin(element);
							logicalMax = IOHIDElementGetLogicalMax(element);
							
							integerValue = integerValue < logicalMin ? logicalMax - logicalMin + 1 : integerValue - 1;
						}
						
						//TODO: POV STATES
						axesStates += "<d p=\"" + axesNames[povNamesIndex >= axesNamesLength ? axesNamesLength-1 : povNamesIndex++] + "\">" + "</d>";
						axesStates += "<d p=\"" + axesNames[povNamesIndex >= axesNamesLength ? axesNamesLength-1 : povNamesIndex++] + "\">" + "</d>";
					}
					else
					{
						float floatValue;
						CFIndex logicalMin,
							logicalMax;
						
						//Get the minimum and maximum values.
						logicalMin = IOHIDElementGetLogicalMin(element);
						logicalMax = IOHIDElementGetLogicalMax(element);
						
						//Calculate the -1 to 1 float from the min and max possible values.
						floatValue = (integerValue - logicalMin) / (float) (logicalMax - logicalMin) * 2.0f - 1.0f;
						
						axesStates += "<d a=\"" + axesNames[axesNamesIndex >= axesNamesLength ? axesNamesLength-1 : axesNamesIndex++] + "\">" + floatToString(floatValue) + "</d>";
					}
				}
				else if(type == kIOHIDElementTypeInput_Button)
				{
					//Read the value fron the element of the device.
					IOHIDDeviceGetValue(device, element, &value);
					buttonStates += IOHIDValueGetIntegerValue(value) ? "1" : "0";
				}
			}
			state += "<a>" + axesStates + "</a><b>" + buttonStates + "</b></c>";
			//Free the elements.
			CFRelease(elements);
		}
	}
	
	//Finish the string and return.
	state += "</r>";
	return state;
}
