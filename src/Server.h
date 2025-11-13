#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <cstdint>

class Server {
public:
    Server();
    bool initialize(const std::string& userDbFile, const std::string& logFile, uint16_t port);
    void run();
    void stop();
    
private:
    int m_serverSocket;
    uint16_t m_port;
    bool m_running;
    std::string m_userDbFile;
    
    bool createSocket();
    bool bindSocket();
    bool startListening();
    void handleClient(int clientSocket);
    bool authenticateClient(int clientSocket);
    void processVectors(int clientSocket);
};

#endif