#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <unistd.h>
#include <vector>

#include "server.h"
#include "util/files.h"
#include "util/http.h"

// ** Method to clean up & close client connection ** //
void Server::closeClientConnection(int socket, std::thread::id threadId) {
  // Loop through connection list
  for(int i = 0; i < this->connectionList.size(); ++i) {
    std::thread* t = this->connectionList[i];
    if (t->get_id() == threadId) {
      // Remove thread from connection pool
      this->connectionList.erase(this->connectionList.begin() + i);
    }
  }
  // Close socket
  close(socket);
  std::cout << "CONNECTION POOL: " << this->connectionList.size() << std::endl;
}

// ** Method to handle each individual client thread ** //
void Server::acceptConnection(int client_sock) {
  std::vector<char> buf(4096);
  while (true) {
    // Block for data over socket
    int bytes = recv(client_sock, buf.data(), buf.size(), 0);
    if (bytes == 0) {
      // Connection was closed by client
      this->closeClientConnection(client_sock, std::this_thread::get_id());
      break;
    }
    // Pre read the file in to get the number of bytes
    int contentLength = FileUtils::getContentLength("../public/splash.html");
    // Send status code
    HttpUtils::sendStatus(client_sock, 200);
    // Send response headers
    HttpUtils::sendStandardHeaders(client_sock, contentLength);
    // Send content
    FileUtils::sendFileOverSocket(client_sock, "../public/splash.html");
  }
}

// ** Method to bind server to port & accept incoming connections ** //
void Server::open(int port) {
  // Create socket
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  this->server_socket = server_socket;

  // Set up IP address
  sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);
  this->server_address = address;

  // Bind the socket to the IP address
  if (bind(server_socket, (struct sockaddr *) &this->server_address, sizeof(this->server_address)) == -1) {
    throw "Could not bind the socket to the IP address";
  }

  // Listen for connections
  if (listen(this->server_socket, 5) == -1) {
    throw "Error trying to listen for connections";
  }

  std::cout << "Listening for connections!" << std::endl;

  while (true) {
    int client_socket = accept(this->server_socket, (struct sockaddr *) &this->server_address, (socklen_t *) &this->server_address);
    std::cout << "New connection!" << std::endl;
    std::thread* newThreadPtr = new std::thread(&Server::acceptConnection, this, client_socket);
    this->connectionList.push_back(newThreadPtr);
    std::cout << "CONNECTION POOL: " << this->connectionList.size() << std::endl;
  }
}

// ** Destructor ** //
Server::~Server() {
  // Close the socket
  close(this->server_socket);

  // Join all client threads
  for(auto& t: this->connectionList){
    if (t->joinable()) {
      t->join();
    }
  }
}