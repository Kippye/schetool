#pragma once
#include <imgui.h>
#include <gui.h>
#include <window.h>
#include <io_handler.h>

class MainMenuBarGui : public Gui
{
    private:
        IO_Handler* m_ioHandler;
    public:
        MainMenuBarGui(const char* ID) : Gui(ID) {}
        MainMenuBarGui(const char* ID, IO_Handler* ioHandler);
        void draw(Window& window) override;
};