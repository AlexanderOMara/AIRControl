#include "gamecontroller_mac.h"
#include <iostream>

using namespace std;

void runLoopTimerCallback()
{
	cout << ControlStates() << endl;
}

int main()
{
	//Initialize the controller library.
	ControlInit();
	//Create a timer.
	CFRunLoopTimerRef runLoopTimer = CFRunLoopTimerCreate(NULL, CFAbsoluteTimeGetCurrent(), 0.016, 0, 0, (CFRunLoopTimerCallBack)runLoopTimerCallback, NULL);
	//Attach the timer to the loop.
	CFRunLoopAddTimer(CFRunLoopGetCurrent(), runLoopTimer, kCFRunLoopCommonModes);
	//Run the loop.
	CFRunLoopRun();
    return 0;
}

