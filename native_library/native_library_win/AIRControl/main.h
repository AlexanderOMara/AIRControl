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

#include "FlashRuntimeExtensions.h"
/*
NOTE:
In order to compile this with MinGW, FlashRuntimeExtensions.h must be modified to use stdint.h.
Find this block of code:

#ifdef WIN32
	typedef unsigned __int32	uint32_t;
	typedef unsigned __int8		uint8_t;
	typedef          __int32	int32_t;
#else
	#include <stdint.h>
#endif

And replace it with this block of code:

//#ifdef WIN32
//	typedef unsigned __int32	uint32_t;
//	typedef unsigned __int8		uint8_t;
//	typedef          __int32	int32_t;
//#else
	#include <stdint.h>
//#endif
*/

extern "C"
{
	__declspec(dllexport) void initializer(void** extData, FREContextInitializer* ctxInitializer, FREContextFinalizer* ctxFinalizer);
}

