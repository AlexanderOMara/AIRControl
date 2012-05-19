#include "gamecontroller_win.h"
#include <iostream>
#include <windows.h>

using namespace std;

int main()
{
	while(true)
	{
		cout << ControlStates() << endl;
		Sleep(1000);
	}
    return 0;
}
