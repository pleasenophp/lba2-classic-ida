#pragma once

#include <deque>
#include <mutex>

#include "ElasticBuffer.h"
#include "soloud.h"

namespace Ida
{
    class SmackerStream : public SoLoud::AudioSource
    {
        unsigned char mBitDepth;

        SoLoud::AudioSourceInstance *mInstance = nullptr;

        std::mutex mMutex;
        std::deque<float> mBuffer;
        ElasticBuffer<float> mSampleBuffer;

    public:
        SmackerStream(unsigned char bitDepth, float sampleRate, unsigned char numChannels);
        virtual ~SmackerStream();
        void addNextChunk(const unsigned char *buffer, unsigned int bufferSize);
        unsigned int readNext(float *buffer, unsigned int numberOfSamples);
        virtual SoLoud::AudioSourceInstance *createInstance();
    };
};  // namespace Ida
