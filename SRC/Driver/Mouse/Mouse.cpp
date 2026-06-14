#include "Mouse.hpp"
#include <unistd.h>
#include <bit>

// Constants
constexpr uint16_t CorsairVendorID = 0x1B1C;
constexpr uint16_t CablePID = 0x1B5E;
constexpr uint16_t WirlessPID = 0x1BDC;
constexpr uint8_t CableOffset = 0x08;
constexpr uint8_t WirlessOffset = 0x09; //if the Slipstream Reciver and Mouse were paired its 0x09 else its 0x0a(Dont know why but it is what it is)

Mouse::Mouse() //: cable(0x1b1c, 0x1b5e), wirless(0x1b1c, 0x1bdc)
{
    hidContext.emplace();

    // Get Main USB Device List for Scanning
    hidDevices.emplace();

    //For decide Logic
    currentMode = ConnectionType::Unknown;
    LastMode = ConnectionType::Unknown;
}

Mouse::~Mouse()
{
    //Reset containers to trigger custom deleters in right order(Context must be the last)
    hidDevices.reset();//Not necessary but safty first
    wirelessHID.reset();
    cableHID.reset();
    hidContext.reset();
}

bool Mouse::writeToMouse(std::array<uint8_t, 64> buffer)
{
    switch (currentMode)
    {
    case ConnectionType::Cable:
        buffer[0] = CableOffset;
        return cableHID->write(buffer);
    case ConnectionType::Wireless:
        buffer[0] = WirlessOffset;
        return wirelessHID->write(buffer);
    default:
        return false;
    }
}

// Two bools which are necessary to write
bool Mouse::setEssentials()
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

// Split uint32_t into 4 uint8_t
std::array<uint8_t, 4> splitBytes(uint32_t value)
{
    std::array<uint8_t, 4> buffer{};
    
    buffer[0] = static_cast<uint8_t>((value >> 24) & 0xFF);// Most significant byte
    buffer[1] = static_cast<uint8_t>((value >> 16) & 0xFF);
    buffer[2] = static_cast<uint8_t>((value >> 8) & 0xFF);
    buffer[3] = static_cast<uint8_t>(value & 0xFF);// Least significant byte

    return buffer;
}

uint32_t checkEndian(uint32_t value) 
{
    // CPU = Little-Endian -> Swap the Bytes (No need to chek for MixedEndian its to uncommon...)
    return (std::endian::native == std::endian::little)  ? std::byteswap(value) : value; 
}

bool Mouse::setDPI(uint32_t dpi)//From 1 to 10000
{
    std::array<uint8_t, 64> buffer{};

    buffer[1] = 0x01;
    buffer[2] = 0x20;

    std::array<uint8_t, 4> dpiValue = splitBytes(checkEndian(dpi));
    buffer[4] = dpiValue[0];
    buffer[5] = dpiValue[1];
    // buffer[6] = dpiValue[2];//Dont need them for dpi 10000 fits in two bytes
    // buffer[7] = dpiValue[3];

    return writeToMouse(buffer);
}

bool Mouse::setBrightness(uint32_t brightness)//set Brightness fom 0 to 1000
{
    std::array<uint8_t, 64> buffer{};

    buffer[1] = 0x01;
    buffer[2] = 0x02;
    std::array<uint8_t, 4> brightnessValue = splitBytes(checkEndian(brightness));
    buffer[4] = brightnessValue[0];
    buffer[5] = brightnessValue[1];
    // buffer[6] = brightnessValue[2];//Dont need them for brightness 1000 fits in two bytes
    // buffer[7] = brightnessValue[3];

    return writeToMouse(buffer);
}

//Cause of Alpha we need to calculate the opacity per channel(ICUE does this too). Proably just set it with brightness... 
uint8_t getOpacitySingleChannel(uint8_t color, uint8_t alpha)
{
     uint16_t temp = static_cast<uint16_t>(color * alpha);

    //Bit shift for division by 255
    return static_cast<uint8_t>((temp + 1 + (temp >> 8)) >> 8);
}

bool Mouse::setColor(Color logo, Color profileButton)
{
    std::array<uint8_t, 64> buffer{};

    buffer[1] = 0x06;
    buffer[3] = 0x06;
    buffer[7] = getOpacitySingleChannel(profileButton.red, profileButton.alpha);
    buffer[8] = getOpacitySingleChannel(logo.red, logo.alpha);
    buffer[9] = getOpacitySingleChannel(profileButton.green, profileButton.alpha);
    buffer[10] = getOpacitySingleChannel(logo.green, logo.alpha);
    buffer[11] = getOpacitySingleChannel(profileButton.blue, profileButton.alpha);
    buffer[12] = getOpacitySingleChannel(logo.blue, logo.alpha);

    return writeToMouse(buffer);
}

bool Mouse::setColor(Color color)
{
    return setColor(color, color);
}

bool Mouse::setPollingRate(uint8_t rate)
{
    std::array<uint8_t, 64> buffer{};

    buffer[1] = 0x01;
    buffer[2] = 0x01;
    buffer[4] = rate; // 0x01=125HZ/8ms, 0x02=250Hz/4ms, 0x03=500Hz/2ms, 0x04=1000Hz/1ms

    return writeToMouse(buffer);
}

bool Mouse::decideMode()
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

bool Mouse::blockDefaultReset()
{
    // Every 22 seconds this needs to be sent if not -> The mouse resets to default mode
    //sleep(22);

    std::array<uint8_t, 64> buffer{};
    buffer[1] = 0x12;

    return writeToMouse(buffer);
}

bool Mouse::setAngleSnapping(bool enabled)
{
    std::array<uint8_t, 64> buffer{};

    buffer[1] = 0x01;
    buffer[2] = 0x07;
    buffer[4] = enabled ? 0x01 : 0x00;

    return writeToMouse(buffer);
}

bool Mouse::setButtonResponseOptimization(bool enabled)
{
    std::array<uint8_t, 64> buffer{};

    buffer[1] = 0x01;
    buffer[2] = 0xb0;
    buffer[4] = enabled ? 0x01 : 0x00;

    return writeToMouse(buffer);
}

bool Mouse::setPowerSavingMode(bool enabled, Color logo, Color profileButton)
{
    if (enabled)
    {
        return setButtonResponseOptimization(true) && setColor(Color{0, 0, 0, 0}, Color{0, 0, 0, 0});//Icue only disabled the logo so we improve this hahha
    }
    else
    {
        return setButtonResponseOptimization(false) && setColor(logo, profileButton);
    }
}

bool Mouse::setPowerSavingMode(bool enabled, Color color)
{
    return setPowerSavingMode(enabled, color, color);
}