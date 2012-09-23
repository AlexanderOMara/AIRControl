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


#include <sstream>
#include <vector>

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
	unsigned int i;
	unsigned long str_size = str.size();
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
/*static std::string controllerName(unsigned int controllerID, JOYCAPS joycaps)
{
	//Fallback controller name.
	return "null";
}*/

//Set the total supported controller ID's (16 usually).
//static unsigned int controllers = joyGetNumDevs();
//Vector of cached controller names read from the registry.
//static std::vector<std::string> controllerNames(controllers);

//Returns an XML string of all the game controller states.
std::string ControlStates()
{
	std::string state;
	
	//The string we will output the states as.
	state = "<r>";

	//Finish the string and return.
	state += "</r>";
	return state;
}
