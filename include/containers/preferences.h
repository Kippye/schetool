#pragma once

#include "gui_constants.h"
#include "interface_style.h"

// Contains values for every user preference used in the program.
class Preferences {
    private:
        GuiStyle m_style;
        FontSize m_fontSize;
        bool m_notificationsEnabled;

    public:
        Preferences() = delete;
        // Create a specific set of preferences.
        // To get a default set, use the static getDefault() function instead.
        Preferences(GuiStyle style, FontSize fontSize, bool notificationsEnabled) {
            m_style = style;
            m_fontSize = fontSize;
            m_notificationsEnabled = notificationsEnabled;
        }

        // Get a Preferences instance containing default values for every preference.
        static Preferences getDefault() {
            return Preferences(InterfaceStyleHandler::getDefaultStyle(), InterfaceStyleHandler::getDefaultFontSize(), true);
        }

        GuiStyle getStyle() const {
            return m_style;
        }
        FontSize getFontSize() const {
            return m_fontSize;
        }
        bool getNotificationsEnabled() const {
            return m_notificationsEnabled;
        }

        void setStyle(GuiStyle style) {
            m_style = style;
        }
        void setFontSize(FontSize fontSize) {
            m_fontSize = fontSize;
        }
        void setNotificationsEnabled(bool enabled) {
            m_notificationsEnabled = enabled;
        }
};