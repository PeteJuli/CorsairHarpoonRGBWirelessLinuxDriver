#include <iostream>
#include <unistd.h>
#include <bit>
#include <cstdint>
#include "Driver/Mouse/MouseDriver.hpp"

int main()
{
    MouseDriver mouse;

    mouse.create();

    while (true) //Maybe change this
    {
        mouse.running();
    }

    //cleanup
    mouse.destroy();
    return 0;
}
