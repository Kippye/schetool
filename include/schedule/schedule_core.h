#pragma once

#include <vector>
#include <string>
#include <optional>
#include <format>
#include <iostream>
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
        ColumnSortComparison m_columnSortComparison;
        // NOTE: If flags is ScheduleElementFlags_None, simply returns the first column it finds
        Column& getColumnWithFlags(ScheduleColumnFlags flags);
        // Takes a "display" column index and returns a reference to the corresponding column in the schedule
        Column& getColumn(size_t index);
        std::vector<size_t> getColumnSortedNewIndices(size_t index);

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
        bool existsColumnAtIndex(size_t index) const;
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

        // Add a row at the end of the schedule
        void addRow();
        void addRow(size_t index);
        bool removeRow(size_t index);
        // Duplicate the row at the given index, return the index of the created row (in the future, this may either be next to the target row or the last row index).
        // If duplicating the row failed for whatever reason, std::nullopt is returned.
        std::optional<size_t> duplicateRow(size_t index);
        // Get all elements of a row. If the row doesn't exist, an empty vector is returned.
        std::vector<ElementBase*> getRow(size_t index);
        // Set all elements of a row. NOTE: The element data must be in the correct order. If the row doesn't exist, nothing happens.
        bool setRow(size_t index, std::vector<ElementBase*> elementData);
        std::vector<size_t> getSortedRowIndices() const;

        // ELEMENTS.
        // Get the value of the element as Element<T>. NOTE: You MUST provide the correct type.
        template <typename T>
        T& getValue(ElementBase* element) {
            return ((Element<T>*)element)->getValueReference();
        }
        // Get the value of the element as Element<T> as a CONST ref. NOTE: You MUST provide the correct type.
        template <typename T>
        const T& getValueConstRef(const ElementBase* element) const {
            return ((Element<T>*)element)->getConstValueReference();
        }

        // Get a pointer to the ElementBase at col; row
        ElementBase* getElement(size_t col, size_t row) {
            Column& column = getColumn(col);

            if (column.hasElement(row) == false) {
                std::cout << std::format("ScheduleCore::getElement could not get element at {}; {}", col, row) << std::endl;
                return nullptr;
            }

            return column.getElement(row);
        }
        // Get a constant pointer to the ElementBase at col; row
        const ElementBase* getElementConst(size_t col, size_t row) const {
            const Column& column = getColumnConst(col);

            if (column.hasElement(row) == false) {
                std::cout << std::format("ScheduleCore::getElementConst could not get element at {}; {}", col, row)
                          << std::endl;
                return nullptr;
            }

            return column.getElementConst(row);
        }

        // Simple function that gets an ElementBase* at column; row and casts it to Element<T>*. In the future, this might check that the returned type is actually correct.
        template <typename T>
        Element<T>* getElementAsSpecial(size_t column, size_t row) {
            ElementBase* element = getElement(column, row);

            if (element == nullptr) {
                std::cout << std::format("ScheduleCore::getElementAsSpecial could not get element at {}; {}", column, row)
                          << std::endl;
                return nullptr;
            }

            return (Element<T>*)element;
        }

        // Use this function to completely replace the element at column; row with the given ElementBase*.
        // The target element pointer is replaced by the provided pointer!
        // TODO: FIX MEMORY LEAK
        bool setElement(size_t column, size_t row, ElementBase* other, bool resort = true) {
            if (getElement(column, row) == nullptr) {
                std::cout << std::format("ScheduleCore::setElement failed to set element at {}; {} - element does not exist",
                                         column,
                                         row)
                          << std::endl;
                return false;
            }
            if (!other) {
                std::cout
                    << std::format(
                           "ScheduleCore::setElement failed to set element at {}; {} - provided element pointer is nullptr",
                           column,
                           row)
                    << std::endl;
                return false;
            }

            delete getColumn(column).getElement(row);
            getColumn(column).rows[row] = other;

            if (resort) {
                sortColumns();
            }
            return true;
        }

        // Shortcut for getting the value of an Element at col; row
        template <typename T>
        T& getElementValue(size_t col, size_t row) {
            const Column& elementColumn = getColumn(col);
            if (elementColumn.hasElement(row) == false) {
                throw std::runtime_error(
                    std::format("ScheduleCore::getElementValue could not get element value at {}; {}", col, row));
            }
            return getValue<T>(elementColumn.rows[row]);
        }
        // Shortcut for getting the value of an Element at col; row as a CONST reference
        template <typename T>
        const T& getElementValueConstRef(size_t col, size_t row) const {
            const Column& elementColumn = getColumnConst(col);
            if (elementColumn.hasElement(row) == false) {
                throw std::runtime_error(
                    std::format("ScheduleCore::getElementValueConstRef could not get element value at {}; {}", col, row));
            }
            return getValueConstRef<T>(elementColumn.rows[row]);
        }

        // Shortcut for setting the value of the Element at col; row to value. You must provide the correct type for the Element.
        template <typename T>
        bool setElementValue(size_t col, size_t row, const T& value, bool resort = true) {
            ElementBase* element = getElement(col, row);

            if (element == nullptr) {
                std::cout << std::format(
                                 "ScheduleCore::setElementValue failed to set element at {}; {} - element does not exist",
                                 col,
                                 row)
                          << std::endl;
                return false;
            }

            ((Element<T>*)element)->setValue(value);

            ScheduleColumnFlags columnFlags = getColumn(col).flags;
            if (columnFlags & ScheduleColumnFlags_Start) {
                getElementAsSpecial<TimeContainer>(getFlaggedColumnIndex(ScheduleColumnFlags_End), row)
                    ->setValue(getElementAsSpecial<TimeContainer>(col, row)->getValue() +
                               getElementAsSpecial<TimeContainer>(getFlaggedColumnIndex(ScheduleColumnFlags_Duration), row)
                                   ->getValue());
            } else if (columnFlags & ScheduleColumnFlags_Duration) {
                getElementAsSpecial<TimeContainer>(getFlaggedColumnIndex(ScheduleColumnFlags_End), row)
                    ->setValue(
                        getElementAsSpecial<TimeContainer>(getFlaggedColumnIndex(ScheduleColumnFlags_Start), row)->getValue() +
                        getElementAsSpecial<TimeContainer>(col, row)->getValue());
            } else if (columnFlags & ScheduleColumnFlags_End) {
                getElementAsSpecial<TimeContainer>(getFlaggedColumnIndex(ScheduleColumnFlags_Duration), row)
                    ->setValue(
                        getElementAsSpecial<TimeContainer>(col, row)->getValue() -
                        getElementAsSpecial<TimeContainer>(getFlaggedColumnIndex(ScheduleColumnFlags_Start), row)->getValue());
            }

            if (resort) {
                sortColumns();
            }
            return true;
        }
};