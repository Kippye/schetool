#include "miniaudio.h"
#include <string>

class AudioEngine {
    private:
        const std::string soundsFolder = "./sounds/";
        const std::string m_supportedExtensions[3] = {".wav", ".flac", ".mp3"};
        ma_engine m_engine;
        bool m_isInit = false;

    public:
        AudioEngine();
        ~AudioEngine();

        bool getIsInitialized() const;
        void playSound(const char* filename);
};