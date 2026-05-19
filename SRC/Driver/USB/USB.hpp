#pragma once

#include <libusb-1.0/libusb.h>
#include <memory>
#include <span>

class USB
{
    //Custom deleters for RAII
    std::unique_ptr<libusb_context, decltype([](libusb_context* ctx) { if (ctx) libusb_exit(ctx); })> context;
    std::unique_ptr<libusb_device_handle, decltype([](libusb_device_handle* h) { if (h) libusb_close(h); })> handle;

    bool interfaceClaimed{false};

public:
    USB();
    USB(uint16_t vid, uint16_t pid);

    [[nodiscard]] bool isDeviceConnected(uint16_t vendorID, uint16_t productID);
    [[nodiscard]] bool isOpen() const noexcept;

    bool write(std::span<const uint8_t, 64> buffer);
};