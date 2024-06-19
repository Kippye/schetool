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
        GuiEvent<size_t, size_t, WeekdayContainer>    setElementValueWeekday;
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