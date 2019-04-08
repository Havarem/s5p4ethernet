#include <string>

#include <mbed.h>
#include <rtos.h>

#include "Logger.h"

#include "http_client.h"

#define DEBUG_MAIN "MAIN THREAD"

Thread http_monitor(osPriorityBelowNormal, 2048);

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

int
main()
{
  Logger::level = Debug;
  Logger::getLogger().logDebug(DEBUG_MAIN, "S5 Project - Team 4 - Ethernet Module");
  http_monitor.start(http_monitor_thread);

  start_http_sender(&http_monitor);
  ThisThread::sleep_for(5000);
  while (1) {
      http_message_t message = create_http_message(14, 15, 16, '4', 1, 1, 1, 1, 0, 0);

      http_message_t *m = http_messages.alloc();
      if (NULL != m) {
        strcpy(m->message, message.message);
        http_messages.put(m);
      }
      ThisThread::sleep_for(1000);
  }
}
