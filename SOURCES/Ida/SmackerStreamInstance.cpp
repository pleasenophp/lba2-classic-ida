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
		unsigned int readSamples = mParent->readNext(buffer, samplesToRead);

		if (bufferSize > readSamples)
		{
			memset(buffer + readSamples, 0, (bufferSize - readSamples) * sizeof(float));
		}

		return readSamples;
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

