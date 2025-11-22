#include <UnitTest++/UnitTest++.h>
#include <vector>
#include <limits>
#include "VectorProcessor.h"

SUITE(VectorProcessorTests) {
    TEST(EmptyVector) {
        std::vector<uint32_t> empty;
        uint32_t result = VectorProcessor::computeProduct(empty);
        
        CHECK_EQUAL(0, result);
    }
    
    TEST(SingleElement) {
        std::vector<uint32_t> single = {42};
        uint32_t result = VectorProcessor::computeProduct(single);
        
        CHECK_EQUAL(42, result);
    }
    
    TEST(MultipleElements) {
        std::vector<uint32_t> vec = {2, 3, 4, 5};
        uint32_t result = VectorProcessor::computeProduct(vec);
        
        CHECK_EQUAL(120, result); // 2*3*4*5 = 120
    }
    
    TEST(WithZero) {
        std::vector<uint32_t> vec = {1, 2, 0, 4, 5};
        uint32_t result = VectorProcessor::computeProduct(vec);
        
        CHECK_EQUAL(0, result);
    }
    
    TEST(AllOnes) {
        std::vector<uint32_t> vec = {1, 1, 1, 1, 1};
        uint32_t result = VectorProcessor::computeProduct(vec);
        
        CHECK_EQUAL(1, result);
    }
    
    TEST(OverflowDetection) {
        std::vector<uint32_t> vec = {65536, 65536}; // 2^16 * 2^16 = 2^32
        uint32_t result = VectorProcessor::computeProduct(vec);
        
        CHECK_EQUAL(std::numeric_limits<uint32_t>::max(), result);
    }
    
    TEST(LargeNumbersNoOverflow) {
        std::vector<uint32_t> vec = {1000, 1000, 1000}; // 1,000,000,000 < 2^32
        uint32_t result = VectorProcessor::computeProduct(vec);
        
        CHECK_EQUAL(1000000000, result);
    }
    
    TEST(BoundaryValues) {
        std::vector<uint32_t> maxVal = {std::numeric_limits<uint32_t>::max()};
        uint32_t result1 = VectorProcessor::computeProduct(maxVal);
        CHECK_EQUAL(std::numeric_limits<uint32_t>::max(), result1);
        
        std::vector<uint32_t> withOne = {std::numeric_limits<uint32_t>::max(), 1};
        uint32_t result2 = VectorProcessor::computeProduct(withOne);
        CHECK_EQUAL(std::numeric_limits<uint32_t>::max(), result2);
    }
}