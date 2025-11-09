#ifndef VECTORPROCESSOR_H
#define VECTORPROCESSOR_H

#include <cstdint>

class VectorProcessor {
public:
    VectorProcessor() = default;
    void processVectors(int clientSocket);
    
private:
    uint32_t computeProduct(const uint32_t* vector, uint32_t size);
    bool receiveVector(int clientSocket, uint32_t*& vector, uint32_t& size);
    void sendResults(int clientSocket, const uint32_t* results, uint32_t count);
};

#endif // VECTORPROCESSOR_H