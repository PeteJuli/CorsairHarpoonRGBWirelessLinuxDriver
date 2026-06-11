#pragma once

#include "../HID/HID.hpp"
#include <optional>

enum class ConnectionType 
{
    Unknown,
    Cable,
    Wireless
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
    bool setColor(uint8_t Red, uint8_t Green, uint8_t Blue);
    bool setPollingRate(uint8_t rate);

    ConnectionType getCurrentMode() const noexcept { return currentMode; }
    ConnectionType getLastMode() const noexcept { return LastMode; }
};