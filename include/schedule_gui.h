#pragma once
#include "element_editor_subgui.h"
#include "filter_editor_subgui.h"
#include <gui.h>
#include <window.h>
#include <input.h>
#include <element_base.h>
#include <select_container.h>
#include <select_options.h>
#include <schedule_core.h>
#include <schedule_column.h>

class ScheduleGui : public Gui
{
    private:
        const ScheduleCore* m_scheduleCore = NULL;

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
    public:
        ScheduleGui(const char* ID) : Gui(ID) { }

        // Events
        // setElementValue(column, row, value)
        GuiEvent<size_t, size_t, bool>                      setElementValueBool;
        GuiEvent<size_t, size_t, int>                       setElementValueNumber;
        GuiEvent<size_t, size_t, double>                    setElementValueDecimal;
        GuiEvent<size_t, size_t, std::string>        setElementValueText;
        GuiEvent<size_t, size_t, SelectContainer>    setElementValueSelect;
        GuiEvent<size_t, size_t, TimeContainer>      setElementValueTime;
        GuiEvent<size_t, size_t, DateContainer>      setElementValueDate;
        // column add / remove
        GuiEvent<size_t> removeColumn;
        GuiEvent<size_t> addDefaultColumn;
        // column modification
        GuiEvent<size_t, SCHEDULE_TYPE>         setColumnType;
        GuiEvent<size_t, COLUMN_SORT>           setColumnSort;
        GuiEvent<size_t, std::string>    setColumnName;
        // row modification
        GuiEvent<size_t> addRow;
        GuiEvent<size_t> removeRow;

        // ScheduleGui(const char* ID, Schedule*);
        void setScheduleCore(const ScheduleCore& scheduleCore);
        void draw(Window& window, Input& input) override;
};