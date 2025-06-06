#pragma once

#include "interface_style.h"
#include "preferences.h"
#include "gui.h"
#include "window.h"
#include "input.h"
#include "event_pipe.h"

class MainMenuBarGui : public Gui {
    private:
        float m_height = 0.0f;
        bool m_openNewNameModal = false;
        bool m_openRenameModal = false;
        bool m_openDeleteConfirmationModal = false;
        std::shared_ptr<const InterfaceStyleHandler> m_styleHandler = nullptr;
        bool m_haveFileOpen = false;
        std::vector<std::string> m_fileNames = {};
        Preferences m_preferences = Preferences::getDefault();

        void renameSchedule();
        void newSchedule();
        void displayScheduleList(GuiTextures& guiTextures);

    public:
        MainMenuBarGui(const char* ID, std::shared_ptr<const InterfaceStyleHandler> styleHandler);

        Event<std::string> openScheduleFileEvent;
        Event<> saveEvent;

        Event<> undoEvent;
        Event<> redoEvent;

        Event<Preferences> preferencesChangedEvent;

        // Event pipes
        EventPipe<std::string> createNewScheduleEventPipe;
        EventPipe<std::string> deleteScheduleEventPipe;
        EventPipe<std::string> renameScheduleEventPipe; 

        void draw(Window& window, Input& input, GuiTextures& guiTextures) override;
        float getHeight() const;
        void closeModal();
        void passFileNames(const std::vector<std::string>& fileNames);
        void passHaveFileOpen(bool haveFileOpen);
        void passPreferences(const Preferences& preferences);
};