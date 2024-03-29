#include "imgui.h"
#include "io_handler.h"
#include <main_menu_bar_gui.h>

MainMenuBarGui::MainMenuBarGui(const char* ID, IO_Handler* ioHandler) : Gui(ID)
{
	m_ioHandler = ioHandler;
} 

void MainMenuBarGui::draw(Window& window)
{
    if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New", "CTRL+N")) {}
			if (ImGui::BeginMenu("Open", "CTRL+O"))
			{
				std::vector<std::string> scheduleFilenames = m_ioHandler->getScheduleStemNames();

				for (size_t i = 0; i < scheduleFilenames.size(); i++)
				{
					if (ImGui::MenuItem(scheduleFilenames[i].c_str()))
					{
						m_ioHandler->readSchedule(scheduleFilenames[i].c_str());
					}
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Save", "CTRL+S"))
			{
				m_ioHandler->writeSchedule("test");
			}
			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();
}