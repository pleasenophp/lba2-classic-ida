#pragma once

#include "SmackerStream.h"

#pragma pack(8)
#include "soloud.h"
#pragma pack(1)

using namespace SoLoud;

namespace Ida
{
	class SmackerStreamInstance : public AudioSourceInstance
	{
		SmackerStream *mParent;
		bool mHasEnded = false;
		int mCounter;

		float *mStereoBuffer = nullptr;
		unsigned int mStereoBufferSize = 0;
	public:
		SmackerStreamInstance(SmackerStream* parent);
		virtual ~SmackerStreamInstance() 
		{
			delete[] mStereoBuffer;
		}
		void stop();
		virtual unsigned int getAudio(float *buffer, unsigned int samplesToRead, unsigned int bufferSize);
		virtual result rewind();
		virtual bool hasEnded();
	};
}

