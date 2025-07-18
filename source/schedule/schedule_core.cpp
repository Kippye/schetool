#include <algorithm>
#include <string>
#include <vector>
#include <numeric>
#include "schedule_core.h"
#include "element_base.h"
#include "util.h"

ScheduleCore::ScheduleCore() {
}

size_t ScheduleCore::getFlaggedColumnIndex(ScheduleColumnFlags flags) const {
    for (size_t i = 0; i < getColumnCount(); i++) {
        if (getColumnConst(i).flags & flags) {
            return i;
        }
    }
    return 0;
}

Column& ScheduleCore::getColumnWithFlags(ScheduleColumnFlags flags) {
    return getColumn(getFlaggedColumnIndex(flags));
}

Column& ScheduleCore::getColumn(size_t column) {
    if (column > getColumnCount()) {
        throw std::out_of_range(std::format("ScheduleCore::getMutableColumn: column index {} is out of range.", column));
    }
    return m_schedule.at(m_columnDisplayOrder.at(column));
}

// Get the sorted indices of the column's rows. The vector contains which index of the rows vector corresponds to that position in the SORTED rows
std::vector<size_t> ScheduleCore::getColumnSortedNewIndices(size_t index) {
    return getColumnConst(index).getSortedIndices();
}

void ScheduleCore::clearSchedule() {
    m_schedule.clear();
    m_columnDisplayOrder.clear();
    sortColumns();
}

void ScheduleCore::replaceSchedule(std::vector<Column>& columns) {
    clearSchedule();

    m_schedule = columns;

    m_columnDisplayOrder.resize(m_schedule.size());
    // Don't know the order so just reset it to 0...columnCount
    std::iota(m_columnDisplayOrder.begin(), m_columnDisplayOrder.end(), 0);
    sortColumns();
}

std::vector<Column> ScheduleCore::getAllColumns() const {
    std::vector<Column> displayOrderSchedule = {};

    for (size_t internalIndex : m_columnDisplayOrder) {
        displayOrderSchedule.push_back(m_schedule.at(internalIndex));
    }
    return displayOrderSchedule;
}

// Sorts every column's rows based on "sorter" columns
void ScheduleCore::sortColumns() {
    for (size_t sorterColumn = 0; sorterColumn < getColumnCount(); sorterColumn++) {
        if (getColumn(sorterColumn).sort != COLUMN_SORT_NONE) {
            m_sortedRowIndices = getColumnSortedNewIndices(sorterColumn);
            break;
        }
        // Last column is unsorted too which means we should not sort
        else if (sorterColumn == getColumnCount() - 1)
        {
            m_sortedRowIndices.resize(getRowCount());
            std::iota(m_sortedRowIndices.begin(), m_sortedRowIndices.end(), 0);
        }
    }
}

bool ScheduleCore::checkPassesAllFilters(size_t row,
                                         const std::optional<TimeWrapper>& currentTime,
                                         const std::vector<size_t>& ignoredColumnIndices) const {
    for (size_t col = 0; col < m_schedule.size(); col++) {
        // Skip if this is a column to ignore
        if (std::find(ignoredColumnIndices.begin(), ignoredColumnIndices.end(), col) != ignoredColumnIndices.end()) {
            continue;
        }
        const Column& column = m_schedule.at(col);
        // Check if the row's Element passes every FilterGroup in this Column
        bool passesAllFilters = column.checkElementPassesFilters(
            row,
            currentTime  // Pass override date as current (Uses TimeWrapper::getCurrentTime() if it's empty)
        );
        if (passesAllFilters == false) {
            return false;
        }
    }
    return true;
}

size_t ScheduleCore::getColumnCount() const {
    return m_schedule.size();
}

// Check if the index is less than size. If not, a general "index out of range" error is printed
bool ScheduleCore::existsColumnAtIndex(size_t index) const {
    if (index < getColumnCount() == false) {
        std::cout << std::format(
                         "ScheduleCore::existsColumnAtIndex({}): Index not less than size ({})", index, getColumnCount())
                  << std::endl;
        return false;
    }
    return true;
}

std::optional<size_t> ScheduleCore::getInternalIndexFor(size_t displayOrder) const {
    if (existsColumnAtIndex(displayOrder) == false) {
        return std::nullopt;
    }

    return m_columnDisplayOrder.at(displayOrder);
}

