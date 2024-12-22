#include "SmackerStreamInstance.h"

using namespace std;

namespace Ida
{

	SmackerStreamInstance::SmackerStreamInstance(SmackerStream* parent)
	{
		mParent = parent;
	}

	unsigned int SmackerStreamInstance::getAudio(float* buffer, unsigned int samplesToRead, unsigned int bufferSize)
	{
		unsigned int readSamples = mParent->readNext(buffer, samplesToRead);
		memset(buffer + readSamples, 0, (bufferSize - readSamples) * sizeof(float));

		return readSamples;
	}

	result SmackerStreamInstance::rewind()
	{
		return 0;
	}

	bool SmackerStreamInstance::hasEnded()
	{
		// TODO - see if we need to return true here in the end
		return false;
	}
}

