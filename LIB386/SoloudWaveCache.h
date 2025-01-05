#pragma once

#include <unordered_map>
#include <string>

#pragma pack(8)
#include "soloud.h"
#include "soloud_wav.h"
#pragma pack(1)

class SoloudWaveCache {
public:
    ~SoloudWaveCache();

    SoLoud::Wav* getWave(const char* streamPathName);

    // Clear all cached SoLoud::Wav objects
    void clearCache();

private:
    std::unordered_map<std::string, SoLoud::Wav*> cache; 
};

