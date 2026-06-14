#pragma once

#include <cstdint>
#include <hidapi/hidapi.h>
#include <memory>
#include <span>

namespace HID
{

class Device
{
    // Custom deleters for RAII
    std::unique_ptr<hid_device_info, decltype([](hid_device_info *d) { if (d) hid_free_enumeration(d);})> deviceList;
    std::unique_ptr<hid_device, decltype([](hid_device *d) {if (d) hid_close(d);})> handle;

    bool getInterfaceHandle();

  public:
    Device() {}; // Dummy Constructor for Searching...
    Device(uint16_t vendorID, uint16_t productID);

    bool isCreated() const noexcept;
    bool isDeviceConnected(uint16_t vendorID, uint16_t productID);
    bool write(std::span<const uint8_t, 64> buffer);
};

} // namespace HID
