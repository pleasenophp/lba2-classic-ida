#include <string>
#include "SmackerStream.h"
#include "SmackerStreamInstance.h"

using namespace std;

namespace Ida
{
	SmackerStream::SmackerStream(unsigned char bitDepth, float sampleRate, unsigned char numChannels) : mBitDepth(bitDepth)
	{
		if (bitDepth != 16 && bitDepth != 8)
		{
			throw invalid_argument("Unsupported bit rate: " + to_string(bitDepth));
		}

		if (!numChannels || numChannels > 2)
		{
			throw invalid_argument("Unsupported number of channels: " + to_string(numChannels));
		}

		mChannels = numChannels;
		mBaseSamplerate = sampleRate;
		this->setSingleInstance(1);
	}

	SmackerStream::~SmackerStream()
	{
		if (mInstance)
		{
			((SmackerStreamInstance *)mInstance)->stop();
		}
	}

	void SmackerStream::addNextChunk(const unsigned char *buffer, unsigned int bufferSize)
	{
		// sampleCount = bufferSize / 2 if bit depth is 16 bit and bufferSize if it's 8 bit
		auto sampleCount = bufferSize >> (mBitDepth >> 4);

		// mSampleBuffer.ensureCapacity(sampleCount);
		// float *sampleBuffer = mSampleBuffer.getBuffer();
		float* sampleBuffer = new float[sampleCount];

		if (mBitDepth == 16)
		{
			// Convert 16-bit samples (signed short) to float
			auto shortBuffer = reinterpret_cast<const short *>(buffer);
			for (unsigned int i = 0; i < sampleCount; ++i)
			{
				sampleBuffer[i] = shortBuffer[i] / 32768.0f;
			}
		}
		else if (mBitDepth == 8)
		{
			// Convert 8-bit samples (unsigned char) to float
			for (unsigned int i = 0; i < sampleCount; ++i)
			{
				sampleBuffer[i] = ((signed)buffer[i] - 128) / 128.0f;
			}
		}

		lock_guard<mutex> lock(mMutex);
		mBuffer.insert(mBuffer.end(), sampleBuffer, sampleBuffer + sampleCount);

		delete[] sampleBuffer;
	}

	unsigned int SmackerStream::readNext(float *buffer, unsigned int numberOfSamples)
	{
		lock_guard<mutex> lock(mMutex);

		if (mBuffer.empty())
			return 0;

		unsigned int samplesToRead = min(numberOfSamples, mBuffer.size());
		copy(mBuffer.begin(), mBuffer.begin() + numberOfSamples, buffer);

		mBuffer.erase(mBuffer.begin(), mBuffer.begin() + samplesToRead);

		return samplesToRead;
	}

	AudioSourceInstance *SmackerStream::createInstance()
	{
		mInstance = new SmackerStreamInstance(this);
		return mInstance;
	}
}
