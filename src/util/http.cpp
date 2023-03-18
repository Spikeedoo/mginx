#include <sys/socket.h>
#include <string>

#include "http.h"

void HttpUtils::sendStatus(int socket, int statusCode) {
  std::string response = "HTTP/1.1 200 OK\r\n";
  const char* responseConverted = response.data();
  send(socket, responseConverted, response.size(), 0);
}

void HttpUtils::sendStandardHeaders(int socket, int contentLength) {
  std::string responseTwo = "Content-Type: text/html; encoding=utf8\r\nContent-Length: " + std::to_string(contentLength) + "\r\nConnection: keep-alive\r\n\r\n";
  const char* responseConvertedTwo = responseTwo.data();
  send(socket, responseConvertedTwo, responseTwo.size(), 0);
}