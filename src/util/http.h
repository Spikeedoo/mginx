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

  void sendStatus(int socket, int statusCode);
  void sendStandardHeaders(int socket, std::string mimeType, int contentLength);
  HttpRequest parseRequest(std::vector<std::string> reqLines);
  void handleRequest(int socket, std::vector<char> req);
  void send404(int socket);
}