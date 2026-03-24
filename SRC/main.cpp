#include <iostream>
#include <unistd.h>
#include <bit>
#include <cstdint>
#include "Driver/Mouse/MouseDriver.hpp"

//Hardcode...
int dpi = 800;
int mainColor[4] = {255, 0, 0, 100};//RGBA
int PollingRate = 3; //0=125HZ, 1=250Hz, 2=500Hz, 3=1000Hz

int main()
{
    MouseDriver mouse;

    mouse.create();

    while (true)
    {
        mouse.running();
    }

    //cleanup
    mouse.destroy();
    return 0;
}