#include "Context.hpp"
#include <hidapi/hidapi.h>
#include <iostream>

HID::Context::Context() 
{ 
    // Init HIDAPI
    if (hid_init() < 0)
    {
        std::cerr << "Failed to initialize HIDAPI context" << std::endl;

        return;
    }

    m_created = true;
}

HID::Context::~Context()
{
    //Cleanup HIDAPI context if it was initialized
    if (m_created)
    {
        hid_exit();
    }
}

bool HID::Context::isCreated() const
{
    return m_created;
}

bool HID::Context::isDeviceConnected(uint16_t vendorID, uint16_t productID)
{
    if(!m_created) return false;

    // Get DeviceList from HIDAPI
    hid_device_info *rawDeviceList = hid_enumerate(vendorID, productID);

    if (!rawDeviceList) return false;

    // Free the device list
    hid_free_enumeration(rawDeviceList);

    return true;
}
