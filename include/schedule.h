#pragma once
#include "element_base.h"
#include <algorithm>
#include <vector>
#include <map>
#include <cstddef>
#include <string>
#include <element.h>

// TEMP
#include <iostream>

const size_t ELEMENT_TEXT_MAX_LENGTH = 1024;
const size_t COLUMN_NAME_MAX_LENGTH = 64;
const size_t SELECT_OPTION_NAME_MAX_LENGTH = 20;
const size_t SELECT_OPTION_COUNT_MAX = 20;
const size_t SCHEDULE_NAME_MAX_LENGTH = 48;

typedef int ScheduleElementFlags;

enum ScheduleElementFlags_
{
    ScheduleElementFlags_None     = 0,
    ScheduleElementFlags_Name     = 1 << 0,
    ScheduleElementFlags_Finished = 1 << 1,
    ScheduleElementFlags_Start    = 1 << 2,
    ScheduleElementFlags_Duration = 1 << 3,
    ScheduleElementFlags_End      = 1 << 4,
};

typedef int COLUMN_SORT;

enum COLUMN_SORT_
{
    COLUMN_SORT_NONE,
    COLUMN_SORT_ASCENDING,
    COLUMN_SORT_DESCENDING
};

struct Column
{
    std::vector<ElementBase*> rows;
    SCHEDULE_TYPE type;
    std::string name;
    bool permanent;
    ScheduleElementFlags flags;
    COLUMN_SORT sort;
    bool sorted;
    SelectOptions selectOptions;

    ElementBase* getElement(size_t index)
    {
        return rows[index];
    }
};

struct ColumnSortComparison 
{
    SCHEDULE_TYPE type;
    COLUMN_SORT sortDirection;

    bool operator () (const ElementBase* const left, const ElementBase* const right)
    {
        // TODO!
        return true;

        // switch(type)
        // {
        //     case(SCH_BOOL):
        //     {
        //         return sortDirection == COLUMN_SORT_DESCENDING ? *(const Bool*)left > *(const Bool*)right : *(const Bool*)left < *(const Bool*)right;
        //     }
        //     case(SCH_NUMBER):
        //     {
        //         return sortDirection == COLUMN_SORT_DESCENDING ? *(const Number*)left > *(const Number*)right : *(const Number*)left < *(const Number*)right;
        //     }
        //     case(SCH_DECIMAL):
        //     {
        //         return sortDirection == COLUMN_SORT_DESCENDING ? *(const Decimal*)left > *(const Decimal*)right : *(const Decimal*)left < *(const Decimal*)right;
        //     }
        //     case(SCH_TEXT):
        //     {
        //         return sortDirection == COLUMN_SORT_DESCENDING ? *(const Text*)left > *(const Text*)right : *(const Text*)left < *(const Text*)right;
        //     }
        //     case(SCH_SELECT):
        //     {
        //         return sortDirection == COLUMN_SORT_DESCENDING ? *(const Select*)left > *(const Select*)right : *(const Select*)left < *(const Select*)right;
        //     }
        //     case(SCH_TIME):
        //     {
        //         return sortDirection == COLUMN_SORT_DESCENDING ? *(const Time*)left > *(const Time*)right : *(const Time*)left < *(const Time*)right;
        //     }
        //     case(SCH_DATE):
        //     {
        //         return sortDirection == COLUMN_SORT_DESCENDING ? *(const Date*)left > *(const Date*)right : *(const Date*)left < *(const Date*)right;
        //     }
        // }
    }

    // Setup the sort comparison information before using it
    void setup(SCHEDULE_TYPE _type, COLUMN_SORT _sortDirection)
    {
        type = _type;
        sortDirection = _sortDirection;
    }
};

class Schedule
{
    private:
        std::vector<Column> m_schedule = {};
        ColumnSortComparison m_columnSortComparison;
        bool m_editedSinceWrite = false;
        std::string m_scheduleName;
        size_t getFlaggedColumnIndex(ScheduleElementFlags flags);
        Column* getColumnWithFlags(ScheduleElementFlags flags);
        Column* getMutableColumn(size_t column);
        std::vector<size_t> getColumnSortedNewIndices(size_t index);

