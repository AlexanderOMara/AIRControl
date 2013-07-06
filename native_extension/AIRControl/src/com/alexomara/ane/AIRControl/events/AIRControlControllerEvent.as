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
	import com.alexomara.ane.AIRControl.controllers.elements.AIRControlControllerElement;
	
	import flash.events.Event;

	/**
	 * The event class for all events dispatched by the AIRControlController class and decendents of the AIRControlControllerElement class.
	 */
	public class AIRControlControllerEvent extends Event
	{
		/**
		 * Dispatched on axis state change.
		 */
		public static const AXIS_CHANGE:String = "AIRControlControllerEvent.axisChange";
		
		/**
		 * Dispatched on POV hat switch state change.
		 */
		public static const POV_CHANGE:String = "AIRControlControllerEvent.povChange";
		
		/**
		 * Dispatched on button state change.
		 */
		public static const BUTTON_CHANGE:String = "AIRControlControllerEvent.buttonChange";
		
		private var _controller:AIRControlController;
		private var _element:AIRControlControllerElement;
		private var _elementIndex:uint;
		
		/**
		 * Creates a new AIRControlControllerEvent object.
		 * 
		 * @param type The type of event being dispatched.
		 * @param controller The controller with the element that dispatched the event.
		 * @param element The element that dispatched the event.
		 * @param elementIndex The index of the element that dispatched the event.
		 */
		public function AIRControlControllerEvent(type:String, controller:AIRControlController, element:AIRControlControllerElement, elementIndex:uint)
		{
			_controller = controller;
			_element = element;
			_elementIndex = elementIndex;
			super(type, false, false);
		}
		
		/**
		 * The controller with the element that dispatched the event.
		 */
		public function get controller():AIRControlController
		{
			return _controller;
		}
		
		/**
		 * The element that dispatched the event.
		 */
		public function get element():AIRControlControllerElement
		{
			return _element;
		}
		
		/**
		 * The index of the element that dispatched the event.
		 */
		public function get elementIndex():uint
		{
			return _elementIndex;
		}
		
		/**
		 * Creates a new AIRControlControllerEvent object with all the original properties.
		 * 
		 * @return An exact duplicate of the current event.
		 */
		override public function clone():Event
		{
			return new AIRControlControllerEvent(type, _controller, _element, _elementIndex);
		}
	}
}