#pragma once
#include <gui.h>
#include <window.h>
#include <schedule.h>

class Schedule;

// TEMP
#include <vector>
#include <deque>

class ScheduleGui : public Gui
{
    private:
        Schedule* m_schedule;
    public:
        ScheduleGui(const char* ID) : Gui(ID) { }
        ScheduleGui(const char* ID, Schedule*);
        // TEMP
        std::deque<bool> finishStates = std::deque(12, bool(false));
        void draw(Window& window) override;
};