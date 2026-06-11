#pragma once

#include <hidapi/hidapi.h>
#include <cstdint>
#include <memory>
#include <span>

class HID
{
    //Custom deleters for RAII
    std::unique_ptr<hid_device_info, decltype([](hid_device_info* d) { if (d) hid_free_enumeration(d); })> deviceList;
    std::unique_ptr<hid_device, decltype([](hid_device* d) { if (d) hid_close(d); })> handle;

    bool created{false};
    bool getInterfaceHandle();
public:
    HID();
    HID(uint16_t vendorID, uint16_t productID);
    ~HID();

    bool isCreated() const noexcept;
    bool isDeviceConnected(uint16_t vendorID, uint16_t productID);
    bool write(std::span<const uint8_t, 64> buffer);
};