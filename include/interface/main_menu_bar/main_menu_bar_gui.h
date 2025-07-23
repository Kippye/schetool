#pragma once

#include <optional>
#include "interface_style.h"
#include "preferences.h"
#include "gui.h"
#include "input.h"
#include "event_pipe.h"

class MainMenuBarGui : public Gui {
    private:
        static float height;
        bool m_openNewNameModal = false;
        bool m_openRenameModal = false;
        bool m_openDeleteConfirmationModal = false;
        const InterfaceStyleHandler& m_styleHandler;
        std::optional<std::string> m_openFileName = std::nullopt;
        std::vector<std::string> m_fileNames = {};
        Preferences m_preferences = Preferences::getDefault();

        void renameSchedule();
        void newSchedule();
        void displayScheduleList(GuiTextures& guiTextures);

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
        EventPipe<std::string> renameScheduleEventPipe;

        void draw(const WindowSize& windowSize, Input& input, GuiTextures& guiTextures) override;
        // Static function. Assuming that there is only one MainMenuBarGui instance or they are all the same height.
        // Get the height of the MainMenuBarGui.
        static float getHeight();
        void closeModal();
        void passFileNames(const std::vector<std::string>& fileNames);
        void passOpenFileName(const std::optional<std::string>& openFileName);
        void passPreferences(const Preferences& preferences);
};