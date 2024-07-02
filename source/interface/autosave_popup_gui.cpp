#include "autosave_popup_gui.h"

void AutosavePopupGui::draw(Window& window, Input& input)
{
	if (ImGui::BeginPopupModal("Autosave detected", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
	{
        if (ImGui::BeginItemTooltip())
        {
            ImGui::Text("Note: You should only use the autosave this way temporarily, then open the actual file again.");
            ImGui::EndTooltip();
        }
        if (ImGui::Button("Open autosave"))
        {
            openAutosaveEvent.invoke();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Copy to file and open"))
        {
            applyAutosaveOpenFileEvent.invoke();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Ignore autosave, open file"))
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