#pragma once

#include "gui.h"
#include "input.h"
#include "file_info.h"
#include <optional>

class AutosavePopupGui : public Gui {
    private:
        bool m_openNextFrame = false;
        std::optional<FileInfo> m_baseInfo = std::nullopt;
        std::optional<FileInfo> m_autosaveInfo = std::nullopt;

    public:
        AutosavePopupGui(const char* ID) : Gui(ID) {
        }

        Event<> applyAutosaveEvent;
        Event<> deleteAutosaveEvent;

        void draw(GuiDrawArgs& args) override;
        void open(const FileInfo& baseInfo, const FileInfo& autosaveInfo);
};