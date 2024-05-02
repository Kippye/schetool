#pragma once
#include <imgui.h>
#include <gui.h>
#include <window.h>
#include <input.h>

enum NAME_PROMPT_REASON
{
    NAME_PROMPT_NEW,
    NAME_PROMPT_RENAME
};

#include <cstdarg>

class ScheduleNameModalSubGui : public Gui
{
    private:
        std::string m_scheduleNameBuffer = "";
        NAME_PROMPT_REASON m_promptReason = NAME_PROMPT_RENAME;

    public:
        ScheduleNameModalSubGui(const char* ID);
        void draw(Window& window, Input& input) override;
        void setPromptReason(NAME_PROMPT_REASON);
        static int filterAlphanumerics(ImGuiInputTextCallbackData* data);
        void test(size_t argCount, std::string fileName, ...);
};

class MainMenuBarGui : public Gui
{
    private:
        bool m_openScheduleNameModal = false;
        bool m_openDeleteConfirmationModal = false;
        std::string m_deleteConfirmationScheduleName = "";

        void displayScheduleNameModal();
        void displayDeleteConfirmationModal();
        void renameSchedule();
        void newSchedule();
        void openSchedule(); 
    public:
        MainMenuBarGui(const char* ID);
        void draw(Window& window, Input& input) override;
        void openNewScheduleNameModal(NAME_PROMPT_REASON reason);
};