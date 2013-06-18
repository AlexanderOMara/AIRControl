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

#include "main.h"
#include "gamecontroller_win.h"
#include <string>
#include <stdlib.h>

extern "C"
{
	//A method that get exposed.
	FREObject AIRControl(FREContext ctx, void* funcData, uint32_t argc, FREObject argv[])
	{
		FREObject result;
		std::string str(ControlStates());
		const uint8_t* msg = (const uint8_t*)str.c_str();
		FRENewObjectFromUTF8(strlen((const char*)msg)+1, msg, &result);
		return result;
	}

	//The methods that can be called from ExtensionContext instances.
	FRENamedFunction methods[] = {
		{ (const uint8_t *)"AIRControl", NULL, AIRControl }
	};

	//Called when the runtime creates the extension context instance (ExtensionContext.createExtensionContext).
	void contextInitializer(void* extData, const uint8_t* ctxType, FREContext ctx, uint32_t* numFunctions, const FRENamedFunction** functions)
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
	void initializer(void** extData, FREContextInitializer* ctxInitializer, FREContextFinalizer* ctxFinalizer)
	{
		*ctxInitializer = &contextInitializer;
		*ctxFinalizer = &contextFinalizer;
	}

	//Called when the extension is unloaded.
	void finalizer(void* extData)
	{
		return;
	}
}

