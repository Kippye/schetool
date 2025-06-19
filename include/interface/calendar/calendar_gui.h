#pragma once

#include "gui.h"

class CalendarGui : public Gui {
    private:
    public:
        CalendarGui(const char* ID);
        void draw(const WindowSize& windowSize, Input& input, GuiTextures& guiTextures) override;
};