#pragma once

namespace HID
{

class Context
{
    bool created{false};

  public:
    Context();
    ~Context();

    //Prohibit copying
    Context(const Context &) = delete;//Copy constructor
    Context &operator=(const Context &) = delete; //Copy assignment operator
};

} // namespace HID
