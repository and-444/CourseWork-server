#include "VectorProcessor.h"
#include "Logger.h"
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <vector>
#include <limits>

void VectorProcessor::processVectors(int clientSocket) {
    try {
        // Получение количества векторов
        uint32_t vectorCount;
        ssize_t bytesRead = recv(clientSocket, &vectorCount, sizeof(vectorCount), 0);
        if (bytesRead != sizeof(vectorCount)) {
            Logger::getInstance().log("ERROR: Failed to receive vector count", true);
            return;
        }
        
        // Преобразование из сетевого порядка байт
        vectorCount = ntohl(vectorCount);
        
        std::vector<uint32_t> results;
        results.reserve(vectorCount);
        
        // Обработка каждого вектора
        for (uint32_t i = 0; i < vectorCount; i++) {
            uint32_t* vector = nullptr;
            uint32_t size = 0;
            
            if (receiveVector(clientSocket, vector, size)) {
                uint32_t product = computeProduct(vector, size);
                results.push_back(product);
                delete[] vector;
            } else {
                Logger::getInstance().log("ERROR: Failed to receive vector " + std::to_string(i), true);
                break;
            }
        }
        
        // Отправка результатов
        if (!results.empty()) {
            sendResults(clientSocket, results.data(), results.size());
        }
        
    } catch (const std::exception& e) {
        Logger::getInstance().log("ERROR in vector processing: " + std::string(e.what()), true);
    }
}

bool VectorProcessor::receiveVector(int clientSocket, uint32_t*& vector, uint32_t& size) {
    // Получение размера вектора
    ssize_t bytesRead = recv(clientSocket, &size, sizeof(size), 0);
    if (bytesRead != sizeof(size)) {
        return false;
    }
    size = ntohl(size);
    
    if (size == 0) {
        vector = nullptr;
        return true;
    }
    
    // Выделение памяти для вектора
    vector = new uint32_t[size];
    
    // Получение данных вектора
    bytesRead = recv(clientSocket, vector, size * sizeof(uint32_t), MSG_WAITALL);
    if (bytesRead != static_cast<ssize_t>(size * sizeof(uint32_t))) {
        delete[] vector;
        return false;
    }
    
    // Преобразование данных из сетевого порядка байт
    for (uint32_t i = 0; i < size; i++) {
        vector[i] = ntohl(vector[i]);
    }
    
    return true;
}

uint32_t VectorProcessor::computeProduct(const uint32_t* vector, uint32_t size) {
    if (size == 0) {
        return 0;
    }
    
    uint64_t product = 1;
    for (uint32_t i = 0; i < size; i++) {
        if (vector[i] == 0) {
            return 0;
        }
        
        // Проверка на переполнение
        if (product > std::numeric_limits<uint32_t>::max() / vector[i]) {
            return std::numeric_limits<uint32_t>::max(); // 2^32 - 1
        }
        
        product *= vector[i];
    }
    
    return static_cast<uint32_t>(product);
}

void VectorProcessor::sendResults(int clientSocket, const uint32_t* results, uint32_t count) {
    // Отправка количества результатов
    uint32_t netCount = htonl(count);
    send(clientSocket, &netCount, sizeof(netCount), 0);
    
    // Подготовка и отправка результатов
    std::vector<uint32_t> netResults;
    netResults.reserve(count);
    for (uint32_t i = 0; i < count; i++) {
        netResults.push_back(htonl(results[i]));
    }
    
    send(clientSocket, netResults.data(), count * sizeof(uint32_t), 0);
}