#pragma once
#include "element_editor_subgui.h"
#include "filter_editor_subgui.h"
#include "gui.h"
#include "window.h"
#include "input.h"
#include "element_base.h"
#include "select_container.h"
#include "select_options.h"
#include "schedule_events.h"
#include "schedule_core.h"
#include "schedule_column.h"

class ScheduleGui : public Gui
{
    private:
        const ScheduleCore& m_scheduleCore;
        void displayColumnContextPopup(unsigned int column, ImGuiTableFlags tableFlags);
    public:
        ScheduleGui(const char* ID, const ScheduleCore& scheduleCore, ScheduleEvents& scheduleEvents);

        // Events
        // setElementValue(column, row, value)
        Event<size_t, size_t, bool>                      setElementValueBool;
        Event<size_t, size_t, int>                       setElementValueNumber;
        Event<size_t, size_t, double>                    setElementValueDecimal;
        Event<size_t, size_t, std::string>        setElementValueText;
        Event<size_t, size_t, SelectContainer>    setElementValueSelect;
        Event<size_t, size_t, WeekdayContainer>    setElementValueWeekday;
        Event<size_t, size_t, TimeContainer>      setElementValueTime;
        Event<size_t, size_t, DateContainer>      setElementValueDate;
        // column add / remove
        Event<size_t> removeColumn;
        Event<size_t> addDefaultColumn;
        // column modification
        Event<size_t, SCHEDULE_TYPE>         setColumnType;
        Event<size_t, COLUMN_SORT>           setColumnSort;
        Event<size_t, std::string>    setColumnName;
        Event<size_t, ColumnResetOption> setColumnResetOption;
        // entire column modification
        Event<size_t, bool> resetColumn;
        // row modification
        Event<size_t> addRow;
        Event<size_t> removeRow;

        void draw(Window& window, Input& input) override;
};