    public:
        const std::map<SCHEDULE_TYPE, const char*> scheduleTypeNames =
        {
            {SCH_BOOL, "Checkbox"},
            {SCH_NUMBER, "Number"},
            {SCH_DECIMAL, "Decimal"},
            {SCH_TEXT, "Text"},
            {SCH_SELECT, "Select"},
            {SCH_WEEKDAY, "Select Weekday"},
            {SCH_TIME, "Time"},
            {SCH_DATE, "Date"},
        };

        // WHOLE-SCHEDULE FUNCTIONS

        // Clear the current Schedule and replace it with an empty Schedule with default Columns.
        void createDefaultSchedule();
        // Set the schedule's name to the provided name. NOTE: Does not affect filename. Only called by IO_Manager and MainMenuBarGui through IO_Manager.
        void setScheduleName(const std::string& name);
        std::string getScheduleName();

        bool getEditedSinceWrite();
        void setEditedSinceWrite(bool to);

        // Clears the Schedule and deletes all the Columns.
        void clearSchedule();
        // Replaces the m_schedule vector of Columns with the provided. NOTE: ALSO DELETES ALL PREVIOUS ELEMENTS
        void replaceSchedule(std::vector<Column> columns);

        // Get a constant pointer to the Column at the index. TODO: Return const ref instead
        const Column* getColumn(size_t column);
        // Get a constant reference to every Column in the Schedule (m_schedule)
        const std::vector<Column>& getColumns();
        // DO NOT USE UNLESS MEANING TO OVERWRITE THE ENTIRE SCHEDULE!
        std::vector<Column>& getMutableColumns();
        SelectOptions& getColumnSelectOptions(size_t column);
        void setColumnType(size_t column, SCHEDULE_TYPE type);
        void setColumnName(size_t column, const char* name);
        void setColumnSort(size_t column, COLUMN_SORT sortDirection);

        size_t getColumnCount();
        size_t getRowCount();

        void sortColumns();
        void updateColumnSelects(size_t index);

        // Sets every Element in the Column index to a default value of the given type. Do NOT change the column's type before running this. The Column type should only be changed after every row of it IS that type.
        void resetColumn(size_t index, SCHEDULE_TYPE type);
        void addRow(size_t index);
        void removeRow(size_t index);
        void addDefaultColumn(size_t index);
        void addColumn(size_t index, const Column& column);
        void removeColumn(size_t column);

        template <typename T>
        T getValue(ElementBase* element)
        {
            switch(element->getType())
            {
                case(SCH_BOOL):
                {
                    return (T)((Element<bool>*)element)->getValue();
                }
                case(SCH_NUMBER):
                {
                    return (T)((Element<int>*)element)->getValue();
                }
                case(SCH_DECIMAL):
                {
                    return (T)((Element<double>*)element)->getValue();
                }
                case(SCH_TEXT):
                {
                    return (T)((Element<std::string>*)element)->getValue();
                }
                case(SCH_SELECT):
                {
                    return (T)((Element<SelectContainer>*)element)->getValue();
                }
                case(SCH_TIME):
                {
                    return (T)((Element<TimeContainer>*)element)->getValue();
                }
                case(SCH_DATE):
                {
                    return (T)((Element<DateContainer>*)element)->getValue();
                }
            }
        }

        // Get a pointer to the ElementBase at column; row
        ElementBase* getElement(size_t column, size_t row)
        {
            return getMutableColumn(column)->getElement(row);
        }

        // Simple function that gets an ElementBase* at column; row and casts it to Element<T>*. In the future, this might check that the returned type is actually correct.
        template <typename T>
        Element<T>* getElementAsSpecial(size_t column, size_t row)
        {
            return (Element<T>*)getElement(column, row);
        }

