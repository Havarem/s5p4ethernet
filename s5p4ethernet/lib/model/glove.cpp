#include "glove.h"

Glove::Glove(Acceleration _acceleration, Hand _hand, char _symbol[64], char _direction)
{
  acceleration = _acceleration;
  hand = _hand;
  strcpy(symbol, _symbol);
  direction = _direction;
}

std::string
Glove::postData()
{
  char tmp[] = "gloveX=%d&gloveY=%d&gloveZ=%d&symbol=%s&pinky=%d&ring=%d&middle=%d&index=%d&thumb=%d&direction=&s\r\n";
  char data[256];
  sprintf(data, tmp, acceleration.getX(), acceleration.getY(), acceleration.getZ(), symbol, (int)hand.getPinky(), (int)hand.getRing(), (int)hand.getMiddle(), (int)hand.getIndex(), (int)hand.getPinky(), direction);

  std::string result(data);

  return result;
}

void
Glove::setAcceleration(Acceleration _accel)
{
  acceleration = _accel;
}

void
Glove::setHand(Hand _hand)
{
  hand = _hand;
}

void
Glove::setSymbol(char _symbols[64])
{
  strcpy(symbol, _symbols);
}

void
Glove::setDirection(uint8_t _direction)
{
  direction = _direction;
}