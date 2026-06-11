#include "MouseDriver.hpp"
#include <iostream>
#include <unistd.h>

// Constants
constexpr uint16_t CorsairVendorID = 0x1B1C;
constexpr uint16_t CablePID = 0x1B5E;
constexpr uint16_t WirlessPID = 0x1BDC;
constexpr uint8_t CableOffset = 0x08;
constexpr uint8_t WirlessOffset = 0x09; //if the Slipstream Reciver and Mouse were paired its 0x09 else its 0x0a(Dont know why but it is what it is)

bool MouseDriver::writeToMouse(std::span<uint8_t, 64> buffer)
{
    switch (currentMode)
    {
    case ConnectionType::Unknown:
        return false;
    case ConnectionType::Cable:
        buffer[0] = CableOffset;
        return cableHID->write(buffer);
    case ConnectionType::Wireless:
        buffer[0] = WirlessOffset;
        return wirelessHID->write(buffer);
    }
}

// Two bools which are necessary to write
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

    //block default reset needs to be called after them
    return writeToMouse(essentials0) && writeToMouse(essentials1) && blockDefaultReset();
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

bool MouseDriver::setBrightness(uint32_t brightness)//set Brightness fom 0 to 1000
{
    std::array<uint8_t, 64> buffer{};

    buffer[1] = 0x01;
    buffer[2] = 0x02;
    uint8_t brightnessValue[4];
    splitBytes(swapEndian(brightness), brightnessValue);
    buffer[4] = brightnessValue[0];
    buffer[5] = brightnessValue[1];
    // buffer[6] = brightnessValue[2];//Dont need them for brightness
    // buffer[7] = brightnessValue[3];

    return writeToMouse(buffer);
}

bool MouseDriver::setColor(Color logo, Color profileButton)
{
    std::array<uint8_t, 64> buffer{};

    buffer[1] = 0x06;
    buffer[3] = 0x06;
    buffer[7] = profileButton.red;
    buffer[8] = logo.red;
    buffer[9] = profileButton.green;
    buffer[10] = logo.green;
    buffer[11] = profileButton.blue;
    buffer[12] = logo.blue;

    return writeToMouse(buffer);
}

bool MouseDriver::setColor(Color color)
{
    return setColor(color, color);
}

bool MouseDriver::setPollingRate(uint8_t rate)
{
    std::array<uint8_t, 64> buffer{};

    buffer[1] = 0x01;
    buffer[2] = 0x01;
    buffer[4] = rate; // 0x01=125HZ/8ms, 0x02=250Hz/4ms, 0x03=500Hz/2ms, 0x04=1000Hz/1ms

    return writeToMouse(buffer);
}

MouseDriver::MouseDriver() //: cable(0x1b1c, 0x1b5e), wirless(0x1b1c, 0x1bdc)
{
    // Get Main USB Device List for Scanning
    hidDevices.emplace();

    //For decide Logic
    currentMode = ConnectionType::Unknown;
    LastMode = ConnectionType::Unknown;
}

bool MouseDriver::decideMode()
{
    // Start with Cable cause if both are connected we want to use the cable connection
    if (hidDevices->isDeviceConnected(CorsairVendorID, CablePID))
    {
        LastMode = currentMode;
        currentMode = ConnectionType::Cable;
        if (LastMode != currentMode)
        {
            cableHID.emplace(CorsairVendorID, CablePID);
        }

        return true;
    }
    else if (hidDevices->isDeviceConnected(CorsairVendorID, WirlessPID))
    {
        LastMode = currentMode;
        currentMode = ConnectionType::Wireless;
        if (LastMode != currentMode)
        {
            wirelessHID.emplace(CorsairVendorID, WirlessPID);
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