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
	import com.alexomara.ane.AIRControl.AIRControlNS;
	import com.alexomara.ane.AIRControl.events.AIRControlControllerEvent;
	
	/**
	 * Dispatched on axis state change.
	 * 
	 * @eventType com.alexomara.ane.AIRControl.events.AIRControlControllerEvent
	 */
	[Event(name="AIRControlControllerEvent.axisChange", type="com.alexomara.ane.AIRControl.events.AIRControlControllerEvent")]
	
	/**
	 * The class for AIRControlController axis elements.
	 * 
	 * <p>This class is used for controller elements which are created automatically and should not be instantiated manually.</p>
	 * 
	 * @see com.alexomara.ane.AIRControl.controllers.AIRControlController
	 */
	public class AIRControlControllerAxis extends AIRControlControllerElement
	{
		private var _position:Number = 0;
		
		/**
		 * Creates a new AIRControlControllerAxis object.
		 */
		public function AIRControlControllerAxis()
		{
		}
		
		/**
		 * The current position of the axis as a floating point number from -1 to 1.
		 */
		public function get position():Number
		{
			return _position;
		}
		
		/**
		 * Sets the element state.
		 * 
		 * <p>This method is used internally to set the state of the element.</p>
		 * 
		 * @private
		 */
		AIRControlNS function set position(value:Number):void
		{
			_position = value;
		}
	}
}