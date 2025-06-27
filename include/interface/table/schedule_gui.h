#pragma once
#include "gui.h"
#include "input.h"
#include "select_container.h"
#include "schedule_events.h"
#include "schedule_core.h"
#include "schedule_column.h"
#include "schedule_coordinates.h"
#include "event_pipe.h"
#include <optional>
#include <functional>

class ScheduleGui : public Gui {
    private:
        static const ImGuiTable* scheduleTable;
        const ScheduleCore& m_scheduleCore;
        bool m_nextMouseReleaseOpenColumnContext = true;
        unsigned int m_filterGroupListColumn = 0;
        std::optional<size_t> m_rowContextRow = std::nullopt;
        std::optional<ScheduleCoordinates> m_cellContextCoords = std::nullopt;
        std::optional<size_t> m_draggedRow = std::nullopt;
        TimeWrapper m_scheduleDateOverride = TimeWrapper();

        std::function<void(TimeWrapper)> viewedDateChangedListener = [&](TimeWrapper newDateOverride) {
            m_scheduleDateOverride = newDateOverride;
        };

        void drawScheduleTable(const WindowSize& windowSize, Input& input, GuiTextures& guiTextures);
        // Draws the contents of the table cell at the given column and row.
        // Returns true if the row should be continued, false if it was removed or cancelled for some other reason.
        bool drawTableCellContents(
            size_t column, size_t row, const WindowSize& windowSize, Input& input, GuiTextures& guiTextures);
        void drawColumnHeaderContext(size_t column, ImGuiTable* table, ImGuiTableFlags tableFlags);
        void openRowContextPopup(size_t row);
        void drawRowContext();
        void openCellContextPopup(size_t column, size_t row);
        void drawCellContext();

        template <typename T>
        T getElementValue(size_t column, size_t row, bool useDefaultValue) const {
            return useDefaultValue == true ? Element<T>::getDefaultValue()
                                           : m_scheduleCore.getElementValueConstRef<T>(column, row);
        }

    public:
        ScheduleGui(const char* ID, const ScheduleCore& scheduleCore, ScheduleEvents& scheduleEvents);

        // Events
        // setElementValue(column, row, value)
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
        Event<size_t> duplicateColumn;
        // column modification
        Event<size_t, SCHEDULE_TYPE> setColumnType;
        Event<size_t, COLUMN_SORT> setColumnSort;
        Event<size_t, std::string> setColumnName;
        Event<size_t, ColumnResetOption> setColumnResetOption;
        // entire column modification
        Event<size_t, bool> resetColumn;
        // row modification
        Event<size_t> addRow;
        Event<size_t> removeRow;
        Event<size_t> duplicateRow;
        // Event pipes
        EventPipe<size_t, SelectOptionsModification> modifyColumnSelectOptions;

        bool isEditableElementClicked(bool isEditingDisabled) const;

        void draw(const WindowSize& windowSize, Input& input, GuiTextures& guiTextures) override;

        static const ImGuiTable* getScheduleTable();
};