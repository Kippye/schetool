#include "text_input_modal_subgui.h"
#include "gui_templates.h"

TextInputModalSubGui::TextInputModalSubGui(
    const char* ID, const char* popupName, const char* acceptButtonText, size_t textMaxLength, bool showCloseButton)
    : Gui(ID) {
    m_popupName = popupName;
    m_acceptButtonText = acceptButtonText;
    m_textMaxLength = textMaxLength;
    m_showCloseButton = showCloseButton;
}

void TextInputModalSubGui::draw(Window& window, Input& input, GuiTextures& guiTextures) {
    float nameInputWidth = ImGui::CalcTextSize(std::string(m_textMaxLength + 2, 'a').c_str()).x;

    m_visible = true;

    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::SetNextWindowSize(ImVec2(nameInputWidth + style.FramePadding.x * 2.0f, 100));
    // ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), 0, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal(
            m_popupName, m_showCloseButton ? &m_visible : NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
    {
        m_textBuffer.reserve(m_textMaxLength);
        char* buf = m_textBuffer.data();
        ImGui::PushItemWidth(nameInputWidth - style.FramePadding.x * 2.0f);
        ImGui::InputText(std::string("##TextInput").append(m_popupName).c_str(),
                         buf,
                         m_textMaxLength,
                         ImGuiInputTextFlags_CallbackCharFilter,
                         gui_callbacks::filterAlphanumerics);
        ImGui::PopItemWidth();
        if (std::string(buf).empty() == false) {
            float size = 128.0f + style.FramePadding.x * 2.0f;
            float avail = ImGui::GetContentRegionAvail().x;
            float off = (avail - size) * 0.5f;
            if (off > 0.0f) {
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
            }
            if (ImGui::Button(m_acceptButtonText.c_str(), ImVec2(128, 0))) {
                invokeEvent(std::string(buf));
            }
        }
        ImGui::EndPopup();
    }
}

// Open the popup and empty the text buffer
void TextInputModalSubGui::open() {
    m_textBuffer = "";
    ImGui::OpenPopup(m_popupName);
}

void TextInputModalSubGui::invokeEvent(const std::string& text) {
    acceptButtonPressedEvent.invoke(text);
}