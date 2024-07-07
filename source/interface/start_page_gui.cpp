#include "start_page_gui.h"

void StartPageNewNameModalSubGui::invokeEvent(const std::string& text)
{
    createNewScheduleEvent.invoke(text);
}


StartPageGui::StartPageGui(const char* ID) : Gui(ID)
{
    setVisible(false);
    addSubGui("StartPageNewNameModalSubGui", new StartPageNewNameModalSubGui("StartPageNewNameModalSubGui"));
}

void StartPageGui::draw(Window& window, Input& input)
{
    if (m_visible == false) { return; }

	ImGui::SetNextWindowSize(ImVec2((float)window.SCREEN_WIDTH, (float)window.SCREEN_HEIGHT));
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));

	ImGui::Begin(m_ID.c_str(), NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
        ImGui::NewLine();
        ImGui::Text("Schetool");
        ImGui::Text("Create a new file");
        if (ImGui::Button("New##StartPageGuiNewButton"))
        {
            m_openScheduleNameModal = true;
        }
        ImGui::Text("Open a file");
        for (size_t i = 0; i < m_fileNames.size(); i++)
        {
            if (ImGui::Button(std::string(m_fileNames[i]).append("##StartPageGuiFileListButton").c_str()))
            {
                openScheduleFileEvent.invoke(std::string(m_fileNames[i]));
            }
        }
    ImGui::End();

    if (auto nameModalSubGui = getSubGui<StartPageNewNameModalSubGui>("StartPageNewNameModalSubGui"))
	{
		nameModalSubGui->draw(window, input);

        if (m_openScheduleNameModal)
        {
            nameModalSubGui->open();
            m_openScheduleNameModal = false;
        }
	}
}

void StartPageGui::passFileNames(const std::vector<std::string>& fileNames)
{
	m_fileNames = fileNames;
}