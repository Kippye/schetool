#pragma once

#include "gui.h"
#include "window.h"
#include "input.h"
#include "file_info.h"

class AutosavePopupGui : public Gui
{
    private:
        bool m_openNextFrame = false;
        FileInfo m_baseInfo;
        std::string m_baseEditTimeString;
        FileInfo m_autosaveInfo;
        std::string m_autosaveEditTimeString;
    public:
        AutosavePopupGui(const char* ID) : Gui(ID) {}

        Event<> openAutosaveEvent;
        Event<> ignoreAutosaveOpenFileEvent;

        void draw(Window& window, Input& input) override;
        void open(const FileInfo& baseInfo, const FileInfo& autosaveInfo, const std::string& baseEditTime, const std::string& autosaveEditTime);
};