// Add a column from previous data. NOTE: Creates copies of all passed values, because this will probably mostly be used for duplicating columns
void ScheduleCore::addColumn(size_t index, const Column& column) {
    // TODO: make sure that EVERY column has the same amount of rows!!!
    // TODO: give the new column correct creation date & time

    m_schedule.emplace_back(column);
    m_columnDisplayOrder.insert(m_columnDisplayOrder.begin() + index, m_schedule.size() - 1);

    // Sort columns just in case, because the added Column could have a sort other than COLUMN_SORT_NONE
    sortColumns();
}

void ScheduleCore::addDefaultColumn(size_t index, SCHEDULE_TYPE columnType) {
    Column addedColumn = Column({}, columnType, schedule_consts::scheduleTypeNames.at(columnType), false);

    for (size_t i = 0; i < getRowCount(); i++) {
        switch (columnType) {
            case (SCH_BOOL): {
                addedColumn.addElement(Element(columnType, Element<bool>::getDefaultValue(), TimeWrapper::getCurrentTime()));
                break;
            }
            case (SCH_NUMBER): {
                addedColumn.addElement(Element(columnType, Element<int>::getDefaultValue(), TimeWrapper::getCurrentTime()));
                break;
            }
            case (SCH_DECIMAL): {
                addedColumn.addElement(Element(columnType, Element<double>::getDefaultValue(), TimeWrapper::getCurrentTime()));
                break;
            }
            case (SCH_TEXT): {
                addedColumn.addElement(
                    Element(columnType, Element<std::string>::getDefaultValue(), TimeWrapper::getCurrentTime()));
                break;
            }
            case (SCH_SELECT): {
                addedColumn.addElement(
                    Element(columnType, Element<SingleSelectContainer>::getDefaultValue(), TimeWrapper::getCurrentTime()));
                break;
            }
            case (SCH_MULTISELECT): {
                addedColumn.addElement(
                    Element(columnType, Element<SelectContainer>::getDefaultValue(), TimeWrapper::getCurrentTime()));
                break;
            }
            case (SCH_WEEKDAY): {
                addedColumn.addElement(
                    Element(columnType, Element<WeekdayContainer>::getDefaultValue(), TimeWrapper::getCurrentTime()));
                break;
            }
            case (SCH_TIME): {
                addedColumn.addElement(
                    Element(columnType, Element<TimeContainer>::getDefaultValue(), TimeWrapper::getCurrentTime()));
                break;
            }
            case (SCH_DATE): {
                addedColumn.addElement(
                    Element(columnType, Element<DateContainer>::getDefaultValue(), TimeWrapper::getCurrentTime()));
                break;
            }
            default: {
                std::cout << "ScheduleCore::addDefaultColumn(): Adding a Column of type: '" << columnType
                          << "' has not been implemented!" << std::endl;
                return;
            }
        }
    }
    m_schedule.push_back(addedColumn);
    m_columnDisplayOrder.insert(m_columnDisplayOrder.begin() + index, m_schedule.size() - 1);

    // I think default columns don't cause a need for sorting, since their sort is always COLUMN_SORT_NONE
}

bool ScheduleCore::removeColumn(size_t column) {
    // a permanent column can't be removed
    if ((existsColumnAtIndex(column) == false || getColumn(column).permanent)) {
        return false;
    }

    bool resortRequired = getColumn(column).sort != COLUMN_SORT_NONE;

    // Remove the corresponding column from the schedule
    size_t internalIndex = m_columnDisplayOrder.at(column);
    if (existsColumnAtIndex(internalIndex) == false) {
        std::cout << std::format("ScheduleCore::removeColumn(): Internal index '{}' for display index '{}' is out of range!",
                                 internalIndex,
                                 column)
                  << std::endl;
        return false;
    }
    // The last index = pop from end
    if (internalIndex == getColumnCount() - 1) {
        m_schedule.pop_back();
    } else {
        m_schedule.erase(m_schedule.begin() + internalIndex);  // invalidates pointers to Columns past this one
    }
    m_columnDisplayOrder.erase(m_columnDisplayOrder.begin() + column);
    // Update indices of columns after the removed one
    for (size_t i = 0; i < m_columnDisplayOrder.size(); i++) {
        if (m_columnDisplayOrder[i] > internalIndex) {
            m_columnDisplayOrder[i]--;
        }
    }

    if (resortRequired) {
        sortColumns();
    }

    return true;
}

