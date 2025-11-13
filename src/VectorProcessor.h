#ifndef VECTORPROCESSOR_H
#define VECTORPROCESSOR_H

#include <vector>
#include <cstdint>

class VectorProcessor {
public:
    static uint32_t computeProduct(const std::vector<uint32_t>& vector);
};

#endif