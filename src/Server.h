#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <cstdint>

class Server {
public:
    Server(const std::string& configFile, const std::string& logFile, uint16_t port);
    ~Server();
    
    bool initialize();
    void run();
    void stop();

private:
    std::string configFile_;
    std::string logFile_;
    uint16_t port_;
    int serverSocket_;
    bool running_;
    
    bool loadConfiguration();
    bool createSocket();
    bool bindSocket();
    bool startListening();
    void handleClient(int clientSocket);
    void cleanup();
};

#endif // SERVER_H