#pragma once

#include "gui.h"

class TextInputModalSubGui : public Gui {
    private:
        std::string m_textBuffer = "";
        size_t m_textMaxLength;
        const char* m_popupName;
        std::string m_acceptButtonText;
        bool m_showCloseButton;
        bool m_focusInput = false;

    public:
        Event<std::string> acceptButtonPressedEvent;

        TextInputModalSubGui(const char* ID,
                             const char* popupName = "Text input",
                             const char* acceptButtonText = "Accept",
                             size_t textMaxLength = 48,
                             bool showCloseButton = true);

        void draw(Window& window, Input& input, GuiTextures& guiTextures) override;
        void open(const std::string& fillText = "");
        virtual void invokeEvent(const std::string& text);
};