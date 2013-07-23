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
	
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.text.TextField;
	import flash.utils.getTimer;
	
	[SWF(width="100", height="100", frameRate="60", backgroundColor="#FFFFFF", quality="HIGH", useGPU="true", useDirectBlit="true")]
	
	public class AIRControl_Example_DebugOutput extends Sprite
	{
		private var controller:AIRControlController;
		private var fps:TextField = new TextField();
		private var lastFrame:uint = getTimer();
		private var traceBuffer:String = "";
		
		public function AIRControl_Example_DebugOutput()
		{
			//Add the FPS text field.
			fps.width = stage.stageWidth;
			fps.height = stage.stageHeight;
			this.addChild(fps);
			
			//Add a callback for debug output.
			AIRControl.debugOutput = function(s:String):void{
				//trace statements are a little slow, buffer all the output to trace at once.
				traceBuffer += s + "\n";
			};
			
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
					if(!controller)
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
			
			//Trace the debug output.
			trace(traceBuffer);
			traceBuffer = "";
			
			//Update the FPS timer.
			var time:uint = getTimer();
			fps.text = "FPS: " + int(1000/(time - lastFrame));
			trace(fps.text);
			lastFrame = time;
		}
	}
}