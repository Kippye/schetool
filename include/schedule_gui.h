#pragma once
#include "imgui.h"
#include <gui.h>
#include <window.h>
#include <schedule.h>
#include <time_container.h>

class Schedule;

class ScheduleGui : public Gui
{
    private:
        Schedule* m_schedule;
        int m_timeEditorColumn = -1;
        int m_timeEditorRow = -1;
        tm m_timeEditorTime;
        ImRect m_timeEditorAvoidRect;
        ImVec4 m_dayColours[7] =
        {
            ImVec4(0.0f / 255.0f, 62.0f / 255.0f, 186.0f / 255.0f, 1),
            ImVec4(198.0f / 255.0f, 138.0f / 255.0f, 0.0f / 255.0f, 1),
            ImVec4(0.0f / 255.0f, 160.0f / 255.0f, 16.0f / 255.0f, 1),
            ImVec4(86.0f / 255.0f, 47.0f / 255.0f, 0.0f / 255.0f, 1),
            ImVec4(181.0f / 255.0f, 43.0f / 255.0f, 43.0f / 255.0f, 1),
            ImVec4(94.0f / 255.0f, 60.0f / 255.0f, 188.0f / 255.0f, 1),
            ImVec4(216.0f / 255.0f, 188.0f / 255.0f, 47.0f / 255.0f, 1),
        };
        void displayColumnContextPopup(unsigned int column, ImGuiTableFlags tableFlags);
        bool displayTimeEditor(Time& value);
    public:
        ScheduleGui(const char* ID) : Gui(ID) { }
        ScheduleGui(const char* ID, Schedule*);
        void draw(Window& window) override;
        static int filterNumbers(ImGuiInputTextCallbackData* data);
};