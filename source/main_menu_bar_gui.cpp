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
			if (ImGui::MenuItem("Open", "CTRL+O"))
			{
				m_ioHandler->readSchedule("C:\\Users\\Remoa\\Documents\\Devil\\schetool\\test.blf");
			}
			if (ImGui::MenuItem("Save", "CTRL+S"))
			{
				m_ioHandler->writeSchedule("C:\\Users\\Remoa\\Documents\\Devil\\schetool\\test.blf");
			}
			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();
}