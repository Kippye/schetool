#pragma once

#include <optional>
#include <map>
#include "interface_style.h"
#include "preferences.h"
#include "confirmation_modal_subgui.h"
#include "text_input_modal_subgui.h"
#include "gui.h"
#include "event_pipe.h"
#include "file_info.h"
#include "messages/status_message_queue.h"

class MainMenuBarGui : public Gui {
    private:
        static float height;

        const std::map<StatusMessageType, float> STATUS_MSG_TYPE_DURATIONS = {
            {StatusMessageType::Info, 1.0f}, {StatusMessageType::Warning, 2.0f}, {StatusMessageType::Error, 5.0f}};
        std::optional<StatusMessage> m_currentMessage = std::nullopt;
        float m_currentMsgDurationLeft = 0.0f;

        const InterfaceStyleHandler& m_styleHandler;
        // MODALS
        std::shared_ptr<ConfirmationModalSubGui> m_deleteModalSubGui = nullptr;
        std::shared_ptr<ConfirmationModalSubGui> m_closeWithEditsModalSubGui = nullptr;
        std::shared_ptr<TextInputModalSubGui> m_newNameModalSubGui = nullptr;
        std::shared_ptr<TextInputModalSubGui> m_renameModalSubGui = nullptr;
        // STATE PASSED FROM OUTSIDE
        std::optional<FileInfo> m_openFile = std::nullopt;
        std::vector<FileInfo> m_fileInfoList = {};
        bool m_fileHasEdits = false;
        Preferences m_preferences = Preferences::getDefault();

        // Display a list of schedules for the Open menu.
        // Returns the file info to be opened in the delete confirmation modal.
        std::optional<FileInfo> displayScheduleList(GuiTextures& guiTextures);

    public:
        MainMenuBarGui(const char* ID, const InterfaceStyleHandler& styleHandler);

        Event<FileInfo> openScheduleFileEvent;
        Event<> saveEvent;
        Event<> undoEvent;
        Event<> redoEvent;

        Event<Preferences> preferencesChangedEvent;

        // Event pipes
        EventPipe<std::string> createNewScheduleEventPipe;
        EventPipe<std::string> deleteScheduleEventPipe;
        EventPipe<std::string> closeWithoutSaveEventPipe;
        EventPipe<std::string> saveAndCloseEventPipe;
        EventPipe<std::string> renameScheduleEventPipe;

        void draw(GuiDrawArgs& args) override;
        // Static function. Assuming that there is only one MainMenuBarGui instance or they are all the same height.
        // Get the height of the MainMenuBarGui.
        static float getHeight();
        void closeModal();
        void passFileInfoList(const std::vector<FileInfo>& fileInfoList);
        void passOpenFileInfo(const std::optional<FileInfo>& openFile);
        void passFileHasEdits(bool hasEdits);
        void passPreferences(const Preferences& preferences);
};