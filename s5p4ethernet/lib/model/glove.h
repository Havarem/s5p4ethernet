#ifndef __S5P4_ETHERNET_GLOVE
#define __S5P4_ETHERNET_GLOVE

#include <mbed.h>
#include <string>

#include "acceleration.h"
#include "hand.h"

class Glove {
  private:
    Acceleration acceleration;
    Hand hand;

  public:
    Glove(Acceleration, Hand);

    std::string postData();
};

#endif