#pragma once

#include "gui.h"
#include "event_pipe.h"
#include "time_wrapper.h"
#include "date_container.h"
#include "schedule_core.h"
#include "schedule_events.h"
#include "schedule_coordinates.h"
#include "element_display_templates.h"
#include "calendar_item_window_subgui.h"
#include <optional>

typedef ScheduleCoordinates TableCoordinates;

class CalendarGui : public Gui {
    private:
        std::shared_ptr<CalendarItemWindowSubGui> m_itemWindowSubGui = nullptr;
        const ScheduleCore& m_scheduleCore;
        TimeWrapper m_viewedMonth;
        TimeWrapper m_selectedDate;
        TimeWrapper m_scheduleDateOverride = TimeWrapper();
        // DRAG & DROP + STATE
        // Contains the best-effort (not perfect) row heights of the table.
        // There are a maximum of 6 rows in a month's calendar.
        // When there are 5, the last one's height is just ignored.
        std::vector<float> m_tableRowHeights = std::vector<float>(6);
        std::vector<float> m_prevTableRowHeights = std::vector<float>(6);
        std::optional<ImGuiID> m_hoveredItemChildID = std::nullopt;
        std::optional<ImGuiID> m_activeItemChildID = std::nullopt;
        std::optional<size_t> m_draggedItemRow = std::nullopt;
        // Contains the table cell coordinates where the dragged item was dropped.
        // Should be cleared along with m_draggedItemRow when this table cell is reached.
        std::optional<TableCoordinates> m_draggedItemDropCell = std::nullopt;

        // Only meaningful while draw() - specifically drawCalendarTable() - is running.
        TableCoordinates m_currentTableCoords = TableCoordinates(0, 0);
        std::optional<TableCoordinates> m_hoveredCellCoords = std::nullopt;

        // If this has a value, the CalendarItemWindowSubGui will be opened and the value passed to it
        std::optional<size_t> m_openItemWindowAtRow = std::nullopt;

        std::function<void(TimeWrapper)> viewedDateChangedListener = [&](TimeWrapper newDateOverride) {
            m_scheduleDateOverride = newDateOverride;
            if (m_itemWindowSubGui) {
                m_itemWindowSubGui->passScheduleDateOverride(newDateOverride);
            }
        };
        std::function<void(std::shared_ptr<const ScheduleEdit>)> editUndoneListener =
            [&](std::shared_ptr<const ScheduleEdit> undoneEdit) {
                if (undoneEdit->getType() == ScheduleEditType::RowAddOrRemove) {
                    auto rowAddOrRemoveEdit = std::dynamic_pointer_cast<const RowEdit>(undoneEdit);
                    // Remove undone -> Row added
                    if (rowAddOrRemoveEdit->getIsRemove()) {
                        rowAddedListener(rowAddOrRemoveEdit->getRow());
                    } else  // Add undone -> Row removed
                    {
                        rowRemovedListener(rowAddOrRemoveEdit->getRow());
                    }
                }
            };
        std::function<void(std::shared_ptr<const ScheduleEdit>)> editRedoneListener =
            [&](std::shared_ptr<const ScheduleEdit> redoneEdit) {
                if (redoneEdit->getType() == ScheduleEditType::RowAddOrRemove) {
                    auto rowAddOrRemoveEdit = std::dynamic_pointer_cast<const RowEdit>(redoneEdit);
                    // Remove redone -> Row removed
                    if (rowAddOrRemoveEdit->getIsRemove()) {
                        rowRemovedListener(rowAddOrRemoveEdit->getRow());
                    } else  // Add redone -> Row added
                    {
                        rowAddedListener(rowAddOrRemoveEdit->getRow());
                    }
                }
            };
        std::function<void(size_t)> rowAddedListener = [&](size_t addedRowIndex) {
            // Item window is open
            if (m_itemWindowSubGui && m_itemWindowSubGui->getCurrentItemRow().has_value()) {
                size_t itemWindowRow = m_itemWindowSubGui->getCurrentItemRow().value();
                if (addedRowIndex > itemWindowRow) {
                    return;
                }
                // A row was added before (or at) the open row -> bump it up by 1
                m_itemWindowSubGui->updateItemRow(itemWindowRow + 1);
            }
        };
        std::function<void(size_t)> rowRemovedListener = [&](size_t removedRowIndex) {
            // Item window is open
            if (m_itemWindowSubGui && m_itemWindowSubGui->getCurrentItemRow().has_value()) {
                size_t itemWindowRow = m_itemWindowSubGui->getCurrentItemRow().value();
                if (removedRowIndex > itemWindowRow) {
                    return;
                }
                // The row for the item that is open in the window was removed -> close the window.
                if (removedRowIndex == itemWindowRow) {
                    m_itemWindowSubGui->close();
                }
                // A row was removed before the open row -> bump it down by 1
                m_itemWindowSubGui->updateItemRow(itemWindowRow - 1);
            }
        };

