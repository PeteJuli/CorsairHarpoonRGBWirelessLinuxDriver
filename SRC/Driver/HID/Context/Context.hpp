#pragma once

namespace HID
{

class Context
{
    bool created{false};

  public:
    Context();
    ~Context();

    // Verhindere, dass dieser Kontext kopiert wird
    Context(const Context &) = delete;
    Context &operator=(const Context &) = delete;
};

} // namespace HID
