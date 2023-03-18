#include <netinet/in.h>
#include <vector>
#include <thread>
#include <sys/socket.h>

class Server {
  public:
    void open(int port);
    void closeClientConnection(int socket, std::thread::id threadId);
    void acceptConnection(int client_sock);
    ~Server();

  private:
    int server_socket;
    sockaddr_in server_address;
    std::vector<std::thread*> connectionList;
};