#include <iostream>
#include <string>

#include "server.h"

int main() {
  std::cout << "Starting MGINX on port 80...\n" << std::endl;
  Server s;
  try {
    s.open(80);
  } catch(std::string err) {
    std::cout << err << std::endl;
  }
}