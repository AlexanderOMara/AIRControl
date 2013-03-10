#include "gamecontroller_mac.h"
#include <iostream>

using namespace std;

int main()
{
	while(true)
	{
		cout << ControlStates() << endl;
		usleep(1000/60);
	}
    return 0;
}
