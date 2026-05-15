#include "MouseDriver.hpp"
#include <iostream>
#include <unistd.h>

//Two bools which are necessary
bool MouseDriver::setEssentials()
{
    uint8_t essentials0[64]{};//Maybe some kind of customeMode bool?
    essentials0[1] = 0x01;
    essentials0[2] = 0x03;
    essentials0[4] = 0x02;

    uint8_t essentials1[64]{};//Set ColorMode to static I guess...
    essentials1[1] = 0x0d;
    essentials1[3] = 0x01;

    if (mode == 0)//Mode
    {
        essentials0[0] = 0x08;
        essentials1[0] = 0x08;

        return cableUSB.write(essentials0) && cableUSB.write(essentials1);
    }else
    {
        essentials0[0] = 0x0a;
        essentials1[0] = 0x0a;

        return wirlessUSB.write(essentials0) && wirlessUSB.write(essentials1);
    }

    return false;
}

uint32_t swapEndian(uint32_t value)
{
    return (value >> 24) |
           ((value >> 8) & 0x0000FF00) |
           ((value << 8) & 0x00FF0000) |
           (value << 24);
}

void splitBytes(uint32_t value, uint8_t * bytes)
{
    // Split uint32_t into 4 uint8_t
    bytes[0] = (value >> 24) & 0xFF; // Most significant byte
    bytes[1] = (value >> 16) & 0xFF;
    bytes[2] = (value >> 8) & 0xFF;
    bytes[3] = value & 0xFF;         // Least significant byte
}

bool MouseDriver::setDPI(uint32_t DPI)
{
    uint8_t buffer[64]{};
    buffer[1] = 0x01;
    buffer[2] = 0x20;
    uint8_t dpiValue[4];
    splitBytes(swapEndian(DPI), dpiValue);
    buffer[4] = dpiValue[0];
    buffer[5] = dpiValue[1];
    //buffer[6] = dpiValue[2];//Dont need them for dpi
    //buffer[7] = dpiValue[3];

    if (mode == 0)//Mode
    {
        buffer[0] = 0x08;
        return cableUSB.write(buffer);
    }else
    {
        buffer[0] = 0x0a;
        return wirlessUSB.write(buffer);
    }

    return false;
}

bool MouseDriver::setColor(uint8_t Red, uint8_t Green, uint8_t Blue, uint8_t Alpha)
{
    uint8_t buffer[64]{};
    for (int i = 0; i < 64; i++)
    {
        buffer[i] = 0x0;
    }

    buffer[1] = 0x06;
    buffer[3] = 0x06;
    buffer[7] = 0xff;
    buffer[8] = Red;
    buffer[10] = Green;
    buffer[12] = Blue;

    if (mode == 0)//Cable
    {
        buffer[0] = 0x08;

        cableUSB.write(buffer);
        return true;
    }else
    {
        buffer[0] = 0x0a;
        return wirlessUSB.write(buffer);
    }

    return false;
}

void MouseDriver::decideMode()
{
    //Need some rework with plug and unplug of the Mice... Maybe do it with "popen("lsusb -d 1b1c:1bdc", "r");"
    if (wirlessUSB.isCreated() && !cableUSB.isCreated())//Only in this case choose wirelsee
    {
        usbCount = wirlessUSB.getDeviceListCount();
        mode = 1;
    }else//all other choose cable
    {
        usbCount = cableUSB.getDeviceListCount();
        mode = 0;
    }
}

bool MouseDriver::create()
{
    mode = -1;

    cableUSB.create(0x1b1c, 0x1b5e);

    wirlessUSB.create(0x1b1c, 0x1bdc);

    if (cableUSB.isCreated() || wirlessUSB.isCreated())
    {

        //Mode decide
        decideMode();

        if (mode != -1)
        {
            setEssentials();

            //Change this with variables for easy changing...
            setColor(0xff, 0, 0, 0);
            setDPI(800);

            return true;
        }
    }


   return false;
}

bool MouseDriver::running()
{
    if (mode == 0)
    {
        if (usbCount != cableUSB.getDeviceListCount())
        {
            destroy();

            create();
        }
    }

    if (mode == 1)
    {
        if (usbCount != wirlessUSB.getDeviceListCount())
        {
            destroy();

            create();
        }
    }


    return false;
}

bool MouseDriver::blockDefaultReset()
{
    //Every 22 seconds this needs to be sent if not -> The mouse resets to default mode
    sleep(22);

    uint8_t buffer[64]{};
    buffer[1] = 0x12;

    if (mode == 0)//Cable
    {
        buffer[0] = 0x08;
        return cableUSB.write(buffer);
    }else
    {
        buffer[0] = 0x0a;
        return wirlessUSB.write(buffer);
    }

    return false;
}

void MouseDriver::destroy()
{
    if (cableUSB.isCreated())
        cableUSB.destroy();

    if (wirlessUSB.isCreated())
        wirlessUSB.destroy();
}
