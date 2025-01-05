#pragma once

#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <vector>

namespace Ida
{
    template <typename T>
    class ElasticBuffer
    {
    public:
        ElasticBuffer(size_t initialSize = 0, double growthRate = 2)
            : currentCapacity(initialSize), growthRate(growthRate)
        {
            if (growthRate <= 1.0)
            {
                throw std::invalid_argument("Growth rate must be greater than 1.0.");
            }
            if (initialSize > 0)
            {
                buffer.reserve(initialSize);
            }
        }

        void ensureCapacity(size_t requiredSize)
        {
            if (!requiredSize)
            {
                return;
            }

            if (!buffer.capacity())
            {
                currentCapacity = requiredSize * 2;
                buffer.reserve(currentCapacity);
            }

            if (requiredSize > currentCapacity)
            {
                while (currentCapacity < requiredSize)
                {
                    currentCapacity = static_cast<size_t>(std::ceil(currentCapacity * growthRate));
                }
                buffer.reserve(currentCapacity);
            }
            buffer.resize(requiredSize);
        }

        T *getBuffer()
        {
            return buffer.data();
        }

    private:
        std::vector<T> buffer;
        size_t currentCapacity;
        double growthRate;
    };
}  // namespace Ida
