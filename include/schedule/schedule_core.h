#pragma once

#include <vector>
#include <string>
#include <optional>
#include <format>
#include <iostream>
#include <memory>
#include "schedule_column.h"
#include "schedule_constants.h"
#include "element.h"
#include "single_select_container.h"
#include "weekday_container.h"
#include "select_options.h"

class ScheduleCore {
    private:
        std::vector<Column> m_schedule = {};
        // Translates between display order index (input) to m_schedule column index (content / output)
        std::vector<size_t> m_columnDisplayOrder = {};
        std::vector<size_t> m_sortedRowIndices = {};
        // NOTE: If flags is ScheduleElementFlags_None, simply returns the first column it finds
        Column& getColumnWithFlags(ScheduleColumnFlags flags);
        // Takes a "display" column index and returns a reference to the corresponding column in the schedule
        Column& getColumn(size_t index);
        std::vector<size_t> getColumnSortedNewIndices(size_t index);

        // INTERNAL: Use this function to completely replace the element at column; row.
        // The target element pointer will point to the provided value.
        // NOTE: Your provided pointer will contain the column element's previous value. It essentially becomes garbage!
        template <typename T>
        bool replaceElement(size_t column, size_t row, Element<T> otherValue) {
            if (getElement(column, row).expired()) {
                std::cout << std::format("ScheduleCore::setElement(): Failed to set element at {}; {} - element does not exist",
                                         column,
                                         row)
                          << std::endl;
                return false;
            }
            const SCHEDULE_TYPE columnType = getColumn(column).type;

            if (otherValue.getType() != columnType) {
                std::cout
                    << std::format(
                           "ScheduleCore::setElement(): Failed to set element at {}; {} - provided element's type is incorrect",
                           column,
                           row)
                    << std::endl;
                return false;
            }

            return getColumn(column).replaceElement(row, otherValue);
        }

    public:
        ScheduleCore();
        // WHOLE-SCHEDULE FUNCTIONS
        // Clears the Schedule and deletes all the Columns.
        void clearSchedule();
        // Replaces the m_schedule vector of Columns with the provided. NOTE: ALSO DELETES ALL PREVIOUS ELEMENTS
        void replaceSchedule(std::vector<Column>& columns);
        // Get a constant reference to every Column in the Schedule
        std::vector<Column> getAllColumns() const;
        void sortColumns();

        // Checks if the row at the given index passes every FilterGroup in every column.
        // Optionally, pass a vector of indices of columns whose filters should not be applied. Invalid indices in this vector will simply have no effect.
        bool checkPassesAllFilters(size_t row,
                                   const std::optional<TimeWrapper>& currentTime = std::nullopt,
                                   const std::vector<size_t>& ignoredColumnIndices = {}) const;

        // COLUMNS
        size_t getColumnCount() const;
        bool existsColumnAtIndex(size_t index, bool warn = false) const;
        std::optional<size_t> getInternalIndexFor(size_t displayOrder) const;

        void addColumn(size_t index, const Column& column);
        // Add a column with default values of the given type at the given index.
        void addDefaultColumn(size_t index, SCHEDULE_TYPE columnType = SCH_TEXT);
        bool removeColumn(size_t column);
        // Duplicate the column at the given index, return the index of the created column (in the future, this may either be next to the target column or the last column index).
        // If duplicating the column failed for whatever reason, std::nullopt is returned.
        std::optional<size_t> duplicateColumn(size_t index);
        // Get a constant reference to the Column at the index.
        const Column& getColumnConst(size_t column) const;
        // Get the index of the first column with the given flags
        // Returns '0' if no column with the flags is found.
        size_t getFlaggedColumnIndex(ScheduleColumnFlags flags) const;
        // Set the values of every Element to be copies of the provided Column's Element values. NOTE: If the provided Column has fewer rows, only those will be modified. If it has more rows, ones past the end will be ignored.
        bool setColumnElements(size_t index, const Column& columnData);
        bool setColumnType(size_t column, SCHEDULE_TYPE type);
        bool setColumnName(size_t column, const std::string& name);
        bool setColumnSort(size_t column, COLUMN_SORT sortDirection);
        bool setColumnResetOption(size_t column, ColumnResetOption resetSetting);
        // Change the column's display order from the current value (oldOrder) to the new value (newOrder)
        // Returns true if successful, false if either of the indices are out of range.
        bool setColumnDisplayOrder(size_t oldOrder, size_t newOrder);
        const SelectOptions& getColumnSelectOptions(size_t column) const;
        bool modifyColumnSelectOptions(size_t column, const SelectOptionsModification& selectOptionsModification);

        bool addColumnFilterGroup(size_t column, size_t groupIndex, const FilterGroup& filterGroup);
        // Use the count as the group index
        bool addColumnFilterGroup(size_t column, const FilterGroup& filterGroup);
        bool removeColumnFilterGroup(size_t column, size_t groupIndex);
        bool setColumnFilterGroupName(size_t column, size_t groupIndex, const std::string& name);
        bool setColumnFilterGroupOperator(size_t column, size_t groupIndex, LogicalOperatorEnum logicalOperator);
        bool setColumnFilterGroupEnabled(size_t column, size_t groupIndex, bool enabled);

