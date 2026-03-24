#include "USB.hpp"

#include <iostream>

size_t USB::getDeviceListCount()
{
    libusb_device **list;

    return libusb_get_device_list(context, &list);;
}

bool USB::isCreated()
{
    return created;
}

bool USB::create(uint16_t vid, uint16_t pid)//1b1c:1b5e
{
    created = false;

    //Init LIBUSB
    if (libusb_init(&context) < 0)
        return false;

    //Open Handle
    handle = libusb_open_device_with_vid_pid(context, vid, pid);//Cable = 0x1b5e //Adapter = 0x1bdc

    if (!handle)
    {
        libusb_exit(context);
        return false;
    }

    // Kernel Driver detach
    if (libusb_kernel_driver_active(handle, 1) == 1)
    {
        libusb_detach_kernel_driver(handle, 1);
    }

    // Interface 1 claimen
    if (libusb_claim_interface(handle, 1) < 0)
    {
        libusb_close(handle);
        libusb_exit(context);
        return false;
    }

    created = true;

    return true;
}

bool USB::destroy()
{
    if (libusb_release_interface(handle, 1) < 0)
        return false;

    libusb_close(handle);

    libusb_exit(context);

    created = false;

    return true;
}

bool USB::write(uint8_t * buffer)
{
    int transferred = 0;

    //Some hardcode in our case endpoint = 0x4 and packetsize to 64 bytes
    if (libusb_bulk_transfer(handle, 0x04, buffer, 64, &transferred,1000) < 0)
    {
        return false;
    }

    return true;
}

