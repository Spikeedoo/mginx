#include <stdio.h>
#include <sys/socket.h>

#include "files.h"

int FileUtils::getContentLength(std::string path) {
  // Convert string to char array since we are using a C library
  const char* convertedPath = path.c_str();
  FILE *fd = fopen(convertedPath, "rb");
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
  return contentLength;
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