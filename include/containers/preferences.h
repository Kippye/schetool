#pragma once

#include "gui_constants.h"
#include "interface_style.h"

// Contains values for every user preference used in the program.
class Preferences {
    private:
        bool m_notificationsEnabled;
        bool m_notificationSoundEnabled;
        GuiStyleDefinition m_style;
        FontSize m_fontSize;
        bool m_rowHighlightingEnabled;

    public:
        Preferences() = delete;
        // Create a specific set of preferences.
        // To get a default set, use the static getDefault() function instead.
        Preferences(bool notificationsEnabled,
                    bool notificationSoundEnabled,
                    const GuiStyleDefinition& style,
                    FontSize fontSize,
                    bool rowHighlightingEnabled) {
            m_style = style;
            m_fontSize = fontSize;
            m_notificationsEnabled = notificationsEnabled;
            m_notificationSoundEnabled = notificationSoundEnabled;
            m_rowHighlightingEnabled = rowHighlightingEnabled;
        }

        // Get a Preferences instance containing default values for every preference.
        static Preferences getDefault() {
            return Preferences(
                true, true, InterfaceStyleHandler::getDefaultStyle(), InterfaceStyleHandler::getDefaultFontSize(), true);
        }

        bool getNotificationsEnabled() const {
            return m_notificationsEnabled;
        }
        bool getNotificationSoundEnabled() const {
            return m_notificationSoundEnabled;
        }
        GuiStyleDefinition getStyle() const {
            return m_style;
        }
        FontSize getFontSize() const {
            return m_fontSize;
        }
        bool getRowHighlightingEnabled() const {
            return m_rowHighlightingEnabled;
        }

        void setNotificationsEnabled(bool enabled) {
            m_notificationsEnabled = enabled;
        }
        void setNotificationSoundEnabled(bool enabled) {
            m_notificationSoundEnabled = enabled;
        }
        void setStyle(const GuiStyleDefinition& style) {
            m_style = style;
        }
        void setFontSize(FontSize fontSize) {
            m_fontSize = fontSize;
        }
        void setRowHighlightingEnabled(bool enabled) {
            m_rowHighlightingEnabled = enabled;
        }
};