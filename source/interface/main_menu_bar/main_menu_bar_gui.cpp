#include "main_menu_bar/main_menu_bar_gui.h"
#include "main_menu_bar/delete_modal_subgui.h"
#include "text_input_modal_subgui.h"
#include "gui_templates.h"
#include "gui_constants.h"
#include "util.h"

MainMenuBarGui::MainMenuBarGui(const char* ID, std::shared_ptr<const InterfaceStyleHandler> styleHandler) : Gui(ID) {
    m_styleHandler = styleHandler;
    // Add subguis
    addSubGui(new DeleteModalSubGui("DeleteModalSubGui"));
    addSubGui(new TextInputModalSubGui("NewNameModalSubGui", "Enter name", "Create schedule"));
    addSubGui(new TextInputModalSubGui("RenameModalSubGui", "Enter new name"));

    // Link event pipes
    createNewScheduleEventPipe.addEvent(getSubGui<TextInputModalSubGui>("NewNameModalSubGui")->acceptButtonPressedEvent);
    deleteScheduleEventPipe.addEvent(getSubGui<DeleteModalSubGui>("DeleteModalSubGui")->deleteScheduleEvent);
    renameScheduleEventPipe.addEvent(getSubGui<TextInputModalSubGui>("RenameModalSubGui")->acceptButtonPressedEvent);
}

// Helper function that gets the input shortcuts for an INPUT_EVENT and turns them into a string in the format:
// CTRL+SHIFT+X{separator}CTRL+SHIFT+Y{...}
std::string getInputEventShortcutsString(const Input& input, INPUT_EVENT inputEvent) {
    return containers::combine(Input::getShortcutStrings(input.getEventShortcuts(inputEvent)), "  ");
}

void MainMenuBarGui::draw(Window& window, Input& input, GuiTextures& guiTextures) {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (m_openFileName.has_value() == false) {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            }
            auto renameShortcuts = input.getEventShortcuts(INPUT_EVENT_SC_RENAME);
            if (ImGui::MenuItem("Rename",
                                renameShortcuts.size() > 0 ? renameShortcuts.front().getShortcutString().c_str() : NULL))
            {
                renameSchedule();
            }
            if (m_openFileName.has_value() == false) {
                ImGui::PopItemFlag();
            }
            auto newFileShortcuts = input.getEventShortcuts(INPUT_EVENT_SC_NEW);
            if (ImGui::MenuItem("New",
                                newFileShortcuts.size() > 0 ? newFileShortcuts.front().getShortcutString().c_str() : NULL))
            {
                newSchedule();
            }
            if (ImGui::BeginMenu("Open", m_fileNames.empty() == false)) {
                displayScheduleList(guiTextures);
            }
            if (m_openFileName.has_value() == false) {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            }
            auto saveShortcuts = input.getEventShortcuts(INPUT_EVENT_SC_SAVE);
            if (ImGui::MenuItem("Save", saveShortcuts.size() > 0 ? saveShortcuts.front().getShortcutString().c_str() : NULL)) {
                saveEvent.invoke();
            }
            if (m_openFileName.has_value() == false) {
                ImGui::PopItemFlag();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            auto undoShortcuts = input.getEventShortcuts(INPUT_EVENT_SC_UNDO);
            if (ImGui::MenuItem("Undo", undoShortcuts.size() > 0 ? undoShortcuts.front().getShortcutString().c_str() : NULL)) {
                undoEvent.invoke();
            }
            std::string redoShortcutsString = getInputEventShortcutsString(input, INPUT_EVENT_SC_REDO);
            if (ImGui::MenuItem("Redo", redoShortcutsString.c_str())) {
                redoEvent.invoke();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Preferences")) {
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Notifications");
            ImGui::SameLine();
            // ImGui::SetCursorPosX(styleSelectDropdownX);
            bool notificationsEnabled = m_preferences.getNotificationsEnabled();
            if (ImGui::Checkbox("##NotificationsEnabledCheckbox", &notificationsEnabled)) {
                m_preferences.setNotificationsEnabled(notificationsEnabled);
                preferencesChangedEvent.invoke(m_preferences);
            }
            ImGui::SeparatorText("Interface");
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Interface theme");
            ImGui::SameLine();
            const float styleSelectDropdownX = ImGui::GetCursorPosX();
            if (std::optional<GuiStyle> newStyle = gui_templates::Dropdown(
                    "##StyleSelectDropdown", m_styleHandler->getCurrentStyle(), InterfaceStyleHandler::styleNames))
            {
                m_preferences.setStyle(newStyle.value());
                preferencesChangedEvent.invoke(m_preferences);
            }
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Font scale");
            ImGui::SameLine();
            ImGui::SetCursorPosX(styleSelectDropdownX);
            if (std::optional<FontSize> newFontSize = gui_templates::Dropdown(
                    "##FontSizeSelectDropdown", m_styleHandler->getFontSize(), gui_fonts::fontSizeNames))
            {
                m_preferences.setFontSize(newFontSize.value());
                preferencesChangedEvent.invoke(m_preferences);
            }
            ImGui::EndMenu();
        }
        m_height = ImGui::GetWindowHeight();
    }
    ImGui::EndMainMenuBar();

    // check shortcuts (dunno if this is the best place for this? TODO )
    if (m_openFileName.has_value() && input.getEventInvokedLastFrame(INPUT_EVENT_SC_RENAME)) {
        renameSchedule();
    }
    if (input.getEventInvokedLastFrame(INPUT_EVENT_SC_NEW)) {
        newSchedule();
    }

    if (auto newNameModalSubGui = getSubGui<TextInputModalSubGui>("NewNameModalSubGui")) {
        newNameModalSubGui->draw(window, input, guiTextures);
        if (m_openNewNameModal) {
            newNameModalSubGui->open();
            m_openNewNameModal = false;
        }
    }

    if (auto renameModalSubGui = getSubGui<TextInputModalSubGui>("RenameModalSubGui")) {
        renameModalSubGui->draw(window, input, guiTextures);
        if (m_openRenameModal) {
            renameModalSubGui->open(m_openFileName.value_or(""));
            m_openRenameModal = false;
        }
    }

    if (auto deleteModalSubGui = getSubGui<DeleteModalSubGui>("DeleteModalSubGui")) {
        deleteModalSubGui->draw(window, input, guiTextures);
        if (m_openDeleteConfirmationModal) {
            ImGui::OpenPopup("Confirm Schedule deletion");
            m_openDeleteConfirmationModal = false;
        }
    }
}