std::optional<size_t> ScheduleCore::duplicateColumn(size_t column) {
    if (existsColumnAtIndex(column) == false || getColumn(column).permanent) {
        return std::nullopt;
    }

    const Column& columnData = getColumn(column);
    size_t prevColumnCount = getColumnCount();
    addColumn(getColumnCount(), columnData);
    // The column was actually added (probably unneeded safety check)
    if (getColumnCount() == prevColumnCount + 1) {
        return prevColumnCount;
    }
    return std::nullopt;
}

const Column& ScheduleCore::getColumnConst(size_t column) const {
    if (existsColumnAtIndex(column) == false) {
        throw std::out_of_range(std::format("ScheduleCore::getColumn: column index {} is out of range.", column));
    }
    return m_schedule.at(m_columnDisplayOrder.at(column));
}

bool ScheduleCore::setColumnElements(size_t index, const Column& columnData) {
    if (existsColumnAtIndex(index) == false) {
        return false;
    }
    if (getColumn(index).type != columnData.type) {
        std::cout << std::format(
                         "ScheduleCore::setColumnElements: The target Column and columnData types must match but are {} and {}",
                         (size_t)getColumn(index).type,
                         (size_t)columnData.type)
                  << std::endl;
        return false;
    }

    for (size_t row = 0; row < getRowCount(); row++) {
        // break early if the provided columnData was shorter than the result of getColumnCount()
        if (row >= columnData.rows.size()) {
            break;
        }

        switch (getColumn(index).type) {
            case (SCH_BOOL): {
                setElementValue(index, row, columnData.getElementValue<bool>(row));
                break;
            }
            case (SCH_NUMBER): {
                setElementValue(index, row, columnData.getElementValue<int>(row));
                break;
            }
            case (SCH_DECIMAL): {
                setElementValue(index, row, columnData.getElementValue<double>(row));
                break;
            }
            case (SCH_TEXT): {
                setElementValue(index, row, columnData.getElementValue<std::string>(row));
                break;
            }
            case (SCH_SELECT): {
                setElementValue(index, row, columnData.getElementValue<SingleSelectContainer>(row));
                break;
            }
            case (SCH_MULTISELECT): {
                setElementValue(index, row, columnData.getElementValue<SelectContainer>(row));
                break;
            }
            case (SCH_WEEKDAY): {
                setElementValue(index, row, columnData.getElementValue<WeekdayContainer>(row));
                break;
            }
            case (SCH_TIME): {
                setElementValue(index, row, columnData.getElementValue<TimeContainer>(row));
                break;
            }
            case (SCH_DATE): {
                setElementValue(index, row, columnData.getElementValue<DateContainer>(row));
                break;
            }
            default: {
                std::cout << "ScheduleCore::setColumnElements: Setting an Element of type: " << getColumn(index).type
                          << " has not been implemented!" << std::endl;
                break;
            }
        }
    }

    return true;
}

bool ScheduleCore::setColumnType(size_t col, SCHEDULE_TYPE type) {
    if (existsColumnAtIndex(col) == false) {
        return false;
    }
    if (getColumn(col).permanent == true) {
        std::cout
            << std::format(
                   "ScheduleCore::setColumnType tried to set type of a permanent Column at column index {}! Returning false.",
                   col)
            << std::endl;
        return false;
    }

    // TODO: try to convert types..? i guess there's no point in doing that. only really numbers could be turned into text.
    // Reset values to defaults of the target type
    Column& column = getColumn(col);
    size_t rowCount = column.rows.size();

    switch (type) {
        case (SCH_BOOL): {
            for (size_t row = 0; row < rowCount; row++) {
                setElement(col, row, Element<bool>(type, Element<bool>::getDefaultValue()), false);
            }
            break;
        }
        case (SCH_NUMBER): {
            for (size_t row = 0; row < rowCount; row++) {
                setElement(col, row, Element<int>(type, Element<int>::getDefaultValue()), false);
            }
            break;
        }
        case (SCH_DECIMAL): {
            for (size_t row = 0; row < rowCount; row++) {
                setElement(col, row, Element<double>(type, Element<double>::getDefaultValue()), false);
            }
            break;
        }
        case (SCH_TEXT): {
            for (size_t row = 0; row < rowCount; row++) {
                setElement(col, row, Element<std::string>(type, Element<std::string>::getDefaultValue()), false);
            }
            break;
        }
        case (SCH_SELECT): {
            for (size_t row = 0; row < rowCount; row++) {
                Element<SingleSelectContainer> selectElement =
                    Element<SingleSelectContainer>(type, Element<SingleSelectContainer>::getDefaultValue());
                // Update the select to have the correct number of options
                selectElement.getValueReference().update(
                    SelectOptionsModification(OPTION_MODIFICATION_COUNT_UPDATE).getUpdateInfo(),
                    column.selectOptions.getOptionCount());
                setElement(col, row, selectElement, false);
            }
            break;
        }
        case (SCH_MULTISELECT): {
            for (size_t row = 0; row < rowCount; row++) {
                Element<SelectContainer> selectElement =
                    Element<SelectContainer>(type, Element<SelectContainer>::getDefaultValue());
                // Update the select to have the correct number of options
                selectElement.getValueReference().update(
                    SelectOptionsModification(OPTION_MODIFICATION_COUNT_UPDATE).getUpdateInfo(),
                    column.selectOptions.getOptionCount());
                setElement(col, row, selectElement, false);
            }
            break;
        }
        case (SCH_WEEKDAY): {
            for (size_t row = 0; row < rowCount; row++) {
                Element<WeekdayContainer> weekdayElement =
                    Element<WeekdayContainer>(type, Element<WeekdayContainer>::getDefaultValue());
                setElement(col, row, weekdayElement, false);
            }
            break;
        }
        case (SCH_TIME): {
            for (size_t row = 0; row < rowCount; row++) {
                setElement(col, row, Element<TimeContainer>(type, Element<TimeContainer>::getDefaultValue()), false);
            }
            break;
        }
        case (SCH_DATE): {
            for (size_t row = 0; row < rowCount; row++) {
                setElement(col, row, Element<DateContainer>(type, Element<DateContainer>::getDefaultValue()), false);
            }
            break;
        }
        default: {
            std::cout << "ScheduleCore::setColumnType: Setting a column to type: " << type << " has not been implemented!"
                      << std::endl;
            return false;
        }
    }

    column.type = type;
    sortColumns();
    return true;
}

