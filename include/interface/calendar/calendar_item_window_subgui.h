#pragma once

#include "gui.h"
#include "schedule_core.h"
#include "schedule_coordinates.h"
#include "schedule_events.h"
#include "event_pipe.h"
#include <optional>

class CalendarItemWindowSubGui : public Gui {
    private:
        const unsigned short PROPERTY_NAME_VISIBLE_CHARS = 24;
        const ScheduleCore& m_scheduleCore;
        TimeWrapper m_scheduleDateOverride;
        std::optional<size_t> m_currentItemRow = std::nullopt;
        // Properties reordering state
        std::optional<size_t> m_draggedPropertyColumn = std::nullopt;
        std::optional<size_t> m_draggedPropertySrcOrder = std::nullopt;
        std::optional<size_t> m_draggedPropertyOrder = std::nullopt;

        bool m_editingItemNameJustStarted = false;
        bool m_editingItemName = false;

        bool isEditablePropertyClicked(bool isEditingDisabled) const;
        void drawItemProperty(GuiDrawArgs& drawArgs, ScheduleCoordinates coords);
        void drawPropertyContext(size_t col, bool& needToBreak);
        template <typename T>
        T getElementValue(ScheduleCoordinates coords, bool useDefaultValue) const {
            return useDefaultValue == true ? Element<T>::getDefaultValue()
                                           : m_scheduleCore.getElementValueConstRef<T>(coords.column(), coords.row());
        }

    public:
        CalendarItemWindowSubGui(const char* ID, const ScheduleCore& scheduleCore, ScheduleEvents& scheduleEvents);

        // Events
        Event<size_t, size_t, bool> setElementValueBool;
        Event<size_t, size_t, int> setElementValueNumber;
        Event<size_t, size_t, double> setElementValueDecimal;
        Event<size_t, size_t, std::string> setElementValueText;
        Event<size_t, size_t, SingleSelectContainer> setElementValueSelect;
        Event<size_t, size_t, SelectContainer> setElementValueMultiselect;
        Event<size_t, size_t, WeekdayContainer> setElementValueWeekday;
        Event<size_t, size_t, TimeContainer> setElementValueTime;
        Event<size_t, size_t, DateContainer> setElementValueDate;
        // column add / remove
        Event<size_t, SCHEDULE_TYPE> addDefaultColumn;
        Event<size_t> removeColumn;
        Event<size_t> duplicateColumn;
        // column modification
        Event<size_t, SCHEDULE_TYPE> setColumnType;
        Event<size_t, std::string> setColumnName;
        Event<size_t, ColumnResetOption> setColumnResetOption;
        Event<size_t, size_t> setColumnOrder;
        Event<size_t, bool> resetColumn;
        // Event pipes
        EventPipe<size_t, SelectOptionsModification> modifyColumnSelectOptions;

        void draw(GuiDrawArgs& args) override;
        std::optional<size_t> getCurrentItemRow() const;
        void passScheduleDateOverride(const TimeWrapper& dateOverride);
        void open(size_t itemRow);
        // Use only to update the item row when rows are added / removed while the window is open
        // To open the window with a specific item row, use open() instead.
        void updateItemRow(size_t newItemRow);
        // Clear item row and close the popup
        void close();
};