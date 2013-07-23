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


#include <windows.h>
#include <regstr.h>
#include <stdio.h>
#include <sstream>
#include <vector>

//Data structure for controllers.
struct ControlDevice {
	bool connected;
	unsigned int ID;
	std::string * IDstr;
	std::string * name;
	WORD vendorID;
	std::string * vendorIDstr;
	WORD productID;
	std::string * productIDstr;
	std::string * axesLetters;
	//X and Y axes are required.
	bool axisZ;
	bool axisR;
	bool axisU;
	bool axisV;
	int axisXmin;
	int axisXmax;
	int axisYmin;
	int axisYmax;
	int axisZmin;
	int axisZmax;
	int axisRmin;
	int axisRmax;
	int axisUmin;
	int axisUmax;
	int axisVmin;
	int axisVmax;
	bool pov;
	unsigned int buttons;
	JOYINFOEX joyinfo;
	JOYCAPS joycaps;
	ControlDevice()
	{
		connected = false;
		ID = 0;
		IDstr = new std::string();
		name = new std::string();
		vendorID = 0;
		vendorIDstr = new std::string();
		productID = 0;
		productIDstr = new std::string();
		axesLetters = new std::string();
		axisZ = false;
		axisR = false;
		axisU = false;
		axisV = false;
		axisXmin = 0;
		axisXmax = 0;
		axisYmin = 0;
		axisYmax = 0;
		axisZmin = 0;
		axisZmax = 0;
		axisRmin = 0;
		axisRmax = 0;
		axisUmin = 0;
		axisUmax = 0;
		axisVmin = 0;
		axisVmax = 0;
		pov = false;
		buttons = 0;
		joyinfo.dwSize = sizeof(joyinfo);
		joyinfo.dwFlags = JOY_RETURNALL;
	}
	~ControlDevice()
	{
		delete IDstr;
		delete name;
		delete vendorIDstr;
		delete productIDstr;
		delete axesLetters;
	}
};


//Set the total supported controller ID's (16 usually).
static unsigned int joyNumDevs = joyGetNumDevs();

//Vector of cached controller names read from the registry.
static std::vector<std::string> controllerNames(joyNumDevs);

//Vector of controller structures.
static std::vector<ControlDevice *> devices(joyNumDevs);
//Limit the scanning for new controllers.
static DWORD lastControllerDetect = 0;
//Save if has inited.
static bool inited = false;
//Remeber rather than recalculate.
static unsigned int joycapsSize = sizeof(JOYCAPS);

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

//Looks up a controller description in the registry.
static std::string controllerName(unsigned int controllerID, JOYCAPS joycaps)
{
	//Descrition of the controller.
	char *productKey = (char*)malloc(0);
	//Registry key path.
	char subkey[256];
	//Registry keys.
	HKEY rootKey,
		currentKey;
	//Successful read variable.
	LONG regSuccess;
	//For reading in the key.
	char keyName[256],
		keyData[256];
	DWORD keyNameSizeOf;
	std::string controllerDescription;

	//Set it to fallback by default.
	controllerDescription = joycaps.szPname;

	//Merge the registry path together.
	snprintf(subkey, 256, "%s\\%s\\%s", REGSTR_PATH_JOYCONFIG, joycaps.szRegKey, REGSTR_KEY_JOYCURR);

	//Open the registry key, checking 2 locations.
	regSuccess = RegOpenKeyEx(rootKey = HKEY_LOCAL_MACHINE, subkey, 0, KEY_READ, &currentKey);
	if(regSuccess != ERROR_SUCCESS)
	{
		regSuccess = RegOpenKeyEx(rootKey = HKEY_CURRENT_USER, subkey, 0, KEY_READ, &currentKey);
	}

	if(regSuccess == ERROR_SUCCESS)
	{
		//Create the key name for the current controller.
		snprintf(keyName, 256, "Joystick%d%s", controllerID + 1, REGSTR_VAL_JOYOEMNAME);

		//Read in the first registry key data to get the name of the key which hold the controller description.
		keyNameSizeOf = sizeof(keyData);
		regSuccess = RegQueryValueEx(currentKey, keyName, NULL, NULL, (LPBYTE)keyData, &keyNameSizeOf);
		RegCloseKey(currentKey);

		if(regSuccess == ERROR_SUCCESS)
		{
			//Create the registry path to the second key which stores the name of the device.
			snprintf(subkey, 256, "%s\\%s", REGSTR_PATH_JOYOEM, keyData);
			//Open the second key.
			regSuccess = RegOpenKeyEx(rootKey, subkey, 0, KEY_READ, &currentKey);

			if(regSuccess == ERROR_SUCCESS)
			{
				//Read in the second key data.
				keyNameSizeOf = sizeof(keyData);
				regSuccess = RegQueryValueEx(currentKey, REGSTR_VAL_JOYOEMNAME, NULL, NULL, NULL, &keyNameSizeOf);

				if(regSuccess == ERROR_SUCCESS)
				{
					//Read the key data into the description.
					free(productKey);
					productKey = (char*)malloc(keyNameSizeOf);
					regSuccess = RegQueryValueEx(currentKey, REGSTR_VAL_JOYOEMNAME, NULL, NULL, (LPBYTE) productKey, &keyNameSizeOf);
				}
				//Close the key.
				RegCloseKey(currentKey);

				//Set the description.
				if(regSuccess == ERROR_SUCCESS)
				{
					controllerDescription = productKey;
					//If the returned value is blank, use the fallback.
					if(controllerDescription.empty())
					{
						controllerDescription = joycaps.szPname;
					}
				}
			}
		}
	}
	free(productKey);

	//Fallback controller name.
	return controllerDescription;
}

