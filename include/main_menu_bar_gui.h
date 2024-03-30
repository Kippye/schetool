#pragma once
#include <imgui.h>
#include <gui.h>
#include <window.h>
#include <io_handler.h>

enum NAME_PROMPT_REASON
{
    NAME_PROMPT_NEW,
    NAME_PROMPT_RENAME
};

class MainMenuBarGui : public Gui
{
    private:
        IO_Handler* m_ioHandler;
        bool m_openScheduleNameModal = false;
        bool m_openDeleteConfirmationModal = false;
        NAME_PROMPT_REASON m_currentNamePromptReason;
        std::string m_scheduleNameBuffer = "";
        std::string m_deleteConfirmationScheduleName = "";

        void displayScheduleNameModal();
        void displayDeleteConfirmationModal();
    public:
        MainMenuBarGui(const char* ID) : Gui(ID) {}
        MainMenuBarGui(const char* ID, IO_Handler* ioHandler);
        void draw(Window& window) override;
        void openNewScheduleNameModal(NAME_PROMPT_REASON reason);
        static int filterAlphanumerics(ImGuiInputTextCallbackData* data);
};