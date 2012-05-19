/*
Copyright (c) 2012 Alexander O'Mara alexomara.com

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
#include <sstream>
#include <stdio.h>
#include <vector>;
#include <regstr.h>

//Converts uint to string.
static std::string uintToString(unsigned int i)
{
	std::stringstream ss;
	ss << i;
	return ss.str();
}

//Converts uint to string.
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

//Looks up a controller description in the registry.
static std::string controllerName(unsigned int controllerID, JOYCAPS joycaps)
{
	//Descrition of the controller.
	char * controllerDescription;
	//Registry key path.
	char subkey[256];
	//Registry keys.
	HKEY rootKey, currentKey;
	//Successful read variable.
	LONG regSuccess;
	//For reading in the key.
	char keyName[256],
		keyData[256];
	DWORD keyNameSizeOf;

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
					controllerDescription = (char*)malloc(keyNameSizeOf);
					regSuccess = RegQueryValueEx(currentKey, REGSTR_VAL_JOYOEMNAME, NULL, NULL, (LPBYTE) controllerDescription, &keyNameSizeOf);
				}
				//Close the key.
				RegCloseKey(currentKey);

				//Return the description.
				if(regSuccess == ERROR_SUCCESS)
				{
					return replaceQuotes(controllerDescription);
				}
				free(controllerDescription);
			}
		}
	}

	//Fallback controller name.
	return "null";
}

//Set the total supported controller ID's (16 usually).
static unsigned int controllers = joyGetNumDevs();
//Vector of cached controller names read from the registry.
static std::vector<std::string> controllerNames(controllers);

//Returns an XML string of all the game controller states.
std::string ControlStates()
{
	std::string state;
	//For total supported controllers, current ID, and indexing.
	unsigned int controllerID,
		controller,
		controllerAxis,
		controllerButton;
	//X is always 0, Y is always 1 (it appears that these are required in all game controllers).
	int	controllerZ,
		controllerR,
		controllerU,
		controllerV,
		controllerPOVX,
		controllerPOVY;

	//Controller types.
	JOYINFOEX joyinfo;
	JOYCAPS joycaps;

	//The string we will output the states as.
	state = "<r>";

	//Set the total supported controller ID's (16 usually).
	controllers = joyGetNumDevs();

	//Loop through the possible controllers.
	for(controller = 0; controller < controllers; controller++)
	{
		//Set JOYINFOEX info.
		joyinfo.dwSize = sizeof(joyinfo);
		joyinfo.dwFlags = JOY_RETURNALL;

		//Set controller ID, adding state information if available.
		controllerID = JOYSTICKID1 + controller;
		if(joyGetPosEx(controllerID, &joyinfo) == JOYERR_NOERROR && joyGetDevCaps(controllerID, &joycaps, sizeof(JOYCAPS)) == JOYERR_NOERROR)
		{
			//If the controller name hasn' been set, look it up in the registry and cache it.
			if(controllerNames[controllerID] == "")
			{
				controllerNames[controllerID] = controllerName(controllerID, joycaps);
			}
			//Keep count on which axes exist.
			controllerAxis = 2;
			//Set which axes exists, and which don't (-1).
			controllerZ = ((joycaps.wCaps & JOYCAPS_HASZ) ? controllerAxis++ : -1);
			controllerR = ((joycaps.wCaps & JOYCAPS_HASR) ? controllerAxis++ : -1);
			controllerU = ((joycaps.wCaps & JOYCAPS_HASU) ? controllerAxis++ : -1);
			controllerV = ((joycaps.wCaps & JOYCAPS_HASV) ? controllerAxis++ : -1);
			controllerPOVX = ((joycaps.wCaps & JOYCAPS_HASPOV) ? controllerAxis++ : -1);
			controllerPOVY = ((joycaps.wCaps & JOYCAPS_HASPOV) ? controllerAxis++ : -1);

			//Add controller to the list.
			state += "<c id=\"" +
				uintToString(controllerID) +
				"\" name=\"" +
				(controllerNames[controllerID] == "null" ? "" : controllerNames[controllerID]) +
				"\" drvr=\"" +
				replaceQuotes(joycaps.szPname) +
				"\" x=\"0\" y=\"1\" z=\"" +
				intToString(controllerZ) +
				"\" r=\"" +
				intToString(controllerR) +
				"\" u=\"" +
				intToString(controllerU) +
				"\" v=\"" +
				intToString(controllerV) +
				"\" px=\"" +
				intToString(controllerPOVX) +
				"\" py=\"" +
				intToString(controllerPOVY) +
				"\"><a>";

			//Calculate the current axis position from either extremity on a range or -1 - 1 (((cur - min) / (max - min) * 2) - 1) Y is reversed.
			state += floatToString(((float)(joyinfo.dwXpos - joycaps.wXmin) / (float)(joycaps.wXmax - joycaps.wXmin) * 2) - 1) +
				"," +
				floatToString(((float)(joyinfo.dwYpos - joycaps.wYmin) / (float)(joycaps.wYmax - joycaps.wYmin) * -2) + 1);

			//Add as many other axes as exist.
			if(controllerZ != -1)
			{
				state += "," + floatToString(((float)(joyinfo.dwZpos - joycaps.wZmin) / (float)(joycaps.wZmax - joycaps.wZmin) * 2) - 1);
			}
			if(controllerR != -1)
			{
				state += "," + floatToString(((float)(joyinfo.dwRpos - joycaps.wRmin) / (float)(joycaps.wRmax - joycaps.wRmin) * 2) - 1);
			}
			if(controllerU != -1)
			{
				state += "," + floatToString(((float)(joyinfo.dwUpos - joycaps.wUmin) / (float)(joycaps.wUmax - joycaps.wUmin) * 2) - 1);
			}
			if(controllerV != -1)
			{
				state += "," + floatToString(((float)(joyinfo.dwVpos - joycaps.wVmin) / (float)(joycaps.wVmax - joycaps.wVmin) * 2) - 1);
			}
			//POV axes.
			if(controllerPOVX != -1 && controllerPOVY != -1)
			{
				if(joyinfo.dwPOV == JOY_POVCENTERED)
				{
					state += ",0,0";
				}
				else
				{
					if(joyinfo.dwPOV > JOY_POVFORWARD && joyinfo.dwPOV < JOY_POVBACKWARD)
					{
						state += ",1";
					}
					else if(joyinfo.dwPOV > JOY_POVBACKWARD)
					{
						state += ",-1";
					}
					else
					{
						state += ",0";
					}

					if(joyinfo.dwPOV > JOY_POVLEFT || joyinfo.dwPOV < JOY_POVRIGHT)
					{
						state += ",-1";
					}
					else if(joyinfo.dwPOV > JOY_POVRIGHT && joyinfo.dwPOV < JOY_POVLEFT)
					{
						state += ",1";
					}
					else
					{
						state += ",0";
					}
				}
			}
			state += "</a><b>";

			//Loop through the total supported joysticks. States are stored in 32 on/off bits.
			for(controllerButton = 0; controllerButton < joycaps.wNumButtons; controllerButton++)
			{
				//Shift the bits to the right to make the last bit the button we want to check, and see if it is on by matching it with 1.
				if(joyinfo.dwButtons >> controllerButton & 1)
				{
					state += "1";
				}
				else
				{
					state += "0";
				}
			}
			state += "</b></c>";
		}
		else
		{
			//The controller has been disconnected, remove it from the cache.
			controllerNames[controllerID] = "";
		}
	}

	//Finish the string and return.
	state += "</r>";
	return state;
}
