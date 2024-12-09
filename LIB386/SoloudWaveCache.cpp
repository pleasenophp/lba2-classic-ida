#include "SoloudWaveCache.h"
#include <stdexcept>

SoloudWaveCache::~SoloudWaveCache() {
    clearCache();
}

SoLoud::Wav* SoloudWaveCache::getWave(const char* streamPathName) {
    if (!streamPathName) {
        throw std::invalid_argument("StreamPathName cannot be null.");
    }

    std::string pathKey(streamPathName);

    auto it = cache.find(pathKey);
    if (it != cache.end()) {
        return it->second; 
    }

    SoLoud::Wav* newWave = new SoLoud::Wav();

    auto errorCode = newWave->load(streamPathName);
    if (errorCode != SoLoud::SO_NO_ERROR) {
        delete newWave;
        throw std::runtime_error("Failed to load audio from: " + pathKey + "; [" + std::to_string(errorCode) + "]");
    }

    cache[pathKey] = newWave;

    return newWave;
}

void SoloudWaveCache::clearCache() {
    for (auto& pair : cache) {
        delete pair.second;
    }
    cache.clear();
}

