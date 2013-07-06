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
package com.alexomara.ane.AIRControl.controllers
{
	import com.alexomara.ane.AIRControl.AIRControlNS;
	import com.alexomara.ane.AIRControl.controllers.elements.AIRControlControllerPOV;
	import com.alexomara.ane.AIRControl.controllers.elements.AIRControlControllerButton;
	import com.alexomara.ane.AIRControl.controllers.elements.AIRControlControllerAxis;
	import com.alexomara.ane.AIRControl.events.AIRControlControllerEvent;
	
	import flash.events.EventDispatcher;
	
	/**
	 * Dispatched on axis state change.
	 * 
	 * @eventType com.alexomara.ane.AIRControl.events.AIRControlControllerEvent
	 */
	[Event(name="AIRControlControllerEvent.axisChange", type="com.alexomara.ane.AIRControl.events.AIRControlControllerEvent")]
	
	/**
	 * Dispatched on POV hat switch state change.
	 * 
	 * @eventType com.alexomara.ane.AIRControl.events.AIRControlControllerEvent
	 */
	[Event(name="AIRControlControllerEvent.povChange", type="com.alexomara.ane.AIRControl.events.AIRControlControllerEvent")]
	
	/**
	 * Dispatched on button state change.
	 * 
	 * @eventType com.alexomara.ane.AIRControl.events.AIRControlControllerEvent
	 */
	[Event(name="AIRControlControllerEvent.buttonChange", type="com.alexomara.ane.AIRControl.events.AIRControlControllerEvent")]
	
	/**
	 * The class for AIRControl controllers.
	 * 
	 * <p>This class is used for controllers objects which are created automatically and should not be instantiated manually.</p>
	 * 
	 * @see com.alexomara.ane.AIRControl.AIRControl
	 */
	public class AIRControlController extends EventDispatcher
	{
		private var _ID:uint;
		private var _axes:Vector.<AIRControlControllerAxis>;
		private var _axesTotal:uint;
		private var _povs:Vector.<AIRControlControllerPOV>;
		private var _povsTotal:uint;
		private var _buttons:Vector.<AIRControlControllerButton>;
		private var _buttonsTotal:uint;
		private var _vendorID:uint;
		private var _productID:uint;
		private var _name:String;
		private var _AXIS_X:int;
		private var _AXIS_Y:int;
		private var _AXIS_Z:int;
		private var _AXIS_R:int;
		private var _AXIS_U:int;
		private var _AXIS_V:int;
		
		/**
		 * Creates a new AIRControlController object.
		 * 
		 * @param ID The unique identifier assigned to the controller.
		 * @param axes The total number of axes on the controller.
		 * @param povs The total number of POV hat switches on the controller.
		 * @param buttons The total number of buttons on the controller.
		 * @param axesLetters The axes letters in the order they appear on the controller.
		 * @param vendorID The unique identified of the hardware manufacturer of the controller.
		 * @param productID The unique identified of the product by the manufacturer of the controller.
		 * @param name The description of the controller.
		 */
		public function AIRControlController(ID:uint, axes:uint, povs:uint, buttons:uint, axesLetters:String, vendorID:uint, productID:uint, name:String)
		{
			_ID = ID;
			_axes = new Vector.<AIRControlControllerAxis>(axes, true);
			_axesTotal = axes;
			_povs = new Vector.<AIRControlControllerPOV>(povs, true);
			_povsTotal = povs;
			_buttons = new Vector.<AIRControlControllerButton>(buttons, true);
			_buttonsTotal = buttons;
			_vendorID = vendorID;
			_productID = productID;
			_name = name;
			
			//Count down the axes, povs, and buttons, assigning them all new objects.
			while(axes--)
			{
				_axes[axes] = new AIRControlControllerAxis();
			}
			while(povs--)
			{
				_povs[povs] = new AIRControlControllerPOV();
			}
			while(buttons--)
			{
				_buttons[buttons] = new AIRControlControllerButton();
			}
			
			//Set the indexes of each axis letter.
			_AXIS_X = axesLetters.indexOf("X");
			_AXIS_Y = axesLetters.indexOf("Y");
			_AXIS_Z = axesLetters.indexOf("Z");
			_AXIS_R = axesLetters.indexOf("R");
			_AXIS_U = axesLetters.indexOf("U");
			_AXIS_V = axesLetters.indexOf("V");
		}
		
		/**
		 * The unique identifier assigned to the controller.
		 * 
		 * <p>This identifier is only necessarily unique among currently attached controllers. It cannot be used to reliably match a controller after it has been detached. This value has no direct relation to the controller index.</p>
		 * 
		 * <ul>
		 * <li>On Windows the value is assigned to the controller offset from JOYSTICKID1.</li>
		 * <li>On Mac OS X the value is assigned incrementally on controller connection.</li>
		 * </ul>
		 */
		public function get ID():uint
		{
			return _ID;
		}
		
		/**
		 * Returns the raw vector of controller axes.
		 * 
		 * <p>This method is used internally to set the state of the elements.</p>
		 * 
		 * @private
		 */
		AIRControlNS function get axes():Vector.<AIRControlControllerAxis>
		{
			return _axes;
		}
		
		/**
		 * Returns the axis at the specified index on the controller or null if out of bounds.
		 * 
		 * <ul>
		 * <li>On Windows the maximum supported controller axes is 6.</li>
		 * <li>On Mac OS X there is no explicitly imposed limit.</li>
		 * </ul>
		 * 
		 * @param index The index of the axis element.
		 * 
		 * @return The axis at the specified index or null.
		 * 
		 * @see #axesTotal
		 */
		public function axis(index:uint):AIRControlControllerAxis
		{
			return index < _axesTotal ? _axes[index] : null;
		}
		
		/**
		 * The total axes on the controller.
		 * 
		 * <ul>
		 * <li>On Windows the maximum supported controller axes is 6.</li>
		 * <li>On Mac OS X there is no explicitly imposed limit.</li>
		 * </ul>
		 * 
		 * @see #axis()
		 */
		public function get axesTotal():uint
		{
			return _axesTotal;
		}
		
		/**
		 * Returns the raw vector of controller POV hat switchs.
		 * 
		 * <p>This method is used internally to set the state of the elements.</p>
		 * 
		 * @private
		 */
		AIRControlNS function get povs():Vector.<AIRControlControllerPOV>
		{
			return _povs;
		}
		
		/**
		 * Returns the POV hat switch at the specified index on the controller or null if out of bounds.
		 * 
		 * <ul>
		 * <li>On Windows the maximum supported controller POV hat switches is 1.</li>
		 * <li>On Mac OS X there is no explicitly imposed limit.</li>
		 * </ul>
		 * 
		 * @param index The index of the POV hat switch element.
		 * 
		 * @return The POV hat switch at the specified index or null.
		 * 
		 * @see #povsTotal
		 */
		public function pov(index:uint):AIRControlControllerPOV
		{
			return index < _povsTotal ? _povs[index] : null;
		}
		
		/**
		 * The total POV hat switches on the controller.
		 * 
		 * <ul>
		 * <li>On Windows the maximum supported controller POV hat switches is 1.</li>
		 * <li>On Mac OS X there is no explicitly imposed limit.</li>
		 * </ul>
		 * 
		 * @see #pov()
		 */
		public function get povsTotal():uint
		{
			return _povsTotal;
		}
		
		/**
		 * Returns the raw vector of controller buttons.
		 * 
		 * <p>This method is used internally to set the state of the elements.</p>
		 * 
		 * @private
		 */
		AIRControlNS function get buttons():Vector.<AIRControlControllerButton>
		{
			return _buttons;
		}
		
		/**
		 * Returns the button at the specified index on the controller or null if out of bounds.
		 * 
		 * <ul>
		 * <li>On Windows the maximum supported controller buttons is 32.</li>
		 * <li>On Mac OS X there is no explicitly imposed limit.</li>
		 * </ul>
		 * 
		 * @param index The index of the button element.
		 * 
		 * @return The button at the specified index or null.
		 * 
		 * @see #buttonsTotal
		 */
		public function button(index:uint):AIRControlControllerButton
		{
			return index < _buttonsTotal ? _buttons[index] : null;
		}
		
		/**
		 * The total buttons on the controller.
		 * 
		 * <ul>
		 * <li>On Windows the maximum supported controller buttons is 32.</li>
		 * <li>On Mac OS X there is no explicitly imposed limit.</li>
		 * </ul>
		 * 
		 * @see #button()
		 */
		public function get buttonsTotal():uint
		{
			return _buttonsTotal;
		}
		
		/**
		 * The unique identified of the hardware manufacturer of the controller.
		 * 
		 * <p>This identifier can be used to determine the manufacturer of the controller.</p>
		 * 
		 * <p>This property can be used in conjunction with productID to match a compatile controller after detach and attach.</p>
		 * 
		 * <ul>
		 * <li>On Windows the value is assigned from JOYCAPS.wMid.</li>
		 * <li>On Mac OS X the value is assigned from kIOHIDVendorIDKey.</li>
		 * </ul>
		 * 
		 * @see #productID
		 */
		public function get vendorID():uint
		{
			return _vendorID;
		}
		
		/**
		 * The unique identified of the product by the manufacturer of the controller.
		 * 
		 * <p>This identifier can be used to determine the product by the manufacturer of the controller.</p>
		 * 
		 * <p>This property can be used in conjunction with ventorID to match a compatile controller after detach and attach.</p>
		 * 
		 * <ul>
		 * <li>On Windows the value is assigned from JOYCAPS.wPid.</li>
		 * <li>On Mac OS X the value is assigned from kIOHIDProductIDKey.</li>
		 * </ul>
		 * 
		 * @see #vendorID
		 */
		public function get productID():uint
		{
			return _productID;
		}
		
		/**
		 * The description of the controller.
		 * 
		 * <ul>
		 * <li>On Windows the value is assigned from JOYCAPS.szRegKey.</li>
		 * <li>On Mac OS X the value is assigned from kIOHIDProductKey.</li>
		 * </ul>
		 */
		public function get name():String
		{
			return _name;
		}
		
		/**
		 * The index of the axis with the specified letter or -1 if not available.
		 * 
		 * <p>This property is only accurate on Windows and does not always reflect the axis that should logically be used for this purpose.</p>
		 * 
		 * <ul>
		 * <li>On Windows the value corresponds to the axis letter Windows assigned to the axis.</li>
		 * <li>On Mac OS X the value is assigned incrementally.</li>
		 * </ul>
		 */
		public function get AXIS_X():int
		{
			return _AXIS_X;
		}
		
		/**
		 * The index of the axis with the specified letter or -1 if not available.
		 * 
		 * <p>This property is only accurate on Windows and does not always reflect the axis that should logically be used for this purpose.</p>
		 * 
		 * <ul>
		 * <li>On Windows the value corresponds to the axis letter Windows assigned to the axis.</li>
		 * <li>On Mac OS X the value is assigned incrementally.</li>
		 * </ul>
		 */
		public function get AXIS_Y():int
		{
			return _AXIS_Y;
		}
		
		/**
		 * The index of the axis with the specified letter or -1 if not available.
		 * 
		 * <p>This property is only accurate on Windows and does not always reflect the axis that should logically be used for this purpose.</p>
		 * 
		 * <ul>
		 * <li>On Windows the value corresponds to the axis letter Windows assigned to the axis.</li>
		 * <li>On Mac OS X the value is assigned incrementally.</li>
		 * </ul>
		 */
		public function get AXIS_Z():int
		{
			return _AXIS_Z;
		}
		
		/**
		 * The index of the axis with the specified letter or -1 if not available.
		 * 
		 * <p>This property is only accurate on Windows and does not always reflect the axis that should logically be used for this purpose.</p>
		 * 
		 * <ul>
		 * <li>On Windows the value corresponds to the axis letter Windows assigned to the axis.</li>
		 * <li>On Mac OS X the value is assigned incrementally.</li>
		 * </ul>
		 */
		public function get AXIS_R():int
		{
			return _AXIS_R;
		}
		
		/**
		 * The index of the axis with the specified letter or -1 if not available.
		 * 
		 * <p>This property is only accurate on Windows and does not always reflect the axis that should logically be used for this purpose.</p>
		 * 
		 * <ul>
		 * <li>On Windows the value corresponds to the axis letter Windows assigned to the axis.</li>
		 * <li>On Mac OS X the value is assigned incrementally.</li>
		 * </ul>
		 */
		public function get AXIS_U():int
		{
			return _AXIS_U;
		}
		
		/**
		 * The index of the axis with the specified letter or -1 if not available.
		 * 
		 * <p>This property is only accurate on Windows and does not always reflect the axis that should logically be used for this purpose.</p>
		 * 
		 * <ul>
		 * <li>On Windows the value corresponds to the axis letter Windows assigned to the axis.</li>
		 * <li>On Mac OS X the value is assigned incrementally.</li>
		 * </ul>
		 */
		public function get AXIS_V():int
		{
			return _AXIS_V;
		}
	}
}