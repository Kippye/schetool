#pragma once
#include <map>
#include "gui.h"
#include "window.h"
#include "input.h"
#include "main_menu_bar_gui.h"
#include "element_base.h"
#include "select_container.h"
#include "single_select_container.h"
#include "schedule_events.h"
#include "schedule_core.h"
#include "schedule_column.h"

class ScheduleGui : public Gui
{
    private:
        const ScheduleCore& m_scheduleCore;
        const std::shared_ptr<const MainMenuBarGui> m_mainMenuBarGui = nullptr;
        bool m_openDateSelectPopup = false;
        bool m_nextMouseReleaseOpenColumnContext = true;
        unsigned int m_dateSelectorYear = 1, m_dateSelectorMonth = 1;
        unsigned int m_filterGroupListColumn = 0;
        TimeWrapper m_scheduleDateOverride = TimeWrapper();
        void displayColumnContextPopup(unsigned int column, ImGuiTable* table, ImGuiTableFlags tableFlags);
        template <typename T>
        T getElementValue(size_t column, size_t row, bool useDefaultValue) const
        {
            return useDefaultValue == true ? Element<T>::getDefaultValue() : m_scheduleCore.getElementValueConstRef<T>(column, row);
        }
    public:
        ScheduleGui(const char* ID, const ScheduleCore& scheduleCore, ScheduleEvents& scheduleEvents, const std::shared_ptr<const MainMenuBarGui> mainMenuBarGui);

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

        bool isEditableElementClicked(bool isEditingDisabled) const;

        void draw(Window& window, Input& input, GuiTextures& guiTextures) override;
        void clearDateOverride();
};