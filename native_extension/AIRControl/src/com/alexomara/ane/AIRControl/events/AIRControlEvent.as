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
package com.alexomara.ane.AIRControl.events
{
	import com.alexomara.ane.AIRControl.controllers.AIRControlController;
	
	import flash.events.Event;

	/**
	 * The event class for all events dispatched by the main AIRControl class.
	 */
	public class AIRControlEvent extends Event
	{
		/**
		 * Dispatched on controller attach.
		 */
		public static const CONTROLLER_ATTACH:String = "AIRControlEvent.controllerAttach";
		
		/**
		 * Dispatched on controller detach.
		 */
		public static const CONTROLLER_DETACH:String = "AIRControlEvent.controllerDetach";
		
		private var _controller:AIRControlController;
		private var _controllerIndex:uint;
		
		/**
		 * Creates a new AIRControlEvent object.
		 * 
		 * @param type The type of event being dispatched.
		 * @param controller The controller that dispatched the event.
		 * @param controllerIndex The index of the controller that dispatched the event.
		 */
		public function AIRControlEvent(type:String, controller:AIRControlController, controllerIndex:uint)
		{
			_controller = controller;
			_controllerIndex = controllerIndex;
			super(type, false, false);
		}
		
		/**
		 * The controller that dispatched the event.
		 */
		public function get controller():AIRControlController
		{
			return _controller;
		}
		
		/**
		 * The index of the controller that dispatched the event.
		 */
		public function get controllerIndex():uint
		{
			return _controllerIndex;
		}
		
		/**
		 * Creates a new AIRControlEvent object with all the original properties.
		 * 
		 * @return An exact duplicate of the current event.
		 */
		override public function clone():Event
		{
			return new AIRControlEvent(type, _controller, _controllerIndex);
		}
	}
}