#include "main_menu_bar/main_menu_bar_gui.h"
#include "gui_templates.h"
#include "gui_constants.h"
#include "util.h"

float MainMenuBarGui::height = 0.0f;

MainMenuBarGui::MainMenuBarGui(const char* ID, const InterfaceStyleHandler& styleHandler)
    : Gui(ID), m_styleHandler(styleHandler) {
    // Add subguis
    m_deleteModalSubGui = getSubGui<ConfirmationModalSubGui>(
        addSubGui(new ConfirmationModalSubGui("DeleteModalSubGui", "Delete schedule", "Delete {}?")));
    m_closeWithEditsModalSubGui = getSubGui<ConfirmationModalSubGui>(addSubGui(
        new ConfirmationModalSubGui("CloseWithEditsModalSubGui", "Close file", "Save changes before closing?", "Yes", "No")));
    m_newNameModalSubGui = getSubGui<TextInputModalSubGui>(
        addSubGui(new TextInputModalSubGui("NewNameModalSubGui", "Enter name", "Create schedule")));
    m_renameModalSubGui =
        getSubGui<TextInputModalSubGui>(addSubGui(new TextInputModalSubGui("RenameModalSubGui", "Enter new name")));

    // Link event pipes
    createNewScheduleEventPipe.addEvent(m_newNameModalSubGui->acceptButtonPressedEvent);
    deleteScheduleEventPipe.addEvent(m_deleteModalSubGui->appliedEvent);
    saveAndCloseEventPipe.addEvent(m_closeWithEditsModalSubGui->appliedEvent);
    closeWithoutSaveEventPipe.addEvent(m_closeWithEditsModalSubGui->cancelledEvent);
    renameScheduleEventPipe.addEvent(m_renameModalSubGui->acceptButtonPressedEvent);
}

// Helper function that gets the input shortcuts for an INPUT_EVENT and turns them into a string in the format:
// CTRL+SHIFT+X{separator}CTRL+SHIFT+Y{...}
std::string getInputEventShortcutsString(const Input& input, INPUT_EVENT inputEvent) {
    return containers::combine(Input::getShortcutStrings(input.getEventShortcuts(inputEvent)), "  ");
}

