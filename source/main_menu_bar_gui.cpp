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
			if (ImGui::MenuItem("Rename", "CTRL+F2"))
			{
				openNewScheduleNameModal(NAME_PROMPT_RENAME);
			}
			if (ImGui::MenuItem("New", "CTRL+N")) 
			{
				openNewScheduleNameModal(NAME_PROMPT_NEW);
			}
			if (ImGui::BeginMenu("Open", "CTRL+O"))
			{
				std::vector<std::string> scheduleFilenames = m_ioHandler->getScheduleStemNamesSortedByEditTime();

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
				m_ioHandler->writeSchedule(m_ioHandler->getOpenScheduleFilename().c_str());
			}
			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();

	float nameInputWidth = ImGui::CalcTextSize(std::string(48, 'a').c_str()).x;

	ImGuiStyle& style = ImGui::GetStyle();
	ImGui::SetNextWindowSize(ImVec2(nameInputWidth + style.FramePadding.x * 2.0f, 100));
	if (ImGui::BeginPopupModal("Enter Schedule name", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
	{
		m_scheduleNameBuffer.reserve(46);
		char* buf = m_scheduleNameBuffer.data();
		ImGui::PushItemWidth(nameInputWidth - style.FramePadding.x * 2.0f);
		ImGui::InputText("##NewScheduleNameInput", buf, 46, ImGuiInputTextFlags_CallbackCharFilter, filterAlphanumerics);
		ImGui::PopItemWidth();
		if (std::string(buf).empty() == false)
		{
			float size = 128.0f + style.FramePadding.x * 2.0f;
			float avail = ImGui::GetContentRegionAvail().x;
			float off = (avail - size) * 0.5f;
			if (off > 0.0f)
			{
        		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
			}
			if (ImGui::Button("Accept name", ImVec2(128, 0)))
			{
				if (m_currentNamePromptReason == NAME_PROMPT_NEW)
				{
					// Only hide modal if a valid name was provided and it was successfully used to write a file
					if (m_ioHandler->createNewSchedule(buf))
					{
						ImGui::CloseCurrentPopup();
					}
				}
				else if (m_currentNamePromptReason == NAME_PROMPT_RENAME)
				{
					m_ioHandler->setOpenScheduleFilename(buf, true);
					ImGui::CloseCurrentPopup();
				}
			}	
		}
		ImGui::EndPopup();
	}

	if (m_openScheduleNameModal)
	{
		ImGui::OpenPopup("Enter Schedule name");
		m_openScheduleNameModal = false;
	}
}

void MainMenuBarGui::openNewScheduleNameModal(NAME_PROMPT_REASON reason)
{
	m_scheduleNameBuffer = "";
	m_currentNamePromptReason = reason;
	m_openScheduleNameModal = true;
}

int MainMenuBarGui::filterAlphanumerics(ImGuiInputTextCallbackData* data)
{
	if (
		(data->EventChar >= 97 && data->EventChar <= 122) // lowercase letters in english alphabet
		|| (data->EventChar >= 60 && data->EventChar <= 90 && data->EventChar != 63) // capital letters in english alphabet MINUS some other things such as "?"
		|| (data->EventChar >= 48 && data->EventChar <= 57) // numbers
		|| data->EventChar == 32) // space
	{
		return 0;
	}
	return 1;
}