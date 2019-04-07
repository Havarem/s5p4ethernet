#ifndef __HTTP_CLIENT_H
#define __HTTP_CLIENT_H

#include <stdio.h>
#include <string>

#include <mbed.h>

#include "EthernetInterface.h"

#define CRLF "\r\n";
#define HTTP1_PROTOCOL "HTTP/1.1"
#define DEFAULT_TARGET std::string("/")

/*typedef enum HttpMethod {
  GET = 0,
  POST = 1,
  PUT = 2,
  DELETE = 3
} HttpMethod;

typedef enum HttpHeaderField {
  USER_AGENT = 0,
  CONTENT_TYPE = 1,
  CONTENT_LENGTH = 2
} HttpHeaderField;*/

class HttpClient {
  public:
    //static std::string MethodName[4];
    //static std::string HeaderFieldName[3];

    HttpClient();
    //HttpClient(HttpMethod, std::string, std::string);
    HttpClient(std::string, std::string, std::string);
    ~HttpClient();

    //void setMethod(HttpMethod);
    void setMethod(std::string);
    void setTarget(std::string);

    //void addHeaderField(HttpHeaderField, std::string);
    void addHeaderField(std::string, std::string);

    void setContent(std::string);

    std::string getCurrentError();

    std::string send();

    void closeTcp();

  private:
    //HttpMethod method;
    std::string method;
    std::string target;
    std::string protocol;

    std::string host;
    //std::string headerFieldValues[sizeof HttpClient::HeaderFieldName];
    std::string headerFieldValues[2];
    std::string content;

    std::string response;

    EthernetInterface eth;
    TCPSocket socket;

    int currentError;

};

#endif
