#include "autosave_popup_gui.h"
#include "gui_templates.h"

void AutosavePopupGui::draw(Window& window, Input& input)
{
    ImVec2 popupSize = ImVec2(256.0f, 0.0f);
    ImGui::SetNextWindowContentSize(popupSize);
    ImGui::SetNextWindowPos(ImVec2(((float)window.SCREEN_WIDTH) / 2.0f, ((float)window.SCREEN_HEIGHT) / 2.0f), 0, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("Autosave found", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
	{
        const ImVec2 labelSize = ImVec2(128.0f, 0.0f);
        const ImVec2 buttonSize = ImVec2(128.0f, 0.0f);
        ImGui::BeginColumns("testColumns", 2, ImGuiOldColumnFlags_NoResize);
            // ImGui::SetColumnWidth(0, buttonSize.x);
            // ImGui::SetColumnWidth(1, buttonSize.x);
            gui_templates::TextWithBackground(labelSize, "Autosave file");
            gui_templates::TextWithBackground(labelSize, "%s", m_autosaveInfo.name.c_str());
            gui_templates::TextWithBackground(labelSize, "%s", m_autosaveInfo.editTimeString.c_str());
            if (ImGui::Button("Open autosave", buttonSize))
            {
                openAutosaveEvent.invoke();
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::BeginItemTooltip())
            {
                ImGui::Text("Note: Autosaves autosave to the same file.\nRename them and remove the suffix to change this.");
                ImGui::EndTooltip();
            }
            ImGui::NextColumn();

            gui_templates::TextWithBackground(labelSize, "Base file");
            gui_templates::TextWithBackground(labelSize, "%s", m_baseInfo.name.c_str());
            gui_templates::TextWithBackground(labelSize, "%s", m_baseInfo.editTimeString.c_str());
            if (ImGui::Button("Open file", buttonSize))
            {
                ignoreAutosaveOpenFileEvent.invoke();
                ImGui::CloseCurrentPopup();
            }
        ImGui::EndColumns();
		ImGui::EndPopup();
	}

    if (m_openNextFrame)
    {
        ImGui::OpenPopup("Autosave found");
        m_openNextFrame = false;
    }
}

void AutosavePopupGui::open(const std::string& baseName, const std::string& autosaveName, const std::string& baseEditTime, const std::string& autosaveEditTime)
{
    m_baseInfo = FileInfo{baseName, baseEditTime};
    m_autosaveInfo = FileInfo{autosaveName, autosaveEditTime};
    m_openNextFrame = true;
}