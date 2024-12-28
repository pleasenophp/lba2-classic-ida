#include <assert.h>
#include "SmackerStreamInstance.h"

using namespace std;

namespace Ida
{
	SmackerStreamInstance::SmackerStreamInstance(SmackerStream* parent)
	{
		mParent = parent;
		mCounter = 0;
	}

	unsigned int SmackerStreamInstance::getAudio(float *buffer, unsigned int samplesToRead, unsigned int bufferSize)
	{
		if (mChannels == 1) {

			unsigned int readSamples = mParent->readNext(buffer, samplesToRead);

			if (bufferSize > readSamples)
			{
				memset(buffer + readSamples, 0, (bufferSize - readSamples) * sizeof(float));
			}

			return readSamples;
		}
		else if (mChannels == 2) 
		{
			// For the stereo sound SMK audio has sequence of the samples L0, R0, L1, R1, L2, R2, ... LN, RN
			// The SoLoud requires to give them double as many samples as requested, but in this order: L0, L1, L2, ... LN, R0, R1, R2, ... RN

			samplesToRead <<= 1;

			// TODO - pre-allocate
			float *tempBuffer = new float[samplesToRead];

			auto readSamples = mParent->readNext(tempBuffer, samplesToRead);

			if (!readSamples) 
			{
				delete[] tempBuffer;
				return 0;
			}


			assert(readSamples % 2 == 0);
			auto channelSamples = readSamples >> 1;
			auto leftChannel = buffer;
			auto rightChannel = buffer + channelSamples;

			for (unsigned int i = 0; i < channelSamples; ++i) 
			{
				leftChannel[i] = *(tempBuffer + i * 2);
				rightChannel[i] = *(tempBuffer + i * 2 + 1);
			}

			delete[] tempBuffer;

			if (bufferSize > readSamples)
			{
				memset(buffer + readSamples, 0, (bufferSize - readSamples) * sizeof(float));
			}

			return channelSamples;
		}

		return 0;
	}

	result SmackerStreamInstance::rewind()
	{
		return 0;
	}

	bool SmackerStreamInstance::hasEnded()
	{
		return mHasEnded;
	}

	void SmackerStreamInstance::stop() 
	{
		mHasEnded = true;
	}
}

