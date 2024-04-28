#pragma once

#include <gui.h>
#include <schedule.h>

class Schedule;

class EditHistoryGui : public Gui
{
    private:
        Schedule* m_schedule;
    public:
        EditHistoryGui(const char* ID) : Gui(ID) { }
        EditHistoryGui(const char* ID, Schedule*);
        void draw(Window& window, Input& input) override;
};