void MainMenuBarGui::draw(GuiDrawArgs& args) {
    m_newNameModalSubGui->draw(args);
    m_renameModalSubGui->draw(args);
    m_deleteModalSubGui->draw(args);
    m_closeWithEditsModalSubGui->draw(args);

    bool openNewNameModal = false;
    bool openRenameModal = false;
    std::optional<FileInfo> openDeleteModal = std::nullopt;
    bool openCloseWithEditsModal = false;

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            bool disableFileSpecificItems = !m_openFile.has_value();
            if (disableFileSpecificItems) {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            }
            auto renameShortcuts = args.input.getEventShortcuts(INPUT_EVENT_SC_RENAME);
            if (ImGui::MenuItem("Rename",
                                renameShortcuts.size() > 0 ? renameShortcuts.front().getShortcutString().c_str() : NULL))
            {
                openRenameModal = true;
            }
            if (disableFileSpecificItems) {
                ImGui::PopItemFlag();
            }
            auto newFileShortcuts = args.input.getEventShortcuts(INPUT_EVENT_SC_NEW);
            if (ImGui::MenuItem("New",
                                newFileShortcuts.size() > 0 ? newFileShortcuts.front().getShortcutString().c_str() : NULL))
            {
                openNewNameModal = true;
            }
            if (ImGui::BeginMenu("Open", m_fileInfoList.empty() == false)) {
                auto filenameToDelete = displayScheduleList(args.guiTextures);

                if (filenameToDelete.has_value()) {
                    openDeleteModal = filenameToDelete;
                }
            }
            auto saveShortcuts = args.input.getEventShortcuts(INPUT_EVENT_SC_SAVE);
            if (disableFileSpecificItems) {
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
            }
            if (ImGui::MenuItem("Save", saveShortcuts.size() > 0 ? saveShortcuts.front().getShortcutString().c_str() : NULL)) {
                saveEvent.invoke();
            }
            auto closeShortcuts = args.input.getEventShortcuts(INPUT_EVENT_SC_CLOSE);
            if (ImGui::MenuItem("Close", closeShortcuts.size() > 0 ? closeShortcuts.front().getShortcutString().c_str() : NULL))
            {
                if (m_fileHasEdits == false) {
                    saveAndCloseEventPipe.invoke("");
                } else {
                    openCloseWithEditsModal = true;
                }
            }
            if (disableFileSpecificItems) {
                ImGui::PopItemFlag();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            auto undoShortcuts = args.input.getEventShortcuts(INPUT_EVENT_SC_UNDO);
            if (ImGui::MenuItem("Undo", undoShortcuts.size() > 0 ? undoShortcuts.front().getShortcutString().c_str() : NULL)) {
                undoEvent.invoke();
            }
            std::string redoShortcutsString = getInputEventShortcutsString(args.input, INPUT_EVENT_SC_REDO);
            if (ImGui::MenuItem("Redo", redoShortcutsString.c_str())) {
                redoEvent.invoke();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Preferences")) {
            ImGui::SeparatorText("Notifications");
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Popup");
            ImGui::SameLine();
            bool notificationsEnabled = m_preferences.getNotificationsEnabled();
            if (ImGui::Checkbox("##NotificationsEnabledCheckbox", &notificationsEnabled)) {
                m_preferences.setNotificationsEnabled(notificationsEnabled);
                preferencesChangedEvent.invoke(m_preferences);
            }
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Sound");
            ImGui::SameLine();
            bool notificationSoundEnabled = m_preferences.getNotificationSoundEnabled();
            if (ImGui::Checkbox("##NotificationSoundEnabledCheckbox", &notificationSoundEnabled)) {
                m_preferences.setNotificationSoundEnabled(notificationSoundEnabled);
                preferencesChangedEvent.invoke(m_preferences);
            }
            ImGui::SeparatorText("Interface");
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Interface theme");
            ImGui::SameLine();
            const float styleSelectDropdownX = ImGui::GetCursorPosX();
            if (!m_styleHandler.getCurrentStyle().name.empty()) {
                if (std::optional<GuiStyleDefinition> newStyle =
                        gui_templates::Dropdown("##StyleSelectDropdown",
                                                m_styleHandler.getCurrentStyle(),
                                                InterfaceStyleHandler::getStyleDefinitionToName()))
                {
                    m_preferences.setStyle(newStyle.value());
                    preferencesChangedEvent.invoke(m_preferences);
                }
            } else {
                ImGui::NewLine();
            }
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Font scale");
            ImGui::SameLine();
            ImGui::SetCursorPosX(styleSelectDropdownX);
            if (std::optional<FontSize> newFontSize =
                    gui_templates::Dropdown("##FontSizeSelectDropdown", m_styleHandler.getFontSize(), gui_fonts::fontSizeNames))
            {
                m_preferences.setFontSize(newFontSize.value());
                preferencesChangedEvent.invoke(m_preferences);
            }
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Row highlighting");
            ImGui::SetItemTooltip(
                "Change row background color depending on the state (e.g. finished or current) of the item at that row.");
            ImGui::SameLine();
            bool rowHighlightingEnabled = m_preferences.getRowHighlightingEnabled();
            if (ImGui::Checkbox("##RowHighlightingEnabledCheckbox", &rowHighlightingEnabled)) {
                m_preferences.setRowHighlightingEnabled(rowHighlightingEnabled);
                preferencesChangedEvent.invoke(m_preferences);
            }
            ImGui::EndMenu();
        }
        height = ImGui::GetWindowHeight();
    }

    m_saveIndicatorDurationLeft = std::max(0.0f, m_saveIndicatorDurationLeft - args.deltaTime);
    if (m_saveIndicatorDurationLeft > 0.0f) {
        ImGui::SetCursorPos(
            ImVec2(ImGui::GetWindowWidth() / 2.0f - gui_size_calculations::getTextButtonWidth("Saved!") / 2.0f, 0.0f));
        ImGui::Text("Saved!");
    }
    ImGui::EndMainMenuBar();

    // Check shortcuts (dunno if this is the best place for this? TODO )
    if (m_openFile.has_value() && args.input.getEventInvokedLastFrame(INPUT_EVENT_SC_RENAME)) {
        openRenameModal = true;
    }
    if (args.input.getEventInvokedLastFrame(INPUT_EVENT_SC_NEW)) {
        openNewNameModal = true;
    }
    if (m_openFile.has_value() && args.input.getEventInvokedLastFrame(INPUT_EVENT_SC_CLOSE)) {
        if (m_fileHasEdits == false) {
            saveAndCloseEventPipe.invoke("");
        } else {
            openCloseWithEditsModal = true;
        }
    }

    if (openNewNameModal) {
        m_newNameModalSubGui->open();
    }
    if (openRenameModal) {
        m_renameModalSubGui->open(m_openFile.has_value() ? m_openFile->getStem() : "");
    }
    if (openDeleteModal.has_value()) {
        m_deleteModalSubGui->open(openDeleteModal->getStem());
    }
    if (openCloseWithEditsModal) {
        m_closeWithEditsModalSubGui->open();
    }
}