bool ScheduleCore::setColumnName(size_t column, const std::string& name) {
    if (existsColumnAtIndex(column) == false) {
        return false;
    }

    getColumn(column).name = name;
    return true;
}

bool ScheduleCore::setColumnSort(size_t column, COLUMN_SORT sortDirection) {
    if (existsColumnAtIndex(column) == false) {
        return false;
    }

    getColumn(column).sort = sortDirection;
    sortColumns();
    return true;
}

bool ScheduleCore::setColumnResetOption(size_t column, ColumnResetOption option) {
    if (existsColumnAtIndex(column) == false) {
        return false;
    }

    getColumn(column).resetOption = option;
    return true;
}

bool ScheduleCore::setColumnDisplayOrder(size_t oldOrder, size_t newOrder) {
    if (existsColumnAtIndex(oldOrder) == false || existsColumnAtIndex(newOrder) == false) {
        return false;
    }

    containers::move(m_columnDisplayOrder, oldOrder, newOrder);
    return true;
}

const SelectOptions& ScheduleCore::getColumnSelectOptions(size_t column) const {
    return getColumnConst(column).selectOptions;
}

bool ScheduleCore::modifyColumnSelectOptions(size_t column, const SelectOptionsModification& selectOptionsModification) {
    if (existsColumnAtIndex(column) == false) {
        return false;
    }

    if (getColumn(column).modifySelectOptions(selectOptionsModification) == false) {
        std::cout << "ScheduleCore::modifyColumnSelectOptions: Applying the following modification failed:" << std::endl;
        std::cout << selectOptionsModification.getDataString();
        return false;
    }

    sortColumns();
    return true;
}

bool ScheduleCore::addColumnFilterGroup(size_t column, size_t groupIndex, const FilterGroup& filterGroup) {
    if (existsColumnAtIndex(column) == false) {
        return false;
    }

    return getColumn(column).addFilterGroup(groupIndex, filterGroup);
}

bool ScheduleCore::addColumnFilterGroup(size_t column, const FilterGroup& filterGroup) {
    if (existsColumnAtIndex(column) == false) {
        return false;
    }

    return addColumnFilterGroup(column, getColumn(column).getFilterGroupCount(), filterGroup);
}

bool ScheduleCore::removeColumnFilterGroup(size_t column, size_t groupIndex) {
    if (existsColumnAtIndex(column) == false) {
        return false;
    }

    return getColumn(column).removeFilterGroup(groupIndex);
}

bool ScheduleCore::setColumnFilterGroupName(size_t column, size_t groupIndex, const std::string& name) {
    if (existsColumnAtIndex(column) == false) {
        return false;
    }
    if (getColumn(column).hasFilterGroupAt(groupIndex) == false) {
        return false;
    }

    getColumn(column).getFilterGroup(groupIndex).setName(name);
    return true;
}

