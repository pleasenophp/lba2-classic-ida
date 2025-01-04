#include "ElasticBuffer.h"
#include <stdexcept>
#include <cmath>
#include <iostream>

using namespace std;

ElasticBuffer::ElasticBuffer(size_t initialSize, double growthRate)
    : currentCapacity(initialSize), growthRate(growthRate) {
    if (growthRate <= 1) {
        throw invalid_argument("Growth rate must be greater than 1.0.");
    }
    if (initialSize > 0) {
        buffer.reserve(initialSize);
    }
}

void ElasticBuffer::ensureCapacity(size_t requiredSize) {
    if (!buffer.capacity()) {
        currentCapacity = requiredSize * 2;
        buffer.reserve(currentCapacity);
        // TODO - remove debug messages
        std::cout << "Buffer initially reallocated to " << currentCapacity << " bytes." << std::endl;
    }

    if (requiredSize > currentCapacity) {
        while (currentCapacity < requiredSize) {
            currentCapacity = static_cast<size_t>(std::ceil(currentCapacity * growthRate));
        }
        buffer.reserve(currentCapacity);
        std::cout << "Buffer reallocated to " << currentCapacity << " bytes." << std::endl;
    }
    buffer.resize(requiredSize);
}

// Access the raw buffer pointer
char* ElasticBuffer::getBuffer() {
    return buffer.data();
}

