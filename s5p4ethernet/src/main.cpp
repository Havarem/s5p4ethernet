#include <string>

#include <mbed.h>
#include <rtos.h>
#include "EthernetInterface.h"

#include "glove.h"
#include "httpclient.h"
#include "Logger.h"

Serial receptor(p13, p14, 115200);
DigitalOut led(LED2);
DigitalOut led3(LED3);

Thread listenReceptor;
Thread tcpWriter;

char label, pattern, direction;
char symbol[64];

void
listenReceptorRoutine(void)
{
  symbol[0] = 1;
  pattern = 0x1E;
  direction = 2;
  label = 'a';

  while (1) {
    led = 1;
    ThisThread::sleep_for(1000);
    symbol[0] = symbol[0]++;
    pattern = pattern++ % 32;
    direction = direction % 5;
    tcpWriter.flags_set(0x01);
    ThisThread::flags_wait_any(0x0A, true);
    Logger::getLogger().logDebug("MAIN THREAD", "Signal resceived for listening:");
    led = 0;
  }
}

void
tcpWriterRoutine(void)
{
  HttpClient client("POST", "glove.drewknolton.com", "/entry.php");
  client.addHeaderField("Content-Type", "application/x-www-form-urlencoded");

  if (client.getCurrentError().compare("No erreur") == 0) {
    listenReceptor.start(listenReceptorRoutine);

    while(1) {
      ThisThread::flags_wait_any(0x01, true);
      led3 = 1;
      Logger::getLogger().logDebug("MAIN THREAD", "Signal resceived for TCP to send:");

      Glove glove;

      if (label == 's' || label == 'a') {
        finger_t pinky  = (pattern & 0x01)      == 0 ? CLOSE : OPEN;
        finger_t ring   = (pattern & 0x02) >> 1 == 0 ? CLOSE : OPEN;
        finger_t middle = (pattern & 0x04) >> 2 == 0 ? CLOSE : OPEN;
        finger_t index  = (pattern & 0x08) >> 3 == 0 ? CLOSE : OPEN;
        finger_t thumb  = (pattern * 0x10) >> 4 == 0 ? CLOSE : OPEN;

        Hand hand(pinky, ring, middle, index, thumb);
        glove.setHand(hand);
        glove.setSymbol(symbol);
      }

      if (label == 'd' || label == 'a') {
        Acceleration accel(0, 0, 0);
        glove.setAcceleration(accel);
        glove.setDirection(direction);
      }

      client.setContent(glove.postData());
      std::string response = client.send();

      Logger::getLogger().logDebug("MAIN THREAD", "Response:");
      //printf("%s\r\n", response.c_str());
      listenReceptor.flags_set(0x0A);
      led3 = 0;
    }
  } else {
    Logger::getLogger().logError("MAIN THREAD", "%s", client.getCurrentError().c_str());
  }
}

int
main()
{
  ThisThread::sleep_for(3000);
  Logger::getLogger().logDebug("MAIN THREAD", "Welcome");

  listenReceptor.start(listenReceptorRoutine);
  tcpWriter.start(tcpWriterRoutine);

  osThreadId id = ThisThread::get_id();
  osThreadSetPriority(id, osPriorityIdle);
  while (1) { }
}
