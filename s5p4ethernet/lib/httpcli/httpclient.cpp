#include "httpclient.h"

HttpClient::HttpClient()
{
  currentError = 0;
  protocol = HTTP1_PROTOCOL;
  setMethod(std::string("GET"));
  setTarget(DEFAULT_TARGET);
  setContent(std::string(""));
}

HttpClient::HttpClient(std::string _method, std::string _host, std::string _target)
  : HttpClient()
{
  host = _host;
  setMethod(_method);
  setTarget(_target);

  nsapi_error_t statusNetwork = eth.connect();
  if (statusNetwork < 0) {
    currentError = statusNetwork;
  } else {
    socket.open(&eth);
    /*
    nsapi_error_t tcpError = socket.connect(host.c_str(), 80);
    if (tcpError < 0) {
      currentError = tcpError;
    }
    */
  }
}

HttpClient::~HttpClient()
{
  eth.disconnect();
}

/*void
HttpClient::setMethod(HttpMethod _method)
{
  method = _method;
}*/
void
HttpClient::setMethod(std::string _method)
{
  method = _method;
}

void
HttpClient::setTarget(std::string _target)
{
  if (_target[0] != '/') {
    char buffer[64];
    sprintf(buffer, "/%s", _target.c_str());
    target = std::string(buffer);
  } else {
    target = _target;
  }
}

/*void
HttpClient::addHeaderField(HttpHeaderField _field, std::string _value)
{
  this->headerFieldValues[(int)_field] = _value;
}*/
void
HttpClient::addHeaderField(std::string _field, std::string _value)
{
  if (_field.compare("User-Agent")) {
    headerFieldValues[0] = _value;
  } else if (_field.compare("Content-Type")) {
    headerFieldValues[1] = _value;
  }
}

void
HttpClient::setContent(std::string _content)
{
  content = _content;
}

std::string
HttpClient::getCurrentError()
{
  switch (currentError)
  {
  case -3001 :
    return std::string("Would Block.");
  case -3002 :
    return std::string("Unsupported.");
  case -3003 :
    return std::string("Invalid configuration.");
  case -3004 :
    return std::string("No connection.");
  case -3005 :
    return std::string("No socket.");
  case -3006 :
    return std::string("No address.");
  case -3007 :
    return std::string("No memory.");
  case -3008 :
    return std::string("No SSID.");
  case -3009 :
    return std::string("DNS Failure.");
  case -3010 :
    return std::string("DHCP Failure.");
  case -3011 :
    return std::string("Authentication failure.");
  case -3012 :
    return std::string("Device error.");
  case -3013 :
    return std::string("Operation in progress.");
  case -3014 :
    return std::string("Operation already in progress.");
  case -3015 :
    return std::string("Socket already connected.");
  case -3016 :
    return std::string("Connection lost.");
  case -3017 :
    return std::string("Connection timeout.");
  case -3018 :
    return std::string("Address already in use.");
  case -3019 :
    return std::string("Operation timeout.");
  case -3020 :
    return std::string("Device busy.");
  default :
    if (currentError < 0) return std::string("Unknown Error");
    return std::string("No erreur");
  }
}

std::string
HttpClient::send()
{
  char result[512];

  sprintf(result, "%s %s %s\r\n", method.c_str(), target.c_str(), protocol.c_str());
  sprintf(result, "%sHost: %s\r\n", result, host.c_str());
  sprintf(result, "%s%s: %s\r\n", result, "User-Agent", "S5-P4-Glove-Eth-Module");
  sprintf(result, "%s%s: %s\r\n", result, "Content-Type", "application/x-www-form-urlencoded");

  if (method.compare("POST") == 0) {
    sprintf(result, "%s%s: %d\r\n", result, "Content-Length", sizeof (content));
  }

  // Needs 2 CRLF to be valid
  sprintf(result, "%s\r\n", result);

  // Add content if necessary
  if (content.compare("") != 0) {
    sprintf(result, "%s%s\r\n", result, content.c_str());
  }

  // printf("\r\nMessage: \r\n%s\r\n", result);
  int scount = socket.send(result, strlen(result));
  currentError = scount;

  if (scount >= 0) {
    char rbuffer[1024];
    nsapi_error_t tcpError = socket.connect(host.c_str(), 80);
    if (tcpError < 0) {
      currentError = tcpError;
      return getCurrentError();
    } else {
      socket.recv(rbuffer, sizeof rbuffer);
      return std::string(rbuffer);
    }
  }
  return std::string("We didn't received data (%d)", scount);
}

void
HttpClient::closeTcp()
{
  socket.close();
}