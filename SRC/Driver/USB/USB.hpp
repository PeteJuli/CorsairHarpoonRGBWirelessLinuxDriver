#ifndef _USB_HPP_
#define _USB_HPP_
#include <libusb-1.0/libusb.h>

class USB
{
    libusb_context * context;
    libusb_device_handle * handle;
    bool created;

public:
    size_t getDeviceListCount();
    bool isCreated();
    bool create(uint16_t vid, uint16_t pid);
    bool write(uint8_t * buffer);
    bool destroy();
};

#endif //!_USB_HPP_