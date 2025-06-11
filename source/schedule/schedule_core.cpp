#include <algorithm>
#include <string>
#include <vector>
#include <numeric>
#include "schedule_core.h"
#include "element_base.h"

ScheduleCore::ScheduleCore() {
}

// NOTE: If flags is ScheduleElementFlags_None, simply returns the first column it finds
size_t ScheduleCore::getFlaggedColumnIndex(ScheduleColumnFlags flags) const {
    for (size_t i = 0; i < m_schedule.size(); i++) {
        if (m_schedule.at(i).flags & flags) {
            return i;
        }
    }
    return 0;
}

// Private function, because it returns a mutable column pointer. NOTE: If flags is ScheduleElementFlags_None, simply returns the first column it finds
Column* ScheduleCore::getColumnWithFlags(ScheduleColumnFlags flags) {
    return &m_schedule.at(getFlaggedColumnIndex(flags));
}

Column* ScheduleCore::getMutableColumn(size_t column) {
    if (column > getColumnCount()) {
        throw std::out_of_range(std::format("ScheduleCore::getMutableColumn: column index {} is out of range.", column));
    }
    return &m_schedule.at(column);
}

// Sorts a copy of the column's rows. Then compares each element of the two rows vectors and returns a vector that contains which index of the OLD rows vector corresponds to that position in the NEW SORTED rows
std::vector<size_t> ScheduleCore::getColumnSortedNewIndices(size_t index) {
    Column& column = m_schedule.at(index);
    std::vector<ElementBase*> rows = column.rows;
    std::vector<size_t> newIndices(rows.size());
    std::iota(newIndices.begin(), newIndices.end(), 0);

    m_columnSortComparison.setup(column.type, column.sort);
    std::sort(
        newIndices.begin(), newIndices.end(), [&](size_t i, size_t j) { return m_columnSortComparison(rows[i], rows[j]); });
    return newIndices;
}

void ScheduleCore::clearSchedule() {
    m_schedule.clear();
    sortColumns();
}

void ScheduleCore::replaceSchedule(std::vector<Column>& columns) {
    clearSchedule();

    m_schedule = columns;
    sortColumns();
}

const std::vector<Column>& ScheduleCore::getAllColumns() const {
    return m_schedule;
}

std::vector<Column>& ScheduleCore::getAllColumnsMutable() {
    return m_schedule;
}

