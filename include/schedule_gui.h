#pragma once
#include <gui.h>
#include <window.h>
#include <schedule.h>

class Schedule;

class ScheduleGui : public Gui
{
    private:
        Schedule* m_schedule;
    public:
        ScheduleGui(const char* ID) : Gui(ID) { }
        ScheduleGui(const char* ID, Schedule*);
        void draw(Window& window) override;
};