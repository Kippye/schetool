#include <vector>
#include <string>
#include "schedule.h"
#include "main_menu_bar/main_menu_bar_gui.h"
#include "view_tab_bar_gui.h"
#include "schedule/element_editor_subgui.h"
#include "schedule/filter_editor_subgui.h"
#include "edit_history_gui.h"

Schedule::Schedule() : m_core(), m_editHistory(m_core), m_scheduleGui(nullptr) {
}

void Schedule::init(Input& input, Interface& interface) {
    m_scheduleGui = interface.addGui<ScheduleGui>("ScheduleGui", m_core, m_scheduleEvents);

    if (m_scheduleGui) {
        m_scheduleGui->addColumnFilterGroup.addListener(addFilterGroupListener);
        m_scheduleGui->removeColumnFilterGroup.addListener(removeFilterGroupListener);
        m_scheduleGui->setColumnFilterGroupName.addListener(setFilterGroupNameListener);
        m_scheduleGui->setColumnFilterGroupOperator.addListener(setFilterGroupOperatorListener);
        m_scheduleGui->setColumnFilterGroupEnabled.addListener(setFilterGroupEnabledListener);
        m_scheduleGui->addColumnFilter.addListener(addFilterListener);
        m_scheduleGui->setColumnFilterOperator.addListener(setFilterOperatorListener);
        m_scheduleGui->removeColumnFilter.addListener(removeFilterListener);
        m_scheduleGui->removeColumnFilterRule.addListener(removeFilterRuleListener);
        m_scheduleGui->addColumnFilterRule.addListener(addFilterRuleListener);
        m_scheduleGui->editColumnFilterRule.addListener(editFilterRuleListener);

        m_scheduleGui->setElementValueBool.addListener(setElementValueListenerBool);
        m_scheduleGui->setElementValueNumber.addListener(setElementValueListenerNumber);
        m_scheduleGui->setElementValueDecimal.addListener(setElementValueListenerDecimal);
        m_scheduleGui->setElementValueText.addListener(setElementValueListenerText);
        m_scheduleGui->setElementValueSelect.addListener(setElementValueListenerSelect);
        m_scheduleGui->setElementValueWeekday.addListener(setElementValueListenerWeekday);
        m_scheduleGui->setElementValueTime.addListener(setElementValueListenerTime);
        m_scheduleGui->setElementValueDate.addListener(setElementValueListenerDate);

        m_scheduleGui->addDefaultColumn.addListener(addDefaultColumnListener);
        m_scheduleGui->removeColumn.addListener(removeColumnListener);
        m_scheduleGui->duplicateColumn.addListener(duplicateColumnListener);
        m_scheduleGui->resetColumn.addListener(resetColumnListener);

        m_scheduleGui->setColumnType.addListener(setColumnTypeListener);
        m_scheduleGui->setColumnSort.addListener(setColumnSortListener);
        m_scheduleGui->setColumnName.addListener(setColumnNameListener);
        m_scheduleGui->setColumnResetOption.addListener(setColumnResetOptionListener);
        m_scheduleGui->setColumnOrder.addListener(setColumnOrderListener);
        m_scheduleGui->createColumnReorderEdit.addListener(createColumnReorderEditListener);
        m_scheduleGui->modifyColumnSelectOptions.addListener(modifyColumnSelectOptionsListener);

        m_scheduleGui->addRow.addListener(addRowListener);
        m_scheduleGui->removeRow.addListener(removeRowListener);
        m_scheduleGui->duplicateRow.addListener(duplicateRowListener);
    }
    m_viewGuis.insert({ScheduleView::Table, m_scheduleGui});

    m_calendarGui = interface.addGui<CalendarGui>("CalendarGui", m_core, m_scheduleEvents);

    if (m_calendarGui) {
        m_calendarGui->addColumnFilterGroup.addListener(addFilterGroupListener);
        m_calendarGui->removeColumnFilterGroup.addListener(removeFilterGroupListener);
        m_calendarGui->setColumnFilterGroupName.addListener(setFilterGroupNameListener);
        m_calendarGui->setColumnFilterGroupOperator.addListener(setFilterGroupOperatorListener);
        m_calendarGui->setColumnFilterGroupEnabled.addListener(setFilterGroupEnabledListener);
        m_calendarGui->addColumnFilter.addListener(addFilterListener);
        m_calendarGui->setColumnFilterOperator.addListener(setFilterOperatorListener);
        m_calendarGui->removeColumnFilter.addListener(removeFilterListener);
        m_calendarGui->removeColumnFilterRule.addListener(removeFilterRuleListener);
        m_calendarGui->addColumnFilterRule.addListener(addFilterRuleListener);
        m_calendarGui->editColumnFilterRule.addListener(editFilterRuleListener);

        m_calendarGui->setElementValueBool.addListener(setElementValueListenerBool);
        m_calendarGui->setElementValueNumber.addListener(setElementValueListenerNumber);
        m_calendarGui->setElementValueDecimal.addListener(setElementValueListenerDecimal);
        m_calendarGui->setElementValueText.addListener(setElementValueListenerText);
        m_calendarGui->setElementValueSelect.addListener(setElementValueListenerSelect);
        m_calendarGui->setElementValueWeekday.addListener(setElementValueListenerWeekday);
        m_calendarGui->setElementValueTime.addListener(setElementValueListenerTime);
        m_calendarGui->setElementValueDate.addListener(setElementValueListenerDate);

        m_calendarGui->addDefaultColumn.addListener(addDefaultColumnListener);
        m_calendarGui->removeColumn.addListener(removeColumnListener);
        m_calendarGui->duplicateColumn.addListener(duplicateColumnListener);
        m_calendarGui->resetColumn.addListener(resetColumnListener);

        m_calendarGui->setColumnType.addListener(setColumnTypeListener);
        m_calendarGui->setColumnName.addListener(setColumnNameListener);
        m_calendarGui->setColumnResetOption.addListener(setColumnResetOptionListener);
        // This is kind of a HACK
        // We merge the ScheduleGui's two events into one by adding both of the listeners to it.
        // It makes sense and it works, but it sure as hell isn't consistent.
        m_calendarGui->setColumnOrder.addListener(setColumnOrderListener);
        m_calendarGui->setColumnOrder.addListener(createColumnReorderEditListener);
        m_calendarGui->modifyColumnSelectOptions.addListener(modifyColumnSelectOptionsListener);

        m_calendarGui->addRow.addListener(addRowListener);
        m_calendarGui->removeRow.addListener(removeRowListener);
        m_calendarGui->duplicateRow.addListener(duplicateRowListener);
    }
    m_viewGuis.insert({ScheduleView::Calendar, m_calendarGui});

    if (auto mainMenuBarGui = interface.getGuiByID<MainMenuBarGui>("MainMenuBarGui")) {
        mainMenuBarGui->undoEvent.addListener(undoListener);
        mainMenuBarGui->redoEvent.addListener(redoListener);
    }

    m_viewTabBarGui = interface.getGuiByID<ViewTabBarGui>("ViewTabBarGui");

    if (m_viewTabBarGui) {
        m_viewTabBarGui->viewSwitched.addListener(viewSwitchListener);
        // Pipe the viewedDateChanged event through an EventPipe in ScheduleEvents so that guis using ScheduleEvents can listen to it easily.
        m_scheduleEvents.viewedDateChanged.addEvent(m_viewTabBarGui->viewedDateChanged);
    }
    if (auto editHistoryGui = interface.getGuiByID<EditHistoryGui>("EditHistoryGui")) {
        editHistoryGui->passScheduleEditHistory(&m_editHistory);
        editHistoryGui->undoEvent.addListener(undoListener);
        editHistoryGui->redoEvent.addListener(redoListener);
    }

    input.addEventListener(INPUT_EVENT_SC_UNDO, undoListener);
    input.addEventListener(INPUT_EVENT_SC_REDO, redoListener);
}

