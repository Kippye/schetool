#pragma once

#include "gui.h"
#include "time_wrapper.h"
#include "date_container.h"
#include "schedule_core.h"
#include "schedule_events.h"
#include "schedule_coordinates.h"
#include "element_display_templates.h"
#include <optional>

typedef ScheduleCoordinates TableCoordinates;

class CalendarGui : public Gui {
    private:
        const ScheduleCore& m_scheduleCore;
        TimeWrapper m_viewedMonth;
        TimeWrapper m_selectedDate;
        TimeWrapper m_scheduleDateOverride = TimeWrapper();
        std::optional<ImGuiID> m_hoveredItemChildID;
        // Only meaningful while draw() - specifically drawCalendarTable() - is running.
        TableCoordinates m_currentTableCoords = TableCoordinates(0, 0);
        // TEMP?
        std::optional<GuiPassReferences> m_guiPass = std::nullopt;

        std::function<void(TimeWrapper)> viewedDateChangedListener = [&](TimeWrapper newDateOverride) {
            m_scheduleDateOverride = newDateOverride;
        };

        void drawWeekdayHeaders(float headerWidth);
        void drawCalendarTable();
        void drawCalendarDayContent(size_t& dayIndex, int month, int dayNumber);
        void drawCalendarDayItems(const DateContainer& calendarDayDate);
        void drawItemProperty(ScheduleCoordinates coords);
        template <typename T>
        T getElementValue(ScheduleCoordinates coords, bool useDefaultValue) const {
            return useDefaultValue == true ? Element<T>::getDefaultValue()
                                           : m_scheduleCore.getElementValueConstRef<T>(coords.column(), coords.row());
        }

    public:
        CalendarGui(const char* ID, const ScheduleCore& scheduleCore, ScheduleEvents& scheduleEvents);

        // Events
        Event<size_t, size_t, bool> setElementValueBool;
        Event<size_t, size_t, int> setElementValueNumber;
        Event<size_t, size_t, double> setElementValueDecimal;
        Event<size_t, size_t, std::string> setElementValueText;
        Event<size_t, size_t, SelectContainer> setElementValueSelect;
        Event<size_t, size_t, WeekdayContainer> setElementValueWeekday;
        Event<size_t, size_t, TimeContainer> setElementValueTime;
        Event<size_t, size_t, DateContainer> setElementValueDate;
        // column add / remove
        Event<size_t, SCHEDULE_TYPE> addDefaultColumn;
        Event<size_t> removeColumn;
        // Event<size_t> duplicateColumn;
        // column modification
        Event<size_t, SCHEDULE_TYPE> setColumnType;
        Event<size_t, std::string> setColumnName;
        Event<size_t, ColumnResetOption> setColumnResetOption;
        // entire column modification
        Event<size_t, bool> resetColumn;
        // row modification
        Event<size_t> addRow;
        Event<size_t> removeRow;
        Event<size_t> duplicateRow;

        void draw(const WindowSize& windowSize, Input& input, GuiTextures& guiTextures) override;
};