#pragma once

#include <cstdint>
#include <memory>
#include <span>

// Forward Declarations
struct hid_device_info;
typedef struct hid_device_ hid_device;

namespace HID
{
  // Forward Declarations
  class Context;

  class Device
  {
  public:
    explicit Device(const Context &context, uint16_t vendorID, uint16_t productID);

    [[nodiscard]] bool isCreated() const noexcept;

    [[nodiscard]] bool write(std::span<const uint8_t, 64> buffer);

  private:
    // Type Alias
    using HidEnumPtr = std::unique_ptr<hid_device_info, void (*)(hid_device_info *)>;
    using HidHandlePtr = std::unique_ptr<hid_device, void (*)(hid_device *)>;

    HidEnumPtr m_deviceList{nullptr, nullptr};
    HidHandlePtr m_handle{nullptr, nullptr};

    [[nodiscard]] bool getDevice(uint16_t vendorID, uint16_t productID);
    [[nodiscard]] bool getInterfaceHandle();
  };

} // namespace HID
