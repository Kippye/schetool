#pragma once

#include "gui.h"
#include "event.h"

class ViewTabBarGui : public Gui {
    private:
        size_t m_selectedTab = 0;

    public:
        ViewTabBarGui(const char* ID);

        Event<size_t> viewSwitched;

        void draw(const WindowSize& windowSize, Input& input, GuiTextures& guiTextures) override;
};