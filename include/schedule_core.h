#pragma once

#include <vector>
#include <string>
#include "filter.h"
#include "schedule_column.h"
#include "schedule_constants.h"
#include "element.h"
#include "select_options.h"

const size_t ELEMENT_TEXT_MAX_LENGTH = 1024;
const size_t SELECT_OPTION_NAME_MAX_LENGTH = 20;

class ScheduleCore
{
    private:
        std::vector<Column> m_schedule = {};
        ColumnSortComparison m_columnSortComparison;
        std::vector<size_t> m_sortedRowIndices = {};
        size_t getFlaggedColumnIndex(ScheduleColumnFlags flags) const;
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
        void addDefaultColumn(size_t index);
        bool removeColumn(size_t column);
        // Get a constant pointer to the Column at the index.
        const Column* getColumn(size_t column) const;
        // Set the values of every Element to be copies of the provided Column's Element values. NOTE: If the provided Column has fewer rows, only those will be modified. If it has more rows, ones past the end will be ignored.
        void setColumnElements(size_t index, const Column& columnData);
        bool setColumnType(size_t column, SCHEDULE_TYPE type);
        bool setColumnName(size_t column, const std::string& name);
        bool setColumnSort(size_t column, COLUMN_SORT sortDirection);
        const SelectOptions& getColumnSelectOptions(size_t column) const;
        // NOTE: For OPTION_MODIFICATION_ADD the first string in optionName is used as the name.
        bool modifyColumnSelectOptions(size_t column, const SelectOptionsModification& selectOptionsModification);
        template <typename T>
        bool addColumnFilter(size_t column, const Filter<T>& filter)
        {
            if (existsColumnAtIndex(column) == false) { return false; }

            getMutableColumn(column)->addFilter(getColumn(column)->type, filter);
            return true;
        }
        template <typename T>
        bool replaceColumnFilter(size_t column, size_t index, const Filter<T>& filter)
        {
            if (existsColumnAtIndex(column) == false) { return false; }
            
            getMutableColumn(column)->replaceFilter(index, filter);
            return true;
        }
        bool removeColumnFilter(size_t column, size_t index);
        // NOTE: Does NOT resort on its own. Sets every Element in the Column index to a default value of the given type. Do NOT change the column's type before running this. The Column type should only be changed after every row of it IS that type.
        void resetColumn(size_t index, SCHEDULE_TYPE type);

        // ROWS
        size_t getRowCount() const;
        bool existsRowAtIndex(size_t index) const;

        void addRow(size_t index);
        bool removeRow(size_t index);
        // Get all elements of a row. If the row doesn't exist, an empty vector is returned.
        std::vector<ElementBase*> getRow(size_t index);
        // Set all elements of a row. NOTE: The element data must be in the correct order. If the row doesn't exist, nothing happens.
        bool setRow(size_t index, std::vector<ElementBase*> elementData);
        std::vector<size_t> getSortedRowIndices() const;

        // ELEMENTS.
        // Get the value of the element as Element<T>. NOTE: You MUST provide the correct type.
        template <typename T>
        T& getValue(ElementBase* element)
        {
            return ((Element<T>*)element)->getValueReference();
        }
        // Get the value of the element as Element<T> as a CONST ref. NOTE: You MUST provide the correct type.
        template <typename T>
        const T& getValueConstRef(const ElementBase* element) const
        {
            return ((Element<T>*)element)->getConstValueReference();
        }

        // Get a pointer to the ElementBase at column; row
        ElementBase* getElement(size_t column, size_t row)
        {
            Column* mutableColumn = getMutableColumn(column);

            if (mutableColumn == nullptr || mutableColumn->hasElement(row) == false)
            {
                printf("ScheduleCore::getElement could not get element at %zu; %zu\n", column, row);
                return nullptr;
            }

            return mutableColumn->getElement(row);
        }
        // Get a pointer to the ElementBase at column; row
        const ElementBase* getElementConst(size_t column, size_t row) const
        {
            const Column* col = getColumn(column);

            if (col == nullptr || col->hasElement(row) == false)
            {
                printf("ScheduleCore::getElementConst could not get element at %zu; %zu\n", column, row);
                return nullptr;
            }

            return col->getElementConst(row);
        }

        // Simple function that gets an ElementBase* at column; row and casts it to Element<T>*. In the future, this might check that the returned type is actually correct.
        template <typename T>
        Element<T>* getElementAsSpecial(size_t column, size_t row)
        {
            ElementBase* element = getElement(column, row);

            if (element == nullptr)
            {
                printf("ScheduleCore::getElementAsSpecial could not get element at %zu; %zu\n", column, row);
                return nullptr;
            }

            return (Element<T>*)element;
        }

