#include "main_menu_bar_gui.h"
#include "gui_templates.h"

TextInputModalSubGui::TextInputModalSubGui(const char* ID, const char* popupName, const char* acceptButtonText, size_t textMaxLength, bool showCloseButton) : Gui(ID)
{
    m_popupName = popupName;
    m_acceptButtonText = acceptButtonText;
    m_textMaxLength = textMaxLength;
    m_showCloseButton = showCloseButton;
}

void TextInputModalSubGui::draw(Window& window, Input& input)
{
	float nameInputWidth = ImGui::CalcTextSize(std::string(m_textMaxLength + 2, 'a').c_str()).x;

    m_visible = true;

	ImGuiStyle& style = ImGui::GetStyle();
	ImGui::SetNextWindowSize(ImVec2(nameInputWidth + style.FramePadding.x * 2.0f, 100));
	// ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), 0, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal(m_popupName, m_showCloseButton ? &m_visible : NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
	{
		m_textBuffer.reserve(m_textMaxLength);
		char* buf = m_textBuffer.data();
		ImGui::PushItemWidth(nameInputWidth - style.FramePadding.x * 2.0f);
		ImGui::InputText(std::string("##TextInput").append(m_popupName).c_str(), buf, m_textMaxLength, ImGuiInputTextFlags_CallbackCharFilter, gui_callbacks::filterAlphanumerics);
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
			if (ImGui::Button(m_acceptButtonText.c_str(), ImVec2(128, 0)))
			{
                invokeEvent(std::string(buf));
			}	
		}
		ImGui::EndPopup();
	}
}

// Open the popup and empty the text buffer
void TextInputModalSubGui::open()
{
    m_textBuffer = "";
    ImGui::OpenPopup(m_popupName);
}

void TextInputModalSubGui::invokeEvent(const std::string& text) {}

void NewNameModalSubGui::invokeEvent(const std::string& text)
{
    createNewScheduleEvent.invoke(text);
}

void RenameModalSubGui::invokeEvent(const std::string& text)
{
    renameScheduleEvent.invoke(text);
}

void DeleteModalSubGui::draw(Window& window, Input& input)
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

void DeleteModalSubGui::setAffectedScheduleName(const std::string& name)
{
	m_deleteConfirmationScheduleName = name;
}


MainMenuBarGui::MainMenuBarGui(const char* ID) : Gui(ID)
{
	// add subguis
	addSubGui(new NewNameModalSubGui("NewNameModalSubGui"));
	addSubGui(new RenameModalSubGui("RenameModalSubGui"));
	addSubGui(new DeleteModalSubGui("DeleteModalSubGui"));
}

void MainMenuBarGui::draw(Window& window, Input& input)
{
    if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
            if (m_haveFileOpen == false)
            {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            }
			if (ImGui::MenuItem("Rename", "CTRL+F2"))
			{
				renameSchedule();
			}
            if (m_haveFileOpen == false)
            {
                ImGui::PopItemFlag();
            }
			if (ImGui::MenuItem("New", "CTRL+N")) 
			{
				newSchedule();
			}
			if (ImGui::BeginMenu("Open", "CTRL+O"))
			{
				displayScheduleList();
			}
            if (m_haveFileOpen == false)
            {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            }
			if (ImGui::MenuItem("Save", "CTRL+S"))
			{
				saveEvent.invoke();
			}
            if (m_haveFileOpen == false)
            {
                ImGui::PopItemFlag();
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
	if (m_haveFileOpen && input.getEventInvokedLastFrame(INPUT_EVENT_SC_RENAME)) { renameSchedule(); }
	if (input.getEventInvokedLastFrame(INPUT_EVENT_SC_NEW)) { newSchedule(); }

	if (auto newNameModalSubGui = getSubGui<NewNameModalSubGui>("NewNameModalSubGui"))
	{
		newNameModalSubGui->draw(window, input);
        if (m_openNewNameModal)
        {
            newNameModalSubGui->open();
            m_openNewNameModal = false;
        }
	}

    if (auto renameModalSubGui = getSubGui<RenameModalSubGui>("RenameModalSubGui"))
	{
		renameModalSubGui->draw(window, input);
	}

	if (auto deleteModalSubGui = getSubGui<DeleteModalSubGui>("DeleteModalSubGui"))
	{
		deleteModalSubGui->draw(window, input);
	}

	// if (m_openNewNameModal)
	// {
	// 	ImGui::OpenPopup("Enter name");
	// 	m_openNewNameModal = false;
	// }
    if (m_openRenameModal)
	{
		ImGui::OpenPopup("Enter new name");
		m_openRenameModal = false;
	}
	if (m_openDeleteConfirmationModal)
	{
		ImGui::OpenPopup("Confirm Schedule deletion");
		m_openDeleteConfirmationModal = false;
	}
}

void MainMenuBarGui::newSchedule()
{
    m_openNewNameModal = true;
}

void MainMenuBarGui::renameSchedule()
{
    m_openRenameModal = true;
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
			if (auto deleteModalSubGui = getSubGui<DeleteModalSubGui>("DeleteModalSubGui"))
			{
				deleteModalSubGui->setAffectedScheduleName(m_fileNames[i]);
			}
			m_openDeleteConfirmationModal = true;
		}
	}
	ImGui::EndMenu();
}

void MainMenuBarGui::closeModal()
{
	ImGui::CloseCurrentPopup();
}

void MainMenuBarGui::passFileNames(const std::vector<std::string>& fileNames)
{
	m_fileNames = fileNames;
}

void MainMenuBarGui::passHaveFileOpen(bool haveFileOpen)
{
    m_haveFileOpen = haveFileOpen;
}