#include <string>

namespace FileUtils {
  void sendFileOverSocket(int socket, std::string path);
  int getContentLength(std::string path);
}