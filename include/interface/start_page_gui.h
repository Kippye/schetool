#pragma once

#include "gui.h"
#include "window.h"
#include "input.h"

class StartPageGui : public Gui
{
    private:

    public:
        StartPageGui(const char* ID);

        void draw(Window& window, Input& input) override;
};