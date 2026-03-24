#ifndef _MouseDriver_HPP_
#define _MouseDriver_HPP_
#include "../USB/USB.hpp"

class MouseDriver
{
    USB cableUSB;
    USB wirlessUSB;
    int mode;
    size_t usbCount;
    bool setEssentials();
    void decideMode();

public:
    bool create();
    bool running();
    bool blockDefaultReset();
    void destroy();
    bool setDPI(uint32_t DPI);
    bool setColor(uint8_t Red, uint8_t Green, uint8_t Blue, uint8_t Alpha);
};



#endif //!_MouseDriver_HPP_