        // Use this function to completely replace the element at column; row with the ElementBase in value.
        // NOTE: If the types match, a copy is performed.
        // If the types do not match, the target element pointer is replaced by the value pointer!
        // NOTE: Currently, does not add to the edit history
        bool setElement(size_t column, size_t row, ElementBase* other, bool resort = true)
        {
            if (getElement(column, row) == nullptr)
            {
                printf("ScheduleCore::setElement failed to set element at %zu; %zu - element does not exist\n", column, row);
                return false;
            }

            // IF the provided Element fits the column's type, set the target Element's value directly
            if (getColumn(column)->type == other->getType())
            {
                switch(other->getType())
                {
                    case(SCH_BOOL):
                    {
                        getElementAsSpecial<bool>(column, row)->setValue(((Element<bool>*)other)->getValue());
                        break;
                    }
                    case(SCH_NUMBER):
                    {
                        getElementAsSpecial<int>(column, row)->setValue(((Element<int>*)other)->getValue());
                        break;
                    }
                    case(SCH_DECIMAL):
                    {
                        getElementAsSpecial<double>(column, row)->setValue(((Element<double>*)other)->getValue());
                        break;
                    }
                    case(SCH_TEXT):
                    {
                        getElementAsSpecial<std::string>(column, row)->setValue(((Element<std::string>*)other)->getValue());
                        break;
                    }
                    case(SCH_SELECT):
                    {
                        getElementAsSpecial<SelectContainer>(column, row)->setValue(((Element<SelectContainer>*)other)->getValue());
                        break;
                    }
                    case(SCH_WEEKDAY):
                    {
                        getElementAsSpecial<WeekdayContainer>(column, row)->setValue(((Element<WeekdayContainer>*)other)->getValue());
                        break;
                    }
                    case(SCH_TIME):
                    {
                        getElementAsSpecial<TimeContainer>(column, row)->setValue(((Element<TimeContainer>*)other)->getValue());
                        break;
                    }
                    case(SCH_DATE):
                    {
                        getElementAsSpecial<DateContainer>(column, row)->setValue(((Element<DateContainer>*)other)->getValue());
                        break;
                    }
                    default:
                    {
                        std::cout << "ScheduleCore::setElement has not been implemented for Element type " << other->getType() << std::endl;
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

            if (resort)
            {
                sortColumns();
            }
            return true;
        }

        // Shortcut for getting the value of an Element at column; row
        template <typename T>
        T& getElementValue(size_t column, size_t row)
        {
            const Column* elementColumn = getColumn(column);
            if (elementColumn == nullptr || elementColumn->hasElement(row) == false)
            {
                printf("ScheduleCore::getElementValue could not return value at %zu; %zu\n", column, row);
                return *(new T()); // memory leak ON PURPOSE
            }
            return getValue<T>(elementColumn->rows[row]);
        }
        // Shortcut for getting the value of an Element at column; row as a CONST reference
        template <typename T>
        const T& getElementValueConstRef(size_t column, size_t row) const
        {
            const Column* elementColumn = getColumn(column);
            if (elementColumn == nullptr || elementColumn->hasElement(row) == false)
            {
                printf("ScheduleCore::getElementValueConstRef could not return value at %zu; %zu\n", column, row);
                return *(new T()); // memory leak ON PURPOSE
            }
            return getValueConstRef<T>(elementColumn->rows[row]);
        }

        // Shortcut for setting the value of the Element at column; row to value. You must provide the correct type for the Element.
        template <typename T>
        bool setElementValue(size_t column, size_t row, const T& value, bool resort = true)
        {
            ElementBase* element = getElement(column, row);

            if (element == nullptr)
            {
                printf("ScheduleCore::setElementValue failed to set element at %zu; %zu - element does not exist\n", column, row);
                return false;
            }

            ((Element<T>*)element)->setValue(value);

            ScheduleColumnFlags columnFlags = getColumn(column)->flags;
            if (columnFlags & ScheduleColumnFlags_Start)
            {
                getElementAsSpecial<TimeContainer>(getFlaggedColumnIndex(ScheduleColumnFlags_End), row)->setValue(
                    getElementAsSpecial<TimeContainer>(column, row)->getValue() 
                    + getElementAsSpecial<TimeContainer>(getFlaggedColumnIndex(ScheduleColumnFlags_Duration), row)->getValue());
            }
            else if (columnFlags & ScheduleColumnFlags_Duration)
            {
                getElementAsSpecial<TimeContainer>(getFlaggedColumnIndex(ScheduleColumnFlags_End), row)->setValue(
                    getElementAsSpecial<TimeContainer>(getFlaggedColumnIndex(ScheduleColumnFlags_Start), row)->getValue() 
                    + getElementAsSpecial<TimeContainer>(column, row)->getValue());
            }
            else if (columnFlags & ScheduleColumnFlags_End)
            {
                getElementAsSpecial<TimeContainer>(getFlaggedColumnIndex(ScheduleColumnFlags_Duration), row)->setValue(
                    getElementAsSpecial<TimeContainer>(column, row)->getValue()
                    - getElementAsSpecial<TimeContainer>(getFlaggedColumnIndex(ScheduleColumnFlags_Start), row)->getValue());
            }

            if (resort)
            {
                sortColumns();
            }
            return true;
        }
};