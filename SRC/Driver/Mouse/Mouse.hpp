#pragma once

#include "../HID/Device/Device.hpp"
#include "../HID/Context/Context.hpp"
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
    uint8_t alpha = 255; // Default to fully opaque
};

class Mouse
{
public:
    Mouse();
    ~Mouse();

    bool decideMode();
    bool blockDefaultReset();

    bool setEssentials();
    bool setDPI(uint32_t dpi);
    bool setBrightness(uint32_t brightness);
    bool setColor(Color color);
    bool setColor(Color logo, Color profileButton);
    bool setPollingRate(uint8_t rate);
    bool setAngleSnapping(bool enabled);
    bool setButtonResponseOptimization(bool enabled);
    bool setPowerSavingMode(bool enabled, Color logo, Color profileButton);
    bool setPowerSavingMode(bool enabled, Color color);

    ConnectionType getCurrentMode() const noexcept;
    ConnectionType getLastMode() const noexcept;

private:
    std::optional<HID::Context> m_hidContext;
    std::optional<HID::Device> m_cableHID;
    std::optional<HID::Device> m_wirelessHID;

    ConnectionType m_currentMode;
    ConnectionType m_lastMode;

    bool writeToMouse(std::array<uint8_t, 64> buffer);
};