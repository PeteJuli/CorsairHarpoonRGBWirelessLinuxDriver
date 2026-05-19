#include "USB.hpp"
#include <iostream>

bool USB::isDeviceConnected(uint16_t vendorID, uint16_t productID)
{
    libusb_device **rawDevice = nullptr;
    const ssize_t count = libusb_get_device_list(context.get(), &rawDevice);

    if (count < 0)
    {
        std::cerr << "Error retrieving USB device list" << std::endl;
        return false;
    }

    std::unique_ptr<libusb_device *[], decltype([](libusb_device **d){ libusb_free_device_list(d, 1); })> devs(rawDevice);

    //
    for (const auto &dev : std::span(devs.get(), static_cast<size_t>(count)))
    {
        libusb_device_descriptor desc;
        if (libusb_get_device_descriptor(dev, &desc) < 0)
        {
            continue;
        }

        if (desc.idVendor == vendorID && desc.idProduct == productID)
        {
            return true;
        }
    }

    return false;
}

USB::USB()
{
    // Init LIBUSB
    libusb_context *rawContext = nullptr;
    if (libusb_init(&rawContext) < 0)
    {
        std::cerr << "Failed to initialize libusb context" << std::endl;
    }
    context.reset(rawContext);

    // Needs to set interfaceClaimed to true, otherwise the isOpen() function will return false
    interfaceClaimed = true;
}

USB::USB(uint16_t vid, uint16_t pid)
{
    // Init LIBUSB
    libusb_context *rawContext = nullptr;
    if (libusb_init(&rawContext) < 0)
    {
        std::cerr << "Failed to initialize libusb context" << std::endl;
    }
    context.reset(rawContext);

    // Open Device Handle
    libusb_device_handle *rawHandle = libusb_open_device_with_vid_pid(context.get(), vid, pid);
    if (!rawHandle)
    {
        std::cerr << "Failed to open USB device (VID/PID mismatch or permissions)" << std::endl;
    }
    handle.reset(rawHandle);

    // Kernel Driver detach when active
    if (libusb_kernel_driver_active(handle.get(), 1) == 1)
    {
        libusb_detach_kernel_driver(handle.get(), 1);
    }

    // Interface 1 claimen
    if (libusb_claim_interface(handle.get(), 1) < 0)
    {
        std::cerr << "Failed to claim interface 1" << std::endl;
    }

    interfaceClaimed = true;
}

bool USB::isOpen() const noexcept
{
    return handle != nullptr && interfaceClaimed;
}

bool USB::write(std::span<const uint8_t, 64> buffer)
{
    if (!isOpen())
        return false;

    int transferred = 0;

    // Some hardcode in our case endpoint = 0x4 and packetsize to 64 bytes
    return (libusb_bulk_transfer(handle.get(), 0x04, const_cast<uint8_t *>(buffer.data()), static_cast<int>(buffer.size()), &transferred, 1000) == 0);
}