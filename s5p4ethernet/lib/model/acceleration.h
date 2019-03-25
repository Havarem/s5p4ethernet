#ifndef __S5P4_ETHERNET_ACCELERATION
#define __S5P4_ETHERNET_ACCELERATION

#include <mbed.h>

class Acceleration {
  private:
    uint16_t x;
    uint16_t y;
    uint16_t z;

  public:
    Acceleration() {};
    Acceleration(uint16_t, uint16_t, uint16_t);

    uint16_t getX() { return x; };
    uint16_t getY() { return y; };
    uint16_t getZ() { return z; };
};

#endif