void Schedule::setName(const std::string& name) {
    if (name.size() > schedule_consts::SCHEDULE_NAME_MAX_LENGTH) {
        m_scheduleName = name.substr(0, schedule_consts::SCHEDULE_NAME_MAX_LENGTH);
    } else {
        m_scheduleName = name;
    }
}

void Schedule::updatePreferences(const SchedulePreferences& preferences) {
    // Hide all other views
    for (auto [_, viewGui] : m_viewGuis) {
        viewGui->setVisible(false);
    }
    // Show current view
    if (m_viewGuis.contains(preferences.getView())) {
        m_viewGuis.at(preferences.getView())->setVisible(true);
    } else {
        std::cout << std::format("Schedule::updatePreferences(): No gui for view value {} in viewGuis map",
                                 (unsigned short)preferences.getView())
                  << std::endl;
    }
    m_preferences = preferences;
    // Update view tab bar gui. Does not invoke the event (to avoid an infinite loop)
    m_viewTabBarGui->setSelectedView(m_preferences.getView());
}

SchedulePreferences Schedule::getPreferences() const {
    return m_preferences;
}

void Schedule::hideAllViews() {
    for (auto [_, viewGui] : m_viewGuis) {
        viewGui->setVisible(false);
    }
}

std::string Schedule::getName() {
    return m_scheduleName;
}

