#include "Driver/Mouse/Mouse.hpp"
#include <chrono>
#include <thread>
#include <iostream>

int main()
{
    Mouse mouse;

    // Count for DefaultReset
    int count = 0;

    while (true) // Maybe change this
    {
        if (count == 22)
        {
            mouse.blockDefaultReset();
            count = 0;
            std::cout << "Default Settings blocked!" << std::endl;
        }

        if (mouse.decideMode())
        {
            if (mouse.getLastMode() != mouse.getCurrentMode())
            {
                mouse.setPollingRate(0x04);

                // Sometime the restart of pullingrate needs to be delayed to set the other things
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                // cause of sleep
                count++;

                mouse.setEssentials();

                mouse.setColor(Color{255, 0, 0});

                mouse.setBrightness(1000);

                mouse.setDPI(800);
            }
        }

        count++;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    return 0;
}