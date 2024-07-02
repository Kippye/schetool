#include "autosave_popup_gui.h"

void AutosavePopupGui::draw(Window& window, Input& input)
{
	if (ImGui::BeginPopupModal("Autosave detected", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
	{
        const ImVec2 buttonSize = ImVec2(160.0f, 0.0f);
        if (ImGui::Button("Open autosave", buttonSize))
        {
            openAutosaveEvent.invoke();
            ImGui::CloseCurrentPopup();
        }
        if (ImGui::BeginItemTooltip())
        {
            ImGui::Text("Note: You should only use the autosave this way temporarily, then open the actual file again.");
            ImGui::EndTooltip();
        }
        ImGui::SameLine();
        if (ImGui::Button("Copy to and open file", buttonSize))
        {
            applyAutosaveOpenFileEvent.invoke();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Ignore and open file", buttonSize))
        {
            ignoreAutosaveOpenFileEvent.invoke();
            ImGui::CloseCurrentPopup();
        }
		ImGui::EndPopup();
	}

    if (m_openNextFrame)
    {
        ImGui::OpenPopup("Autosave detected");
        m_openNextFrame = false;
    }
}

void AutosavePopupGui::open()
{
    m_openNextFrame = true;
}