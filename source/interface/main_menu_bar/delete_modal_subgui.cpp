#include "main_menu_bar/delete_modal_subgui.h"

void DeleteModalSubGui::draw(Window& window, Input& input, GuiTextures& guiTextures) {
    ImGui::SetNextWindowSize(ImVec2(386.0f, 100.0f));
    if (ImGui::BeginPopupModal("Confirm Schedule deletion", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
        ImGuiStyle& style = ImGui::GetStyle();
        ImGui::Text("%s %s?", "Delete", m_deleteConfirmationScheduleName.c_str());
        ImGui::NewLine();
        float size = 120.0f + style.FramePadding.x * 2.0f;
        float avail = ImGui::GetContentRegionAvail().x;
        float off = (avail - size) * 0.23f;
        if (off > 0.0f) {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
        }
        if (ImGui::Button("Delete##DeleteSchedule", ImVec2(120, 0))) {
            deleteScheduleEvent.invoke(std::string(m_deleteConfirmationScheduleName));

            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        // janky offset but it works so i don't much care
        off = (avail - size) * 0.04f;
        if (off > 0.0f) {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
        }
        if (ImGui::Button("Cancel##CancelScheduleDeletion", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void DeleteModalSubGui::setAffectedScheduleName(const std::string& name) {
    m_deleteConfirmationScheduleName = name;
}