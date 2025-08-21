#pragma once

#include "gui.h"
#include <string_view>
#include <format>

class ConfirmationModalSubGui : public Gui {
    private:
        std::string m_eventArg = "";
        std::string m_title = "Confirm action";
        std::string_view m_bodyFormat = "Are you sure?";
        std::string m_bodyText = "Are you sure?";
        std::string m_confirmButtonText = "Confirm";
        std::string m_cancelButtonText = "Cancel";

    public:
        // title - The title of the modal popup.
        // bodyFormat - A string format for the body text. Any {} will be filled with the event arg string.
        // confirmButtonText - The text in the confirm button.
        // cancelButtonText - The text in the cancel button.
        ConfirmationModalSubGui(const char* ID,
                                const std::string& title,
                                const std::string_view& bodyFormat,
                                const std::string& confirmButtonText = "Confirm",
                                const std::string& cancelButtonText = "Cancel")
            : Gui(ID),
              m_title(title),
              m_bodyFormat(bodyFormat),
              m_confirmButtonText(confirmButtonText),
              m_cancelButtonText(cancelButtonText) {
            m_bodyText = std::vformat(m_bodyFormat, std::make_format_args(m_eventArg));
        }

        Event<std::string> appliedEvent;
        Event<std::string> cancelledEvent;

        void draw(const WindowSize& windowSize, Input& input, GuiTextures& guiTextures) override;
        // Just call ImGui::OpenPopup with the ID of this popup.
        // Optionally, pass the string that is passed to events and used in the bodyFormat here.
        void open(const std::string& eventArg = "");
};