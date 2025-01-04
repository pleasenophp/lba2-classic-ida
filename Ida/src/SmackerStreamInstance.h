#pragma once

#include "SmackerStream.h"
#include "ElasticBuffer.h"

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

		ElasticBuffer<float> mStereoBuffer;

	public:
		SmackerStreamInstance(SmackerStream *parent);
		virtual ~SmackerStreamInstance() = default;
		void stop();
		virtual unsigned int getAudio(float *buffer, unsigned int samplesToRead, unsigned int bufferSize);
		virtual result rewind();
		virtual bool hasEnded();
	};
}
