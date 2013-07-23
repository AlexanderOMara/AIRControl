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
package com.alexomara.ane.AIRControl
{
	import com.alexomara.ane.AIRControl.controllers.AIRControlController;
	import com.alexomara.ane.AIRControl.controllers.elements.AIRControlControllerAxis;
	import com.alexomara.ane.AIRControl.controllers.elements.AIRControlControllerButton;
	import com.alexomara.ane.AIRControl.controllers.elements.AIRControlControllerPOV;
	import com.alexomara.ane.AIRControl.events.AIRControlControllerEvent;
	import com.alexomara.ane.AIRControl.events.AIRControlEvent;
	
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.IEventDispatcher;
	import flash.external.ExtensionContext;
	import flash.utils.Dictionary;
	import flash.utils.getTimer;
	
	/**
	 * Dispatched on controller attach.
	 * 
	 * @eventType com.alexomara.ane.AIRControl.events.AIRControlEvent
	 */
	[Event(name="AIRControlEvent.controllerAttach", type="com.alexomara.ane.AIRControl.events.AIRControlEvent")]
	
	/**
	 * Dispatched on controller detach.
	 * 
	 * @eventType com.alexomara.ane.AIRControl.events.AIRControlEvent
	 */
	[Event(name="AIRControlEvent.controllerDetach", type="com.alexomara.ane.AIRControl.events.AIRControlEvent")]
	
	/**
	 * The AIRControl static class is used to interface with the AIRControl native extension.
	 * 
	 * <p>This class cannot be instantiated and will throw an error if attempted. All properties and methods must be used statically.</p>
	 */
	public class AIRControl implements IEventDispatcher
	{
		/**
		 * The major and minor versions of this extension.
		 */
		public static const VERSION:Number = 1.1;
		/**
		 * The revision and build numbers of this extension.
		 */
		public static const REVISION:Number = 0.0;
		/**
		 * Set to a function that takes a single string argument to get debug output. Set to null to disable.
		 */
		public static var debugOutput:Function = null;
		
		private static const _eventDispatcher:EventDispatcher = new EventDispatcher();
		private static const _eventCache:Dictionary = new Dictionary(true);
		
		private static var _context:ExtensionContext;
		private static var _controllers:Vector.<AIRControlController> = new Vector.<AIRControlController>();
		private static var _controllersDetached:Vector.<AIRControlController> = new Vector.<AIRControlController>();
		private static var _controllersAttached:Vector.<AIRControlController> = new Vector.<AIRControlController>();
		
		/**
		 * This is not an instantiable class.
		 * 
		 * <p>Throws an error to prevent instantiating.</p>
		 * 
		 * @private
		 */
		public function AIRControl()
		{
			//Warn the user that they are doing it wrong.
			throw new Error("AIRControl is not an instantiable class.");
		}
		
		/**
		 * Updates the controller states.
		 * 
		 * <p>Calls the method in the native code to get the current state of the controllers, updates all the controllers, and dispatches events. This function must be called manually every time the application should recieve updated controller information such as on ENTER_FRAME or EXIT_FRAME.</p>
		 * 
		 * <p>This function will also create the native extension context on first call and any first calls following the dispose method.</p>
		 * 
		 * <ul>
		 * <li>On Windows checking for new controllers is limited to once per second to avoid performance issues.</li>
		 * <li>On Mac OS X new controllers are detected immediately.</li>
		 * </ul>
		 * 
		 * @see com.alexomara.ane.AIRControl.events.AIRControlEvent#CONTROLLER_ATTACH
		 * @see com.alexomara.ane.AIRControl.events.AIRControlEvent#CONTROLLER_DETACH
		 * @see com.alexomara.ane.AIRControl.events.AIRControlControllerEvent#AXIS_CHANGE
		 * @see com.alexomara.ane.AIRControl.events.AIRControlControllerEvent#POV_CHANGE
		 * @see com.alexomara.ane.AIRControl.events.AIRControlControllerEvent#BUTTON_CHANGE
		 * @see #dispose()
		 */
		public static function update():void
		{
			var statesString:String;
			var states:Array;
			var i:uint;
			var il:uint;
			var j:uint;
			var jl:uint;
			var state:Array;
			var ID:uint;
			var axes:Array;
			var povs:Array;
			var buttons:Array;
			var axesLetters:String;
			var vendorID:uint;
			var productID:uint;
			var name:String;
			var controller:AIRControlController;
			var temp:Vector.<AIRControlController>;
			var axis:AIRControlControllerAxis;
			var pov:AIRControlControllerPOV;
			var button:AIRControlControllerButton;
			var axisPrevPosition:Number;
			var povPrevX:int;
			var povPrevY:int;
			var buttonPrevDown:Boolean;
			var debugMode:Boolean;
			//Initialize the extension context if not yet created.
			if(!_context)
			{
				//Output creating extension context.
				debugOutput && debugOutput("AIRControl[" + getTimer() + "]: Creating extension context.");
				_context = ExtensionContext.createExtensionContext("com.alexomara.ane.AIRControl.AIRControl", "");
			}
			
			//Remember if calling extension in debug mode even if debugOutput is nulled.
			debugMode = debugOutput !== null ? true : false;
			
			//Output the time before calling extension.
			debugOutput && debugOutput("AIRControl[" + getTimer() + "]: Calling native function.");
			
			//Call the native extension method to get the current controller states.
			statesString = ( debugMode ? _context.call("AIRControl", true) : _context.call("AIRControl") ) as String;
			
			//Output the time completed and raw response.
			debugOutput && debugOutput("AIRControl[" + getTimer() + "]: Completed native function call, RAW OUTPUT: " + statesString);
			
			if(statesString === null)
			{
				//There is something wrong with the native extension.
				throw new Error("Native extension call returned invalid data.");
			}
			else
			{
				//Make sure attached and detached vectors are empty.
				_controllersAttached.length = 0;
				_controllersDetached.length = 0;
				
				//Assume all controllers are detached until found later, rather than copy the entire vector, swap the two vectors.
				temp = _controllersDetached;
				_controllersDetached = _controllers;
				_controllers = temp;
				temp = null;
				
				//Check if any controllers are listed.
				if(statesString.length)
				{
					//Split the list on the delimiter.
					states = statesString.split("\t\t");
					il = states.length;
					for(i = 0; i < il; i++)
					{
						//Skip the debug output if present.
						if(debugMode && i == il-1 && states[i].indexOf("DEBUG") === 0)
						{
							continue;
						}
						
						//Split the details about this controller on the delimiter.
						state = states[i].split("\t");
						if(state.length < 8)
						{
							//Invalid information, skip controller.
							continue;
						}
						
						//Output start of controller states.
						debugOutput && debugOutput("AIRControl[" + getTimer() + "]: Parsing state started for controller ID: " + state[0]);
						
						//Read in data about the controller, making sure fields that are not available or underscores are empty arrays.
						ID = uint(state[0]);
						axes = state[1] === "_" ? [] : state[1].split(",");
						povs = state[2] === "_" ? [] : state[2].split(",");
						buttons = state[3] === "_" ? [] : state[3].split("");
						axesLetters = state[4];
						vendorID = uint(state[5]);
						productID = uint(state[6]);
						name = state[7];
						
						//Make sure the POVs are in pairs.
						if(buttons.length % 2 !== 0)
						{
							buttons.push("0");
						}
						
						//Preliminary type-casting.
						state[0] = uint(state[0]);
						state[5] = uint(state[5]);
						state[6] = uint(state[6]);
						
						//Loop over the remaining unmatched controllers.
						controller = null;
						jl = _controllersDetached.length;
						for(j = 0; j < jl; j++)
						{
							//Check if the controller at the index matches.
							if(_controllersDetached[j] && _controllersDetached[j].ID === ID)
							{
								//The controller is still attached, add this controller back to the vector.
								controller = _controllersDetached[j];
								_controllers.push(controller);
								//Null this controller entry in the detached vector and break off the loop.
								_controllersDetached[j] = null;
								//Output controller matched.
								debugOutput && debugOutput("AIRControl[" + getTimer() + "]: Controller matched ID: " + ID);
								break;
							}
						}
						
						//If the controller was not matched, add a new one.
						if(!controller)
						{
							controller = new AIRControlController(ID, axes.length, povs.length/2, buttons.length, axesLetters, vendorID, productID, name);
							_controllersAttached.push(controller);
							//Output controller not matched.
							debugOutput && debugOutput("AIRControl[" + getTimer() + "]: Controller new ID: " + ID);
						}
						
						//Update the inputs on the controller.
						for(j = axes.length; j--;)
						{
							//Get the element and the previous state.
							axis = controller.AIRControlNS::axes[j];
							axisPrevPosition = axis.position;
							
							//Set the new state.
							axis.AIRControlNS::position = Number(axes[j]);
							
							//Fire change events if the element state has changed.
							if(axisPrevPosition !== axis.position)
							{
								//If event not yet cached, create and cache.
								if(!_eventCache[axis])
								{
									_eventCache[axis] = new AIRControlControllerEvent(AIRControlControllerEvent.AXIS_CHANGE, controller, axis, j);
									//Output new event creation.
									debugOutput && debugOutput("AIRControl[" + getTimer() + "]: New AXIS_CHANGE event object for controller ID: " + ID + " axis: " + j);
								}
								//Fire events.
								axis.dispatchEvent(_eventCache[axis]);
								controller.dispatchEvent(_eventCache[axis])
							}
						}
						for(j = povs.length/2; j--;)
						{
							//Get the element and the previous state.
							pov = controller.AIRControlNS::povs[j];
							povPrevX = pov.X;
							povPrevY = pov.Y;
							
							//Set the new state.
							pov.AIRControlNS::X = int(povs[j*2]);
							pov.AIRControlNS::Y = int(povs[j*2+1]);
							
							//Fire change events if the element state has changed.
							if(povPrevX !== pov.X || povPrevY !== pov.Y)
							{
								//If event not yet cached, create and cache.
								if(!_eventCache[pov])
								{
									_eventCache[pov] = new AIRControlControllerEvent(AIRControlControllerEvent.POV_CHANGE, controller, pov, j);
									//Output new event creation.
									debugOutput && debugOutput("AIRControl[" + getTimer() + "]: New POV_CHANGE event object for controller ID: " + ID + " POV: " + j);
								}
								//Fire events.
								pov.dispatchEvent(_eventCache[pov]);
								controller.dispatchEvent(_eventCache[pov]);
							}
						}
						for(j = buttons.length; j--;)
						{
							//Get the element and the previous state.
							button = controller.AIRControlNS::buttons[j];
							buttonPrevDown = button.down;
							
							//Set the new state.
							button.AIRControlNS::down = buttons[j] === "1";
							
							//Fire change events if the element state has changed.
							if(buttonPrevDown !== button.down)
							{
								//If event not yet cached, create and cache.
								if(!_eventCache[button])
								{
									_eventCache[button] = new AIRControlControllerEvent(AIRControlControllerEvent.BUTTON_CHANGE, controller, button, j);
									//Output new event creation.
									debugOutput && debugOutput("AIRControl[" + getTimer() + "]: New BUTTON_CHANGE event object for controller ID: " + ID + " button: " + j);
								}
								//Fire events.
								button.dispatchEvent(_eventCache[button]);
								controller.dispatchEvent(_eventCache[button]);
							}
						}
						//Output completion of controller states.
						debugOutput && debugOutput("AIRControl[" + getTimer() + "]: Parsing state completed for controller ID: " + state[0]);
					}
				}
				
				//Output current execution state.
				debugOutput && debugOutput("AIRControl[" + getTimer() + "]: Starting detach event dispatching.");
				
				//Loop over the detached controllers, firing events.
				il = _controllersDetached.length;
				for(i = 0; i < il; i++)
				{
					//If there is still a controller reference at this index it was not found and was detached.
					if(_controllersDetached[i])
					{
						//Fire the controller detach event.
						dispatchEvent(new AIRControlEvent(AIRControlEvent.CONTROLLER_DETACH, _controllersDetached[i], i));
					}
				}
				//Empty the list.
				_controllersDetached.length = 0;
				
				//Output current execution state.
				debugOutput && debugOutput("AIRControl[" + getTimer() + "]: Completed detach event dispatching, starting attach event dispatching.");
				
				//Loop over the attached controllers, firing events.
				il = _controllersAttached.length;
				for(i = 0; i < il; i++)
				{
					//Add to the list of controllers.
					_controllers.push(_controllersAttached[i]);
					//Fire the controller attach event.
					dispatchEvent(new AIRControlEvent(AIRControlEvent.CONTROLLER_ATTACH, _controllersAttached[i], _controllers.length-1));
				}
				//Empty the list.
				_controllersAttached.length = 0;
			}
			
			//Output finished.
			debugOutput && debugOutput("AIRControl[" + getTimer() + "]: Completed update.");
		}
		
		/**
		 * Detaches all the controllers and disposes of the native extension context.
		 * 
		 * <p>If the extensions will not be used for a period of time, this can be used to free up memory. Each currently connected controller will fire the CONTROLLER_DETACH event counting down.</p>
		 * 
		 * @see com.alexomara.ane.AIRControl.events.AIRControlEvent#CONTROLLER_DETACH
		 */
		public static function dispose():void
		{
			//Fire the detach event for each controller.
			var i:uint;
			for(i = _controllers.length; i--;)
			{
				dispatchEvent(new AIRControlEvent(AIRControlEvent.CONTROLLER_DETACH, _controllers[i], i));
			}
			//Clean up what we can.
			_controllers.length = 0;
			_controllersDetached.length = 0;
			_controllersAttached.length = 0;
			//Dispose of the native extension context.
			if(_context)
			{
				_context.dispose();
				_context = null;
			}
		}
		
		/**
		 * Returns the controller at a specific index.
		 * 
		 * <p>Game controller index positions are dynamic and change when controllers of lower index are detached. If a controller is detached, higher indexed controllers will shift lower to fill the space. If the controller is reattached, it is appended to the end of the list.</p>
		 * 
		 * <p>To avoid issues with referencing disconnected controllers, it is better to match controllers with CONTROLLER_ATTACH and CONTROLLER_DETACH.</p>
		 * 
		 * @param index The index of the controller element.
		 * 
		 * @return The controller at the specified index or null.
		 * 
		 * @see #controllersTotal
		 * @see com.alexomara.ane.AIRControl.events.AIRControlEvent#CONTROLLER_ATTACH
		 * @see com.alexomara.ane.AIRControl.events.AIRControlEvent#CONTROLLER_DETACH
		 */
		public static function controller(index:uint):AIRControlController
		{
			return index < _controllers.length ? _controllers[index] : null;
		}
		
		/**
		 * The total controllers currently connected.
		 * 
		 * <ul>
		 * <li>On Windows the maximum supported controllers is determined by joyGetNumDevs(), which returns 16 on current versions of Windows.</li>
		 * <li>On Mac OS X there is no explicitly imposed limit.</li>
		 * </ul>
		 * 
		 * @see #controller()
		 */
		public static function get controllersTotal():uint
		{
			return _controllers.length;
		}
		
		//Implement all the functions for IEventDispatcher so that event listeners can be added to this class.
		/**
		 * @see com.alexomara.ane.AIRControl.events.AIRControlEvent#CONTROLLER_ATTACH
		 * @see com.alexomara.ane.AIRControl.events.AIRControlEvent#CONTROLLER_DETACH
		 */
		static public function addEventListener(type:String, listener:Function, useCapture:Boolean = false, priority:int = 0, useWeakReference:Boolean = false):void
		{
			_eventDispatcher.addEventListener(type, listener, useCapture, priority, useWeakReference);
		}
		
		/**
		 * Internally required.
		 * 
		 * @private 
		 */
		public function addEventListener(type:String, listener:Function, useCapture:Boolean = false, priority:int = 0, useWeakReference:Boolean = false):void
		{
			_eventDispatcher.addEventListener(type, listener, useCapture, priority, useWeakReference);
		}
		
		/**
		 * @see com.alexomara.ane.AIRControl.events.AIRControlEvent#CONTROLLER_ATTACH
		 * @see com.alexomara.ane.AIRControl.events.AIRControlEvent#CONTROLLER_DETACH
		 */
		static public function removeEventListener(type:String, listener:Function, useCapture:Boolean = false):void
		{
			_eventDispatcher.removeEventListener(type, listener, useCapture);
		}
		
		/**
		 * Internally required.
		 * 
		 * @private
		 */
		public function removeEventListener(type:String, listener:Function, useCapture:Boolean = false):void
		{
			_eventDispatcher.removeEventListener(type, listener, useCapture);
		}
		
		/**
		 * @see com.alexomara.ane.AIRControl.events.AIRControlEvent#CONTROLLER_ATTACH
		 * @see com.alexomara.ane.AIRControl.events.AIRControlEvent#CONTROLLER_DETACH
		 */
		static public function dispatchEvent(event:Event):Boolean
		{
			return _eventDispatcher.dispatchEvent(event);
		}
		
		/**
		 * Internally required.
		 * 
		 * @private
		 */
		public function dispatchEvent(event:Event):Boolean
		{
			return _eventDispatcher.dispatchEvent(event);
		}
		
		/**
		 * @see com.alexomara.ane.AIRControl.events.AIRControlEvent#CONTROLLER_ATTACH
		 * @see com.alexomara.ane.AIRControl.events.AIRControlEvent#CONTROLLER_DETACH
		 */
		static public function hasEventListener(type:String):Boolean
		{
			return _eventDispatcher.hasEventListener(type);
		}
		
		/**
		 * Internally required.
		 * 
		 * @private
		 */
		public function hasEventListener(type:String):Boolean
		{
			return _eventDispatcher.hasEventListener(type);
		}
		
		/**
		 * @see com.alexomara.ane.AIRControl.events.AIRControlEvent#CONTROLLER_ATTACH
		 * @see com.alexomara.ane.AIRControl.events.AIRControlEvent#CONTROLLER_DETACH
		 */
		static public function willTrigger(type:String):Boolean
		{
			return _eventDispatcher.willTrigger(type);
		}
		
		/**
		 * Internally required.
		 * 
		 * @private
		 */
		public function willTrigger(type:String):Boolean
		{
			return _eventDispatcher.willTrigger(type);
		}
	}
}