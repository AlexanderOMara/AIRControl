/*
Copyright (C) 2012-2013 Alexander O'Mara alexomara.com

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

//Data structure for controller axes.
struct ControlDeviceElementAxis {
	IOHIDElementRef element;
	IOHIDElementCookie cookie;
	CFIndex logicalMin;
	CFIndex logicalMax;
	float state;
};

//Data structure for controller POVs.
struct ControlDeviceElementPOV {
	IOHIDElementRef element;
	IOHIDElementCookie cookie;
	CFIndex logicalMin;
	CFIndex logicalMax;
	bool hasNullState;
	int state;
};

//Data structure for controller axes.
struct ControlDeviceElementButton {
	IOHIDElementRef element;
	IOHIDElementCookie cookie;
	bool state;
};

//Data structure for controllers.
struct ControlDevice {
	IOHIDDeviceRef device;
	int ID;
	std::string * IDstr;
	std::string * name;
	int vendorID;
	std::string * vendorIDstr;
	int productID;
	std::string * productIDstr;
	std::string * axesLetters;
	std::vector<ControlDeviceElementAxis *> * elementsAxes;
	std::vector<ControlDeviceElementPOV *> * elementsPOV;
	std::vector<ControlDeviceElementButton *> * elementsButton;
	unsigned int elementsAxesCount;
	unsigned int elementsPOVCount;
	unsigned int elementsButtonCount;
	ControlDevice()
	{
		device = NULL;
		name = new std::string();
		ID = 0;
		IDstr = new std::string();
		vendorID = 0;
		vendorIDstr = new std::string();
		productID = 0;
		productIDstr = new std::string();
		axesLetters = new std::string();
		elementsAxes = new std::vector<ControlDeviceElementAxis *>();
		elementsPOV = new std::vector<ControlDeviceElementPOV *>();
		elementsButton = new std::vector<ControlDeviceElementButton *>();
		elementsAxesCount = 0;
		elementsPOVCount = 0;
		elementsButtonCount = 0;
	}
	~ControlDevice()
	{
		unsigned int elementIndex;
		
		//Dispose of all the elements.
		for(elementIndex = 0; elementIndex < elementsAxesCount; elementIndex++)
		{
			delete elementsAxes->at(elementIndex);
		}
		elementsAxes->clear();
		for(elementIndex = 0; elementIndex < elementsPOVCount; elementIndex++)
		{
			delete elementsPOV->at(elementIndex);
		}
		elementsPOV->clear();
		for(elementIndex = 0; elementIndex < elementsButtonCount; elementIndex++)
		{
			delete elementsButton->at(elementIndex);
		}
		elementsButton->clear();
		
		delete IDstr;
		delete name;
		delete vendorIDstr;
		delete productIDstr;
		delete axesLetters;
		delete elementsAxes;
		delete elementsPOV;
		delete elementsButton;
	}
};

//The HID manager.
static IOHIDManagerRef hidManager = NULL;
//The list of connected devices.
static std::vector<ControlDevice *> devices;
//Unique identifier counter.
static unsigned int deviceID = 0;
//The hypothetical axes names.
static char axesNames[] = {'X', 'Y', 'Z', 'R', 'U', 'V', '?'};
static unsigned int axesNamesLength = 7;

//Converts uint to string.
static std::string uintToString(unsigned int i)
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

//Replaces characters.
static std::string replaceCharacters(std::string str, char find, char replace)
{
	unsigned int i,
		str_size;
	str_size = str.size();
	for(i = 0; i < str_size; i++)
	{
		if(str[i] == find)
		{
			str[i] = replace;
		}
	}
	return str;
}

//Reads properties of a device as an integer.
static int IOHIDDeviceGetPropertyInt(IOHIDDeviceRef deviceRef, CFStringRef key) {
	CFTypeRef propertyRef;
	//Get the reference to the property.
	propertyRef = IOHIDDeviceGetProperty(deviceRef, key);
	if (propertyRef == NULL || CFGetTypeID(propertyRef) != CFNumberGetTypeID()) {
		return 0;
	}
	int value;
	//Copy the referenced value into value.
	CFNumberGetValue((CFNumberRef)propertyRef, kCFNumberSInt32Type, &value);
	return value;
}

//Looks up a controller name.
static std::string controllerName(IOHIDDeviceRef deviceRef, std::string fallbackName)
{
	CFStringRef productKey;
	std::string controllerName;
	
	//Get the reference to the name.
	productKey = (CFStringRef)IOHIDDeviceGetProperty(deviceRef, CFSTR(kIOHIDProductKey));
	
	//Check the reference.
	if(productKey == NULL || CFGetTypeID(productKey) != CFStringGetTypeID())
	{
		controllerName = fallbackName;
	}
	else
	{
		char * controllerKey;
		CFIndex length;
		//Copy the name from the reference into an string.
		CFStringGetBytes(productKey, CFRangeMake(0, CFStringGetLength(productKey)), kCFStringEncodingUTF8, '?', false, NULL, 100, &length);
		controllerKey = (char *)malloc(length + 1);
		CFStringGetBytes(productKey, CFRangeMake(0, CFStringGetLength(productKey)), kCFStringEncodingUTF8, '?', false, (UInt8 *)controllerKey, length + 1, NULL);
		//Null terminate the string.
		controllerKey[length] = '\x00';
		//Set it and free it.
		controllerName = controllerKey;
		free(controllerKey);
	}
	
	//If the name is still blank, use the fallback.
	if(controllerName == "")
	{
		controllerName = fallbackName;
	}
	
	return controllerName;
}

static void deviceValueChanged(void * context, IOReturn result, void * sender, IOHIDValueRef valueRef)
{
	ControlDevice * device;
	IOHIDElementRef element;
	IOHIDElementCookie cookie;
	IOHIDElementType type;
	unsigned int elementIndex;
		
	//Get information on the device and element changed.
	device = (ControlDevice *)context;
	element = IOHIDValueGetElement(valueRef);
	cookie = IOHIDElementGetCookie(element);
	type = IOHIDElementGetType(element);
	
	//Check the type of element changed and update the current value.
	switch(type)
	{
		case kIOHIDElementTypeInput_Button:
		{
			ControlDeviceElementButton * elementButton;
			for(elementIndex = 0; elementIndex < device->elementsButtonCount; elementIndex++)
			{
				elementButton = device->elementsButton->at(elementIndex);
				//Check element identifier for a match.
				if(cookie == elementButton->cookie)
				{
					elementButton->state = IOHIDValueGetIntegerValue(valueRef);
					break;
				}
			}
		}
		break;
		case kIOHIDElementTypeInput_Misc:
		case kIOHIDElementTypeInput_Axis:
		{
			//Workaround for a supposed bug where this value is impossibly high.
			if(IOHIDValueGetLength(valueRef) > 4)
			{
				return;
			}
			
			int value = IOHIDValueGetIntegerValue(valueRef);
			
			//Check if POV element.
			if(IOHIDElementGetUsage(element) == kHIDUsage_GD_Hatswitch)
			{
				ControlDeviceElementPOV * elementPOV;
				for(elementIndex = 0; elementIndex < device->elementsPOVCount; elementIndex++)
				{
					elementPOV = device->elementsPOV->at(elementIndex);
					//Check element identifier for a match.
					if(cookie == elementPOV->cookie)
					{
						//Workaround for POV hat switches that do not have null states.
						if(!elementPOV->hasNullState)
						{
							value = value < elementPOV->logicalMin ? elementPOV->logicalMax - elementPOV->logicalMin + 1 : value - 1;
						}
						elementPOV->state = value;
						break;
					}
				}
			}
			else
			{
				ControlDeviceElementAxis * elementAxis;
				for(elementIndex = 0; elementIndex < device->elementsAxesCount; elementIndex++)
				{
					elementAxis = device->elementsAxes->at(elementIndex);
					//Check element identifier for a match.
					if(cookie == elementAxis->cookie)
					{
						//Sanity check.
						if(value < elementAxis->logicalMin)
						{
							value = elementAxis->logicalMin;
						}
						if(value > elementAxis->logicalMax)
						{
							value = elementAxis->logicalMax;
						}
						//Calculate the -1 to 1 float from the min and max possible values.
						elementAxis->state = (value - elementAxis->logicalMin) / (float)(elementAxis->logicalMax - elementAxis->logicalMin) * 2.0f - 1.0f;
						break;
					}
				}
			}
		}
		break;
		default:
		break;
	}
}

//Callback for a matching device being attached.
static void deviceMatchingCallback(void * context, IOReturn result, void * sender, IOHIDDeviceRef deviceRef)
{
	CFArrayRef elements;
	CFIndex elementIndex,
		elementCount;
	
	//Create a device object to store information on the device.
	ControlDevice * device = new ControlDevice();
	device->ID = deviceID++;
	device->IDstr->assign(uintToString(device->ID));
	device->device = deviceRef;
	device->vendorID = IOHIDDeviceGetPropertyInt(deviceRef, CFSTR(kIOHIDVendorIDKey));
	device->vendorIDstr->assign(uintToString(device->vendorID));
	device->productID = IOHIDDeviceGetPropertyInt(deviceRef, CFSTR(kIOHIDProductIDKey));
	device->productIDstr->assign(uintToString(device->productID));
	device->name->assign(replaceCharacters(controllerName(deviceRef, ""), '\t', ' '));
	if(device->name->empty())
	{
		device->name->assign("Controller " + uintToString(deviceID));
	}
	
	//Get the input elements from the controller.
	elements = IOHIDDeviceCopyMatchingElements(deviceRef, NULL, kIOHIDOptionsTypeNone);
	elementCount = CFArrayGetCount(elements);
	
	//Loop over the elements.
	for(elementIndex = 0; elementIndex < elementCount; elementIndex++)
	{
		IOHIDElementRef element;
		IOHIDElementType type;
		
		//Get the current element.
		element = (IOHIDElementRef)CFArrayGetValueAtIndex(elements, elementIndex);
		//Get the type of element.
		type = IOHIDElementGetType(element);
		
		//Check the type, axes are more likely to be a Misc than an Axis for some reason.
		switch(type)
		{
			case kIOHIDElementTypeInput_Button:
			{
				ControlDeviceElementButton * buttonElement = new ControlDeviceElementButton();
				buttonElement->element = element;
				buttonElement->cookie = IOHIDElementGetCookie(element);
				
				device->elementsButton->push_back(buttonElement);
			}
			break;
			case kIOHIDElementTypeInput_Misc:
			case kIOHIDElementTypeInput_Axis:
			{
				//Check if POV element.
				if(IOHIDElementGetUsage(element) == kHIDUsage_GD_Hatswitch)
				{
					ControlDeviceElementPOV * povElement = new ControlDeviceElementPOV();
					povElement->element = element;
					povElement->cookie = IOHIDElementGetCookie(element);
					povElement->logicalMin = IOHIDElementGetLogicalMin(element);
					povElement->logicalMax = IOHIDElementGetLogicalMax(element);
					povElement->hasNullState = !!IOHIDElementHasNullState(element);

					device->elementsPOV->push_back(povElement);
				}
				else
				{
					ControlDeviceElementAxis * axisElement = new ControlDeviceElementAxis();
					axisElement->element = element;
					axisElement->cookie = IOHIDElementGetCookie(element);
					axisElement->logicalMin = IOHIDElementGetLogicalMin(element);
					axisElement->logicalMax = IOHIDElementGetLogicalMax(element);
					
					//Add the axis letter.
					device->axesLetters->push_back(axesNames[device->elementsAxes->size() < axesNamesLength ? device->elementsAxes->size() : axesNamesLength-1]);
					
					device->elementsAxes->push_back(axisElement);
				}
			}
			break;
			default:
			break;
		}
	}
	//Free memory.
	CFRelease(elements);
	
	//Cache the element counts.
	device->elementsAxesCount = device->elementsAxes->size();
	device->elementsPOVCount = device->elementsPOV->size();
	device->elementsButtonCount = device->elementsButton->size();
	
	//Add to the list of devices.
	devices.push_back(device);
	
	//Register input callbacks on the device, passing a pointer to the device in the callback.
	IOHIDDeviceRegisterInputValueCallback(deviceRef, deviceValueChanged, device);
}

//Callback for a matching device being removed.
static void deviceRemovalCallback(void * context, IOReturn result, void * sender, IOHIDDeviceRef deviceRef)
{
	unsigned int devicesCount,
		devicesIndex;
	
	//Loop over the devices.
	devicesCount = devices.size();
	for(devicesIndex = 0; devicesIndex < devicesCount; devicesIndex++)
	{
		//Check if this device is the one that was removed.
		if(devices.at(devicesIndex)->device == deviceRef)
		{
			//Dispose of the device.
			IOHIDDeviceRegisterInputValueCallback(devices.at(devicesIndex)->device, NULL, devices.at(devicesIndex));
			delete devices.at(devicesIndex);
			devices.erase(devices.begin()+devicesIndex);
			break;
		}
	}
}

void ControlInit()
{
	//Initialize the HID manager, but only if not yet created.
	if(hidManager == NULL)
	{
		CFStringRef keys[2];
		CFNumberRef values[2];
		CFDictionaryRef dictionaries[3];
		CFArrayRef deviceMatch;
		int value;
		
		//Create HID manager reference with default selector and blank options.
		hidManager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
		//Open the HID manager.
		IOHIDManagerOpen(hidManager, kIOHIDOptionsTypeNone);
		//Attach the HID manager to the current run loop.
		IOHIDManagerScheduleWithRunLoop(hidManager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
		
		//Create keys.
		keys[0] = CFSTR(kIOHIDDeviceUsagePageKey);
		keys[1] = CFSTR(kIOHIDDeviceUsageKey);
		
		//Create joystick values.
		value = kHIDPage_GenericDesktop;
		values[0] = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &value);
		value = kHIDUsage_GD_Joystick;
		values[1] = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &value);
		//Add the joystick dictionary to the list of dictionaries.
		dictionaries[0] = CFDictionaryCreate(kCFAllocatorDefault, (const void **)keys, (const void **) values, 2, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
		//Free the joystick values.
		CFRelease(values[0]);
		CFRelease(values[1]);
		
		//Create gamepad values.
		value = kHIDPage_GenericDesktop;
		values[0] = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &value);
		value = kHIDUsage_GD_GamePad;
		values[1] = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &value);
		//Add the gamepad dictionary to the list of dictionaries.
		dictionaries[1] = CFDictionaryCreate(kCFAllocatorDefault, (const void **)keys, (const void **) values, 2, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
		//Free the gamepad values.
		CFRelease(values[0]);
		CFRelease(values[1]);
		
		//Create multi-axis controller values.
		value = kHIDPage_GenericDesktop;
		values[0] = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &value);
		value = kHIDUsage_GD_MultiAxisController;
		values[1] = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &value);
		//Add the multi-axis controller dictionary to the list of dictionaries.
		dictionaries[2] = CFDictionaryCreate(kCFAllocatorDefault, (const void **)keys, (const void **) values, 2, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
		//Free the multi-axis controller values.
		CFRelease(values[0]);
		CFRelease(values[1]);
		
		//Create CFArrayRef from the dictionaries.
		deviceMatch = CFArrayCreate(kCFAllocatorDefault, (const void **)dictionaries, 3, &kCFTypeArrayCallBacks);
		//Free the dictionaries.
		CFRelease(dictionaries[0]);
		CFRelease(dictionaries[1]);
		CFRelease(dictionaries[2]);
		
		//Set the device matching.
		IOHIDManagerSetDeviceMatchingMultiple(hidManager, deviceMatch);
		//Free the device matcher.
		CFRelease(deviceMatch);
		
		//Add event listener for matching device connection/removal.
		IOHIDManagerRegisterDeviceMatchingCallback(hidManager, deviceMatchingCallback, NULL);
		IOHIDManagerRegisterDeviceRemovalCallback(hidManager, deviceRemovalCallback, NULL);
	}
}

void ControlTerminate()
{
	//Destroy the HID manager if created.
	if(hidManager != NULL)
	{
		unsigned int devicesCount,
			devicesIndex;
		
		//Dispose of the devices.
		devicesCount = devices.size();
		for(devicesIndex = 0; devicesIndex < devicesCount; devicesIndex++)
		{
			IOHIDDeviceRegisterInputValueCallback(devices.at(devicesIndex)->device, NULL, devices.at(devicesIndex));
			delete devices.at(devicesIndex);
		}
		devices.clear();
		
		//Detach the HID manager from the current run loop.
		IOHIDManagerUnscheduleFromRunLoop(hidManager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
		//Close the HID manager.
		IOHIDManagerClose(hidManager, 0);
		
		//Free the HID manager.
		CFRelease(hidManager);
		hidManager = NULL;
	}
}

//Returns a delimited string of all the game controller states.
std::string ControlStates()
{
	//Only output data if properly initialized.
	if(hidManager == NULL)
	{
		return "";
	}
	std::string state;
	unsigned int devicesCount,
		devicesIndex;
	
	//The string we will output the states as.
	state = "";
	
	//Loop over the devices.
	devicesCount = devices.size();
	for(devicesIndex = 0; devicesIndex < devicesCount; devicesIndex++)
	{
		ControlDevice * device;
		device = devices.at(devicesIndex);
		
		//Delimit multiple controllers.
		if(devicesIndex)
		{
			state += "\t\t";
		}
		
		//Add the device.
		state += *(device->IDstr) + '\t';
		
		unsigned int elementCount,
			elementIndex;
		
		//Loop over axes if available, else add a placeholder.
		elementCount = device->elementsAxesCount;
		if(elementCount)
		{
			for(elementIndex = 0; elementIndex < elementCount; elementIndex++)
			{
				if(elementIndex)
				{
					state += ',';
				}
				state += floatToString(device->elementsAxes->at(elementIndex)->state);
			}
		}
		else
		{
			state += '_';
		}
		state += '\t';
		
		//Loop over POVs if available, else add a placeholder.
		elementCount = device->elementsPOVCount;
		if(elementCount)
		{
			for(elementIndex = 0; elementIndex < elementCount; elementIndex++)
			{
				if(elementIndex)
				{
					state += ',';
				}
				//Convert the value to 2 axes.
				switch(device->elementsPOV->at(elementIndex)->state)
				{
					case 0:
						state += "0,-1";
					break;
					case 1:
						state += "1,-1";
					break;
					case 2:
						state += "1,0";
					break;
					case 3:
						state += "1,1";
					break;
					case 4:
						state += "0,1";
					break;
					case 5:
						state += "-1,1";
					break;
					case 6:
						state += "-1,0";
					break;
					case 7:
						state += "-1,-1";
					break;
					default:
						state += "0,0";
					break;
				}
			}
		}
		else
		{
			state += '_';
		}
		state += '\t';
		
		//Loop over buttons oif available, else add a placeholder.
		elementCount = device->elementsButtonCount;
		if(elementCount)
		{
			for(elementIndex = 0; elementIndex < elementCount; elementIndex++)
			{
				state += device->elementsButton->at(elementIndex)->state ? '1' : '0';
			}
		}
		else
		{
			state += '_';
		}
		
		//Finish the string.
		state += '\t' + (device->axesLetters->empty() ? "_" : *(device->axesLetters)) + '\t' + *(device->vendorIDstr) + '\t' + *(device->productIDstr) + '\t' + *(device->name);
	}
	
	//Return the state string.
	return state;
}
