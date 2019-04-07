#include "http_client.h"

Mail<http_message_t, 10> http_messages;

/**
 * Private section
 * */

static Thread http_sender(osPriorityNormal, 4096);
static Thread* http_state_monitor;

static EthernetInterface eth;
static TCPSocket socket;

static Timer timer;

static char*
get_current_error(nsapi_error_t error)
{
  char result[32];
  switch ((int)error)
  {
  case -3001 :
    strcpy(result, "Would Block.");
  case -3002 :
    strcpy(result, "Unsupported.");
  case -3003 :
    strcpy(result, "Invalid configuration.");
  case -3004 :
    strcpy(result, "No connection.");
  case -3005 :
    strcpy(result, "No socket.");
  case -3006 :
    strcpy(result, "No address.");
  case -3007 :
    strcpy(result, "No memory.");
  case -3008 :
    strcpy(result, "No SSID.");
  case -3009 :
    strcpy(result, "DNS Failure.");
  case -3010 :
    strcpy(result, "DHCP Failure.");
  case -3011 :
    strcpy(result, "Authentication failure.");
  case -3012 :
    strcpy(result, "Device error.");
  case -3013 :
    strcpy(result, "Operation in progress.");
  case -3014 :
    strcpy(result, "Operation already in progress.");
  case -3015 :
    strcpy(result, "Socket already connected.");
  case -3016 :
    strcpy(result, "Connection lost.");
  case -3017 :
    strcpy(result, "Connection timeout.");
  case -3018 :
    strcpy(result, "Address already in use.");
  case -3019 :
    strcpy(result, "Operation timeout.");
  case -3020 :
    strcpy(result, "Device busy.");
  default :
    if (error < 0) strcpy(result, "Unknown Error");
    strcpy(result, "No erreur");
  }
  return result;
}

static void
http_sender_thread(void)
{
  Logger::getLogger().logDebug(DEBUG_HTTP_CLIENT, "HTTP Sender Thread has been started.");

  nsapi_error_t ethernet_status = eth.connect();
  if (0 > (int)ethernet_status) {
    Logger::getLogger().logError(DEBUG_HTTP_CLIENT, "Ethernet: %s", get_current_error(ethernet_status));
    http_state_monitor->flags_set(0x01);
    http_sender.terminate();
  }

  Logger::getLogger().logInfo(DEBUG_HTTP_CLIENT, "Ethernet connected.");

  while (1) {
    osEvent evt = http_messages.get();
    if (evt.status == osEventMail) {
      http_message_t *message = (http_message_t *)evt.value.p;
      http_message_t current;
      strcpy(current.message, message->message);
      http_messages.free(message);

      char http_request[256];
      strcpy(http_request, "POST /entry.php HTTP/1.1\r\n");
      strcat(http_request, "Host: glove.drewknolton.com\r\n");
      strcat(http_request, "Content-Type: application/x-www-form-urlencoded\r\n");
      strcat(http_request, "Content-Length: %d\r\n\r\n%s\0");
      sprintf(http_request, http_request, (strlen(current.message) + 1), current.message);

      nsapi_error_t tcp_status = socket.open(&eth);
      int open = 0;
      if (0 > (int)tcp_status) {
        Logger::getLogger().logError(DEBUG_HTTP_CLIENT, "Socket: %s (%d)", get_current_error(tcp_status), (int)tcp_status);
        http_state_monitor->flags_set(0x02);
      } else {
        nsapi_error_t tcp_connect = socket.connect("glove.drewknolton.com", 80);
        if (0 > (int)tcp_connect) {
          Logger::getLogger().logError(DEBUG_HTTP_CLIENT, "Socket: %s (%d)", get_current_error(tcp_connect), (int)tcp_connect);
          http_state_monitor->flags_set(0x08);
        } else {
          nsapi_error_t tcp_connect_error = socket.send(http_request, strlen(http_request));
          if (0 > (int)tcp_connect_error) {
            Logger::getLogger().logError(DEBUG_HTTP_CLIENT, "Socket: %s (%d)", get_current_error(tcp_connect_error), (int)tcp_connect_error);
            http_state_monitor->flags_set(0x04);
          }
        }

        socket.close();
      }
    }
  }
}

/**
 * Public section
 * */

void
start_http_sender(Thread * _http_state_monitor)
{
  http_state_monitor = _http_state_monitor;
  http_sender.start(http_sender_thread);
}

http_message_t
create_http_message(uint16_t gx, uint16_t gy, uint16_t gz, char s,
                          uint8_t p, uint8_t r, uint8_t m, uint8_t i, uint8_t t, uint8_t d) {
  char mes[100];

  strcpy(mes, "gx=%d&gy=%d&gz=%d&s=%c&p=%d&r=%d&m=%d&i=%d&t=%d&d=%d");
  http_message_t result;
  sprintf(result.message, mes, gx, gy, gz, s, p, r, m, i, t, d);
  return result;
}