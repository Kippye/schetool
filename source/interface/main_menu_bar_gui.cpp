#include <main_menu_bar_gui.h>

int ScheduleNameModalSubGui::filterAlphanumerics(ImGuiInputTextCallbackData* data)
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

void ScheduleNameModalSubGui::draw(Window& window, Input& input)
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
				if (m_promptReason == NAME_PROMPT_NEW)
				{
					// Only hide modal if a valid name was provided and it was successfully used to write a file
					// if (m_ioHandler->createNewSchedule(buf))
					// {
					// 	ImGui::CloseCurrentPopup();
					// }
					createNewScheduleEvent.invoke(std::string(buf));
				}
				else if (m_promptReason == NAME_PROMPT_RENAME)
				{
					// Hide the modal if the open file was successfully renamed
					// if (m_ioHandler->setOpenScheduleFilename(buf, true))
					// {
					// 	ImGui::CloseCurrentPopup(); // TODO: do this from IO_Manager
					// }
					renameScheduleEvent.invoke(std::string(buf), true);
				}
			}	
		}
		ImGui::EndPopup();
	}
}

// NOTE: Also clears the name buffer
void ScheduleNameModalSubGui::setPromptReason(NAME_PROMPT_REASON promptReason)
{
	m_scheduleNameBuffer = "";
	m_promptReason = promptReason;
}


void ScheduleDeleteModalSubGui::draw(Window& window, Input& input)
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
			deleteScheduleEvent.invoke(std::string(m_deleteConfirmationScheduleName));

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

void ScheduleDeleteModalSubGui::setAffectedScheduleName(const std::string& name)
{
	m_deleteConfirmationScheduleName = name;
}


MainMenuBarGui::MainMenuBarGui(const char* ID) : Gui(ID)
{
	// add subguis
	addSubGui("ScheduleNameModalSubGui", new ScheduleNameModalSubGui("ScheduleNameModalSubGui"));
	addSubGui("ScheduleDeleteModalSubGui", new ScheduleDeleteModalSubGui("ScheduleDeleteModalSubGui"));
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
				displayScheduleList();
			}
			if (ImGui::MenuItem("Save", "CTRL+S"))
			{
				saveEvent.invoke();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo", "CTRL+Z"))
			{
				undoEvent.invoke();
			}
			if (ImGui::MenuItem("Redo", "CTRL+Y")) 
			{
				redoEvent.invoke();
			}
			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();

	// check shortcuts (dunno if this is the best place for this? TODO )
	if (input.getEventInvokedLastFrame(INPUT_EVENT_SC_RENAME)) { renameSchedule(); }
	if (input.getEventInvokedLastFrame(INPUT_EVENT_SC_NEW)) { newSchedule(); }

	if (auto nameModalSubGui = getSubGui<ScheduleNameModalSubGui>("ScheduleNameModalSubGui"))
	{
		nameModalSubGui->draw(window, input);
	}

	if (auto deleteModalSubGui = getSubGui<ScheduleDeleteModalSubGui>("ScheduleDeleteModalSubGui"))
	{
		deleteModalSubGui->draw(window, input);
	}

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

void MainMenuBarGui::renameSchedule()
{
	openScheduleNameModal(NAME_PROMPT_RENAME);
}
void MainMenuBarGui::newSchedule()
{
	openScheduleNameModal(NAME_PROMPT_NEW);
}
void MainMenuBarGui::displayScheduleList()
{
	for (size_t i = 0; i < m_fileNames.size(); i++)
	{
		ImGui::SetNextItemAllowOverlap();
		if (ImGui::MenuItem(m_fileNames[i].c_str()))
		{
			openScheduleFileEvent.invoke(std::string(m_fileNames[i]));
		}
		ImGui::SameLine();
		if (ImGui::SmallButton(std::string("X##DeleteSchedule").append(std::to_string(i)).c_str()))
		{
			if (auto deleteModalSubGui = getSubGui<ScheduleDeleteModalSubGui>("ScheduleDeleteModalSubGui"))
			{
				deleteModalSubGui->setAffectedScheduleName(m_fileNames[i]);
			}
			m_openDeleteConfirmationModal = true;
		}
	}
	ImGui::EndMenu();
}

void MainMenuBarGui::openScheduleNameModal(NAME_PROMPT_REASON reason)
{
	if (auto nameModalSubGui = getSubGui<ScheduleNameModalSubGui>("ScheduleNameModalSubGui"))
	{
		nameModalSubGui->setPromptReason(reason);
	}
	m_openScheduleNameModal = true;
}

#include <iostream>

void MainMenuBarGui::closeScheduleNameModal()
{
	ImGui::CloseCurrentPopup();
}

void MainMenuBarGui::passFileNames(const std::vector<std::string>& fileNames)
{
	m_fileNames = fileNames;
}