const ScheduleEditHistory& Schedule::getEditHistory() {
    return getEditHistoryMutable();
}

ScheduleEditHistory& Schedule::getEditHistoryMutable() {
    return m_editHistory;
}

ScheduleEvents& Schedule::getScheduleEvents() {
    return m_scheduleEvents;
}

void Schedule::undo() {
    size_t prevEditIndex = m_editHistory.getEditHistoryIndex();

    if (m_editHistory.undo()) {
        m_scheduleEvents.editUndone.invoke(m_editHistory.getEditHistory().at(prevEditIndex));
    }
}

void Schedule::redo() {
    if (m_editHistory.redo()) {
        m_scheduleEvents.editRedone.invoke(m_editHistory.getEditHistory().at(m_editHistory.getEditHistoryIndex()));
    }
}

void Schedule::createDefaultSchedule() {
    clearSchedule();
    m_editHistory.clearEditHistory();

    m_core.addColumn(getColumnCount(),
                     Column(std::vector<ElementBase*>{}, SCH_TEXT, std::string("Name"), true, ScheduleColumnFlags_Name));
    m_core.addColumn(getColumnCount(),
                     Column(std::vector<ElementBase*>{},
                            SCH_BOOL,
                            std::string("Finished"),
                            true,
                            ScheduleColumnFlags_Finished,
                            COLUMN_SORT_NONE,
                            SelectOptions(),
                            ColumnResetOption::Daily));
    // Add the default filter to hide finished elements
    Filter isUnfinishedFilter = Filter();
    isUnfinishedFilter.addRule(FilterRule<bool>(false));
    m_core.addColumnFilterGroup(getColumnCount() - 1, FilterGroup({isUnfinishedFilter}, "Hide finished rows"));
    m_core.addColumn(getColumnCount(),
                     Column(std::vector<ElementBase*>{}, SCH_TIME, std::string("Start"), true, ScheduleColumnFlags_Start));
    m_core.addColumn(
        getColumnCount(),
        Column(std::vector<ElementBase*>{}, SCH_TIME, std::string("Duration"), true, ScheduleColumnFlags_Duration));
    m_core.addColumn(getColumnCount(),
                     Column(std::vector<ElementBase*>{}, SCH_TIME, std::string("End"), true, ScheduleColumnFlags_End));
    m_core.addColumn(getColumnCount(),
                     Column(std::vector<ElementBase*>{}, SCH_DATE, std::string("Date"), true, ScheduleColumnFlags_Date));
    m_core.sortColumns();
}

void Schedule::clearSchedule() {
    m_core.clearSchedule();
}

void Schedule::replaceSchedule(std::vector<Column>& columns) {
    m_editHistory.clearEditHistory();

    m_core.replaceSchedule(columns);
}

std::vector<Column> Schedule::getAllColumns() {
    return m_core.getAllColumns();
}

// Sorts every column's rows based on "sorter" columns
void Schedule::sortColumns() {
    m_core.sortColumns();
}

bool Schedule::checkPassesAllFilters(size_t row,
                                     const std::optional<TimeWrapper>& currentTime,
                                     const std::vector<size_t>& ignoredColumnIndices) const {
    return m_core.checkPassesAllFilters(row, currentTime, ignoredColumnIndices);
}

size_t Schedule::getColumnCount() {
    return m_core.getColumnCount();
}

// Add a column from previous data. NOTE: Creates copies of all passed values, because this will probably mostly be used for duplicating columns
void Schedule::addColumn(size_t columnIndex, const Column& column, bool addToHistory) {
    if (addToHistory) {
        m_editHistory.addEdit<ColumnEdit>(false, columnIndex, column);
    }

    m_core.addColumn(columnIndex, column);

    m_scheduleEvents.columnAdded.invoke(columnIndex);
}

