#define MINIAUDIO_IMPLEMENTATION
#include "audio.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

AudioEngine::AudioEngine() {
    ma_result result = ma_engine_init(NULL, &m_engine);

    m_isInit = result == MA_SUCCESS;

    if (m_isInit == false) {
        std::cerr << "Failed to initialize audio engine." << std::endl;
    }
}

AudioEngine::~AudioEngine() {
    if (m_isInit) {
        ma_engine_uninit(&m_engine);
    }
}

bool AudioEngine::getIsInitialized() const {
    return m_isInit;
}

void AudioEngine::playSound(const char* filename) {
    if (!m_isInit) {
        return;
    }

    std::string fullRelativePath = soundsFolder + filename;

    // Try to load the sound with each of the supported extensions
    for (size_t i = 0; i < std::size(m_supportedExtensions); i++) {
        std::string relativePathWithExt = soundsFolder + filename + m_supportedExtensions[i];
        if (fs::exists(fs::path(relativePathWithExt))) {
            fullRelativePath = relativePathWithExt;
            break;
        }
    }

    ma_result result = ma_engine_play_sound(&m_engine, fullRelativePath.c_str(), NULL);
}