        bool addColumnFilter(size_t column, size_t groupIndex, size_t filterIndex, const Filter& filter);
        // Use the count as the filter index
        bool addColumnFilter(size_t column, size_t groupIndex, const Filter& filter);
        bool setColumnFilterOperator(size_t column, size_t groupIndex, size_t filterIndex, LogicalOperatorEnum logicalOperator);
        bool removeColumnFilter(size_t column, size_t groupIndex, size_t filterIndex);

        template <typename T>
        bool addColumnFilterRule(
            size_t column, size_t groupIndex, size_t filterIndex, size_t ruleIndex, const FilterRule<T>& filterRule) {
            if (existsColumnAtIndex(column) == false) {
                return false;
            }

            getColumn(column).addFilterRule(groupIndex, filterIndex, ruleIndex, filterRule);
            return true;
        }
        // Use the count as the rule index
        template <typename T>
        bool addColumnFilterRule(size_t column, size_t groupIndex, size_t filterIndex, const FilterRule<T>& filterRule) {
            if (existsColumnAtIndex(column) == false) {
                return false;
            }
            if (getColumn(column).hasFilterAt(groupIndex, filterIndex) == false) {
                return false;
            }

            return addColumnFilterRule(
                column,
                groupIndex,
                filterIndex,
                getColumn(column).getFilterGroupConst(groupIndex).getFilterConst(filterIndex).getRuleCount(),
                filterRule);
        }
        template <typename T>
        bool replaceColumnFilterRule(
            size_t column, size_t groupIndex, size_t filterIndex, size_t ruleIndex, const FilterRule<T>& filterRule) {
            if (existsColumnAtIndex(column) == false) {
                return false;
            }

            getColumn(column).replaceFilterRule(groupIndex, filterIndex, ruleIndex, filterRule);
            return true;
        }
        bool removeColumnFilterRule(size_t column, size_t groupIndex, size_t filterIndex, size_t ruleIndex);
        // Sets every Element in the Column index to a default value of its type.
        // After resetting, calls sortColumns().
        void resetColumn(size_t index);

        // ROWS
        size_t getRowCount() const;
        bool existsRowAtIndex(size_t index) const;
        /* Get the state of the item on this row of the schedule.
        - In the past and Finished == false -> Unfinished
        - In the past and Finished == true -> Finished
        - Started before current time & ends after current time & Finished == false -> Current
        - Any other case or invalid row index -> Normal
        */
        ScheduleItemState getRowItemState(size_t index) const;

        // Add a row at the end of the schedule
        void addRow();
        void addRow(size_t index);
        bool removeRow(size_t index);
        // Duplicate the row at the given index, return the index of the created row (in the future, this may either be next to the target row or the last row index).
        // If duplicating the row failed for whatever reason, std::nullopt is returned.
        std::optional<size_t> duplicateRow(size_t index);
        // Get COPIES OF all elements of a row directly to these shared_ptr. NOTE: Do not store these pointers anywhere else.
        // If the row doesn't exist, an empty vector is returned.
        std::vector<std::shared_ptr<ElementBase>> getRowCopy(size_t index);
        // Set all elements of a row. NOTE: The element data must be in the correct order. If the row doesn't exist, nothing happens.
        bool setRow(size_t index, std::vector<std::shared_ptr<ElementBase>> elementData);
        std::vector<size_t> getSortedRowIndices() const;

        // ELEMENTS.
        // Get a weak pointer to the ElementBase at col; row
        std::weak_ptr<ElementBase> getElement(size_t col, size_t row) {
            Column& column = getColumn(col);

            if (column.hasElement(row) == false) {
                throw std::out_of_range(std::format("ScheduleCore::getElement(): Could not get element at {}; {}", col, row));
            }

            return column.getElement(row);
        }
        // Get a constant weak pointer to the ElementBase at col; row
        std::weak_ptr<const ElementBase> getElementConst(size_t col, size_t row) const {
            const Column& column = getColumnConst(col);

            if (column.hasElement(row) == false) {
                throw std::out_of_range(
                    std::format("ScheduleCore::getElementConst(): Could not get element at {}; {}", col, row));
            }

            return column.getElementConst(row);
        }

        template <typename T>
        Element<T> getElementAsType(size_t col, size_t row) {
            const Column& elementColumn = getColumnConst(col);
            if (Element<T>::getType() != elementColumn.type) {
                throw std::runtime_error(
                    std::format("ScheduleCore::getElementAsType(): Incorrect type for element at {}; {}", col, row));
            }

            if (elementColumn.hasElement(row) == false) {
                throw std::runtime_error(
                    std::format("ScheduleCore::getElementAsType(): Could not get element value at {}; {}", col, row));
            }
            auto element = elementColumn.getElementConst(row);
            if (element.expired()) {
                throw std::runtime_error(
                    std::format("ScheduleCore::getElementAsType(): Element at {}; {} has been deleted", col, row));
            }
            auto elementAccess = element.lock();
            auto typeElementAccess = std::dynamic_pointer_cast<Element<T>>(elementAccess);
            return *typeElementAccess;
        }

