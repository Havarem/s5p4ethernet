#include "glove.h"

Glove::Glove(Acceleration _acceleration, Hand _hand)
{
  acceleration = _acceleration;
  hand = _hand;
}

std::string
Glove::postData()
{
  char tmp[] = "gloveX=%d&gloveY=%d&gloveZ=%d\r\n";
  char data[256];
  sprintf(data, tmp, acceleration.getX(), acceleration.getY(), acceleration.getZ());

  std::string result(data);

  return result;
}
