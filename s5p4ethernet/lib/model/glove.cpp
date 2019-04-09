#include "glove.h"

Glove::Glove(Acceleration _acceleration, Hand _hand, char _symbol, uint8_t _direction)
{
  acceleration = _acceleration;
  hand = _hand;
  symbol = _symbol;
  direction = _direction;
}

std::string
Glove::postData()
{
  char tmp[] = "gloveX=%d&gloveY=%d&gloveZ=%d&symbol=%c&pinky=%d&ring=%d&middle=%d&index=%d&thumb=%d&direction=%d\r\n";
  char data[256];
  sprintf(data, tmp, (int)acceleration.getX(), (int)acceleration.getY(), (int)acceleration.getZ(), symbol, (int)hand.getPinky(), (int)hand.getRing(), (int)hand.getMiddle(), (int)hand.getIndex(), (int)hand.getPinky(), (int)direction);

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
Glove::setSymbol(const char _symbol)
{
  symbol = _symbol;
}

void
Glove::setDirection(uint8_t _direction)
{
  direction = _direction;
}