#include "Context.hpp"
#include <hidapi/hidapi.h>
#include <iostream>

HID::Context::Context() 
{ 
    // Init HIDAPI
    if (hid_init() < 0)
    {
        std::cerr << "Failed to initialize HIDAPI context" << std::endl;
    }else 
    {
        created = true;
    }
}

HID::Context::~Context()
{
    //Cleanup HIDAPI context if it was initialized
    if (created)
    {
        hid_exit();
    }
}