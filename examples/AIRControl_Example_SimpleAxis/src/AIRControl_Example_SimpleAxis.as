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
	Enter AIRControl_Example_SimpleAxis as the project name.
	Choose the AIRControl_Example_SimpleAxis folder as the project location.
	Set application type to Desktop (Adobe AIR).
	Click Next.
	Under Native Extensions add the AIRControl.ane and choose Update AIR application descriptor.
	*/
	import com.alexomara.ane.AIRControl.AIRControl;
	import com.alexomara.ane.AIRControl.controllers.AIRControlController;
	import com.alexomara.ane.AIRControl.events.AIRControlEvent;
	
	import flash.display.Shape;
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.text.TextField;
	import flash.text.TextFormat;
	
	[SWF(width="800", height="600", frameRate="30", backgroundColor="#FFFFFF", quality="HIGH", useGPU="false", useDirectBlit="false")]
	
	public class AIRControl_Example_SimpleAxis extends Sprite
	{
		private var controller:AIRControlController;
		private var circle:Shape
		private var textFormat:TextFormat;
		private var textField:TextField;
		
		public function AIRControl_Example_SimpleAxis()
		{
			//Create a shape to control.
			circle = new Shape();
			circle.graphics.beginFill(0x000000);
			circle.graphics.drawCircle(0, 0, 10);
			circle.graphics.endFill();
			circle.x = stage.stageWidth / 2;
			circle.y = stage.stageHeight / 2;
			this.addChild(circle);
			
			textFormat = new TextFormat();
			textFormat.font = "Arial";
			textFormat.size = 14;
			textFormat.align = "center";
			
			textField = new TextField();
			textField.x = 0;
			textField.y = 0;
			textField.width = stage.stageWidth;
			textField.height = 30;
			textField.wordWrap = true;
			textField.defaultTextFormat = textFormat;
			textField.text = "This example demonstrates the floating-point (Number) input of game controllers.";
			this.addChild(textField);
			
			//Add events listeners to AIRControl.
			AIRControl.addEventListener(AIRControlEvent.CONTROLLER_ATTACH, controllerAttachDetach);
			AIRControl.addEventListener(AIRControlEvent.CONTROLLER_DETACH, controllerAttachDetach);
			
			//Add the frame updater.
			stage.addEventListener(Event.ENTER_FRAME, enterFrame);
		}
		
		private function controllerAttachDetach(e:AIRControlEvent):void
		{
			switch(e.type)
			{
				case AIRControlEvent.CONTROLLER_ATTACH:
					//If there is not controller currently attached, use the one just attached.
					if(!controller && e.controller.axesTotal >= 2)
					{
						controller = e.controller;
					}
				break;
				case AIRControlEvent.CONTROLLER_DETACH:
					//If the current in use was detached, stop using it.
					if(controller === e.controller)
					{
						controller = null;
					}
				break;
			}
		}
		
		private function enterFrame(e:Event):void
		{
			//Update the controller states.
			AIRControl.update();
			
			//If there is a controller connected.
			if(controller)
			{
				//Get the axis positions.
				//NOTE: It is usually better to allow the user to configure which axis they want to use and save it to a variable.
				var x:Number = controller.axis(0).position;
				var y:Number = controller.axis(1).position;
				
				//Move the circle.
				circle.x += x * 10;
				circle.y += y * 10;
				
				//Wrap the movement around the stage.
				if(circle.x > stage.stageWidth)
				{
					circle.x = 0;
				}
				else if(circle.x < 0)
				{
					circle.x = stage.stageWidth;
				}
				
				if(circle.y > stage.stageHeight)
				{
					circle.y = 0;
				}
				else if(circle.y < 0)
				{
					circle.y = stage.stageHeight;
				}
			}
		}
	}
}