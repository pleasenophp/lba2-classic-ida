#include <string>
#include <iostream>
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

		// TODO - support user amount of channels 
		mChannels = 1;

		mBaseSamplerate = sampleRate;
		this->setSingleInstance(1);
	}

	SmackerStream::~SmackerStream() {
		if (mInstance) {
			((SmackerStreamInstance*)mInstance)->stop();
		}
		delete mCurrentChunk;
	}

	void SmackerStream::addNextChunk(const unsigned char* buffer, unsigned int bufferSize)
	{
		lock_guard<mutex> lock(mMutex);

		// sampleCount = bufferSize / 2 if bit depth is 16 bit and bufferSize if it's 8 bit
		int sampleCount = bufferSize >> (mBitDepth >> 4);

		delete mCurrentChunk;
		mCurrentChunk = new float[sampleCount];
		mCurrentChunkSize = sampleCount;

		cout << "ADD NEXT CHUNK " << sampleCount << "\n";

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

		cout << "Requested " << numberOfSamples << "; Current size: " << mCurrentChunkSize << "; currentPosition: " << mCurrentPosition << "; samples to copy: " << samplesToCopy << "\n";

		if (samplesToCopy <= 0) 
			return 0;

		memcpy(buffer, mCurrentChunk + mCurrentPosition, samplesToCopy * sizeof(float));
		mCurrentPosition += samplesToCopy;

		return samplesToCopy;
	}

	AudioSourceInstance *SmackerStream::createInstance()
	{
		mInstance = new SmackerStreamInstance(this);
		return mInstance;
	}

}

