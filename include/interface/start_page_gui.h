#pragma once

#include <vector>
#include "event.h"
#include "gui.h"
#include "window.h"
#include "input.h"
#include "main_menu_bar_gui.h"

class StartPageNewNameModalSubGui : public TextInputModalSubGui
{
    public:
        StartPageNewNameModalSubGui(const char* ID) : TextInputModalSubGui(ID, "Enter name   ", "Create schedule") {}

        Event<std::string> createNewScheduleEvent;

        void invokeEvent(const std::string& text) override;
};

class StartPageGui : public Gui
{
    private:
        std::vector<std::string> m_fileNames = {};
        bool m_openScheduleNameModal = false;
    public:
        StartPageGui(const char* ID);

        Event<std::string> openScheduleFileEvent;

        void draw(Window& window, Input& input, GuiTextures& guiTextures) override;
        void passFileNames(const std::vector<std::string>& fileNames);
};