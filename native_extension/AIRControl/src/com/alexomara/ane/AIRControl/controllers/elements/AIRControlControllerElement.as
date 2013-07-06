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
package com.alexomara.ane.AIRControl.controllers.elements
{
	import flash.events.EventDispatcher;
	
	/**
	 * This is the base class for all the AIRControlController elements.
	 * 
	 * <p>This class is used as the basis for controller elements which are created automatically and should not be instantiated manually.</p>
	 * 
	 * @see com.alexomara.ane.AIRControl.controllers.AIRControlController
	 */
	public class AIRControlControllerElement extends EventDispatcher
	{
		/**
		 * Creates a new AIRControlControllerElement object.
		 */
		public function AIRControlControllerElement()
		{
		}
	}
}