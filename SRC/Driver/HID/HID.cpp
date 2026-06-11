#include "HID.hpp"
#include <iostream>

bool HID::isCreated() const noexcept
{
    return handle != nullptr && created;
}

bool HID::isDeviceConnected(uint16_t vendorID, uint16_t productID)
{
    //Get DeviceList from HIDAPI
    hid_device_info* rawDeviceList = hid_enumerate(vendorID, productID);

    if (!rawDeviceList)
    {
        //std::cerr << "No HID devices found matching VID: " << std::hex << vendorID << " PID: " << std::hex << productID << std::endl;
        return false;
    }

    deviceList.reset(rawDeviceList);

    return true;
}

bool HID::getInterfaceHandle()
{
    //Iterate through devices to find the Mouse
    hid_device_info* currentDevice = deviceList.get();

    while (currentDevice)
    {
        if (currentDevice->interface_number == 1) // Target Interface
        {
            hid_device* rawHandle = hid_open_path(currentDevice->path);

            if (!rawHandle)
            {
                std::cerr << "Failed to open HID device at path: " << currentDevice->path << " (permissions issue?)" << std::endl;
            }else
            {
                handle.reset(rawHandle);
                created = true;
            }

            break;
        }
        currentDevice = currentDevice->next;
    }

    return isCreated();
}

HID::HID()
{
    // Init HIDAPI
    if (hid_init() < 0)
    {
        std::cerr << "Failed to initialize HIDAPI context" << std::endl;
    }
}

HID::HID(uint16_t vendorID, uint16_t productID)
{
    HID();

    isDeviceConnected(vendorID, productID);

    getInterfaceHandle();
    
}

HID::~HID()
{
    //Reset unique_ptrs to trigger custom deleters
    handle.reset();
    deviceList.reset();

    //Cleanup HIDAPI context if it was initialized
    if (created)
    {
        hid_exit();
    }
}

bool HID::write(std::span<const uint8_t, 64> buffer)
{
    if (!isCreated())
        return false;

    return (hid_write(handle.get(), buffer.data(), buffer.size()) > 0 );
}