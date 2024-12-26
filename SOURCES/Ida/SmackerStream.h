#pragma once

#include <mutex>

#pragma pack(8)
#include "soloud.h"
#pragma pack(1)

using namespace SoLoud;

namespace Ida
{
	class SmackerStream : public AudioSource
	{
		unsigned char mBitDepth;

		float *mCurrentChunk = nullptr;
		unsigned int mCurrentChunkSize = 0;
		unsigned int mCurrentPosition = 0;

		std::mutex mMutex;

	public:
		SmackerStream(unsigned char bitDepth, float sampleRate);
		virtual ~SmackerStream() = default;
		void addNextChunk(const unsigned char *buffer, unsigned int sampleCount);
		result readNext(float *buffer, unsigned int numberOfSamples);
		virtual AudioSourceInstance *createInstance();
	};
};