// Sorts every column's rows based on "sorter" columns
void ScheduleCore::sortColumns() {
    for (size_t sorterColumn = 0; sorterColumn < getColumnCount(); sorterColumn++) {
        if (m_schedule.at(sorterColumn).sort != COLUMN_SORT_NONE) {
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

// Add a column from previous data. NOTE: Creates copies of all passed values, because this will probably mostly be used for duplicating columns
void ScheduleCore::addColumn(size_t index, const Column& column) {
    // TODO: make sure that EVERY column has the same amount of rows!!!
    // TODO: give the new column correct creation date & time

    // the last index = just add to the end
    if (index == getColumnCount()) {
        m_schedule.push_back(column);
    } else {
        m_schedule.insert(m_schedule.begin() + index, column);
    }

    // Sort columns just in case, because the added Column could have a sort other than COLUMN_SORT_NONE
    sortColumns();
}

void ScheduleCore::addDefaultColumn(size_t index, SCHEDULE_TYPE columnType) {
    Column addedColumn =
        Column(std::vector<ElementBase*>{}, columnType, schedule_consts::scheduleTypeNames.at(columnType), false);

    for (size_t i = 0; i < getRowCount(); i++) {
        switch (columnType) {
            case (SCH_BOOL): {
                addedColumn.rows.push_back((ElementBase*)new Element<bool>(
                    columnType, Element<bool>::getDefaultValue(), TimeWrapper::getCurrentTime()));
                break;
            }
            case (SCH_NUMBER): {
                addedColumn.rows.push_back(
                    (ElementBase*)new Element<int>(columnType, Element<int>::getDefaultValue(), TimeWrapper::getCurrentTime()));
                break;
            }
            case (SCH_DECIMAL): {
                addedColumn.rows.push_back((ElementBase*)new Element<double>(
                    columnType, Element<double>::getDefaultValue(), TimeWrapper::getCurrentTime()));
                break;
            }
            case (SCH_TEXT): {
                addedColumn.rows.push_back((ElementBase*)new Element<std::string>(
                    columnType, Element<std::string>::getDefaultValue(), TimeWrapper::getCurrentTime()));
                break;
            }
            case (SCH_SELECT): {
                addedColumn.rows.push_back((ElementBase*)new Element<SingleSelectContainer>(
                    columnType, Element<SingleSelectContainer>::getDefaultValue(), TimeWrapper::getCurrentTime()));
                break;
            }
            case (SCH_MULTISELECT): {
                addedColumn.rows.push_back((ElementBase*)new Element<SelectContainer>(
                    columnType, Element<SelectContainer>::getDefaultValue(), TimeWrapper::getCurrentTime()));
                break;
            }
            case (SCH_WEEKDAY): {
                addedColumn.rows.push_back((ElementBase*)new Element<WeekdayContainer>(
                    columnType, Element<WeekdayContainer>::getDefaultValue(), TimeWrapper::getCurrentTime()));
                break;
            }
            case (SCH_TIME): {
                addedColumn.rows.push_back((ElementBase*)new Element<TimeContainer>(
                    columnType, Element<TimeContainer>::getDefaultValue(), TimeWrapper::getCurrentTime()));
                break;
            }
            case (SCH_DATE): {
                addedColumn.rows.push_back((ElementBase*)new Element<DateContainer>(
                    columnType, Element<DateContainer>::getDefaultValue(), TimeWrapper::getCurrentTime()));
                break;
            }
            default: {
                std::cout << "ScheduleCore::addDefaultColumn(): Adding a Column of type: '" << columnType
                          << "' has not been implemented!" << std::endl;
                return;
            }
        }
    }
    m_schedule.insert(m_schedule.begin() + index, addedColumn);

    // I think default columns don't cause a need for sorting, since their sort is always COLUMN_SORT_NONE
}

bool ScheduleCore::removeColumn(size_t column) {
    // a permanent column can't be removed
    if ((existsColumnAtIndex(column) == false || m_schedule.at(column).permanent == true)) {
        return false;
    }

    bool resortRequired = m_schedule.at(column).sort != COLUMN_SORT_NONE;

    // the last index = pop from end
    if (column == getColumnCount() - 1) {
        m_schedule.pop_back();
    } else {
        m_schedule.erase(m_schedule.begin() + column);  // invalidates pointers to Columns past this one
    }

    if (resortRequired) {
        sortColumns();
    }

    return true;
}

const Column* ScheduleCore::getColumn(size_t column) const {
    if (existsColumnAtIndex(column) == false) {
        throw std::out_of_range(std::format("ScheduleCore::getColumn: column index {} is out of range.", column));
    }
    return &m_schedule.at(column);
}

bool ScheduleCore::setColumnElements(size_t index, const Column& columnData) {
    if (existsColumnAtIndex(index) == false) {
        return false;
    }
    if (getColumn(index)->type != columnData.type) {
        std::cout << std::format(
                         "ScheduleCore::setColumnElements: The target Column and columnData types must match but are {} and {}",
                         (size_t)getColumn(index)->type,
                         (size_t)columnData.type)
                  << std::endl;
        return false;
    }

    for (size_t row = 0; row < getRowCount(); row++) {
        // break early if the provided columnData was shorter than the result of getColumnCount()
        if (row >= columnData.rows.size()) {
            break;
        }

        switch (getColumn(index)->type) {
            case (SCH_BOOL): {
                setElementValue(index, row, ((Element<bool>*)columnData.rows[row])->getValue());
                break;
            }
            case (SCH_NUMBER): {
                setElementValue(index, row, ((Element<int>*)columnData.rows[row])->getValue());
                break;
            }
            case (SCH_DECIMAL): {
                setElementValue(index, row, ((Element<double>*)columnData.rows[row])->getValue());
                break;
            }
            case (SCH_TEXT): {
                setElementValue(index, row, ((Element<std::string>*)columnData.rows[row])->getValue());
                break;
            }
            case (SCH_SELECT): {
                setElementValue(index, row, ((Element<SingleSelectContainer>*)columnData.rows[row])->getValue());
                break;
            }
            case (SCH_MULTISELECT): {
                setElementValue(index, row, ((Element<SelectContainer>*)columnData.rows[row])->getValue());
                break;
            }
            case (SCH_WEEKDAY): {
                setElementValue(index, row, ((Element<WeekdayContainer>*)columnData.rows[row])->getValue());
                break;
            }
            case (SCH_TIME): {
                setElementValue(index, row, ((Element<TimeContainer>*)columnData.rows[row])->getValue());
                break;
            }
            case (SCH_DATE): {
                setElementValue(index, row, ((Element<DateContainer>*)columnData.rows[row])->getValue());
                break;
            }
            default: {
                std::cout << "ScheduleCore::setColumnElements: Setting an Element of type: " << getColumn(index)->type
                          << " has not been implemented!" << std::endl;
                break;
            }
        }
    }

    return true;
}

bool ScheduleCore::setColumnType(size_t column, SCHEDULE_TYPE type) {
    if (existsColumnAtIndex(column) == false) {
        return false;
    }
    if (getColumn(column)->permanent == true) {
        std::cout
            << std::format(
                   "ScheduleCore::setColumnType tried to set type of a permanent Column at column index {}! Returning false.",
                   column)
            << std::endl;
        return false;
    }

    // TODO: try to convert types..? i guess there's no point in doing that. only really numbers could be turned into text.
    // reset values to defaults of the (new?) type
    resetColumn(column, type);
    return true;
}

bool ScheduleCore::setColumnName(size_t column, const std::string& name) {
    if (existsColumnAtIndex(column) == false) {
        return false;
    }

    m_schedule.at(column).name = name;
    return true;
}

bool ScheduleCore::setColumnSort(size_t column, COLUMN_SORT sortDirection) {
    if (existsColumnAtIndex(column) == false) {
        return false;
    }

    m_schedule.at(column).sort = sortDirection;
    sortColumns();
    return true;
}

bool ScheduleCore::setColumnResetOption(size_t column, ColumnResetOption option) {
    if (existsColumnAtIndex(column) == false) {
        return false;
    }

    m_schedule.at(column).resetOption = option;
    return true;
}

const SelectOptions& ScheduleCore::getColumnSelectOptions(size_t column) const {
    return m_schedule.at(column).selectOptions;
}

bool ScheduleCore::modifyColumnSelectOptions(size_t column, const SelectOptionsModification& selectOptionsModification) {
    if (existsColumnAtIndex(column) == false) {
        return false;
    }

    if (m_schedule.at(column).modifySelectOptions(selectOptionsModification) == false) {
        std::cout << "ScheduleCore::modifySelectOptions: Applying the following modification failed:" << std::endl;
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

    return getMutableColumn(column)->addFilterGroup(groupIndex, filterGroup);
}

bool ScheduleCore::addColumnFilterGroup(size_t column, const FilterGroup& filterGroup) {
    if (existsColumnAtIndex(column) == false) {
        return false;
    }

    return addColumnFilterGroup(column, getColumn(column)->getFilterGroupCount(), filterGroup);
}

bool ScheduleCore::removeColumnFilterGroup(size_t column, size_t groupIndex) {
    if (existsColumnAtIndex(column) == false) {
        return false;
    }

    return getMutableColumn(column)->removeFilterGroup(groupIndex);
}

bool ScheduleCore::setColumnFilterGroupName(size_t column, size_t groupIndex, const std::string& name) {
    if (existsColumnAtIndex(column) == false) {
        return false;
    }
    if (getColumn(column)->hasFilterGroupAt(groupIndex) == false) {
        return false;
    }

    getMutableColumn(column)->getFilterGroup(groupIndex).setName(name);
    return true;
}

bool ScheduleCore::setColumnFilterGroupOperator(size_t column, size_t groupIndex, LogicalOperatorEnum logicalOperator) {
    if (existsColumnAtIndex(column) == false) {
        return false;
    }
    if (getColumn(column)->hasFilterGroupAt(groupIndex) == false) {
        return false;
    }

    getMutableColumn(column)->getFilterGroup(groupIndex).setOperator(logicalOperator);
    return true;
}

bool ScheduleCore::setColumnFilterGroupEnabled(size_t column, size_t groupIndex, bool enabled) {
    if (existsColumnAtIndex(column) == false) {
        return false;
    }
    if (getColumn(column)->hasFilterGroupAt(groupIndex) == false) {
        return false;
    }

    getMutableColumn(column)->getFilterGroup(groupIndex).setEnabled(enabled);
    return true;
}

bool ScheduleCore::addColumnFilter(size_t column, size_t groupIndex, size_t filterIndex, const Filter& filter) {
    if (existsColumnAtIndex(column) == false) {
        return false;
    }

    return getMutableColumn(column)->addFilter(groupIndex, filterIndex, filter);
}

bool ScheduleCore::addColumnFilter(size_t column, size_t groupIndex, const Filter& filter) {
    if (existsColumnAtIndex(column) == false) {
        return false;
    }
    if (getColumn(column)->hasFilterGroupAt(groupIndex) == false) {
        return false;
    }

    return addColumnFilter(column, groupIndex, getColumn(column)->getFilterGroupConst(groupIndex).getFilterCount(), filter);
}

bool ScheduleCore::setColumnFilterOperator(size_t column,
                                           size_t groupIndex,
                                           size_t filterIndex,
                                           LogicalOperatorEnum logicalOperator) {
    if (existsColumnAtIndex(column) == false) {
        return false;
    }
    if (getColumn(column)->hasFilterAt(groupIndex, filterIndex) == false) {
        return false;
    }

    getMutableColumn(column)->getFilterGroup(groupIndex).getFilter(filterIndex).setOperator(logicalOperator);
    return true;
}

bool ScheduleCore::removeColumnFilter(size_t column, size_t groupIndex, size_t filterIndex) {
    if (existsColumnAtIndex(column) == false) {
        return false;
    }

    return getMutableColumn(column)->removeFilter(groupIndex, filterIndex);
}

bool ScheduleCore::removeColumnFilterRule(size_t column, size_t groupIndex, size_t filterIndex, size_t ruleIndex) {
    if (existsColumnAtIndex(column) == false) {
        return false;
    }

    return getMutableColumn(column)->removeFilterRule(groupIndex, filterIndex, ruleIndex);
}

void ScheduleCore::resetColumn(size_t index, SCHEDULE_TYPE type) {
    Column& column = *getMutableColumn(index);

    size_t rowCount = column.rows.size();

    switch (type) {
        case (SCH_BOOL): {
            for (size_t row = 0; row < rowCount; row++) {
                setElement(index, row, (ElementBase*)new Element<bool>(type, Element<bool>::getDefaultValue()), false);
            }
            break;
        }
        case (SCH_NUMBER): {
            for (size_t row = 0; row < rowCount; row++) {
                setElement(index, row, (ElementBase*)new Element<int>(type, Element<int>::getDefaultValue()), false);
            }
            break;
        }
        case (SCH_DECIMAL): {
            for (size_t row = 0; row < rowCount; row++) {
                setElement(index, row, (ElementBase*)new Element<double>(type, Element<double>::getDefaultValue()), false);
            }
            break;
        }
        case (SCH_TEXT): {
            for (size_t row = 0; row < rowCount; row++) {
                setElement(
                    index, row, (ElementBase*)new Element<std::string>(type, Element<std::string>::getDefaultValue()), false);
            }
            break;
        }
        case (SCH_SELECT): {
            for (size_t row = 0; row < rowCount; row++) {
                auto selectElement =
                    new Element<SingleSelectContainer>(type, Element<SingleSelectContainer>::getDefaultValue());
                // Update the select to have the correct number of options
                selectElement->getValueReference().update(
                    SelectOptionsModification(OPTION_MODIFICATION_COUNT_UPDATE).getUpdateInfo(),
                    column.selectOptions.getOptionCount());
                setElement(index, row, (ElementBase*)selectElement, false);
            }
            break;
        }
        case (SCH_MULTISELECT): {
            for (size_t row = 0; row < rowCount; row++) {
                auto selectElement = new Element<SelectContainer>(type, Element<SelectContainer>::getDefaultValue());
                // Update the select to have the correct number of options
                selectElement->getValueReference().update(
                    SelectOptionsModification(OPTION_MODIFICATION_COUNT_UPDATE).getUpdateInfo(),
                    column.selectOptions.getOptionCount());
                setElement(index, row, (ElementBase*)selectElement, false);
            }
            break;
        }
        case (SCH_WEEKDAY): {
            for (size_t row = 0; row < rowCount; row++) {
                auto weekdayElement = new Element<WeekdayContainer>(type, Element<WeekdayContainer>::getDefaultValue());
                setElement(index, row, (ElementBase*)weekdayElement, false);
            }
            break;
        }
        case (SCH_TIME): {
            for (size_t row = 0; row < rowCount; row++) {
                setElement(index,
                           row,
                           (ElementBase*)new Element<TimeContainer>(type, Element<TimeContainer>::getDefaultValue()),
                           false);
            }
            break;
        }
        case (SCH_DATE): {
            for (size_t row = 0; row < rowCount; row++) {
                setElement(index,
                           row,
                           (ElementBase*)new Element<DateContainer>(type, Element<DateContainer>::getDefaultValue()),
                           false);
            }
            break;
        }
        default: {
            std::cout << "ScheduleCore::resetColumn: Resetting a column to type: " << type << " has not been implemented!"
                      << std::endl;
            return;
        }
    }

    column.type = type;
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
    std::vector<ElementBase*> elementCopies = {};

    for (size_t i = 0; i < getColumnCount(); i++) {
        Column& column = m_schedule[i];

        switch (column.type) {
            case (SCH_BOOL): {
                column.addElement(index, new Element<bool>(column.type, Element<bool>::getDefaultValue()));
                break;
            }
            case (SCH_NUMBER): {
                column.addElement(index, new Element<int>(column.type, Element<int>::getDefaultValue()));
                break;
            }
            case (SCH_DECIMAL): {
                column.addElement(index, new Element<double>(column.type, Element<double>::getDefaultValue()));
                break;
            }
            case (SCH_TEXT): {
                column.addElement(index, new Element<std::string>(column.type, Element<std::string>::getDefaultValue()));
                break;
            }
            case (SCH_SELECT): {
                column.addElement(
                    index, new Element<SingleSelectContainer>(column.type, Element<SingleSelectContainer>::getDefaultValue()));
                break;
            }
            case (SCH_MULTISELECT): {
                column.addElement(index,
                                  new Element<SelectContainer>(column.type, Element<SelectContainer>::getDefaultValue()));
                break;
            }
            case (SCH_WEEKDAY): {
                column.addElement(index,
                                  new Element<WeekdayContainer>(column.type, Element<WeekdayContainer>::getDefaultValue()));
                break;
            }
            case (SCH_TIME): {
                column.addElement(index, new Element<TimeContainer>(column.type, Element<TimeContainer>::getDefaultValue()));
                break;
            }
            case (SCH_DATE): {
                column.addElement(index, new Element<DateContainer>(column.type, Element<DateContainer>::getDefaultValue()));
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

    for (size_t i = 0; i < m_schedule.size(); i++) {
        delete m_schedule[i].rows[row];
        if (row == m_schedule[i].rows.size() - 1) {
            m_schedule[i].rows.pop_back();
        } else {
            m_schedule[i].rows.erase(m_schedule[i].rows.begin() + row);
        }
    }

    sortColumns();
    return true;
}

std::optional<size_t> ScheduleCore::duplicateRow(size_t row) {
    if (existsRowAtIndex(row) == false) {
        return std::nullopt;
    }

    auto duplicatedRowData = getRow(row);
    size_t prevRowCount = getRowCount();
    addRow();
    // The row was actually added (probably unneeded safety check)
    if (getRowCount() == prevRowCount + 1) {
        setRow(prevRowCount, duplicatedRowData);
        return prevRowCount;
    }
    return std::nullopt;
}

std::vector<ElementBase*> ScheduleCore::getRow(size_t index) {
    std::vector<ElementBase*> elementData = {};

    if (existsRowAtIndex(index) == false) {
        return elementData;
    }

    for (size_t col = 0; col < getColumnCount(); col++) {
        elementData.push_back(getElement(col, index));
    }

    return elementData;
}

bool ScheduleCore::setRow(size_t index, std::vector<ElementBase*> elementData) {
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