float MainMenuBarGui::getHeight() const {
    return m_height;
}

void MainMenuBarGui::newSchedule() {
    m_openNewNameModal = true;
}

void MainMenuBarGui::renameSchedule() {
    m_openRenameModal = true;
}

void MainMenuBarGui::displayScheduleList(GuiTextures& guiTextures) {
    for (size_t i = 0; i < m_fileNames.size(); i++) {
        ImGui::SetNextItemAllowOverlap();
        if (ImGui::MenuItem(m_fileNames[i].c_str())) {
            openScheduleFileEvent.invoke(std::string(m_fileNames[i]));
        }
        // Show a remove button on the right when the menu item is hovered
        ImGui::PushStyleVar(
            ImGuiStyleVar_Alpha,
            ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem)
                ? 1.0f
                : 0.0f);
        float removeButtonSize = ImGui::GetItemRectSize().y;
        ImGui::SameLine();
        ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetItemRectMin().y));
        size_t pushedColorCount = 0;
        if (gui_templates::ImageButtonStyleColored(
                std::format("##DeleteScheduleFile{}", i).c_str(),
                guiTextures.getOrLoad("icon_remove").ImID,
                ImVec2(removeButtonSize, removeButtonSize) - ImGui::GetStyle().FramePadding * 2.0f,
                ImVec2(),
                ImVec2(1, 1),
                ImVec4(),
                ImGuiButtonFlags_AlignTextBaseLine))
        {
            if (auto deleteModalSubGui = getSubGui<DeleteModalSubGui>("DeleteModalSubGui")) {
                deleteModalSubGui->setAffectedScheduleName(m_fileNames[i]);
            }
            m_openDeleteConfirmationModal = true;
        }
        ImGui::PopStyleVar();
    }
    ImGui::EndMenu();
}

void MainMenuBarGui::closeModal() {
    ImGui::CloseCurrentPopup();
}

void MainMenuBarGui::passFileNames(const std::vector<std::string>& fileNames) {
    m_fileNames = fileNames;
}

void MainMenuBarGui::passOpenFileName(const std::optional<std::string>& openFileName) {
    m_openFileName = openFileName;
}

void MainMenuBarGui::passPreferences(const Preferences& preferences) {
    m_preferences = preferences;
}