#include "imgui.h"
#include "input.h"
#include "io_handler.h"
#include <main_menu_bar_gui.h>

MainMenuBarGui::MainMenuBarGui(const char* ID, IO_Handler* ioHandler, Schedule* schedule) : Gui(ID)
{
	m_ioHandler = ioHandler;
	m_schedule = schedule;
} 

void MainMenuBarGui::draw(Window& window, Input& input)
{
    if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Rename", "CTRL+F2"))
			{
				renameSchedule();
			}
			if (ImGui::MenuItem("New", "CTRL+N")) 
			{
				newSchedule();
			}
			if (ImGui::BeginMenu("Open", "CTRL+O"))
			{
				openSchedule();
			}
			if (ImGui::MenuItem("Save", "CTRL+S"))
			{
				saveSchedule();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z"))
			{
				undo();
			}
			if (ImGui::MenuItem("Redo", "CTRL+Y")) 
			{
				redo();
			}
			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();

	// check shortcuts (dunno if this is the best place for this? TODO )
	if (input.getCallbackInvokedLastFrame(INPUT_CALLBACK_SC_RENAME)) { renameSchedule(); }
	if (input.getCallbackInvokedLastFrame(INPUT_CALLBACK_SC_NEW)) { newSchedule(); }
	if (input.getCallbackInvokedLastFrame(INPUT_CALLBACK_SC_SAVE)) { saveSchedule(); }
	if (input.getCallbackInvokedLastFrame(INPUT_CALLBACK_SC_UNDO)) { undo(); }
	if (input.getCallbackInvokedLastFrame(INPUT_CALLBACK_SC_REDO)) { redo(); }

	displayScheduleNameModal();
	displayDeleteConfirmationModal();

	if (m_openScheduleNameModal)
	{
		ImGui::OpenPopup("Enter Schedule name");
		m_openScheduleNameModal = false;
	}
	if (m_openDeleteConfirmationModal)
	{
		ImGui::OpenPopup("Confirm Schedule deletion");
		m_openDeleteConfirmationModal = false;
	}
}

void MainMenuBarGui::displayScheduleNameModal()
{
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
					// Hide the modal if the open file was successfully renamed
					if (m_ioHandler->setOpenScheduleFilename(buf, true))
					{
						ImGui::CloseCurrentPopup();
					}
				}
			}	
		}
		ImGui::EndPopup();
	}
}

void MainMenuBarGui::displayDeleteConfirmationModal()
{
	ImGui::SetNextWindowSize(ImVec2(386.0f, 100.0f));
	if (ImGui::BeginPopupModal("Confirm Schedule deletion", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImGui::Text("%s %s?", "Delete", m_deleteConfirmationScheduleName.c_str());
		ImGui::NewLine();
		float size = 120.0f + style.FramePadding.x * 2.0f;
		float avail = ImGui::GetContentRegionAvail().x;
		float off = (avail - size) * 0.23f;
		if (off > 0.0f)
		{
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
		}
		if (ImGui::Button("Delete##DeleteSchedule", ImVec2(120, 0)))
		{
			if (m_ioHandler->deleteSchedule(m_deleteConfirmationScheduleName.c_str()))
			{
			}
			// close it anyway i guess
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		// janky offset but it works so i don't much care
		off = (avail - size) * 0.04f;
		if (off > 0.0f)
		{
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
		}
		if (ImGui::Button("Cancel##CancelScheduleDeletion", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
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

void MainMenuBarGui::renameSchedule()
{
	openNewScheduleNameModal(NAME_PROMPT_RENAME);
}
void MainMenuBarGui::newSchedule()
{
	openNewScheduleNameModal(NAME_PROMPT_NEW);
}
void MainMenuBarGui::openSchedule()
{
	std::vector<std::string> scheduleFilenames = m_ioHandler->getScheduleStemNamesSortedByEditTime();

	for (size_t i = 0; i < scheduleFilenames.size(); i++)
	{
		ImGui::SetNextItemAllowOverlap();
		if (ImGui::MenuItem(scheduleFilenames[i].c_str()))
		{
			m_ioHandler->readSchedule(scheduleFilenames[i].c_str());
		}
		ImGui::SameLine();
		if (ImGui::SmallButton(std::string("X##DeleteSchedule").append(std::to_string(i)).c_str()))
		{
			m_deleteConfirmationScheduleName = scheduleFilenames[i];
			m_openDeleteConfirmationModal = true;
		}
	}
	ImGui::EndMenu();
}
void MainMenuBarGui::saveSchedule()
{
	m_ioHandler->writeSchedule(m_ioHandler->getOpenScheduleFilename().c_str());
}
void MainMenuBarGui::undo()
{
	m_schedule->undo();
}
void MainMenuBarGui::redo()
{
	m_schedule->redo();
}