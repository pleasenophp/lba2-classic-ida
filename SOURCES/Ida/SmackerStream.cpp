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

		// TODO - support user amount of channels 
		mChannels = 1;

		mBaseSamplerate = sampleRate;
		this->setSingleInstance(1);
	}

	SmackerStream::~SmackerStream() {
		if (mInstance) {
			((SmackerStreamInstance*)mInstance)->stop();
		}
		delete mSampleBuffer;
	}

	void SmackerStream::allocateSampleBuffer(unsigned int numberOfSamples) {
		if (numberOfSamples > mSampleBufferSize || !mSampleBuffer) {
			mSampleBufferSize = numberOfSamples << 1;
			delete mSampleBuffer;
			mSampleBuffer = new float[mSampleBufferSize];
		}
	}

	void SmackerStream::addNextChunk(const unsigned char *buffer, unsigned int bufferSize)
	{
		// sampleCount = bufferSize / 2 if bit depth is 16 bit and bufferSize if it's 8 bit
		int sampleCount = bufferSize >> (mBitDepth >> 4);

		this->allocateSampleBuffer(sampleCount);

		if (mBitDepth == 16)
		{
			// Convert 16-bit samples (signed short) to float
			const short* shortBuffer = reinterpret_cast<const short*>(buffer);
			for (unsigned int i = 0; i < sampleCount; ++i)
			{
				mSampleBuffer[i] = shortBuffer[i] / 32768.0f;
			}
		}
		else if (mBitDepth == 8)
		{
			// Convert 8-bit samples (unsigned char) to float
			for (unsigned int i = 0; i < sampleCount; ++i)
			{
				mSampleBuffer[i] = ((signed)buffer[i] - 128) / 128.0f;
			}
		}

		lock_guard<mutex> lock(mMutex);
		mBuffer.insert(mBuffer.end(), mSampleBuffer, mSampleBuffer + sampleCount);
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

