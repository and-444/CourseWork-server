/**
 * @file functional_client.cpp
 * @brief Тестовый клиент для функционального тестирования сервера
 * @author Савин А.В.
 * @version 1.0
 */

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <chrono>
#include <thread>

class FunctionalClient {
private:
    int m_socket;
    std::string m_host;
    uint16_t m_port;
    
public:
    FunctionalClient(const std::string& host = "127.0.0.1", uint16_t port = 33333) 
        : m_socket(-1), m_host(host), m_port(port) {}
    
    ~FunctionalClient() {
        disconnect();
    }
    
    bool connect() {
        m_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (m_socket < 0) {
            std::cerr << "Ошибка создания сокета" << std::endl;
            return false;
        }
        
        struct sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(m_port);
        
        if (inet_pton(AF_INET, m_host.c_str(), &serverAddr.sin_addr) <= 0) {
            std::cerr << "Неверный адрес сервера" << std::endl;
            return false;
        }
        
        if (::connect(m_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            std::cerr << "Ошибка подключения к серверу" << std::endl;
            return false;
        }
        
        return true;
    }
    
    void disconnect() {
        if (m_socket != -1) {
            close(m_socket);
            m_socket = -1;
        }
    }
    
    bool authenticate(const std::string& login, const std::string& password) {
        // Отправка логина
        if (send(m_socket, login.c_str(), login.length(), 0) <= 0) {
            return false;
        }
        
        // Получение соли
        char saltBuffer[17];
        ssize_t bytesRead = recv(m_socket, saltBuffer, 16, 0);
        if (bytesRead != 16) {
            return false;
        }
        saltBuffer[16] = '\0';
        std::string salt(saltBuffer);
        
        // Вычисление хеша
        std::string hash = "MOCK_HASH_FOR_TESTING";
        
        // Отправка хеша
        if (send(m_socket, hash.c_str(), hash.length(), 0) <= 0) {
            return false;
        }
        
        // Получение результата аутентификации
        char responseBuffer[4];
        bytesRead = recv(m_socket, responseBuffer, 3, 0);
        if (bytesRead <= 0) {
            return false;
        }
        responseBuffer[bytesRead] = '\0';
        
        return std::string(responseBuffer) == "OK";
    }
    
    std::vector<uint32_t> sendVectors(const std::vector<std::vector<uint32_t>>& vectors) {
        std::vector<uint32_t> results;
        
        // Отправка количества векторов
        uint32_t numVectors = vectors.size();
        uint32_t netNumVectors = htonl(numVectors);
        if (send(m_socket, &netNumVectors, sizeof(netNumVectors), 0) != sizeof(netNumVectors)) {
            return results;
        }
        
        // Отправка каждого вектора
        for (const auto& vector : vectors) {
            // Размер вектора
            uint32_t vectorSize = vector.size();
            uint32_t netVectorSize = htonl(vectorSize);
            if (send(m_socket, &netVectorSize, sizeof(netVectorSize), 0) != sizeof(netVectorSize)) {
                return results;
            }
            
            // Данные вектора
            if (!vector.empty()) {
                std::vector<uint32_t> netVector = vector;
                for (auto& val : netVector) {
                    val = htonl(val);
                }
                size_t dataSize = vectorSize * sizeof(uint32_t);
                if (send(m_socket, netVector.data(), dataSize, 0) != static_cast<ssize_t>(dataSize)) {
                    return results;
                }
            }
            
            // Получение результата
            uint32_t result;
            ssize_t bytesRead = recv(m_socket, &result, sizeof(result), 0);
            if (bytesRead != sizeof(result)) {
                return results;
            }
            results.push_back(ntohl(result));
        }
        
        return results;
    }
};