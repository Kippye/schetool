#pragma once

#include "gui.h"
#include "input.h"
#include "file_info.h"

class AutosavePopupGui : public Gui {
    private:
        bool m_openNextFrame = false;
        FileInfo m_baseInfo;
        FileInfo m_autosaveInfo;

    public:
        AutosavePopupGui(const char* ID) : Gui(ID) {
        }

        Event<> applyAutosaveEvent;
        Event<> deleteAutosaveEvent;

        void draw(const WindowSize& windowSize, Input& input, GuiTextures& guiTextures) override;
        void open(const FileInfo& baseInfo, const FileInfo& autosaveInfo);
};