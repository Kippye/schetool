#include "preferences_io.h"
#include <format>

namespace fs = std::filesystem;

PreferencesIO::PreferencesIO(fs::path dataDir) : m_dataDir(dataDir) {
    m_converter = PreferencesDataConverter();
    m_converter.setupObjectTable();
}

fs::path PreferencesIO::getPreferencesFilePath() const {
    return m_dataDir / ("preferences" + m_converter.getExtension());
}

Preferences PreferencesIO::getCurrentPreferences() const {
    return m_preferences;
}

void PreferencesIO::setPreferences(const Preferences& preferences) {
    m_preferences = preferences;
    preferencesChangedEvent.invoke(m_preferences);
    writePreferences();
}

bool PreferencesIO::writePreferences() {
    if (fs::exists(m_dataDir) == false) {
        if (fs::create_directories(m_dataDir)) {
            std::cout
                << std::format(
                       "PreferencesIO::writePreferences(): Data directory did not exist. Created a data directory at: '{}'",
                       m_dataDir.string())
                << std::endl;
        } else {
            std::cout
                << std::format(
                       "PreferencesIO::writePreferences(): Data directory did not exist. FAILED to create a data directory at: '{}'",
                       m_dataDir.string())
                << std::endl;
            return false;
        }
    }

    return m_converter.writePreferences(getPreferencesFilePath().string().c_str(), m_preferences) == 0;
}

bool PreferencesIO::readPreferences() {
    fs::path preferencesFilePath = getPreferencesFilePath();
    if (fs::exists(preferencesFilePath) == false) {
        std::cout << std::format(
                         "PreferencesIO::readPreferences(): Tried to read preferences at path to non-existant file: '{}'",
                         preferencesFilePath.string())
                  << std::endl;
        return false;
    }
    if (m_converter.isValidPreferencesFile(preferencesFilePath.string().c_str()) == false) {
        std::cout << std::format("PreferencesIO::readPreferences(): Tried to read invalid preferences file at path: '{}'",
                                 preferencesFilePath.string())
                  << std::endl;
        return false;
    }

    auto loadedPreferences = m_converter.readPreferences(preferencesFilePath.string().c_str());
    if (loadedPreferences.has_value()) {
        m_preferences = loadedPreferences.value();
        preferencesChangedEvent.invoke(m_preferences);
        preferencesLoadedEvent.invoke(m_preferences);
        return true;
    } else {
        return false;
    }
}