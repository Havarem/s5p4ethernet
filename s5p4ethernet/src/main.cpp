#include <string>

#include <mbed.h>
#include <rtos.h>

#include "Logger.h"

#include "http_client.h"
#include "glove.h"

#define DEBUG_MAIN "MAIN THREAD"

Thread http_monitor(osPriorityBelowNormal, 2048);
Serial receptor(p13, p14);

void
http_monitor_thread(void)
{
  while (1) {
    uint32_t flag = ThisThread::flags_wait_any(0xFF, true);
    if (1 == (flag & 0x01)) {
      Logger::getLogger().logError(DEBUG_MAIN, "Can\'t connect to the Ethernet interface.");
    } else if (1 == ((flag >> 1) & 0x01)) {
      Logger::getLogger().logError(DEBUG_MAIN, "Can't open TCP socket.");
    } else if (1 == ((flag >> 2) & 0x01)) {
      Logger::getLogger().logError(DEBUG_MAIN, "Can't send data.");
    }
  }
}

bool start = false;
char type = '\0';
int count_char = 0;
Glove glove;

void inline
receptor_handling(char current)
{
  if (0xFE == current) {

  }

  if (true == start) {
    if ('\0' == type) {
      if ('a' == current || 's' == current || 'd' == current) {
        type = current;
      }
    }
  }

  switch (type) {
    case 's':
      if (0 == count_char) {
        finger_t pinky = ((current >> 4) & 0x01) == 1 ? OPEN : CLOSE;
        finger_t ring = ((current >> 3) & 0x01) == 1 ? OPEN : CLOSE;
        finger_t middle = ((current >> 2) & 0x01) == 1 ? OPEN : CLOSE;
        finger_t index = ((current >> 1) & 0x01) == 1 ? OPEN : CLOSE;
        finger_t thumb = (current & 0x01) == 1 ? OPEN : CLOSE;

        Hand hand(pinky, ring, middle, index, thumb);
        glove.setHand(hand);
      }

      if (1 == count_char) {
        char symbol[64];
        strncat(symbol, &current, 1);
        strncat(symbol, '\0', 1);
        glove.setSymbol(current);
      }

      count_char++;
      break;
    case 'd':
      if (0 == count_char) {
        glove.setDirection(current);
      }
      break;
    case 'a':
      if (0 == count_char) {
        finger_t pinky = ((current >> 4) & 0x01) == 1 ? OPEN : CLOSE;
        finger_t ring = ((current >> 3) & 0x01) == 1 ? OPEN : CLOSE;
        finger_t middle = ((current >> 2) & 0x01) == 1 ? OPEN : CLOSE;
        finger_t index = ((current >> 1) & 0x01) == 1 ? OPEN : CLOSE;
        finger_t thumb = (current & 0x01) == 1 ? OPEN : CLOSE;

        Hand hand(pinky, ring, middle, index, thumb);
        glove.setHand(hand);
      }

      if (1 == count_char) {
        char symbol[64];
        strncat(symbol, &current, 1);
        strncat(symbol, '\0', 1);
        glove.setSymbol(current);
      }

      if (3 == count_char) {
        glove.setDirection(current);
      }
      break;
    case '\0':
    // fallthrough
    default:
      break;
  }

  if (current == 0xFF) {
    type = '\0';
    int count_char = 0;
    start = true;
  }
}

int
main()
{
  Logger::level = Debug;
  Logger::getLogger().logDebug(DEBUG_MAIN, "S5 Project - Team 4 - Ethernet Module");
  http_monitor.start(http_monitor_thread);

  start_http_sender(&http_monitor);
  ThisThread::sleep_for(5000);

  Acceleration acceleration(0, 0, 0);
  glove.setAcceleration(acceleration);
  while (1) {
    if (receptor.readable()) {
      char current = receptor.getc();
      receptor_handling(current);
    }
  }
}
