#include "Server.h"
#include "AuthManager.h"
#include "VectorProcessor.h"
#include "Logger.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <vector>

Server::Server(const std::string& configFile, const std::string& logFile, uint16_t port)
    : configFile_(configFile), logFile_(logFile), port_(port), serverSocket_(-1), running_(false) {
}

Server::~Server() {
    stop();
}

bool Server::initialize() {
    Logger::getInstance().initialize(logFile_);
    
    if (!loadConfiguration()) {
        Logger::getInstance().log("ERROR: Failed to load configuration", true);
        return false;
    }
    
    if (!createSocket()) {
        Logger::getInstance().log("ERROR: Failed to create socket", true);
        return false;
    }
    
    if (!bindSocket()) {
        Logger::getInstance().log("ERROR: Failed to bind socket", true);
        return false;
    }
    
    if (!startListening()) {
        Logger::getInstance().log("ERROR: Failed to start listening", true);
        return false;
    }
    
    Logger::getInstance().log("Server initialized successfully on port " + std::to_string(port_));
    return true;
}

bool Server::loadConfiguration() {
    // Загрузка базы пользователей из файла
    return AuthManager::getInstance().loadUsers(configFile_);
}

bool Server::createSocket() {
    serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket_ == -1) {
        Logger::getInstance().log("ERROR: Socket creation failed", true);
        return false;
    }
    
    // Установка опции переиспользования адреса
    int opt = 1;
    if (setsockopt(serverSocket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        Logger::getInstance().log("ERROR: setsockopt failed", true);
        return false;
    }
    
    return true;
}

bool Server::bindSocket() {
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port_);
    
    if (bind(serverSocket_, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        Logger::getInstance().log("ERROR: Bind failed", true);
        return false;
    }
    
    return true;
}

bool Server::startListening() {
    if (listen(serverSocket_, 10) < 0) {
        Logger::getInstance().log("ERROR: Listen failed", true);
        return false;
    }
    
    running_ = true;
    return true;
}

void Server::run() {
    if (!running_) {
        Logger::getInstance().log("ERROR: Server not initialized", true);
        return;
    }
    
    Logger::getInstance().log("Server started and listening on port " + std::to_string(port_));
    
    while (running_) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        
        int clientSocket = accept(serverSocket_, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSocket < 0) {
            if (running_) {
                Logger::getInstance().log("ERROR: Accept failed", true);
            }
            continue;
        }
        
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
        Logger::getInstance().log("New connection from " + std::string(clientIP));
        
        // Обработка клиента в отдельном потоке
        std::thread clientThread(&Server::handleClient, this, clientSocket);
        clientThread.detach();
    }
}

void Server::handleClient(int clientSocket) {
    AuthManager& authManager = AuthManager::getInstance();
    VectorProcessor processor;
    
    try {
        // Клиент отправляет логин
        char loginBuffer[256];
        ssize_t bytesRead = recv(clientSocket, loginBuffer, sizeof(loginBuffer) - 1, 0);
        if (bytesRead <= 0) {
            close(clientSocket);
            return;
        }
        loginBuffer[bytesRead] = '\0';
        std::string login(loginBuffer);
        
        // Сервер проверяет логин и отправляет соль
        std::string salt = authManager.generateSalt();
        if (!authManager.userExists(login)) {
            std::string errorMsg = "ERR";
            send(clientSocket, errorMsg.c_str(), errorMsg.length(), 0);
            close(clientSocket);
            Logger::getInstance().log("Authentication failed: user '" + login + "' not found");
            return;
        }
        
        // Отправка соли клиенту
        send(clientSocket, salt.c_str(), salt.length(), 0);
        
        // Клиент отправляет хеш
        char hashBuffer[65]; // 64 символа для SHA-256 + null terminator
        bytesRead = recv(clientSocket, hashBuffer, sizeof(hashBuffer) - 1, 0);
        if (bytesRead <= 0) {
            close(clientSocket);
            return;
        }
        hashBuffer[bytesRead] = '\0';
        std::string receivedHash(hashBuffer);
        
        // Сервер проверяет хеш
        if (authManager.verifyHash(login, salt, receivedHash)) {
            std::string successMsg = "OK";
            send(clientSocket, successMsg.c_str(), successMsg.length(), 0);
            Logger::getInstance().log("User '" + login + "' authenticated successfully");
            
            // Обработка векторов
            processor.processVectors(clientSocket);
            
        } else {
            std::string errorMsg = "ERR";
            send(clientSocket, errorMsg.c_str(), errorMsg.length(), 0);
            Logger::getInstance().log("Authentication failed for user '" + login + "'");
        }
        
    } catch (const std::exception& e) {
        Logger::getInstance().log("ERROR in client handling: " + std::string(e.what()), true);
    }
    
    close(clientSocket);
}

void Server::stop() {
    running_ = false;
    if (serverSocket_ != -1) {
        close(serverSocket_);
        serverSocket_ = -1;
    }
    cleanup();
}

void Server::cleanup() {
    Logger::getInstance().log("Server stopped");
}