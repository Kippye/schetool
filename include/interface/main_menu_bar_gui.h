#pragma once

#include "imgui.h"
#include "gui.h"
#include "window.h"
#include "input.h"

class TextInputModalSubGui : public Gui
{
    private:
        std::string m_textBuffer = "";
        size_t m_textMaxLength;
        const char* m_popupName;
        std::string m_acceptButtonText;
        bool m_showCloseButton;
    public:
        TextInputModalSubGui(const char* ID, const char* popupName = "Text input", const char* acceptButtonText = "Accept", size_t textMaxLength = 48, bool showCloseButton = true);

        void draw(Window& window, Input& input) override; 
        void open();
        virtual void invokeEvent(const std::string& text);
};

class NewNameModalSubGui : public TextInputModalSubGui
{
    public:
        NewNameModalSubGui(const char* ID) : TextInputModalSubGui(ID, "Enter name", "Create schedule") {}

        Event<std::string> createNewScheduleEvent;

        void invokeEvent(const std::string& text) override;
};

class RenameModalSubGui : public TextInputModalSubGui
{
    public:
        RenameModalSubGui(const char* ID) : TextInputModalSubGui(ID, "Enter new name") {}

        Event<std::string> renameScheduleEvent;
        
        void invokeEvent(const std::string& text) override;
};

class DeleteModalSubGui : public Gui
{
    private:
        std::string m_deleteConfirmationScheduleName = "";

    public:
        DeleteModalSubGui(const char* ID) : Gui(ID) {}

        Event<std::string> deleteScheduleEvent;

        void draw(Window& window, Input& input) override;
        void setAffectedScheduleName(const std::string& name);
};

class MainMenuBarGui : public Gui
{
    private:
        bool m_openNewNameModal = false;
        bool m_openRenameModal = false;
        bool m_openDeleteConfirmationModal = false;
        bool m_haveFileOpen = false;
        std::vector<std::string> m_fileNames = {};

        void renameSchedule();
        void newSchedule();
        void displayScheduleList(); 
    public:
        MainMenuBarGui(const char* ID);

        Event<std::string> openScheduleFileEvent;
        Event<> undoEvent;
        Event<> redoEvent;
        Event<> saveEvent;

        void draw(Window& window, Input& input) override;
        void closeModal();
        void passFileNames(const std::vector<std::string>& fileNames);
        void passHaveFileOpen(bool haveFileOpen);
};