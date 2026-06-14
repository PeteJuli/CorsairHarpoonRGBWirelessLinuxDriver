#include "Device.hpp"
#include <iostream>

namespace HID
{

Device::Device(uint16_t vendorID, uint16_t productID)
{
    if (isDeviceConnected(vendorID, productID))
    {
        getInterfaceHandle();
    }
}

bool Device::isCreated() const noexcept
{
    return handle != nullptr;
}

bool Device::isDeviceConnected(uint16_t vendorID, uint16_t productID)
{
    // Get DeviceList from HIDAPI
    hid_device_info *rawDeviceList = hid_enumerate(vendorID, productID);

    if (!rawDeviceList)
    {
        // std::cerr << "No HID devices found matching VID: " << std::hex << vendorID << " PID: " << std::hex <<
        // productID << std::endl;
        return false;
    }

    deviceList.reset(rawDeviceList);

    return true;
}

bool Device::getInterfaceHandle()
{
    // Iterate through devices to find the Mouse
    hid_device_info *currentDevice = deviceList.get();

    while (currentDevice)
    {
        if (currentDevice->interface_number == 1) // Target Interface
        {
            hid_device *rawHandle = hid_open_path(currentDevice->path);

            if (!rawHandle)
            {
                std::cerr << "Failed to open HID device at path: " << currentDevice->path << " (permissions issue?)"
                          << std::endl;
            }
            else
            {
                handle.reset(rawHandle);
                return true;
            }

            break;
        }
        currentDevice = currentDevice->next;
    }

    return false;
}

bool Device::write(std::span<const uint8_t, 64> buffer)
{
    if (!isCreated())
        return false;

    return (hid_write(handle.get(), buffer.data(), buffer.size()) > 0);
}

} // namespace HID
