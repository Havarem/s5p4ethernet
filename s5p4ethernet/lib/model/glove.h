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
    char symbol[64];
    uint8_t direction;

  public:
    Glove() {};
    Glove(Acceleration, Hand, char[64], char);

    std::string postData();

    void setAcceleration(Acceleration);
    void setHand(Hand);
    void setSymbol(char[64]);
    void setDirection(uint8_t);
};

#endif