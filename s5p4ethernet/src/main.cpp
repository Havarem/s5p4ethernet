#include <string>

#include <mbed.h>
#include <rtos.h>
#include "EthernetInterface.h"

#include "glove.h"
#include "httpclient.h"
#include "Logger.h"

Serial receptor(p13, p14, 115200);

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
    symbol[0] = symbol[0]++;
    pattern = pattern++ % 32;
    direction = direction % 5;
    tcpWriter.flags_set(0x01);
    ThisThread::flags_wait_any(0x0A);
  }
  /*while (1)
  {
    if (receptor.readable()) {
      char current = receptor.getc();

      if (current == 0xFF) {
        label = receptor.getc();
        strcpy(symbol, "");;
        pattern = '\0';
        direction = '\0';

        switch (current) {
        case 's': {
          symbol[0] = receptor.getc();
          pattern = receptor.getc();
          break;
        }
        case 'd': {
          direction = receptor.getc();
          break;
        }
        case 'a': {
          symbol[0] = receptor.getc();
          pattern = receptor.getc();
          direction = receptor.getc();
          break;
        }
        }

        tcpWriter.signal_set(0x01);
        Thread::signal_wait(0x0A);
      }
    }
  }*/
}

void
tcpWriterRoutine(void)
{
  HttpClient client("POST", "glove.drewknolton.com", "/");
  client.addHeaderField("Content-Type", "application/x-www-form-urlencoded");

  if (client.getCurrentError().compare("") == 0) {
    listenReceptor.start(listenReceptorRoutine);

    while(1) {
      ThisThread::flags_wait_any(0x01);

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
      printf("Response:\r\n%s\r\n", response.c_str());
    }
  }
}

int
main()
{
  Logger::getLogger().logDebug("MAIN THREAD", "Welcome");

  /*Acceleration acceleration(2000, 1050, 443);
  Hand hand(CLOSE, CLOSE, OPEN, UNDEFINED, OPEN);
  char symbol[64];
  strcpy(symbol, "3");
  Glove glove(acceleration, hand, symbol);

  std::string postData = glove.postData();

  HttpClient client("POST", "glove.drewknolton.com", "/");
  if (client.getCurrentError().compare("") == 0) {
    client.addHeaderField("Content-Type", "application/x-www-form-urlencoded");
    client.setContent(postData);
    std::string response = client.send();
    if (client.getCurrentError().compare("") != 0) {
      printf("%s\r\n", client.getCurrentError().c_str());
    } else {
      printf("Success : %s\r\n", response.c_str());
    }
  } else {
      printf("%s\r\n", client.getCurrentError().c_str());
  }*/

  /*printf("\r\n\r\nReceiving data from receptor\r\n\r\n");
  while (1) {
    if (receptor.readable()) {
      char current = receptor.getc();
      if (current == 0xFF) {
        printf("0x%02x ", current);

        while (current != 0xFE) {
          current = receptor.getc();
          printf("0x%02x ", current);
        }

        printf("\r\n");
      }
      //printf("0x%02x ", receptor.getc());
      //fflush(stdout);
    }
  }*/


}

/*void
errorMessage(nsapi_error_t err)
{
  int error = (int) err;

  if (0 == error) printf("\r\n");
  else if (-3001 == error) printf("Would Block.");
  else if (-3002 == error) printf("Unsupported.");
  else if (-3003 == error) printf("Invalid configuration.");
  else if (-3004 == error) printf("No connection.");
  else if (-3005 == error) printf("No socket.");
  else if (-3006 == error) printf("No address.");
  else if (-3007 == error) printf("No memory.");
  else if (-3008 == error) printf("No SSID.");
  else if (-3009 == error) printf("DNS Failure.");
  else if (-3010 == error) printf("DHCP Failure.");
  else if (-3011 == error) printf("Authentication failure.");
  else if (-3012 == error) printf("Device error.");
  else if (-3013 == error) printf("Operation in progress.");
  else if (-3014 == error) printf("Operation already in progress.");
  else if (-3015 == error) printf("Socket already connected.");
  else if (-3016 == error) printf("Connection lost.");
  else if (-3017 == error) printf("Connection timeout.");
  else if (-3018 == error) printf("Address already in use.");
  else if (-3019 == error) printf("Operation timeout.");
  else if (-3020 == error) printf("Device busy.");
  else if (0 < error) printf("\r\n");
  else printf("Unknown error");

  printf("\r\n");
}

int
main() {

  Acceleration acceleration(3116, 12, 13);
  Hand hand(CLOSE, CLOSE, OPEN, UNDEFINED, OPEN);
  Glove glove(acceleration, hand);

  std::string postData = glove.postData();

  printf("Welcome\r\n");
  DigitalOut led(LED1);
  EthernetInterface eth;
  TCPSocket socket;

  nsapi_error_t statusNetwork = eth.connect();
  printf("IP Address is %s - (Code %d)\r\n", eth.get_ip_address(), (int)statusNetwork);
  errorMessage(statusNetwork);

  socket.open(&eth);

  char target[] = "glove.drewknolton.com";
  nsapi_error_t tcpError = socket.connect(target, 80);

  if (0 == int(tcpError)) {
    char sbuffer[] = "POST / HTTP/1.1\r\nHost: %s\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: %d\r\n\r\n";
    char buffer[1024];
    sprintf(buffer, sbuffer, target, postData.size());
    sprintf(buffer, "%s%s\r\n", buffer, postData.c_str());

    printf("%s\r\n", buffer);
    int scount = socket.send(buffer, strlen(buffer));

    char rbuffer[1024];
    int rcount = socket.recv(rbuffer, sizeof rbuffer);
    printf("%s\r\n", rbuffer);
    fflush(stdout);
  } else {
    printf("TCP Error: %d\r\n", (int)tcpError);
    errorMessage(tcpError);
  }

  socket.close();
  eth.disconnect();

  while(1) {
    led = led ? 0 : 1;
    wait_ms(100);
  }
}
*/