void Schedule::addDefaultColumn(size_t columnIndex, SCHEDULE_TYPE colType, bool addToHistory) {
    size_t columnCountBefore = getColumnCount();
    m_core.addDefaultColumn(columnIndex, colType);

    if (addToHistory) {
        if (getColumnCount() > columnCountBefore && columnIndex < m_core.getColumnCount()) {
            m_editHistory.addEdit<ColumnEdit>(false, columnIndex, m_core.getColumnConst(columnIndex));
        }
    }

    m_scheduleEvents.columnAdded.invoke(columnIndex);
}

void Schedule::removeColumn(size_t columnIndex, bool addToHistory) {
    if (m_core.getColumnConst(columnIndex).permanent) {
        return;
    }

    Column columnCopy = m_core.getColumnConst(columnIndex);

    // oh cheese.
    if (m_core.removeColumn(columnIndex)) {
        if (addToHistory) {
            m_editHistory.addEdit<ColumnEdit>(true, columnIndex, columnCopy);
        }

        m_scheduleEvents.columnRemoved.invoke(columnIndex);
    }
}

void Schedule::duplicateColumn(size_t columnIndex, bool addToHistory) {
    auto duplicateColumnIndex = m_core.duplicateColumn(columnIndex);

    if (duplicateColumnIndex.has_value()) {
        if (addToHistory) {
            m_editHistory.addEdit<ColumnEdit>(
                false, duplicateColumnIndex.value(), m_core.getColumnConst(duplicateColumnIndex.value()));
        }
        m_scheduleEvents.columnAdded.invoke(duplicateColumnIndex.value());
    }
}

const Column& Schedule::getColumnConst(size_t column) {
    return m_core.getColumnConst(column);
}

size_t Schedule::getFlaggedColumnIndex(ScheduleColumnFlags flags) const {
    return m_core.getFlaggedColumnIndex(flags);
}

void Schedule::setColumnType(size_t columnIndex, SCHEDULE_TYPE type, bool addToHistory) {
    // for adding to edit history
    Column previousData = Column(m_core.getColumnConst(columnIndex));

    if (m_core.setColumnType(columnIndex, type)) {
        if (addToHistory) {
            m_editHistory.addEdit<ColumnPropertyEdit>(
                columnIndex, COLUMN_PROPERTY_TYPE, previousData, m_core.getColumnConst(columnIndex));
        }
    }
}

void Schedule::setColumnName(size_t columnIndex, const std::string& name, bool addToHistory) {
    Column previousData = Column(m_core.getColumnConst(columnIndex));

    if (m_core.setColumnName(columnIndex, name)) {
        if (addToHistory) {
            m_editHistory.addEdit<ColumnPropertyEdit>(
                columnIndex, COLUMN_PROPERTY_NAME, previousData, m_core.getColumnConst(columnIndex));
        }
    }
}

void Schedule::setColumnSort(size_t columnIndex, COLUMN_SORT sortDirection, bool addToHistory) {
    Column previousData = Column(m_core.getColumnConst(columnIndex));

    if (m_core.setColumnSort(columnIndex, sortDirection)) {
        if (addToHistory) {
            m_editHistory.addEdit<ColumnPropertyEdit>(
                columnIndex, COLUMN_PROPERTY_SORT, previousData, m_core.getColumnConst(columnIndex));
        }
    }
}

void Schedule::setColumnResetOption(size_t columnIndex, ColumnResetOption option, bool addToHistory) {
    Column previousData = Column(m_core.getColumnConst(columnIndex));

    if (m_core.setColumnResetOption(columnIndex, option)) {
        if (addToHistory) {
            m_editHistory.addEdit<ColumnPropertyEdit>(
                columnIndex, COLUMN_PROPERTY_RESET_OPTION, previousData, m_core.getColumnConst(columnIndex));
        }
    }
}

void Schedule::setColumnDisplayOrder(size_t oldOrder, size_t newOrder, bool addToHistory) {
    if (m_core.setColumnDisplayOrder(oldOrder, newOrder)) {
        if (addToHistory) {
            m_editHistory.addEdit<ColumnReorderEdit>(oldOrder, newOrder);
        }
    }
}

