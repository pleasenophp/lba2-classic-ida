#include <cmath>
#include <iostream>
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

		cout << "Requested " << samplesToRead << " samples, and gave " << readSamples << "; Thread: " << this_thread::get_id() << "\n";

		return readSamples;

		/*
		cout << "Samples to read: " << samplesToRead << "; Buffer size: " << bufferSize << "\n";

		for (unsigned int i = 0; i < samplesToRead; ++i)
		{
			float time = static_cast<float>(++mCounter) / 22050.0f;
			buffer[i] = 0.8f * std::sin(2.0f * M_PI * 220.f * time);
		}
		
		if (bufferSize > samplesToRead) 
			memset(buffer + samplesToRead, 0, (bufferSize - samplesToRead) * sizeof(float));

		return samplesToRead;
		*/
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

