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
package
{
	/*
	How to import this example into Flash Builder:
	File > New > ActionScript Project.
	Enter AIRControl_Example_ShowAllInput as the project name.
	Choose the AIRControl_Example_ShowAllInput folder as the project location.
	Set application type to Desktop (Adobe AIR).
	Click Next.
	Under Native Extensions add the AIRControl.ane and choose Update AIR application descriptor.
	*/
	import com.alexomara.ane.AIRControl.AIRControl;
	import com.alexomara.ane.AIRControl.controllers.AIRControlController;
	import com.alexomara.ane.AIRControl.events.AIRControlControllerEvent;
	import com.alexomara.ane.AIRControl.events.AIRControlEvent;
	
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.events.KeyboardEvent;
	import flash.text.TextField;
	import flash.text.TextFormat;
	
	[SWF(width="800", height="600", frameRate="30", backgroundColor="#FFFFFF", quality="HIGH", useGPU="false", useDirectBlit="false")]
	
	public class AIRControl_Example_ShowAllInput extends Sprite
	{
		private var instructionsFormat:TextFormat = new TextFormat();
		private var textFormat:TextFormat = new TextFormat();
		private var headerFormat:TextFormat = new TextFormat();
		private var instructions:TextField = new TextField();
		private var recentEvents:TextField = new TextField();
		private var recentEventsHeader:TextField = new TextField();
		private var currentController:TextField = new TextField();
		private var currentControllerHeader:TextField = new TextField();
		private var currentIndex:uint = 0;
		
		public function AIRControl_Example_ShowAllInput()
		{
			//Setup the text fotmats.
			instructionsFormat.font = "Arial";
			instructionsFormat.size = 14;
			instructionsFormat.align = "center";
			textFormat.font = "_typewriter";
			textFormat.size = 10;
			textFormat.blockIndent = 12;
			textFormat.indent = -12;
			headerFormat.font = "Arial";
			headerFormat.bold = true;
			headerFormat.size = 12;
			
			//Add the instructions text field.
			instructions.width = 800;
			instructions.height = 24;
			instructions.x = 0;
			instructions.y = 0;
			instructions.defaultTextFormat = instructionsFormat;
			instructions.background = true;
			instructions.backgroundColor = 0xEEEEEE;
			instructions.border = true;
			instructions.borderColor = 0x999999;
			instructions.wordWrap = true;
			instructions.text = "Press the left and right cursor keys or the plus and minus keys to cycle between the connected controllers.";
			this.addChild(instructions);
			
			//Add the text field for the current controller information.
			currentController.width = 200;
			currentController.height = 560;
			currentController.x = 0;
			currentController.y = 40;
			currentController.defaultTextFormat = textFormat;
			currentController.background = true;
			currentController.backgroundColor = 0xEEEEEE;
			currentController.border = true;
			currentController.borderColor = 0x999999;
			currentController.wordWrap = true;
			this.addChild(currentController);
			
			//Add the header for the current controller information.
			currentControllerHeader.width = currentController.width;
			currentControllerHeader.height = headerFormat.size + 5;
			currentControllerHeader.x = currentController.x;
			currentControllerHeader.y = currentController.y - currentControllerHeader.height;
			currentControllerHeader.defaultTextFormat = headerFormat;
			currentControllerHeader.text = "Current Controller (0/0)";
			currentControllerHeader.background = true;
			currentControllerHeader.backgroundColor = 0xEEEEEE;
			currentControllerHeader.border = true;
			currentControllerHeader.borderColor = 0x999999;
			this.addChild(currentControllerHeader);
			
			//Add the text field for the recent events log.
			recentEvents.width = 600;
			recentEvents.height = 560;
			recentEvents.y = 40;
			recentEvents.x = 200;
			recentEvents.defaultTextFormat = textFormat;
			recentEvents.background = true;
			recentEvents.backgroundColor = 0xEEEEEE;
			recentEvents.border = true;
			recentEvents.borderColor = 0x999999;
			recentEvents.wordWrap = true;
			this.addChild(recentEvents);
			
			//Add the header for the recent events log.
			recentEventsHeader.width = recentEvents.width;
			recentEventsHeader.height = headerFormat.size + 5;
			recentEventsHeader.x = recentEvents.x;
			recentEventsHeader.y = recentEvents.y - recentEventsHeader.height;
			recentEventsHeader.defaultTextFormat = headerFormat;
			recentEventsHeader.text = "Recent Events";
			recentEventsHeader.background = true;
			recentEventsHeader.backgroundColor = 0xEEEEEE;
			recentEventsHeader.border = true;
			recentEventsHeader.borderColor = 0x999999;
			this.addChild(recentEventsHeader);
			
			//Add the event listeners to detect controller attachment.
			AIRControl.addEventListener(AIRControlEvent.CONTROLLER_ATTACH, controllerAttach);
			AIRControl.addEventListener(AIRControlEvent.CONTROLLER_DETACH, controllerAttach);
			
			//Add the event listeners for every frame and keyboard input.
			stage.addEventListener(Event.ENTER_FRAME, enterFrame);
			stage.addEventListener(KeyboardEvent.KEY_DOWN, keyboardEvent);
		}
		
		private function controllerAttach(e:AIRControlEvent):void
		{
			//Fired on controller attach and detach.
			var output:String;
			switch(e.type)
			{
				case AIRControlEvent.CONTROLLER_ATTACH:
					//Add event listeners for controller state changes.
					e.controller.addEventListener(AIRControlControllerEvent.AXIS_CHANGE, controllerEvent);
					e.controller.addEventListener(AIRControlControllerEvent.BUTTON_CHANGE, controllerEvent);
					e.controller.addEventListener(AIRControlControllerEvent.POV_CHANGE, controllerEvent);
					//Log the attach and a few key variables.
					output = "AIRControlEvent.CONTROLLER_ATTACH, controllerIndex=" + e.controllerIndex + ", controller.ID=" + e.controller.ID + ", controller.name=\"" + e.controller.name + "\"";
					recentEvents.replaceText(0, 0, output + "\n");
					trace(output);
				break;
				case AIRControlEvent.CONTROLLER_DETACH:
					//Remove the event listeners.
					e.controller.removeEventListener(AIRControlControllerEvent.AXIS_CHANGE, controllerEvent);
					e.controller.removeEventListener(AIRControlControllerEvent.BUTTON_CHANGE, controllerEvent);
					e.controller.removeEventListener(AIRControlControllerEvent.POV_CHANGE, controllerEvent);
					//Log the detach and a few key variables.
					output = "AIRControlEvent.CONTROLLER_DETACH, controllerIndex=" + e.controllerIndex + ", controller.ID=" + e.controller.ID + ", controller.name=\"" + e.controller.name + "\"";
					recentEvents.replaceText(0, 0, output + "\n");
					trace(output);
				break;
			}
		}
		
		private function controllerEvent(e:AIRControlControllerEvent):void
		{
			//Log information about what controller fired the event.
			var output:String;
			switch(e.type)
			{
				case AIRControlControllerEvent.AXIS_CHANGE:
					output = "AIRControlControllerEvent.AXIS_CHANGE, controller.ID=" + e.controller.ID + ", controller.name=\"" + e.controller.name + "\", elementIndex=" + e.elementIndex + "\", element.position=" + e.element["position"];
					recentEvents.replaceText(0, 0, output + "\n");
					trace(output);
				break;
				case AIRControlControllerEvent.POV_CHANGE:
					output = "AIRControlControllerEvent.POV_CHANGE, controller.ID=" + e.controller.ID + ", controller.name=\"" + e.controller.name + "\", elementIndex=" + e.elementIndex + "\", element.X=" + e.element["X"] + ", element.Y=" + e.element["Y"];
					recentEvents.replaceText(0, 0, output + "\n");
					trace(output);
				break;
				case AIRControlControllerEvent.BUTTON_CHANGE:
					output = "AIRControlControllerEvent.BUTTON_CHANGE, controller.ID=" + e.controller.ID + ", controller.name=\"" + e.controller.name + "\", elementIndex=" + e.elementIndex + "\", element.down=" + e.element["down"];
					recentEvents.replaceText(0, 0, output + "\n");
					trace(output);
				break;
			}
		}
		
		private function keyboardEvent(e:KeyboardEvent):void
		{
			//Increment or decremennt the controller index when appropriate.
			switch(e.keyCode)
			{
				case 39://right cursor key
				case 187://plus key
					if(currentIndex < AIRControl.controllersTotal-1)
					{
						currentIndex++;
					}
				break;
				case 37://left cursor key
				case 189://minus key
					if(currentIndex)
					{
						currentIndex--;
					}
				break;
			}
		}
		
		private function enterFrame(e:Event):void
		{
			//Update the controller states.
			AIRControl.update();
			var text:String;
			var controller:AIRControlController;
			var i:uint;
			//Make sure the current controller index is not out of range.
			while(currentIndex && currentIndex > AIRControl.controllersTotal-1)
			{
				currentIndex--;
			}
			//Check if controllers are attached.
			if(AIRControl.controllersTotal)
			{
				//Get the controller at the specified index. NOTE: It is usually better to match controllers on attach/detach than requesting specific indexes.
				controller = AIRControl.controller(currentIndex);
				
				//Set the current controller header.
				currentControllerHeader.text = "Current Controller " + (currentIndex+1) + "/" + AIRControl.controllersTotal + ", index: " + currentIndex + "";
				
				//Get the details about the controller.
				text = "ID:\n\t" + controller.ID + 
					"\n\nname:\n\t" + controller.name + 
					"\n\nvendorID:\n\t" + controller.vendorID + 
					"\n\nproductID:\n\t" + controller.productID + 
					"\n\nAXIS_X:\n\t" + controller.AXIS_X + 
					"\n\nAXIS_Y:\n\t" + controller.AXIS_Y + 
					"\n\nAXIS_Z:\n\t" + controller.AXIS_Z + 
					"\n\nAXIS_R:\n\t" + controller.AXIS_R + 
					"\n\nAXIS_U:\n\t" + controller.AXIS_U + 
					"\n\nAXIS_V:\n\t" + controller.AXIS_V;
				
				//Loop over the axes.
				text += "\n\naxes (" + controller.axesTotal + "):";
				for(i = 0; i < controller.axesTotal; i++)
				{
					text += "\n\t" + controller.axis(i).position;
				}
				//Loop over the POV hat switches.
				text += "\n\npovs (" + controller.povsTotal + "):";
				for(i = 0; i < controller.povsTotal; i++)
				{
					text += "\n\tX: " + controller.pov(i).X + " \tY: " + controller.pov(i).Y;
				}
				//Loop over the buttons.
				text += "\n\nbuttons (" + controller.buttonsTotal + "):\n\t";
				for(i = 0; i < controller.buttonsTotal; i++)
				{
					text += controller.button(i).down ? "1" : "0";
				}
				//Set the text.
				currentController.text = text;
			}
			else
			{
				currentController.text = "No controllers detected.";
				currentControllerHeader.text = "Current Controller (0/0)";
			}
		}
	}
}