const SelectOptions& Schedule::getColumnSelectOptions(size_t column) {
    return m_core.getColumnSelectOptions(column);
}

void Schedule::modifyColumnSelectOptions(size_t columnIndex,
                                         const SelectOptionsModification& selectOptionsModification,
                                         bool addToHistory) {
    Column previousData = Column(m_core.getColumnConst(columnIndex));

    if (m_core.modifyColumnSelectOptions(columnIndex, selectOptionsModification)) {
        if (addToHistory) {
            m_editHistory.addEdit<ColumnPropertyEdit>(
                columnIndex, COLUMN_PROPERTY_SELECT_OPTIONS, previousData, m_core.getColumnConst(columnIndex));
        }
    }
}

void Schedule::addColumnFilterGroup(size_t column, FilterGroup filterGroup, bool addToHistory) {
    if (m_core.addColumnFilterGroup(column, filterGroup)) {
        if (addToHistory) {
            size_t groupIndex = m_core.getColumnConst(column).getFilterGroupCount() - 1;
            m_editHistory.addEdit<FilterGroupAddOrRemoveEdit>(false, column, groupIndex, filterGroup);
        }
    }
}

void Schedule::removeColumnFilterGroup(size_t column, size_t groupIndex, bool addToHistory) {
    FilterGroup filterGroup = m_core.getColumnConst(column).getFilterGroupConst(groupIndex);

    if (m_core.removeColumnFilterGroup(column, groupIndex)) {
        if (addToHistory) {
            m_editHistory.addEdit<FilterGroupAddOrRemoveEdit>(true, column, groupIndex, filterGroup);
        }
    }
}

void Schedule::setColumnFilterGroupName(size_t columnIndex, size_t groupIndex, const std::string& name, bool addToHistory) {
    const auto& filterGroup = m_core.getColumnConst(columnIndex).getFilterGroupConst(groupIndex);
    LogicalOperatorEnum logicalOperator = filterGroup.getOperatorType();
    std::string prevName = filterGroup.getName();
    bool enabled = filterGroup.getIsEnabled();

    if (m_core.setColumnFilterGroupName(columnIndex, groupIndex, name)) {
        if (addToHistory) {
            // edit for name, keep operator the same
            m_editHistory.addEdit<FilterGroupChangeEdit>(
                columnIndex, groupIndex, logicalOperator, prevName, enabled, filterGroup);
        }
    }
}

void Schedule::setColumnFilterGroupOperator(size_t columnIndex,
                                            size_t groupIndex,
                                            LogicalOperatorEnum logicalOperator,
                                            bool addToHistory) {
    const auto& filterGroup = m_core.getColumnConst(columnIndex).getFilterGroupConst(groupIndex);
    LogicalOperatorEnum prevOperator = filterGroup.getOperatorType();
    std::string name = filterGroup.getName();
    bool enabled = filterGroup.getIsEnabled();

    if (m_core.setColumnFilterGroupOperator(columnIndex, groupIndex, logicalOperator)) {
        if (addToHistory) {
            // edit for operator, keep name the same
            m_editHistory.addEdit<FilterGroupChangeEdit>(columnIndex, groupIndex, prevOperator, name, enabled, filterGroup);
        }
    }
}

void Schedule::setColumnFilterGroupEnabled(size_t columnIndex, size_t groupIndex, bool enabled, bool addToHistory) {
    const auto& filterGroup = m_core.getColumnConst(columnIndex).getFilterGroupConst(groupIndex);
    LogicalOperatorEnum logicalOperator = filterGroup.getOperatorType();
    std::string name = filterGroup.getName();
    bool prevEnabled = filterGroup.getIsEnabled();

    if (m_core.setColumnFilterGroupEnabled(columnIndex, groupIndex, enabled)) {
        if (addToHistory) {
            // edit for operator, keep name the same
            m_editHistory.addEdit<FilterGroupChangeEdit>(
                columnIndex, groupIndex, logicalOperator, name, prevEnabled, filterGroup);
        }
    }
}

void Schedule::addColumnFilter(size_t columnIndex, size_t groupIndex, Filter filter, bool addToHistory) {
    if (m_core.addColumnFilter(columnIndex, groupIndex, filter)) {
        if (addToHistory) {
            size_t filterIndex = m_core.getColumnConst(columnIndex).getFilterGroupConst(groupIndex).getFilterCount() - 1;
            m_editHistory.addEdit<FilterAddOrRemoveEdit>(false, columnIndex, groupIndex, filterIndex, filter);
        }
    }
}

