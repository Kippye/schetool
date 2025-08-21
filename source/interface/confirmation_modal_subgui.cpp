#include "confirmation_modal_subgui.h"

void ConfirmationModalSubGui::draw(const WindowSize& windowSize, Input& input, GuiTextures& guiTextures) {
    ImGui::SetNextWindowSize(ImVec2(386.0f, 100.0f));
    if (ImGui::BeginPopupModal(m_title.c_str(), NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
        ImGuiStyle& style = ImGui::GetStyle();
        ImGui::Text("%s", m_bodyText.c_str());
        ImGui::NewLine();
        float size = 120.0f + style.FramePadding.x * 2.0f;
        float avail = ImGui::GetContentRegionAvail().x;
        float off = (avail - size) * 0.23f;
        if (off > 0.0f) {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
        }
        if (ImGui::Button(m_confirmButtonText.c_str(), ImVec2(120, 0))) {
            appliedEvent.invoke(m_eventArg);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        // janky offset but it works so i don't much care
        off = (avail - size) * 0.04f;
        if (off > 0.0f) {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
        }
        if (ImGui::Button(m_cancelButtonText.c_str(), ImVec2(120, 0))) {
            cancelledEvent.invoke(m_eventArg);
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void ConfirmationModalSubGui::open(const std::string& eventArg) {
    m_eventArg = eventArg;
    m_bodyText = std::vformat(m_bodyFormat, std::make_format_args(m_eventArg));
    ImGui::OpenPopup(m_title.c_str());
}