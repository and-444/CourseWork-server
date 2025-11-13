#include "VectorProcessor.h"
#include <limits>

uint32_t VectorProcessor::computeProduct(const std::vector<uint32_t>& vector) {
    if (vector.empty()) {
        return 0;
    }
    
    uint64_t product = 1;
    for (uint32_t value : vector) {
        product *= value;
        if (product > std::numeric_limits<uint32_t>::max()) {
            return std::numeric_limits<uint32_t>::max();
        }
    }
    
    return static_cast<uint32_t>(product);
}