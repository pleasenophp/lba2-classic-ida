#pragma once

#include <mutex>
#include <deque>

#pragma pack(8)
#include "soloud.h"
#pragma pack(1)

using namespace SoLoud;
using namespace std;

namespace Ida
{
	class SmackerStream : public AudioSource
	{
		unsigned char mBitDepth;

		AudioSourceInstance *mInstance = nullptr;

		mutex mMutex;
		deque<float> mBuffer;

		float *mSampleBuffer = nullptr;
		unsigned int mSampleBufferSize = 0;

	public:
		SmackerStream(unsigned char bitDepth, float sampleRate, unsigned char numChannels);
		virtual ~SmackerStream();
		void addNextChunk(const unsigned char *buffer, unsigned int bufferSize);
		unsigned int readNext(float *buffer, unsigned int numberOfSamples);
		virtual AudioSourceInstance *createInstance();

		static void AllocateSampleBuffer(float **buffer, unsigned int *currentBufferSize, unsigned int numberOfSamples);
	};
};
