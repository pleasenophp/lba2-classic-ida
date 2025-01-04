#pragma once

#include <vector>
#include <cstddef>

namespace Ida
{
    class ElasticBuffer
    {
    public:
        ElasticBuffer(size_t initialSize = 0, double growthRate = 2);

        void ensureCapacity(size_t requiredSize);

        char *getBuffer();

    private:
        std::vector<char> buffer;
        size_t currentCapacity;
        double growthRate;
    };
}
