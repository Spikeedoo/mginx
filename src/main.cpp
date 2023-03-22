#include <iostream>
#include <string>

#include "server.h"

bool is_int(char str[]) {
  for (int i = 0; str[i] != 0; i++) {
    if (!isdigit(str[i])) {
      return false;
    }
  }
  return true;
}

int main(int argc, char *argv[]) {
  int port = 80;
  if (argc > 1) {
    std::cout << "argc" << std::endl;
    if (is_int(argv[1])) {
      std::cout << "inner" << std::endl;
      port = std::stoi(argv[1]);
    }
  }

  std::cout << "Starting MGINX on port " << port << "...\n" << std::endl;
  Server s;
  try {
    s.open(port);
  } catch(std::string err) {
    std::cout << err << std::endl;
  }
}