        // TEMP
        void setRowHeight();
        bool getIsTableCellRectHovered(ImGuiTable* table,
                                       int col = ImGui::TableGetColumnIndex(),
                                       int row = ImGui::TableGetRowIndex()) const;
        void drawWeekdayHeaders(float headerWidth);
        void drawCalendarTable(GuiTextures& guiTextures);
        void drawCalendarDayContent(GuiTextures& guiTextures, size_t& dayIndex, int month, int dayNumber);
        void drawCalendarDayItems(GuiTextures& guiTextures, const DateContainer& calendarDayDate);
        void drawCalendarDayItem(size_t itemRow, GuiTextures& guiTextures, const DateContainer& calendarDayDate);
        void drawItemProperty(ScheduleCoordinates coords);
        template <typename T>
        T getElementValue(ScheduleCoordinates coords, bool useDefaultValue) const {
            return useDefaultValue == true ? Element<T>::getDefaultValue()
                                           : m_scheduleCore.getElementValueConstRef<T>(coords.column(), coords.row());
        }

    public:
        CalendarGui(const char* ID, const ScheduleCore& scheduleCore, ScheduleEvents& scheduleEvents);

        // Events
        // row modification
        Event<size_t> addRow;
        Event<size_t> removeRow;
        Event<size_t> duplicateRow;
        // Event pipes
        EventPipe<size_t, size_t, bool> setElementValueBool;
        EventPipe<size_t, size_t, int> setElementValueNumber;
        EventPipe<size_t, size_t, double> setElementValueDecimal;
        EventPipe<size_t, size_t, std::string> setElementValueText;
        EventPipe<size_t, size_t, SelectContainer> setElementValueSelect;
        EventPipe<size_t, size_t, WeekdayContainer> setElementValueWeekday;
        EventPipe<size_t, size_t, TimeContainer> setElementValueTime;
        EventPipe<size_t, size_t, DateContainer> setElementValueDate;
        // column add / remove
        EventPipe<size_t, SCHEDULE_TYPE> addDefaultColumn;
        EventPipe<size_t> removeColumn;
        EventPipe<size_t> duplicateColumn;
        // column modification
        EventPipe<size_t, SCHEDULE_TYPE> setColumnType;
        EventPipe<size_t, std::string> setColumnName;
        EventPipe<size_t, ColumnResetOption> setColumnResetOption;
        EventPipe<size_t, size_t> setColumnOrder;
        EventPipe<size_t, SelectOptionsModification> modifyColumnSelectOptions;
        // entire column modification
        EventPipe<size_t, bool> resetColumn;

        // FILTER EVENT PIPES
        // FilterGroup
        EventPipe<size_t, FilterGroup> addColumnFilterGroup;
        EventPipe<size_t, size_t, std::string> setColumnFilterGroupName;
        EventPipe<size_t, size_t, LogicalOperatorEnum> setColumnFilterGroupOperator;
        EventPipe<size_t, size_t, bool> setColumnFilterGroupEnabled;
        EventPipe<size_t, size_t> removeColumnFilterGroup;
        // Filter
        EventPipe<size_t, size_t, Filter> addColumnFilter;
        EventPipe<size_t, size_t, size_t, LogicalOperatorEnum> setColumnFilterOperator;
        EventPipe<size_t, size_t, size_t> removeColumnFilter;
        // FilterRule
        EventPipe<size_t, size_t, size_t, FilterRuleContainer> addColumnFilterRule;
        EventPipe<size_t, size_t, size_t, size_t, FilterRuleContainer, FilterRuleContainer> editColumnFilterRule;
        EventPipe<size_t, size_t, size_t, size_t> removeColumnFilterRule;

        void draw(const WindowSize& windowSize, Input& input, GuiTextures& guiTextures) override;
};