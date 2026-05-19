#include "MouseDriver.hpp"
#include <iostream>
#include <unistd.h>

// Constants
constexpr uint16_t CorsairVendorID = 0x1b1c;
constexpr uint16_t CablePID = 0x1b5e;
constexpr uint16_t WirlessPID = 0x1bdc;
constexpr uint8_t CableOffset = 0x08;
constexpr uint8_t WirlessOffset = 0x0a;

bool MouseDriver::writeToMouse(std::span<uint8_t, 64> buffer)
{
    switch (currentMode)
    {
    case ConnectionType::Unknown:
        return false;
    case ConnectionType::Cable:
        buffer[0] = CableOffset;
        return cableUSB->write(buffer);
    case ConnectionType::Wireless:
        buffer[0] = WirlessOffset;
        return wirlessUSB->write(buffer);
    }
}

// Two bools which are necessary
bool MouseDriver::setEssentials()
{
    // Maybe some kind of customeMode bool?
    std::array<uint8_t, 64> essentials0{};
    essentials0[1] = 0x01;
    essentials0[2] = 0x03;
    essentials0[4] = 0x02;

    // Set ColorMode to static I guess...
    std::array<uint8_t, 64> essentials1{};
    essentials1[1] = 0x0d;
    essentials1[3] = 0x01;

    return writeToMouse(essentials0) && writeToMouse(essentials1);
}

uint32_t swapEndian(uint32_t value)
{
    return (value >> 24) |
           ((value >> 8) & 0x0000FF00) |
           ((value << 8) & 0x00FF0000) |
           (value << 24);
}

void splitBytes(uint32_t value, uint8_t *bytes)
{
    // Split uint32_t into 4 uint8_t
    bytes[0] = (value >> 24) & 0xFF; // Most significant byte
    bytes[1] = (value >> 16) & 0xFF;
    bytes[2] = (value >> 8) & 0xFF;
    bytes[3] = value & 0xFF; // Least significant byte
}

bool MouseDriver::setDPI(uint32_t DPI)
{
    std::array<uint8_t, 64> buffer{};

    buffer[1] = 0x01;
    buffer[2] = 0x20;
    uint8_t dpiValue[4];
    splitBytes(swapEndian(DPI), dpiValue);
    buffer[4] = dpiValue[0];
    buffer[5] = dpiValue[1];
    // buffer[6] = dpiValue[2];//Dont need them for dpi
    // buffer[7] = dpiValue[3];

    return writeToMouse(buffer);
}

bool MouseDriver::setColor(uint8_t Red, uint8_t Green, uint8_t Blue)
{
    std::array<uint8_t, 64> buffer{};

    buffer[1] = 0x06;
    buffer[3] = 0x06;
    buffer[7] = 0xff;
    buffer[8] = Red;
    buffer[10] = Green;
    buffer[12] = Blue;

    return writeToMouse(buffer);
}

bool MouseDriver::setPollingRate(uint8_t rate)
{
    std::array<uint8_t, 64> buffer{};

    buffer[1] = 0x01;
    buffer[2] = 0x01;
    buffer[4] = rate; // 0x01=125HZ, 0x02=250Hz, 0x03=500Hz, 0x04=1000Hz

    return writeToMouse(buffer);
}

MouseDriver::MouseDriver() //: cableUSB(0x1b1c, 0x1b5e), wirlessUSB(0x1b1c, 0x1bdc)
{
    // Get Main USB Device List for Scanning
    usbDevices.emplace();

    //For decide Logic
    currentMode = ConnectionType::Unknown;
    LastMode = ConnectionType::Unknown;
}

bool MouseDriver::decideMode()
{
    // Start with Cable cause if both are connected we want to use the cable connection
    if (usbDevices->isDeviceConnected(CorsairVendorID, CablePID))
    {
        LastMode = currentMode;
        currentMode = ConnectionType::Cable;
        if (LastMode != currentMode)
        {
            cableUSB.emplace(CorsairVendorID, CablePID);
        }

        return true;
    }
    else if (usbDevices->isDeviceConnected(CorsairVendorID, WirlessPID))
    {
        LastMode = currentMode;
        currentMode = ConnectionType::Wireless;
        if (LastMode != currentMode)
        {
            wirlessUSB.emplace(CorsairVendorID, WirlessPID);
        }

        return true;
    }
    else
    {
        LastMode = currentMode;
        currentMode = ConnectionType::Unknown;
        return false;
    }
}

bool MouseDriver::blockDefaultReset()
{
    // Every 22 seconds this needs to be sent if not -> The mouse resets to default mode
    //sleep(22);

    std::array<uint8_t, 64> buffer{};
    buffer[1] = 0x12;

    return writeToMouse(buffer);
}