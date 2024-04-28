#pragma once
#include "element_base.h"
#include "imgui.h"
#include <gui.h>
#include <window.h>
#include <schedule.h>

class Schedule;

class ScheduleGui : public Gui
{
    private:
        Schedule* m_schedule;
        // time editor data (should these be saved somewhere like a struct? i've always thought of making it its separate file, too)
        bool m_editorOpenLastFrame = false;
        bool m_editorOpenThisFrame = false;
        bool m_editorHasMadeEdits = false; 
        int m_editorColumn = -1;
        int m_editorRow = -1;
        unsigned int m_editorViewedYear = 0;
        unsigned int m_editorViewedMonth = 0;
        TimeContainer m_editorTime;
        DateContainer m_editorDate;
        SelectContainer m_editorSelect;
        ImRect m_editorAvoidRect;

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
        bool displayEditor(SCHEDULE_TYPE type);
    public:
        ScheduleGui(const char* ID) : Gui(ID) { }
        ScheduleGui(const char* ID, Schedule*);
        void draw(Window& window, Input& input) override;
        static int filterNumbers(ImGuiInputTextCallbackData* data);
};