        // Use this function to completely replace the element at column; row.
        // The target element will contain this Element.
        template <typename T>
        bool setElement(size_t column, size_t row, Element<T> other, bool resort = true) {
            if (getElement(column, row).expired()) {
                std::cout << std::format("ScheduleCore::setElement(): Failed to set element at {}; {} - element does not exist",
                                         column,
                                         row)
                          << std::endl;
                return false;
            }
            const SCHEDULE_TYPE columnType = getColumn(column).type;
            if (other.getType() != columnType) {
                std::cout
                    << std::format(
                           "ScheduleCore::setElement(): Failed to set element at {}; {} - provided element's type is incorrect",
                           column,
                           row)
                    << std::endl;
                return false;
            }

            auto element = getElement(column, row);

            if (element.expired()) {
                std::cout << std::format(
                                 "ScheduleCore::setElement(): Failed to set element at {}; {} - the element has been deleted",
                                 column,
                                 row)
                          << std::endl;
                return false;
            }

            auto elementAccess = element.lock();
            auto typeElementAccess = std::dynamic_pointer_cast<Element<T>>(elementAccess);
            *typeElementAccess = other;

            if (resort) {
                sortColumns();
            }
            return true;
        }

        // Shortcut for getting the value of an Element at col; row
        template <typename T>
        T getElementValue(size_t col, size_t row) const {
            return getElementValueConstRef<T>(col, row);
        }
        // Shortcut for getting a const reference to the value of an Element at col; row
        template <typename T>
        const T& getElementValueConstRef(size_t col, size_t row) const {
            const Column& elementColumn = getColumnConst(col);
            if (Element<T>::getType() != elementColumn.type) {
                throw std::runtime_error(
                    std::format("ScheduleCore::getElementValueConstRef(): Incorrect type {} for element of type {} at {}; {}",
                                (int)Element<T>::getType(),
                                (int)elementColumn.type,
                                col,
                                row));
            }

            if (elementColumn.hasElement(row) == false) {
                throw std::runtime_error(
                    std::format("ScheduleCore::getElementValueConstRef(): Could not get element value at {}; {}", col, row));
            }
            auto element = elementColumn.getElementConst(row);
            if (element.expired()) {
                throw std::runtime_error(
                    std::format("ScheduleCore::getElementValueConstRef(): Element at {}; {} has been deleted", col, row));
            }
            auto elementAccess = element.lock();
            auto typeElementAccess = std::dynamic_pointer_cast<const Element<T>>(elementAccess);

            return typeElementAccess->getConstValueReference();
        }

        // Shortcut for setting the value of the Element at col; row to value. You must provide the correct type for the Element.
        template <typename T>
        bool setElementValue(size_t col, size_t row, const T& value, bool resort = true, bool calculateTimes = true) {
            auto element = getElement(col, row);
            if (element.expired()) {
                return false;
            }
            auto elementAccess = element.lock();
            if (Element<T>::getType() != elementAccess->getType()) {
                throw std::runtime_error(
                    std::format("ScheduleCore::setelementValue(): Incorrect type {} for element of type {} at {}; {}",
                                (int)Element<T>::getType(),
                                (int)elementAccess->getType(),
                                col,
                                row));
            }
            auto typeElementAccess = std::dynamic_pointer_cast<Element<T>>(elementAccess);

            typeElementAccess->setValue(value);

            if (calculateTimes) {
                ScheduleColumnFlags columnFlags = getColumn(col).flags;
                if (columnFlags & ScheduleColumnFlags_Start) {
                    setElementValue(
                        getFlaggedColumnIndex(ScheduleColumnFlags_End),
                        row,
                        getElementValue<TimeContainer>(col, row) +
                            getElementValue<TimeContainer>(getFlaggedColumnIndex(ScheduleColumnFlags_Duration), row),
                        resort,
                        false);
                } else if (columnFlags & ScheduleColumnFlags_Duration) {
                    setElementValue(getFlaggedColumnIndex(ScheduleColumnFlags_End),
                                    row,
                                    getElementValue<TimeContainer>(getFlaggedColumnIndex(ScheduleColumnFlags_Start), row) +
                                        getElementValue<TimeContainer>(col, row),
                                    resort,
                                    false);
                } else if (columnFlags & ScheduleColumnFlags_End) {
                    setElementValue(getFlaggedColumnIndex(ScheduleColumnFlags_Duration),
                                    row,
                                    getElementValue<TimeContainer>(col, row) -
                                        getElementValue<TimeContainer>(getFlaggedColumnIndex(ScheduleColumnFlags_Start), row),
                                    resort,
                                    false);
                }
            }

            if (resort) {
                sortColumns();
            }
            return true;
        }
};