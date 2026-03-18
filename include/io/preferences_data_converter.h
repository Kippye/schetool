#pragma once

#include "blf/include/blf.hpp"
#include "blf_base_types.h"
#include "preferences.h"

struct BLF_Preferences : BLF_Base {
        static constexpr std::string getName() {
            return "BLF_Preferences";
        }

        bool notificationsEnabled;
        bool notificationSoundEnabled;
        std::string styleName;
        int fontSize;
        bool rowHighlightingEnabled;

        BLF_Preferences()
            : notificationsEnabled(false),
              notificationSoundEnabled(false),
              styleName(""),
              fontSize(0),
              rowHighlightingEnabled(false) {
        }
        BLF_Preferences(const Preferences& preferences)
            : notificationsEnabled(preferences.getNotificationsEnabled()),
              notificationSoundEnabled(preferences.getNotificationSoundEnabled()),
              styleName(preferences.getStyle().name),
              fontSize((int)preferences.getFontSize()),
              rowHighlightingEnabled(preferences.getRowHighlightingEnabled()) {
        }

        Preferences getPreferences() const {
            return Preferences(notificationsEnabled,
                               notificationSoundEnabled,
                               InterfaceStyleHandler::getStyle(styleName).value_or(InterfaceStyleHandler::getDefaultStyle()),
                               (FontSize)fontSize,
                               rowHighlightingEnabled);
        }

        static void addDefinition(ObjectDefinitions& definitions) {
            definitions.add(definitions.getObjectTable().define<BLF_Preferences>(
                getName(),
                blf::arg("notificationsEnabled", &BLF_Preferences::notificationsEnabled),
                blf::arg("notificationSoundEnabled", &BLF_Preferences::notificationSoundEnabled),
                blf::arg("style", &BLF_Preferences::styleName),
                blf::arg("fontSize", &BLF_Preferences::fontSize),
                blf::arg("rowHighlightingEnabled", &BLF_Preferences::rowHighlightingEnabled)));
        }
};

class PreferencesDataConverter {
    private:
        std::string m_extension = ".blf";
        ObjectDefinitions m_definitions;

        template <DerivedBlfBase BlfClass>
        void addObjectDefinition() {
            BlfClass::addDefinition(m_definitions);
        }
        template <typename T>
        const blf::StaticDefinition<T>& getObjectDefinition() {
            return m_definitions.get<T>();
        }

        void setupObjectTable();

    public:
        PreferencesDataConverter();
        // Get the file extension used by the ScheduleDataConverter.
        const std::string& getExtension() const;

        bool isValidPreferencesFile(const char* path) const;
        // Write the current Preferences to a file at the given path.
        int writePreferences(const char* path, const Preferences& preferences);
        // Read a preferences file from the given path, returning a Preferences file if successful.
        std::optional<Preferences> readPreferences(const char* path);
};