#pragma once

#include "../HID/HID.hpp"
#include <optional>

enum class ConnectionType 
{
    Unknown,
    Cable,
    Wireless
};

struct Color
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

class MouseDriver
{
    std::optional<HID> hidDevices;
    std::optional<HID> cableHID;
    std::optional<HID> wirelessHID;

    ConnectionType currentMode;
    ConnectionType LastMode;
    
    bool writeToMouse(std::span<uint8_t, 64> buffer);
public:
    MouseDriver();

    bool decideMode();
    bool blockDefaultReset();


    bool setEssentials();
    bool setDPI(uint32_t DPI);
    bool setBrightness(uint32_t brightness);
    bool setColor(Color color);
    bool setColor(Color logo, Color profileButton);
    bool setPollingRate(uint8_t rate);

    ConnectionType getCurrentMode() const noexcept { return currentMode; }
    ConnectionType getLastMode() const noexcept { return LastMode; }
};