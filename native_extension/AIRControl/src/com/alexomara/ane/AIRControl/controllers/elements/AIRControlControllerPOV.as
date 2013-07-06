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
	 * Dispatched on POV hat switch state change.
	 * 
	 * @eventType com.alexomara.ane.AIRControl.events.AIRControlControllerEvent
	 */
	[Event(name="AIRControlControllerEvent.povChange", type="com.alexomara.ane.AIRControl.events.AIRControlControllerEvent")]
	
	/**
	 * The class for AIRControlController POV hat switch elements.
	 * 
	 * <p>This class is used for controller elements which are created automatically and should not be instantiated manually.</p>
	 * 
	 * @see com.alexomara.ane.AIRControl.controllers.AIRControlController
	 */
	public class AIRControlControllerPOV extends AIRControlControllerElement
	{
		private var _X:int = 0;
		private var _Y:int = 0;
		
		/**
		 * Creates a new AIRControlControllerPOV object.
		 */
		public function AIRControlControllerPOV()
		{
		}
		
		/**
		 * The current position of the X axis on the POV hat switch, -1, 0, or 1.
		 * 
		 * <p>This property does not always reflect the axis that should logically be used for this purpose.</p>
		 */
		public function get X():int
		{
			return _X;
		}
		
		/**
		 * Sets the element state.
		 * 
		 * <p>This method is used internally to set the state of the element.</p>
		 * 
		 * @private
		 */
		AIRControlNS function set X(value:int):void
		{
			//Check that the value is -1 or 1 or assign to 0.
			_X = value*value === 1 ? value : 0;
		}
		
		/**
		 * The current position of the Y axis on the POV hat switch, -1, 0, or 1.
		 * 
		 * <p>This property does not always reflect the axis that should logically be used for this purpose.</p>
		 */
		public function get Y():int
		{
			return _Y;
		}
		
		/**
		 * Sets the element state.
		 * 
		 * <p>This method is used internally to set the state of the element.</p>
		 * 
		 * @private
		 */
		AIRControlNS function set Y(value:int):void
		{
			//Check that the value is -1 or 1 or assign to 0.
			_Y = value*value === 1 ? value : 0;
		}
	}
}