#include <string>

namespace FileUtils {
  struct FileInfo {
    std::string path;
    std::string mimeType;
    int contentLength;
  };

  void sendFileOverSocket(int socket, std::string path);
  FileInfo getFileMetadata(std::string path);
  std::string getMimeType(std::string path);
};