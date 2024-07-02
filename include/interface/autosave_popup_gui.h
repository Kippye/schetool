#pragma once

#include "gui.h"
#include "window.h"
#include "input.h"

class AutosavePopupGui : public Gui
{
    private:
        bool m_openNextFrame = false;
    public:
        AutosavePopupGui(const char* ID) : Gui(ID) {}

        Event<> openAutosaveEvent;
        Event<> applyAutosaveOpenFileEvent;
        Event<> ignoreAutosaveOpenFileEvent;

        void draw(Window& window, Input& input) override;
        void open();
};