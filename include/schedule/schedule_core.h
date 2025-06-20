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
        ColumnSortComparison m_columnSortComparison;
        std::vector<size_t> m_sortedRowIndices = {};
        Column* getColumnWithFlags(ScheduleColumnFlags flags);
        Column* getMutableColumn(size_t column);
        std::vector<size_t> getColumnSortedNewIndices(size_t index);

    public:
        ScheduleCore();
        // WHOLE-SCHEDULE FUNCTIONS
        // Clears the Schedule and deletes all the Columns.
        void clearSchedule();
        // Replaces the m_schedule vector of Columns with the provided. NOTE: ALSO DELETES ALL PREVIOUS ELEMENTS
        void replaceSchedule(std::vector<Column>& columns);
        // Get a constant reference to every Column in the Schedule
        const std::vector<Column>& getAllColumns() const;
        // Generally do not use this. It's meant for reading from file only.
        std::vector<Column>& getAllColumnsMutable();
        void sortColumns();

        // COLUMNS
        size_t getColumnCount() const;
        bool existsColumnAtIndex(size_t index) const;

        void addColumn(size_t index, const Column& column);
        // Add a column with default values of the given type at the given index.
        void addDefaultColumn(size_t index, SCHEDULE_TYPE columnType = SCH_TEXT);
        bool removeColumn(size_t column);
        // Duplicate the column at the given index, return the index of the created column (in the future, this may either be next to the target column or the last column index).
        // If duplicating the column failed for whatever reason, std::nullopt is returned.
        std::optional<size_t> duplicateColumn(size_t index);
        // Get a constant pointer to the Column at the index.
        const Column* getColumn(size_t column) const;
        // Get the index of the first column with the given flags
        size_t getFlaggedColumnIndex(ScheduleColumnFlags flags) const;
        // Set the values of every Element to be copies of the provided Column's Element values. NOTE: If the provided Column has fewer rows, only those will be modified. If it has more rows, ones past the end will be ignored.
        bool setColumnElements(size_t index, const Column& columnData);
        bool setColumnType(size_t column, SCHEDULE_TYPE type);
        bool setColumnName(size_t column, const std::string& name);
        bool setColumnSort(size_t column, COLUMN_SORT sortDirection);
        bool setColumnResetOption(size_t column, ColumnResetOption resetSetting);
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

            getMutableColumn(column)->addFilterRule(groupIndex, filterIndex, ruleIndex, filterRule);
            return true;
        }
        // Use the count as the rule index
        template <typename T>
        bool addColumnFilterRule(size_t column, size_t groupIndex, size_t filterIndex, const FilterRule<T>& filterRule) {
            if (existsColumnAtIndex(column) == false) {
                return false;
            }
            if (getColumn(column)->hasFilterAt(groupIndex, filterIndex) == false) {
                return false;
            }

            return addColumnFilterRule(
                column,
                groupIndex,
                filterIndex,
                getColumn(column)->getFilterGroupConst(groupIndex).getFilterConst(filterIndex).getRuleCount(),
                filterRule);
        }
        template <typename T>
        bool replaceColumnFilterRule(
            size_t column, size_t groupIndex, size_t filterIndex, size_t ruleIndex, const FilterRule<T>& filterRule) {
            if (existsColumnAtIndex(column) == false) {
                return false;
            }

            getMutableColumn(column)->replaceFilterRule(groupIndex, filterIndex, ruleIndex, filterRule);
            return true;
        }
        bool removeColumnFilterRule(size_t column, size_t groupIndex, size_t filterIndex, size_t ruleIndex);
        // Sets every Element in the Column index to a default value of the given type.
        // Do NOT change the column's type before running this.
        // After resetting, sets the column's type to type and calls sortColumns().
        void resetColumn(size_t index, SCHEDULE_TYPE type);

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

        // Get a pointer to the ElementBase at column; row
        ElementBase* getElement(size_t column, size_t row) {
            Column* mutableColumn = getMutableColumn(column);

            if (mutableColumn == nullptr || mutableColumn->hasElement(row) == false) {
                std::cout << std::format("ScheduleCore::getElement could not get element at {}; {}", column, row) << std::endl;
                return nullptr;
            }

            return mutableColumn->getElement(row);
        }
        // Get a pointer to the ElementBase at column; row
        const ElementBase* getElementConst(size_t column, size_t row) const {
            const Column* col = getColumn(column);

            if (col == nullptr || col->hasElement(row) == false) {
                std::cout << std::format("ScheduleCore::getElementConst could not get element at {}; {}", column, row)
                          << std::endl;
                return nullptr;
            }

            return col->getElementConst(row);
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

        // Use this function to completely replace the element at column; row with the ElementBase in value.
        // NOTE: If the types match, a copy is performed.
        // If the types do not match, the target element pointer is replaced by the value pointer!
        // NOTE: Currently, does not add to the edit history
        bool setElement(size_t column, size_t row, ElementBase* other, bool resort = true) {
            if (getElement(column, row) == nullptr) {
                std::cout << std::format("ScheduleCore::setElement failed to set element at {}; {} - element does not exist",
                                         column,
                                         row)
                          << std::endl;
                return false;
            }

            // IF the provided Element fits the column's type, set the target Element's value directly
            if (getColumn(column)->type == other->getType()) {
                switch (other->getType()) {
                    case (SCH_BOOL): {
                        getElementAsSpecial<bool>(column, row)->setValue(((Element<bool>*)other)->getValue());
                        break;
                    }
                    case (SCH_NUMBER): {
                        getElementAsSpecial<int>(column, row)->setValue(((Element<int>*)other)->getValue());
                        break;
                    }
                    case (SCH_DECIMAL): {
                        getElementAsSpecial<double>(column, row)->setValue(((Element<double>*)other)->getValue());
                        break;
                    }
                    case (SCH_TEXT): {
                        getElementAsSpecial<std::string>(column, row)->setValue(((Element<std::string>*)other)->getValue());
                        break;
                    }
                    case (SCH_SELECT): {
                        getElementAsSpecial<SingleSelectContainer>(column, row)
                            ->setValue(((Element<SingleSelectContainer>*)other)->getValue());
                        break;
                    }
                    case (SCH_MULTISELECT): {
                        getElementAsSpecial<SelectContainer>(column, row)
                            ->setValue(((Element<SelectContainer>*)other)->getValue());
                        break;
                    }
                    case (SCH_WEEKDAY): {
                        getElementAsSpecial<WeekdayContainer>(column, row)
                            ->setValue(((Element<WeekdayContainer>*)other)->getValue());
                        break;
                    }
                    case (SCH_TIME): {
                        getElementAsSpecial<TimeContainer>(column, row)->setValue(((Element<TimeContainer>*)other)->getValue());
                        break;
                    }
                    case (SCH_DATE): {
                        getElementAsSpecial<DateContainer>(column, row)->setValue(((Element<DateContainer>*)other)->getValue());
                        break;
                    }
                    default: {
                        std::cout << std::format("ScheduleCore::setElement has not been implemented for Element type {}",
                                                 (size_t)other->getType())
                                  << std::endl;
                        return false;
                    }
                }
            }
            // IF the value being assigned is of a different type than the column's (i.e. the column's type was just changed and is being reset), REPLACE the pointer. Otherwise, the program will crash.
            else
            {
                // TODO: clean previous pointer since it's gone now?
                delete getMutableColumn(column)->getElement(row);
                getMutableColumn(column)->rows[row] = other;
            }

            if (resort) {
                sortColumns();
            }
            return true;
        }

        // Shortcut for getting the value of an Element at column; row
        template <typename T>
        T& getElementValue(size_t column, size_t row) {
            const Column* elementColumn = getColumn(column);
            if (elementColumn == nullptr || elementColumn->hasElement(row) == false) {
                throw std::runtime_error(
                    std::format("ScheduleCore::getElementValue could not get element value at {}; {}", column, row));
            }
            return getValue<T>(elementColumn->rows[row]);
        }
        // Shortcut for getting the value of an Element at column; row as a CONST reference
        template <typename T>
        const T& getElementValueConstRef(size_t column, size_t row) const {
            const Column* elementColumn = getColumn(column);
            if (elementColumn == nullptr || elementColumn->hasElement(row) == false) {
                throw std::runtime_error(
                    std::format("ScheduleCore::getElementValueConstRef could not get element value at {}; {}", column, row));
            }
            return getValueConstRef<T>(elementColumn->rows[row]);
        }

        // Shortcut for setting the value of the Element at column; row to value. You must provide the correct type for the Element.
        template <typename T>
        bool setElementValue(size_t column, size_t row, const T& value) {
            ElementBase* element = getElement(column, row);

            if (element == nullptr) {
                std::cout << std::format(
                                 "ScheduleCore::setElementValue failed to set element at {}; {} - element does not exist",
                                 column,
                                 row)
                          << std::endl;
                return false;
            }

            ((Element<T>*)element)->setValue(value);

            ScheduleColumnFlags columnFlags = getColumn(column)->flags;
            if (columnFlags & ScheduleColumnFlags_Start) {
                getElementAsSpecial<TimeContainer>(getFlaggedColumnIndex(ScheduleColumnFlags_End), row)
                    ->setValue(getElementAsSpecial<TimeContainer>(column, row)->getValue() +
                               getElementAsSpecial<TimeContainer>(getFlaggedColumnIndex(ScheduleColumnFlags_Duration), row)
                                   ->getValue());
            } else if (columnFlags & ScheduleColumnFlags_Duration) {
                getElementAsSpecial<TimeContainer>(getFlaggedColumnIndex(ScheduleColumnFlags_End), row)
                    ->setValue(
                        getElementAsSpecial<TimeContainer>(getFlaggedColumnIndex(ScheduleColumnFlags_Start), row)->getValue() +
                        getElementAsSpecial<TimeContainer>(column, row)->getValue());
            } else if (columnFlags & ScheduleColumnFlags_End) {
                getElementAsSpecial<TimeContainer>(getFlaggedColumnIndex(ScheduleColumnFlags_Duration), row)
                    ->setValue(
                        getElementAsSpecial<TimeContainer>(column, row)->getValue() -
                        getElementAsSpecial<TimeContainer>(getFlaggedColumnIndex(ScheduleColumnFlags_Start), row)->getValue());
            }

            sortColumns();
            return true;
        }
};