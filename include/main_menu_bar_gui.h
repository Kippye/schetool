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

class ScheduleNameModalSubGui : public Gui
{
    private:
        std::string m_scheduleNameBuffer = "";
        NAME_PROMPT_REASON m_promptReason = NAME_PROMPT_RENAME;

    public:
        ScheduleNameModalSubGui(const char* ID) : Gui(ID) {}

        GuiEvent<std::string> createNewScheduleEvent;
        GuiEvent<std::string, bool> renameScheduleEvent;

        void draw(Window& window, Input& input) override;
        void setPromptReason(NAME_PROMPT_REASON);
        static int filterAlphanumerics(ImGuiInputTextCallbackData* data);
};

class ScheduleDeleteModalSubGui : public Gui
{
    private:
        std::string m_deleteConfirmationScheduleName = "";

    public:
        ScheduleDeleteModalSubGui(const char* ID) : Gui(ID) {}

        GuiEvent<std::string> deleteScheduleEvent;

        void draw(Window& window, Input& input) override;
};

class MainMenuBarGui : public Gui
{
    private:
        bool m_openScheduleNameModal = false;
        bool m_openDeleteConfirmationModal = false;

        void renameSchedule();
        void newSchedule();
        void openSchedule(); 
    public:
        MainMenuBarGui(const char* ID);
        void draw(Window& window, Input& input) override;
        void openScheduleNameModal(NAME_PROMPT_REASON reason);
        void closeScheduleNameModal();
};