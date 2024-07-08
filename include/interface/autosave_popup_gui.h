#pragma once

#include "gui.h"
#include "window.h"
#include "input.h"

struct FileInfo
{
    std::string name;
    std::string editTimeString;
};

class AutosavePopupGui : public Gui
{
    private:
        bool m_openNextFrame = false;
        FileInfo m_baseInfo;
        FileInfo m_autosaveInfo;
    public:
        AutosavePopupGui(const char* ID) : Gui(ID) {}

        Event<> openAutosaveEvent;
        Event<> ignoreAutosaveOpenFileEvent;

        void draw(Window& window, Input& input) override;
        void open(const std::string& baseName, const std::string& autosaveName, const std::string& baseEditTime, const std::string& autosaveEditTime);
};