#pragma once

#include <cstdint>

namespace HID
{

  class Context
  {
  public:
    Context();
    ~Context();
    [[nodiscard]] bool isCreated() const;
    [[nodiscard]] bool isDeviceConnected(uint16_t vendorID, uint16_t productID);

    // Prohibit copying
    Context(const Context &) = delete;            // Copy constructor
    Context &operator=(const Context &) = delete; // Copy assignment operator

  private:
    bool m_created{false};
  };

} // namespace HID
