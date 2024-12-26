#include <string>
#include "SmackerStream.h"
#include "SmackerStreamInstance.h"

using namespace std;

namespace Ida {

	SmackerStream::SmackerStream(unsigned char bitDepth, float sampleRate) : mBitDepth(bitDepth)
	{
		if (bitDepth != 16 && bitDepth != 8)
		{
			throw invalid_argument("Unsupported bit rate: "+to_string(bitDepth));
		}

		mBaseSamplerate = sampleRate;
	}

	/*
	SmackerStream::~SmackerStream() {

	}
	*/

	void SmackerStream::addNextChunk(const unsigned char* buffer, unsigned int sampleCount)
	{
		lock_guard<mutex> lock(mMutex);

		delete mCurrentChunk;
		mCurrentChunk = new float[sampleCount];
		mCurrentChunkSize = sampleCount;

		if (mBitDepth == 16)
		{
			// Convert 16-bit samples (signed short) to float
			const short* shortBuffer = reinterpret_cast<const short*>(buffer);
			for (unsigned int i = 0; i < sampleCount; ++i)
			{
				mCurrentChunk[i] = shortBuffer[i] / 32768.0f;
			}
		}
		else if (mBitDepth == 8)
		{
			// Convert 8-bit samples (unsigned char) to float
			for (unsigned int i = 0; i < sampleCount; ++i)
			{
				mCurrentChunk[i] = ((signed)buffer[i] - 128) / 128.0f;
			}
		}

		mCurrentPosition = 0;
	}

	result SmackerStream::readNext(float *buffer, unsigned int numberOfSamples)
	{
		lock_guard<mutex> lock(mMutex);

		if (!mCurrentChunk || !mCurrentChunkSize)
			return 0;

		unsigned int samplesToCopy = min(numberOfSamples, mCurrentChunkSize - mCurrentPosition);
		if (!samplesToCopy)
			return 0;

		memcpy(buffer, mCurrentChunk + mCurrentPosition, samplesToCopy * sizeof(float));
		mCurrentPosition += samplesToCopy;

		return samplesToCopy;
	}

	AudioSourceInstance *SmackerStream::createInstance()
	{
		return new SmackerStreamInstance(this);
	}

}

