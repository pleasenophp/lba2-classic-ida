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
		SmackerStream* mParent;
	public:
		SmackerStreamInstance(SmackerStream* parent);
		virtual unsigned int getAudio(float* buffer, unsigned int samplesToRead, unsigned int bufferSize);
		virtual result rewind();
		virtual bool hasEnded();
		virtual ~SmackerStreamInstance() = default;
	};
}