bool ScheduleCore::setColumnFilterGroupOperator(size_t column, size_t groupIndex, LogicalOperatorEnum logicalOperator) {
    if (existsColumnAtIndex(column) == false) {
        return false;
    }
    if (getColumn(column).hasFilterGroupAt(groupIndex) == false) {
        return false;
    }

    getColumn(column).getFilterGroup(groupIndex).setOperator(logicalOperator);
    return true;
}

bool ScheduleCore::setColumnFilterGroupEnabled(size_t column, size_t groupIndex, bool enabled) {
    if (existsColumnAtIndex(column) == false) {
        return false;
    }
    if (getColumn(column).hasFilterGroupAt(groupIndex) == false) {
        return false;
    }

    getColumn(column).getFilterGroup(groupIndex).setEnabled(enabled);
    return true;
}

bool ScheduleCore::addColumnFilter(size_t column, size_t groupIndex, size_t filterIndex, const Filter& filter) {
    if (existsColumnAtIndex(column) == false) {
        return false;
    }

    return getColumn(column).addFilter(groupIndex, filterIndex, filter);
}

bool ScheduleCore::addColumnFilter(size_t column, size_t groupIndex, const Filter& filter) {
    if (existsColumnAtIndex(column) == false) {
        return false;
    }
    if (getColumn(column).hasFilterGroupAt(groupIndex) == false) {
        return false;
    }

    return addColumnFilter(column, groupIndex, getColumn(column).getFilterGroupConst(groupIndex).getFilterCount(), filter);
}

bool ScheduleCore::setColumnFilterOperator(size_t column,
                                           size_t groupIndex,
                                           size_t filterIndex,
                                           LogicalOperatorEnum logicalOperator) {
    if (existsColumnAtIndex(column) == false) {
        return false;
    }
    if (getColumn(column).hasFilterAt(groupIndex, filterIndex) == false) {
        return false;
    }

    getColumn(column).getFilterGroup(groupIndex).getFilter(filterIndex).setOperator(logicalOperator);
    return true;
}

bool ScheduleCore::removeColumnFilter(size_t column, size_t groupIndex, size_t filterIndex) {
    if (existsColumnAtIndex(column) == false) {
        return false;
    }

    return getColumn(column).removeFilter(groupIndex, filterIndex);
}

bool ScheduleCore::removeColumnFilterRule(size_t column, size_t groupIndex, size_t filterIndex, size_t ruleIndex) {
    if (existsColumnAtIndex(column) == false) {
        return false;
    }

    return getColumn(column).removeFilterRule(groupIndex, filterIndex, ruleIndex);
}

void ScheduleCore::resetColumn(size_t index) {
    Column& column = getColumn(index);

    size_t rowCount = column.rows.size();

    switch (column.type) {
        case (SCH_BOOL): {
            for (size_t row = 0; row < rowCount; row++) {
                setElementValue(index, row, Element<bool>::getDefaultValue(), false);
            }
            break;
        }
        case (SCH_NUMBER): {
            for (size_t row = 0; row < rowCount; row++) {
                setElementValue(index, row, Element<int>::getDefaultValue(), false);
            }
            break;
        }
        case (SCH_DECIMAL): {
            for (size_t row = 0; row < rowCount; row++) {
                setElementValue(index, row, Element<double>::getDefaultValue(), false);
            }
            break;
        }
        case (SCH_TEXT): {
            for (size_t row = 0; row < rowCount; row++) {
                setElementValue(index, row, Element<std::string>::getDefaultValue(), false);
            }
            break;
        }
        case (SCH_SELECT): {
            for (size_t row = 0; row < rowCount; row++) {
                setElementValue(index, row, Element<SingleSelectContainer>::getDefaultValue(), false);
            }
            break;
        }
        case (SCH_MULTISELECT): {
            for (size_t row = 0; row < rowCount; row++) {
                setElementValue(index, row, Element<SelectContainer>::getDefaultValue(), false);
            }
            break;
        }
        case (SCH_WEEKDAY): {
            for (size_t row = 0; row < rowCount; row++) {
                setElementValue(index, row, Element<WeekdayContainer>::getDefaultValue(), false);
            }
            break;
        }
        case (SCH_TIME): {
            for (size_t row = 0; row < rowCount; row++) {
                setElementValue(index, row, Element<TimeContainer>::getDefaultValue(), false);
            }
            break;
        }
        case (SCH_DATE): {
            for (size_t row = 0; row < rowCount; row++) {
                setElementValue(index, row, Element<DateContainer>::getDefaultValue(), false);
            }
            break;
        }
        default: {
            std::cout << "ScheduleCore::resetColumn: Resetting a column of type: " << column.type
                      << " has not been implemented!" << std::endl;
            return;
        }
    }

    sortColumns();
}

