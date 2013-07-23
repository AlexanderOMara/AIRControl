#include "gamecontroller_win.h"
#include <iostream>
#include <windows.h>

using namespace std;

int main()
{
	ControlInit();

	while(true)
	{
		cout << ControlStates(true) << endl;
		Sleep(1000/60);
	}
    return 0;
}
