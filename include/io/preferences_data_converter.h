#pragma once

#include "blf/include/blf.hpp"
#include "blf_base_types.h"
#include "preferences.h"

struct BLF_Preferences : BLF_Base {
        static constexpr std::string getName() {
            return "BLF_Preferences";
        }

        int style;
        int fontSize;
        bool notificationsEnabled;

        BLF_Preferences() : style(0), fontSize(0), notificationsEnabled(false) {
        }
        BLF_Preferences(const Preferences& preferences)
            : style((int)preferences.getStyle()),
              fontSize((int)preferences.getFontSize()),
              notificationsEnabled(preferences.getNotificationsEnabled()) {
        }

        Preferences getPreferences() const {
            return Preferences((GuiStyle)style, (FontSize)fontSize, notificationsEnabled);
        }

        static void addDefinition(ObjectDefinitions& definitions) {
            definitions.add(definitions.getObjectTable().define<BLF_Preferences>(
                getName(),
                blf::arg("style", &BLF_Preferences::style),
                blf::arg("fontSize", &BLF_Preferences::fontSize),
                blf::arg("notificationsEnabled", &BLF_Preferences::notificationsEnabled)));
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
        const blf::LocalObjectDefinition<T>& getObjectDefinition() {
            return m_definitions.get<T>();
        }

    public:
        // Get the file extension used by the ScheduleDataConverter.
        const std::string& getExtension() const;
        void setupObjectTable();

        bool isValidPreferencesFile(const char* path) const;
        // Write the current Preferences to a file at the given path.
        int writePreferences(const char* path, const Preferences& preferences);
        // Read a preferences file from the given path, returning a Preferences file if successful.
        std::optional<Preferences> readPreferences(const char* path);
};