// Return the number of rows in the schedule or 0 if there are no columns (which probably won't happen?)
size_t ScheduleCore::getRowCount() const {
    return (m_schedule.size() > 0 ? m_schedule.at(0).rows.size() : 0);
}

bool ScheduleCore::existsRowAtIndex(size_t index) const {
    if (index < getRowCount() == false) {
        std::cout << std::format("ScheduleCore::existsRowAtIndex({}): Index not less than size ({})", index, getRowCount())
                  << std::endl;
        return false;
    }
    return true;
}

void ScheduleCore::addRow() {
    addRow(getRowCount());
}

void ScheduleCore::addRow(size_t index) {
    for (size_t i = 0; i < getColumnCount(); i++) {
        Column& column = m_schedule[i];

        switch (column.type) {
            case (SCH_BOOL): {
                column.addElement(index, Element(column.type, Element<bool>::getDefaultValue()));
                break;
            }
            case (SCH_NUMBER): {
                column.addElement(index, Element(column.type, Element<int>::getDefaultValue()));
                break;
            }
            case (SCH_DECIMAL): {
                column.addElement(index, Element(column.type, Element<double>::getDefaultValue()));
                break;
            }
            case (SCH_TEXT): {
                column.addElement(index, Element(column.type, Element<std::string>::getDefaultValue()));
                break;
            }
            case (SCH_SELECT): {
                column.addElement(index, Element(column.type, Element<SingleSelectContainer>::getDefaultValue()));
                break;
            }
            case (SCH_MULTISELECT): {
                column.addElement(index, Element(column.type, Element<SelectContainer>::getDefaultValue()));
                break;
            }
            case (SCH_WEEKDAY): {
                column.addElement(index, Element(column.type, Element<WeekdayContainer>::getDefaultValue()));
                break;
            }
            case (SCH_TIME): {
                column.addElement(index, Element(column.type, Element<TimeContainer>::getDefaultValue()));
                break;
            }
            case (SCH_DATE): {
                column.addElement(index, Element(column.type, Element<DateContainer>::getDefaultValue()));
                break;
            }
            default: {
                throw std::runtime_error(std::format("ScheduleCore::addRow: Adding rows of type: {} has not been implemented!",
                                                     (size_t)column.type));
            }
        }
    }

    sortColumns();
}

bool ScheduleCore::removeRow(size_t row) {
    if (existsRowAtIndex(row) == false) {
        return false;
    }

    for (size_t i = 0; i < getColumnCount(); i++) {
        getColumn(i).removeElement(row);
    }

    sortColumns();
    return true;
}

std::optional<size_t> ScheduleCore::duplicateRow(size_t row) {
    if (existsRowAtIndex(row) == false) {
        return std::nullopt;
    }

    // Copy of the source row data
    std::vector<std::shared_ptr<ElementBase>> duplicatedRowData = getRowCopy(row);
    size_t prevRowCount = getRowCount();
    addRow();
    // The row was actually added (probably unneeded safety check)
    if (getRowCount() == prevRowCount + 1) {
        setRow(prevRowCount, duplicatedRowData);
        return prevRowCount;
    }
    return std::nullopt;
}

std::vector<std::shared_ptr<ElementBase>> ScheduleCore::getRowCopy(size_t index) {
    std::vector<std::shared_ptr<ElementBase>> rowCopyData = {};

    if (existsRowAtIndex(index) == false) {
        return rowCopyData;
    }

    for (size_t col = 0; col < getColumnCount(); col++) {
        auto element = getElement(col, index);
        auto elementAccess = element.lock();
        rowCopyData.push_back(elementAccess->getCopy());
    }

    return rowCopyData;
}

bool ScheduleCore::setRow(size_t index, std::vector<std::shared_ptr<ElementBase>> elementData) {
    if (existsRowAtIndex(index) == false) {
        return false;
    }

    for (size_t col = 0; col < getColumnCount(); col++) {
        setElement(col, index, elementData[col], false);
    }

    sortColumns();
    return true;
}

std::vector<size_t> ScheduleCore::getSortedRowIndices() const {
    return m_sortedRowIndices;
}