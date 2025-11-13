#include "Server.h"
#include "AuthManager.h"
#include "VectorProcessor.h"
#include "Logger.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <thread>

// Конструктор сервера
Server::Server() : m_serverSocket(-1), m_port(0), m_running(false), m_userDbFile("") {}

// Инициализация сервера
bool Server::initialize(const std::string& userDbFile, const std::string& logFile, uint16_t port) {
    m_port = port;
    m_userDbFile = userDbFile;
    
    // Инициализация логгера
    if (!Logger::getInstance().initialize(logFile)) {
        std::cerr << "Не удалось инициализировать логгер" << std::endl;
        return false;
    }
    
    // Создание сокета
    if (!createSocket()) {
        return false;
    }
    
    // Привязка сокета к адресу
    if (!bindSocket()) {
        return false;
    }
    
    // Начало прослушивания
    if (!startListening()) {
        return false;
    }
    
    Logger::getInstance().log(LogLevel::INFO, "Сервер инициализирован", 
                             "порт: " + std::to_string(port) + ", база пользователей: " + userDbFile);
    
    return true;
}

// Остановка сервера
void Server::stop() {
    m_running = false;
    if (m_serverSocket != -1) {
        close(m_serverSocket);
        m_serverSocket = -1;
    }
    Logger::getInstance().log(LogLevel::INFO, "Сервер остановлен");
}

// Создание серверного сокета
bool Server::createSocket() {
    m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_serverSocket == -1) {
        Logger::getInstance().log(LogLevel::ERROR, "Не удалось создать сокет");
        return false;
    }
    
    // Установка опций сокета
    int opt = 1;
    if (setsockopt(m_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        Logger::getInstance().log(LogLevel::ERROR, "Не удалось установить опции сокета");
        close(m_serverSocket);
        return false;
    }
    
    return true;
}

// Привязка сокета к адресу и порту
bool Server::bindSocket() {
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(m_port);
    
    if (bind(m_serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        Logger::getInstance().log(LogLevel::ERROR, "Не удалось привязать сокет", 
                                 "порт: " + std::to_string(m_port));
        close(m_serverSocket);
        return false;
    }
    
    return true;
}

// Начало прослушивания входящих соединений
bool Server::startListening() {
    if (listen(m_serverSocket, 10) < 0) {
        Logger::getInstance().log(LogLevel::ERROR, "Не удалось начать прослушивание");
        close(m_serverSocket);
        return false;
    }
    
    return true;
}

// Основной цикл работы сервера
void Server::run() {
    m_running = true;
    Logger::getInstance().log(LogLevel::INFO, "Сервер запущен");
    
    while (m_running) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        
        // Ожидание входящего соединения
        int clientSocket = accept(m_serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSocket < 0) {
            if (m_running) {
                Logger::getInstance().log(LogLevel::ERROR, "Не удалось принять соединение от клиента");
            }
            continue;
        }
        
        // Обработка клиента в отдельном потоке
        std::thread clientThread(&Server::handleClient, this, clientSocket);
        clientThread.detach();
    }
}

// Обработка клиентского соединения
void Server::handleClient(int clientSocket) {
    Logger::getInstance().log(LogLevel::INFO, "Клиент подключился", 
                             "сокет: " + std::to_string(clientSocket));
    
    try {
        // Аутентификация клиента
        if (!authenticateClient(clientSocket)) {
            Logger::getInstance().log(LogLevel::ERROR, "Ошибка аутентификации", 
                                     "сокет: " + std::to_string(clientSocket));
            close(clientSocket);
            return;
        }
        
        Logger::getInstance().log(LogLevel::INFO, "Клиент аутентифицирован", 
                                 "сокет: " + std::to_string(clientSocket));
        
        // Обработка векторов от клиента
        processVectors(clientSocket);
        
    } catch (const std::exception& e) {
        Logger::getInstance().log(LogLevel::ERROR, "Ошибка обработки клиента", 
                                 std::string("сокет: ") + std::to_string(clientSocket) + 
                                 ", ошибка: " + e.what());
    }
    
    close(clientSocket);
    Logger::getInstance().log(LogLevel::INFO, "Клиент отключился", 
                             "сокет: " + std::to_string(clientSocket));
}

// Процесс аутентификации клиента
bool Server::authenticateClient(int clientSocket) {
    AuthManager authManager;
    
    // Используем сохраненный путь к базе пользователей
    if (!authManager.loadUserDatabase(m_userDbFile)) {
        Logger::getInstance().log(LogLevel::ERROR, "Не удалось загрузить базу пользователей", 
                                 "файл: " + m_userDbFile);
        return false;
    }
    
    // Получение логина от клиента
    char loginBuffer[256];
    ssize_t bytesRead = recv(clientSocket, loginBuffer, sizeof(loginBuffer) - 1, 0);
    if (bytesRead <= 0) {
        return false;
    }
    loginBuffer[bytesRead] = '\0';
    std::string login(loginBuffer);
    
    // Отправка соли клиенту
    std::string salt = authManager.generateSalt();
    if (send(clientSocket, salt.c_str(), salt.length(), 0) <= 0) {
        return false;
    }
    
    // Получение хеша от клиента
    char hashBuffer[65];
    bytesRead = recv(clientSocket, hashBuffer, sizeof(hashBuffer) - 1, 0);
    if (bytesRead <= 0) {
        return false;
    }
    hashBuffer[bytesRead] = '\0';
    std::string clientHash(hashBuffer);
    
    // Проверка аутентификации и отправка результата
    bool authResult = authManager.authenticate(login, salt, clientHash);
    std::string response = authResult ? "OK" : "ERR";
    
    if (send(clientSocket, response.c_str(), response.length(), 0) <= 0) {
        return false;
    }
    
    return authResult;
}

// Обработка векторов от клиента
void Server::processVectors(int clientSocket) {
    // Получение количества векторов
    uint32_t numVectors;
    ssize_t bytesRead = recv(clientSocket, &numVectors, sizeof(numVectors), 0);
    if (bytesRead != sizeof(numVectors)) {
        throw std::runtime_error("Не удалось получить количество векторов");
    }

    std::vector<uint32_t> results;
    
    // Обработка каждого вектора
    for (uint32_t i = 0; i < numVectors; i++) {
        // Получение размера вектора
        uint32_t vectorSize;
        bytesRead = recv(clientSocket, &vectorSize, sizeof(vectorSize), 0);
        if (bytesRead != sizeof(vectorSize)) {
            throw std::runtime_error("Не удалось получить размер вектора");
        }
        
        // Получение данных вектора
        std::vector<uint32_t> vector(vectorSize);
        size_t expectedBytes = vectorSize * sizeof(uint32_t);
        bytesRead = recv(clientSocket, vector.data(), expectedBytes, 0);
        if (bytesRead != static_cast<ssize_t>(expectedBytes)) {
            throw std::runtime_error("Не удалось получить данные вектора");
        }
        
        // Вычисление и отправка результата
        uint32_t result = VectorProcessor::computeProduct(vector);
        
        if (send(clientSocket, &result, sizeof(result), 0) != sizeof(result)) {
            throw std::runtime_error("Не удалось отправить результат");
        }
        
        results.push_back(result);
    }
    
    Logger::getInstance().log(LogLevel::INFO, "Векторы обработаны", 
                             "количество: " + std::to_string(results.size()));
}