#include <iostream>
#include <unistd.h>
#include "Driver/Mouse/MouseDriver.hpp"

int main()
{
    MouseDriver mouse;

    //Count for DefaultReset
    int count = 0;

    while (true) // Maybe change this
    {
        if (count == 22)
        {
            mouse.blockDefaultReset();
            count = 0;
        }
        
        if (mouse.decideMode())
        {
            if (mouse.getLastMode() != mouse.getCurrentMode())
            {
                mouse.setPollingRate(0x04);

                mouse.setEssentials();

                mouse.setColor(0xff, 0, 0);

                mouse.setDPI(800);
            }
        }

        count++;
        sleep(1);
    }

     return 0;
}