#pragma once

#include <mutex>
#include <deque>
#include "ElasticBuffer.h"

#pragma pack(8)
#include "soloud.h"
#pragma pack(1)

using namespace SoLoud;
using namespace std;

namespace Ida
{
	class SmackerStream : public AudioSource
	{
		ElasticBuffer<float> mSampleBuffer;

		unsigned char mBitDepth;

		AudioSourceInstance *mInstance = nullptr;

		mutex mMutex;
		deque<float> mBuffer;

	public:
		SmackerStream(unsigned char bitDepth, float sampleRate, unsigned char numChannels);
		virtual ~SmackerStream();
		void addNextChunk(const unsigned char *buffer, unsigned int bufferSize);
		unsigned int readNext(float *buffer, unsigned int numberOfSamples);
		virtual AudioSourceInstance *createInstance();
	};
};

