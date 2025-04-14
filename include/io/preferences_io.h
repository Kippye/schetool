#pragma once

#include "preferences.h"
#include "preferences_data_converter.h"
#include "event.h"
#include <filesystem>

class IO_Handler;

class PreferencesIO {
        friend class IO_Handler;

    private:
        Preferences m_preferences = Preferences::getDefault();
        PreferencesDataConverter m_converter;
        std::filesystem::path m_dataDir = std::filesystem::path();

        std::filesystem::path getPreferencesFilePath() const;
        // NOTE: This should only be called from IO_Handler.
        // Hence why it's a private function.
        // Otherwise, this could easily cause a sort of event loop.
        void setPreferences(const Preferences& preferences);

    public:
        // Invoked when the current preferences are modified by anything.
        Event<Preferences> preferencesChangedEvent;
        // Invoked *only* when preferences are loaded from file.
        Event<Preferences> preferencesLoadedEvent;

        PreferencesIO(std::filesystem::path dataDir);

        Preferences getCurrentPreferences() const;
        // Read preferences from the current data directory.
        // The preferences are applied to the PreferenceHandler's current Preferences instance.
        // Return true if a file was found and read successfully.
        bool readPreferences();
        // Save the current preferences to a file in the current data directory.
        // Return true if the file was successfully written.
        bool writePreferences();
};