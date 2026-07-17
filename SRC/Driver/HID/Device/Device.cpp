#include "Device.hpp"
#include "../Context/Context.hpp"
#include <hidapi/hidapi.h>
#include <iostream>

HID::Device::Device(const Context &context, uint16_t vendorID, uint16_t productID) : m_deviceList{nullptr, hid_free_enumeration}, m_handle{nullptr, hid_close}
{
    if (!context.isCreated())
    {
        std::cerr << "Context is not created!" << std::endl;
        return;
    }

    if (!getDevice(vendorID, productID))
    {
        std::cerr << "Failed to get Device from List!" << std::endl;
        return;
    }

    if (!getInterfaceHandle())
    {
        std::cerr << "Failed to get Interface Handle! (permissions issue?)" << std::endl;
        return;
    }
}

bool HID::Device::isCreated() const noexcept
{
    return m_handle != nullptr;
}

bool HID::Device::getDevice(uint16_t vendorID, uint16_t productID)
{
    // Get DeviceList from HIDAPI
    hid_device_info *rawDeviceList = hid_enumerate(vendorID, productID);

    if (!rawDeviceList)
        return false;

    m_deviceList.reset(rawDeviceList);

    return true;
}

bool HID::Device::getInterfaceHandle()
{
    // Iterate through devices to find the device path for Inteface[1]
    for (auto* currentDevice = m_deviceList.get(); currentDevice != nullptr; currentDevice = currentDevice->next)
    {
        // Interface-Check
        if (currentDevice->interface_number != 1) continue;

        // Try to open Handle
        if (auto* rawHandle = hid_open_path(currentDevice->path))
        {
            m_handle.reset(rawHandle);
            return true;
        }
    }

    return false;
}

bool HID::Device::write(std::span<const uint8_t, 64> buffer)
{
    if (!isCreated())
        return false;

    return (hid_write(m_handle.get(), buffer.data(), buffer.size()) > 0);
}
