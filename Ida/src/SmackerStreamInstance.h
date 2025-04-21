#pragma once

#include "ElasticBuffer.h"
#include "SmackerStream.h"
#include "soloud.h"

namespace Ida
{
    class SmackerStreamInstance : public SoLoud::AudioSourceInstance
    {
        SmackerStream *mParent;
        bool mHasEnded = false;

        ElasticBuffer<float> mStereoBuffer;

    public:
        SmackerStreamInstance(SmackerStream *parent);
        virtual ~SmackerStreamInstance() = default;
        void stop();
        virtual unsigned int getAudio(float *buffer, unsigned int samplesToRead, unsigned int bufferSize);
        virtual SoLoud::result rewind();
        virtual bool hasEnded();
    };
}  // namespace Ida