        // Use this function to completely replace the element at column; row with the ElementBase in value
        // NOTE: If the types match, a copy is performed
        // If the types do not match, the target element pointer is replaced by the value pointer!
        void setElement(size_t column, size_t row, const ElementBase* other, bool resort = true)
        {            
            // IF the provided Element fits the column's type, set the target Element's value directly
            if (getColumn(column)->type == other->getType())
            {
                switch(other->getType())
                {
                    case(SCH_BOOL):
                    {
                        getElementAsSpecial<bool>(column, row)->setValue(((Element<bool>*)other)->getValue());
                    }
                    case(SCH_NUMBER):
                    {
                        getElementAsSpecial<int>(column, row)->setValue(((Element<int>*)other)->getValue());
                    }
                    case(SCH_DECIMAL):
                    {
                        getElementAsSpecial<double>(column, row)->setValue(((Element<double>*)other)->getValue());
                    }
                    case(SCH_TEXT):
                    {
                        getElementAsSpecial<std::string>(column, row)->setValue(((Element<std::string>*)other)->getValue());
                    }
                    case(SCH_SELECT):
                    {
                        getElementAsSpecial<SelectContainer>(column, row)->setValue(((Element<SelectContainer>*)other)->getValue());
                    }
                    case(SCH_TIME):
                    {
                        getElementAsSpecial<TimeContainer>(column, row)->setValue(((Element<TimeContainer>*)other)->getValue());
                    }
                    case(SCH_DATE):
                    {
                        getElementAsSpecial<DateContainer>(column, row)->setValue(((Element<DateContainer>*)other)->getValue());
                    }
                }
            }
            // IF the value being assigned is of a different type than the column's (i.e. the column's type was just changed and is being reset), REPLACE the pointer. Otherwise, the program will crash.
            else
            {
                // TODO: clean previous pointer since it's gone now?
                getMutableColumn(column)->rows[row] = other;
            }
            ScheduleElementFlags columnFlags = getColumn(column)->flags;
            if (columnFlags & ScheduleElementFlags_Start)
            {
                getElementAsSpecial<TimeContainer>(getFlaggedColumnIndex(ScheduleElementFlags_End), row)->setValue(
                    getElementAsSpecial<TimeContainer>(column, row)->getValue() 
                    + getElementAsSpecial<TimeContainer>(getFlaggedColumnIndex(ScheduleElementFlags_Duration), row)->getValue());
            }
            else if (columnFlags & ScheduleElementFlags_Duration)
            {
                getElementAsSpecial<TimeContainer>(getFlaggedColumnIndex(ScheduleElementFlags_End), row)->setValue(
                    getElementAsSpecial<TimeContainer>(getFlaggedColumnIndex(ScheduleElementFlags_Start), row)->getValue() 
                    + getElementAsSpecial<TimeContainer>(column, row)->getValue());
            }
            else if (columnFlags & ScheduleElementFlags_End)
            {
                getElementAsSpecial<TimeContainer>(getFlaggedColumnIndex(ScheduleElementFlags_Duration), row)->setValue(
                    getElementAsSpecial<TimeContainer>(column, row)->getValue()
                    - getElementAsSpecial<TimeContainer>(getFlaggedColumnIndex(ScheduleElementFlags_Start), row)->getValue());
            }

            m_schedule[column].sorted = false;
            if (resort)
            {
                sortColumns();
            }

            setEditedSinceWrite(true);
        }

        // Shortcut for getting the value of an Element at column; row
        template <typename T>
        T getElementValue(size_t column, size_t row)
        {
            return getValue<T>(getColumn(column)->rows[row]);
        }

        // Shortcut for setting the value of the Element at column; row to value. You must provide the correct type for the Element.
        template <typename T>
        void setElementValue(size_t column, size_t row, const T& value)
        {
            ElementBase* element = getElement(column, row);

            switch(element->getType())
            {
                case(SCH_BOOL):
                {
                    ((Element<bool>*)element)->setValue(value);
                }
                case(SCH_NUMBER):
                {
                    ((Element<int>*)element)->setValue(value);
                }
                case(SCH_DECIMAL):
                {
                    ((Element<double>*)element)->setValue(value);
                }
                case(SCH_TEXT):
                {
                    ((Element<std::string>*)element)->setValue(value);
                }
                case(SCH_SELECT):
                {
                    ((Element<SelectContainer>*)element)->setValue(value);
                }
                case(SCH_TIME):
                {
                    ((Element<TimeContainer>*)element)->setValue(value);
                }
                case(SCH_DATE):
                {
                    ((Element<DateContainer>*)element)->setValue(value);
                }
            }
        }
};