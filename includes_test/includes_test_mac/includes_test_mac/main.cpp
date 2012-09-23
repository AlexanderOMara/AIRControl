#include "gamecontroller_mac.h"
#include <iostream>

using namespace std;

int main()
{
	while(true)
	{
		cout << ControlStates() << endl;
		sleep(1);
	}
    return 0;
}