void Schedule::setColumnFilterOperator(
    size_t columnIndex, size_t groupIndex, size_t filterIndex, LogicalOperatorEnum logicalOperator, bool addToHistory) {
    LogicalOperatorEnum prevOperator =
        m_core.getColumnConst(columnIndex).getFilterGroupConst(groupIndex).getFilterConst(filterIndex).getOperatorType();

    if (m_core.setColumnFilterOperator(columnIndex, groupIndex, filterIndex, logicalOperator)) {
        if (addToHistory) {
            m_editHistory.addEdit<FilterChangeEdit>(columnIndex, groupIndex, filterIndex, prevOperator, logicalOperator);
        }
    }
}

void Schedule::removeColumnFilter(size_t columnIndex, size_t groupIndex, size_t filterIndex, bool addToHistory) {
    Filter filter = m_core.getColumnConst(columnIndex).getFilterGroupConst(groupIndex).getFilterConst(filterIndex);

    if (m_core.removeColumnFilter(columnIndex, groupIndex, filterIndex)) {
        if (addToHistory) {
            m_editHistory.addEdit<FilterAddOrRemoveEdit>(true, columnIndex, groupIndex, filterIndex, filter);
        }
    }
}

void Schedule::resetColumn(size_t columnIndex, bool addToHistory) {
    Column columnData = m_core.getColumnConst(columnIndex);

    m_core.resetColumn(columnIndex);

    if (addToHistory) {
        m_editHistory.addEdit<ColumnResetEdit>(columnIndex, columnData);
    }
}

// Return the number of rows in the schedule or 0 if there are no columns or no rows (which probably won't happen?)
size_t Schedule::getRowCount() {
    return m_core.getRowCount();
}

void Schedule::addRow(bool addToHistory) {
    addRow(m_core.getRowCount(), addToHistory);
}

void Schedule::addRow(size_t rowIndex, bool addToHistory) {
    m_core.addRow(rowIndex);

    if (addToHistory) {
        m_editHistory.addEdit<RowEdit>(false, rowIndex, m_core.getRow(rowIndex));
    }

    m_scheduleEvents.rowAdded.invoke(rowIndex);
}

void Schedule::removeRow(size_t rowIndex, bool addToHistory) {
    std::vector<ElementBase*> originalRow = m_core.getRow(rowIndex);
    // temporary vector of copies
    std::vector<ElementBase*> originalRowCopies = {};

    for (size_t i = 0; i < originalRow.size(); i++) {
        originalRowCopies.push_back(originalRow[i]->getCopy());
    }

    if (m_core.removeRow(rowIndex)) {
        if (addToHistory) {
            // add a remove RowEdit to the edit history with copies of the removed Elements
            m_editHistory.addEdit<RowEdit>(true, rowIndex, originalRowCopies);
        }
        m_scheduleEvents.rowRemoved.invoke(rowIndex);
    }

    for (size_t i = 0; i < originalRowCopies.size(); i++) {
        delete originalRowCopies[i];
    }
}

void Schedule::duplicateRow(size_t rowIndex, bool addToHistory) {
    auto duplicateRowIndex = m_core.duplicateRow(rowIndex);

    if (duplicateRowIndex.has_value()) {
        if (addToHistory) {
            m_editHistory.addEdit<RowEdit>(false, duplicateRowIndex.value(), m_core.getRow(duplicateRowIndex.value()));
        }
        m_scheduleEvents.rowAdded.invoke(rowIndex);
    }
}

std::vector<ElementBase*> Schedule::getRow(size_t index) {
    return m_core.getRow(index);
}

void Schedule::setRow(size_t index, std::vector<ElementBase*> elementData) {
    m_core.setRow(index, elementData);
}

std::vector<size_t> Schedule::getSortedRowIndices() {
    return m_core.getSortedRowIndices();
}

void Schedule::applyColumnTimeBasedReset(size_t columnIndex) {
    // Reset the column and do not add to history since the user didn't make the edit.
    resetColumn(columnIndex, false);

    // Reset the viewed date override just in case.
    if (m_viewTabBarGui) {
        m_viewTabBarGui->clearDateOverride();
    }
}