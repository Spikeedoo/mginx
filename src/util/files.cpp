#include <stdio.h>
#include <sys/socket.h>
#include <filesystem>

#include "files.h"

std::string FileUtils::getMimeType(std::string path) {
  std::string DEFAULT_MIME_TYPE = "application/octet-stream";
  // Find last '.'
  std::string::size_type lastDotLocation = path.rfind('.');
  if (lastDotLocation != std::string::npos) {
    ++lastDotLocation;
  } else {
    lastDotLocation = 0;
  }
  // Substring to get the extension
  std::string extensionName = path.substr(lastDotLocation);
  if (extensionName == "html" || extensionName == "htm") return "text/html";
  if (extensionName == "jpeg" || extensionName == "jpg") return "image/jpg";
  if (extensionName == "css") return "text/css";
  if (extensionName == "js") return "application/javascript";
  if (extensionName == "json") return "application/json";
  if (extensionName == "txt") return "text/plain";
  if (extensionName == "gif") return "image/gif";
  if (extensionName == "png") return "image/png";

  return DEFAULT_MIME_TYPE;
}

FileUtils::FileInfo FileUtils::getFileMetadata(std::string path) {
  FileUtils::FileInfo fileInfo;

  // Convert string to char array since we are using a C library
  const char* convertedPath = path.c_str();
  FILE *fd = fopen(convertedPath, "rb");
  // Error if the file does not exist
  if (fd == NULL) {
    throw "File does not exist";
  }

  // Error if the target is a directory
  if (std::filesystem::is_directory(path)) {
    throw "Tried to access directory";
  }

  int bytesRead;
  int contentLength = 0;
  const int BUFFER_SIZE = 256;
  char buffer[BUFFER_SIZE];
  while (!feof(fd)) {
    while (true) {
      if ((bytesRead = fread(&buffer, 1, BUFFER_SIZE, fd)) > 0) {
        contentLength += bytesRead;
      } else {
        break;
      }
    }
    break;
  }
  fclose(fd);

  fileInfo.contentLength = contentLength;

  return fileInfo;
}

void FileUtils::sendFileOverSocket(int socket, std::string path) {
  // Convert string to char array since we are using a C library
  const char* convertedPath = path.c_str();
  FILE *fd = fopen(convertedPath, "rb");
  int bytes_read;
  const int BUFFER_SIZE = 256;
  char buffer[BUFFER_SIZE];
  while (!feof(fd)) {
    if ((bytes_read = fread(&buffer, 1, BUFFER_SIZE, fd)) > 0) {
      // If there are still bytes to read, read them and send them over the socket
      send(socket, buffer, bytes_read, 0);
    } else {
      break;
    }
  }
  fclose(fd);
}