float MainMenuBarGui::getHeight() {
    return height;
}  // STATIC

std::optional<FileInfo> MainMenuBarGui::displayScheduleList(GuiTextures& guiTextures) {
    std::optional<FileInfo> fileToDelete = std::nullopt;

    for (size_t i = 0; i < m_fileInfoList.size(); i++) {
        ImGui::SetNextItemAllowOverlap();
        if (ImGui::MenuItem(m_fileInfoList[i].getStem().c_str())) {
            openScheduleFileEvent.invoke(m_fileInfoList[i]);
        }
        // Show a remove button on the right when the menu item is hovered
        ImGui::PushStyleVar(
            ImGuiStyleVar_Alpha,
            ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem)
                ? 1.0f
                : 0.0f);
        float removeButtonSize = ImGui::CalcTextSize("W").y;
        float padding = (ImGui::GetItemRectSize().y - removeButtonSize) / 2.0f;

        ImGui::SameLine();
        ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetItemRectMin().y));

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(padding, padding));
        if (gui_templates::ImageButtonStyleColored(std::format("##DeleteScheduleFile{}", i).c_str(),
                                                   guiTextures.getOrLoad("icon_remove").ImID,
                                                   ImVec2(removeButtonSize, removeButtonSize),
                                                   ImVec2(),
                                                   ImVec2(1, 1),
                                                   ImVec4(),
                                                   ImGuiButtonFlags_AlignTextBaseLine))
        {
            fileToDelete = m_fileInfoList[i];
        }
        ImGui::PopStyleVar(2);
    }
    ImGui::EndMenu();
    return fileToDelete;
}

void MainMenuBarGui::closeModal() {
    ImGui::CloseCurrentPopup();
}

void MainMenuBarGui::onCurrentFileSaved() {
    m_saveIndicatorDurationLeft = SAVE_INDICATOR_DISPLAY_DURATION;
}

void MainMenuBarGui::passFileInfoList(const std::vector<FileInfo>& fileInfoList) {
    m_fileInfoList = fileInfoList;
}

void MainMenuBarGui::passOpenFileInfo(const std::optional<FileInfo>& openFile) {
    m_openFile = openFile;
}

void MainMenuBarGui::passFileHasEdits(bool hasEdits) {
    m_fileHasEdits = hasEdits;
}

void MainMenuBarGui::passPreferences(const Preferences& preferences) {
    m_preferences = preferences;
}