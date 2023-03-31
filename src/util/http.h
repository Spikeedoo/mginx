#include <vector>
#include <string>
#include <map>

namespace HttpUtils {
  struct HttpRequest {
    std::string protocol;
    std::string url;
    std::string method;
    std::map<std::string, std::string> params;
  };

  struct GatewayItem {
    std::string path;
    std::string service;
  };

  void sendStatus(int socket, int statusCode);
  void sendStandardHeaders(int socket, std::string mimeType, int contentLength);
  HttpRequest parseRequest(std::vector<std::string> reqLines);
  void handleRequest(int socket, std::vector<char> req);
  void handleGatewayRequest(int socket, std::vector<char> req, GatewayItem gateItem);
  void send404(int socket);
}