void ControlInit()
{
	//Create a controller object for each possible controller to keep polling calls to a minimum.
	unsigned int devicesIndex;
	ControlDevice * device;
	for(devicesIndex = 0; devicesIndex < joyNumDevs; devicesIndex++)
	{
		device = new ControlDevice();
		//Set controller ID from the first.
		device->ID = JOYSTICKID1 + devicesIndex;
		device->IDstr->assign(uintToString(device->ID));
		devices.at(devicesIndex) = device;
	}
	inited = true;
}

void ControlTerminate()
{
	unsigned int devicesIndex;
	for(devicesIndex = 0; devicesIndex < joyNumDevs; devicesIndex++)
	{
		delete devices.at(devicesIndex);
	}
	inited = false;
}

//Returns a delimited string of all the game controller states.
std::string ControlStates(bool debugMode)
{
	std::string state,
		debugOut;
	unsigned int devicesIndex,
		controllerButton;
	ControlDevice * device;
	bool pollAll;
	DWORD timeSinceBoot;

	if(!inited)
	{
		return "";
	}

	pollAll = false;
	timeSinceBoot = GetTickCount();

	if(debugMode)
	{
		debugOut += "DEBUG: polling_start:" + uintToString(timeSinceBoot);
	}

	//Limit the checking for new controllers to once per second, also checking for DWORD rolling back over to 0 (mysterious performance bugs surrounding joyGetPosEx and joyGetDevCaps).
	if(timeSinceBoot > lastControllerDetect + 1000 || timeSinceBoot < lastControllerDetect)
	{
		pollAll = true;
		lastControllerDetect = timeSinceBoot;
	}

	if(debugMode)
	{
		debugOut += " poll_all:";
		debugOut += (pollAll ? '1' : '0');
		debugOut += " poll_max:" + uintToString(joyNumDevs);
	}

	//The string we will output the states as.
	state = "";

	//Loop through the possible controllers.
	for(devicesIndex = 0; devicesIndex < joyNumDevs; devicesIndex++)
	{
		//Get the device.
		device = devices.at(devicesIndex);
		//If polling all or the device is connected
		if(device->connected || pollAll)
		{
			//Poll the controller for states and, if not currently connected, the capabilities.
			if(joyGetPosEx(device->ID, &device->joyinfo) == JOYERR_NOERROR && (device->connected || joyGetDevCaps(device->ID, &device->joycaps, joycapsSize) == JOYERR_NOERROR))
			{
				//If the controller was not previousely connected, setup the object.
				if(!device->connected)
				{
					device->connected = true;
					device->vendorID = device->joycaps.wMid;
					device->vendorIDstr->assign(uintToString(device->vendorID));
					device->productID = device->joycaps.wPid;
					device->productIDstr->assign(uintToString(device->productID));
					device->name->assign(replaceCharacters(controllerName(device->ID, device->joycaps), '\t', ' '));
					if(device->name->empty())
					{
						device->name->assign("Controller " + uintToString(device->ID + 1));
					}

					//X and Y axes are required.
					device->axisXmin = device->joycaps.wXmin;
					device->axisXmax = device->joycaps.wXmax;
					device->axisYmin = device->joycaps.wYmin;
					device->axisYmax = device->joycaps.wYmax;
					device->axesLetters->assign("XY");
					//Detect any additional axes.
					if(device->joycaps.wCaps & JOYCAPS_HASZ)
					{
						device->axisZ = true;
						device->axisZmin = device->joycaps.wZmin;
						device->axisZmax = device->joycaps.wZmax;
						device->axesLetters->push_back('Z');
					}else{
						device->axisZ = false;
						device->axisZmin = 0;
						device->axisZmax = 0;
					}
					if(device->joycaps.wCaps & JOYCAPS_HASR)
					{
						device->axisR = true;
						device->axisRmin = device->joycaps.wRmin;
						device->axisRmax = device->joycaps.wRmax;
						device->axesLetters->push_back('R');
					}else{
						device->axisR = false;
						device->axisRmin = 0;
						device->axisRmax = 0;
					}
					if(device->joycaps.wCaps & JOYCAPS_HASU)
					{
						device->axisU = true;
						device->axisUmin = device->joycaps.wUmin;
						device->axisUmax = device->joycaps.wUmax;
						device->axesLetters->push_back('U');
					}else{
						device->axisU = false;
						device->axisUmin = 0;
						device->axisUmax = 0;
					}
					if(device->joycaps.wCaps & JOYCAPS_HASV)
					{
						device->axisV = true;
						device->axisVmin = device->joycaps.wVmin;
						device->axisVmax = device->joycaps.wVmax;
						device->axesLetters->push_back('V');
					}else{
						device->axisV = false;
						device->axisVmin = 0;
						device->axisVmax = 0;
					}

					//Detect POV.
					device->pov = (device->joycaps.wCaps & JOYCAPS_HASPOV);

					//Detect buttons.
					device->buttons = device->joycaps.wNumButtons;
				}

				if(debugMode)
				{
					debugOut += " poll_" + *(device->IDstr) + "_s:" + uintToString(GetTickCount());
				}

				//Add controller to the list, delimit if necessary.
				state += (state.empty() ? "" : "\t\t") + *(device->IDstr) + '\t';

				//Calculate the current axis position from either extremity on a range or -1 - 1 (((cur - min) / (max - min) * 2) - 1). X is always 0, Y is always 1.
				state +=
					floatToString(((float)(device->joyinfo.dwXpos - device->axisXmin) / (float)(device->axisXmax - device->axisXmin) * 2) - 1) + "," +
					floatToString(((float)(device->joyinfo.dwYpos - device->axisYmin) / (float)(device->axisYmax - device->axisYmin) * 2) - 1) +
					(device->axisZ ? ',' + floatToString(((float)(device->joyinfo.dwZpos - device->joycaps.wZmin) / (float)(device->joycaps.wZmax - device->joycaps.wZmin) * 2) - 1) : "") +
					(device->axisR ? ',' + floatToString(((float)(device->joyinfo.dwRpos - device->joycaps.wRmin) / (float)(device->joycaps.wRmax - device->joycaps.wRmin) * 2) - 1) : "") +
					(device->axisU ? ',' + floatToString(((float)(device->joyinfo.dwUpos - device->joycaps.wUmin) / (float)(device->joycaps.wUmax - device->joycaps.wUmin) * 2) - 1) : "") +
					(device->axisV ? ',' + floatToString(((float)(device->joyinfo.dwVpos - device->joycaps.wVmin) / (float)(device->joycaps.wVmax - device->joycaps.wVmin) * 2) - 1) : "") + '\t'
				;

				//Add the POV axes if available, else put a placeholder.
				if(device->pov)
				{
					if(device->joyinfo.dwPOV != JOY_POVCENTERED)
					{
						if(device->joyinfo.dwPOV > JOY_POVFORWARD && device->joyinfo.dwPOV < JOY_POVBACKWARD)
						{
							state += "1";
						}
						else if(device->joyinfo.dwPOV > JOY_POVBACKWARD)
						{
							state += "-1";
						}
						else
						{
							state += "0";
						}

						if(device->joyinfo.dwPOV > JOY_POVLEFT || device->joyinfo.dwPOV < JOY_POVRIGHT)
						{
							state += ",-1";
						}
						else if(device->joyinfo.dwPOV > JOY_POVRIGHT && device->joyinfo.dwPOV < JOY_POVLEFT)
						{
							state += ",1";
						}
						else
						{
							state += ",0";
						}
					}
					else
					{
						state += "0,0";
					}
				}
				else
				{
					state += '_';
				}
				state += '\t';

				//Check if this controller has buttons, else put a placeholder.
				if(device->buttons)
				{
					//Loop through the total supported joysticks. Button states are stored in 32 on/off bits.
					for(controllerButton = 0; controllerButton < device->buttons; controllerButton++)
					{
						//Shift the bits to the right to make the last bit the button we want to check, and see if it is on by matching it with 1.
						state += device->joyinfo.dwButtons >> controllerButton & 1 ? '1' : '0';
					}
				}
				else
				{
					state += '_';
				}

				//Finish the state string.
				state += '\t' + *(device->axesLetters) + '\t' + *(device->vendorIDstr) + '\t' + *(device->productIDstr) + '\t' + *(device->name);

				if(debugMode)
				{
					debugOut += " poll_" + *(device->IDstr) + "_e:" + uintToString(GetTickCount());
				}
			}
			else if(device->connected)
			{
				//The controller was connected, but is no longer attached, remove.
				device->connected = false;
			}
		}
	}

	//If polling all controllers, update the last controller detect time.
	if(pollAll)
	{
		lastControllerDetect = GetTickCount();
	}

	//If in debug mode, add the debug string.
	if(debugMode)
	{
		state += (state.empty() ? "" : "\t\t") + debugOut + " polling_end:" + uintToString(GetTickCount());
	}

	//Return the state string.
	return state;
}
