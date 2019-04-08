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
    break;
  case -3002 :
    strcpy(result, "Unsupported.");
    break;
  case -3003 :
    strcpy(result, "Invalid configuration.");
    break;
  case -3004 :
    strcpy(result, "No connection.");
    break;
  case -3005 :
    strcpy(result, "No socket.");
    break;
  case -3006 :
    strcpy(result, "No address.");
    break;
  case -3007 :
    strcpy(result, "No memory.");
    break;
  case -3008 :
    strcpy(result, "No SSID.");
    break;
  case -3009 :
    strcpy(result, "DNS Failure.");
    break;
  case -3010 :
    strcpy(result, "DHCP Failure.");
    break;
  case -3011 :
    strcpy(result, "Authentication failure.");
    break;
  case -3012 :
    strcpy(result, "Device error.");
    break;
  case -3013 :
    strcpy(result, "Operation in progress.");
    break;
  case -3014 :
    strcpy(result, "Operation already in progress.");
    break;
  case -3015 :
    strcpy(result, "Socket already connected.");
    break;
  case -3016 :
    strcpy(result, "Connection lost.");
    break;
  case -3017 :
    strcpy(result, "Connection timeout.");
    break;
  case -3018 :
    strcpy(result, "Address already in use.");
    break;
  case -3019 :
    strcpy(result, "Operation timeout.");
    break;
  case -3020 :
    strcpy(result, "Device busy.");
    break;
  default :
    if (error < 0) strcpy(result, "Unknown Error");
    else strcpy(result, "No erreur");
    break;
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
      strcat(http_request, "Content-Length: %d\r\n\r\n%s");
      sprintf(http_request, http_request, (strlen(current.message) + 1), current.message);

      socket.set_timeout(-1);

      us_timestamp_t timing;
      timer.reset();
      timer.start();
      nsapi_error_t tcp_status = socket.open(&eth);
      if (0 > (int)tcp_status) {
        Logger::getLogger().logError(DEBUG_HTTP_CLIENT, "Socket: %s (%d)", get_current_error(tcp_status), (int)tcp_status);
        http_state_monitor->flags_set(0x02);
        http_sender.terminate();
      }
      timer.stop();
      timing = timer.read_high_resolution_us();
      Logger::getLogger().logDebug(DEBUG_HTTP_CLIENT, "Opening socket in %llu microseconds", timing);

      timer.reset();
      timer.start();
      nsapi_error_t tcp_connect = socket.connect("glove.drewknolton.com", 80);
      if (0 > (int)tcp_connect) {
        Logger::getLogger().logError(DEBUG_HTTP_CLIENT, "Socket: %s (%d)", get_current_error(tcp_connect), (int)tcp_connect);
        http_state_monitor->flags_set(0x08);
        http_sender.terminate();
      }
      timer.stop();
      timing = timer.read_high_resolution_us();
      Logger::getLogger().logDebug(DEBUG_HTTP_CLIENT, "Connecting socket in %llu microseconds", timing);

      timer.reset();
      timer.start();
      nsapi_error_t tcp_connect_error = socket.send(http_request, strlen(http_request));
      if (0 > (int)tcp_connect_error) {
        Logger::getLogger().logError(DEBUG_HTTP_CLIENT, "Socket: %s (%d)", get_current_error(tcp_connect_error), (int)tcp_connect_error);
        http_state_monitor->flags_set(0x04);
      }
      timer.stop();
      timing = timer.read_high_resolution_us();
      Logger::getLogger().logDebug(DEBUG_HTTP_CLIENT, "Sending data in %llu microseconds", timing);


      socket.set_timeout(0);
      timer.reset();
      timer.start();
      char rbuffer[256];
      nsapi_error_t tcp_receive = socket.recv(rbuffer, 256);
      timer.stop();
      timing = timer.read_high_resolution_us();
      Logger::getLogger().logDebug(DEBUG_HTTP_CLIENT, "Receiving response in %llu microseconds", timing);

      timer.reset();
      timer.start();
      nsapi_error_t tcp_close = socket.close();
      if (0 > (int)tcp_close) {
        Logger::getLogger().logError(DEBUG_HTTP_CLIENT, "Socket: %s (%d)", get_current_error(tcp_close), (int)tcp_close);
      } else {
        Logger::getLogger().logDebug(DEBUG_HTTP_CLIENT, "Socket closed");
      }
      timer.stop();
      timing = timer.read_high_resolution_us();
      Logger::getLogger().logDebug(DEBUG_HTTP_CLIENT, "Closing socket in %llu microseconds", timing);
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
