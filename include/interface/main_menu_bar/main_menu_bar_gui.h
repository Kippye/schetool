#pragma once

#include <optional>
#include "interface_style.h"
#include "preferences.h"
#include "confirmation_modal_subgui.h"
#include "text_input_modal_subgui.h"
#include "gui.h"
#include "event_pipe.h"

class MainMenuBarGui : public Gui {
    private:
        static float height;

        const InterfaceStyleHandler& m_styleHandler;
        // MODALS
        std::shared_ptr<ConfirmationModalSubGui> m_deleteModalSubGui = nullptr;
        std::shared_ptr<ConfirmationModalSubGui> m_closeWithEditsModalSubGui = nullptr;
        std::shared_ptr<TextInputModalSubGui> m_newNameModalSubGui = nullptr;
        std::shared_ptr<TextInputModalSubGui> m_renameModalSubGui = nullptr;
        // STATE PASSED FROM OUTSIDE
        std::optional<std::string> m_openFileName = std::nullopt;
        std::vector<std::string> m_fileNames = {};
        bool m_fileHasEdits = false;
        Preferences m_preferences = Preferences::getDefault();

        // Display a list of schedules for the Open menu.
        // Returns the file name to be opened in the delete confirmation modal.
        std::optional<std::string> displayScheduleList(GuiTextures& guiTextures);

    public:
        MainMenuBarGui(const char* ID, const InterfaceStyleHandler& styleHandler);

        Event<std::string> openScheduleFileEvent;
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
        void passFileNames(const std::vector<std::string>& fileNames);
        void passOpenFileName(const std::optional<std::string>& openFileName);
        void passFileHasEdits(bool hasEdits);
        void passPreferences(const Preferences& preferences);
};