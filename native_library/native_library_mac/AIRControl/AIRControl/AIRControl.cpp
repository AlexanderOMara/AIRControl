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

#include "AIRControl.h"
#include "gamecontroller_mac.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Adobe AIR/Adobe AIR.h>

// Symbols tagged with EXPORT are externally visible.
// Must use the -fvisibility=hidden gcc option.
#define EXPORT __attribute__((visibility("default")))

extern "C"
{
	//A method that get exposed.
	FREObject AIRControl(FREContext ctx, void * funcData, uint32_t argc, FREObject argv[])
	{
		FREObject result;
		uint32_t debugModeFRE = 0;
		bool debugMode = false;
		//If the optional debug argument is defined, check if in debug mode.
		if(argc)
		{
			FREGetObjectAsBool(argv[0], &debugModeFRE);
			debugMode = debugModeFRE;
		}
		const char * str = ControlStates(debugMode).c_str();
		FRENewObjectFromUTF8(strlen(str)+1, (const uint8_t *)str, &result);
		
		return result;
	}
	
	//The methods that can be called from ExtensionContext instances.
	FRENamedFunction methods[] = {
		{ (const uint8_t *)"AIRControl", NULL, AIRControl }
	};
	
	//Called when the runtime creates the extension context instance (ExtensionContext.createExtensionContext).
	void contextInitializer(void * extData, const uint8_t * ctxType, FREContext ctx, uint32_t * numFunctions, const FRENamedFunction ** functions)
	{
		*functions = methods;
		*numFunctions = 1;
		
		return;
	}
	
	//Called when the instance is disposed or garbage collected (CONTEXT_INSTANCE.dispose()).
	void contextFinalizer(FREContext ctx)
	{
		return;
	}
	
	//Called when the extension is loaded for the first time (ExtensionContext.createExtensionContext).
	EXPORT void initializer(void ** extData, FREContextInitializer * ctxInitializer, FREContextFinalizer * ctxFinalizer)
	{
		//Initialize the controller library.
		ControlInit();
		
		*ctxInitializer = &contextInitializer;
		*ctxFinalizer = &contextFinalizer;
		
		return;
	}
	
	//Called when the extension is unloaded.
	EXPORT void finalizer(void* extData)
	{
		//Terminate the controller library.
		ControlTerminate();
		
		return;
	}
}

