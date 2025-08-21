#pragma once

#include "blf/include/blf.hpp"
#include "blf_base_types.h"
#include "preferences.h"

struct BLF_Preferences : BLF_Base {
        static constexpr std::string getName() {
            return "BLF_Preferences";
        }

        std::string styleName;
        int fontSize;
        bool notificationsEnabled;
        bool notificationSoundEnabled;

        BLF_Preferences() : styleName(""), fontSize(0), notificationsEnabled(false), notificationSoundEnabled(false) {
        }
        BLF_Preferences(const Preferences& preferences)
            : styleName(preferences.getStyle().name),
              fontSize((int)preferences.getFontSize()),
              notificationsEnabled(preferences.getNotificationsEnabled()),
              notificationSoundEnabled(preferences.getNotificationSoundEnabled()) {
        }

        Preferences getPreferences() const {
            return Preferences(InterfaceStyleHandler::getStyle(styleName).value_or(InterfaceStyleHandler::getDefaultStyle()),
                               (FontSize)fontSize,
                               notificationsEnabled,
                               notificationSoundEnabled);
        }

        static void addDefinition(ObjectDefinitions& definitions) {
            definitions.add(definitions.getObjectTable().define<BLF_Preferences>(
                getName(),
                blf::arg("style", &BLF_Preferences::styleName),
                blf::arg("fontSize", &BLF_Preferences::fontSize),
                blf::arg("notificationsEnabled", &BLF_Preferences::notificationsEnabled),
                blf::arg("notificationSoundEnabled", &BLF_Preferences::notificationSoundEnabled)));
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