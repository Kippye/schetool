#pragma once
#include <algorithm>
#include <vector>
#include <any>
#include <map>
#include <cstddef>
#include <string>
#include <element.h>
#include <bool_container.h>
#include <number_container.h>
#include <decimal_container.h>
#include <text_container.h>
#include <select_container.h>
#include <time_container.h>
#include <date_container.h>

// TEMP
#include <iostream>

const size_t ELEMENT_TEXT_MAX_LENGTH = 1024;
const size_t COLUMN_NAME_MAX_LENGTH = 64;
const size_t SELECT_OPTION_NAME_MAX_LENGTH = 16;

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
    std::vector<Element*> rows;
    SCHEDULE_TYPE type;
    std::string name;
    bool permanent;
    ScheduleElementFlags flags;
    COLUMN_SORT sort;
    bool sorted;
    SelectOptions selectOptions;

    template<typename T>
    T* getElement(size_t index)
    {
        return (T*)rows[index];
    }
};

struct ColumnSortComparison 
{
    SCHEDULE_TYPE type;
    COLUMN_SORT sortDirection;

    bool operator () (const Element* const left, const Element* const right)
    {
        switch(type)
        {
            case(SCH_BOOL):
            {
                return sortDirection == COLUMN_SORT_DESCENDING ? *(const Bool*)left > *(const Bool*)right : *(const Bool*)left < *(const Bool*)right;
            }
            case(SCH_NUMBER):
            {
                return sortDirection == COLUMN_SORT_DESCENDING ? *(const Number*)left > *(const Number*)right : *(const Number*)left < *(const Number*)right;
            }
            case(SCH_DECIMAL):
            {
                return sortDirection == COLUMN_SORT_DESCENDING ? *(const Decimal*)left > *(const Decimal*)right : *(const Decimal*)left < *(const Decimal*)right;
            }
            case(SCH_TEXT):
            {
                return sortDirection == COLUMN_SORT_DESCENDING ? *(const Text*)left > *(const Text*)right : *(const Text*)left < *(const Text*)right;
            }
            case(SCH_SELECT):
            {
                return sortDirection == COLUMN_SORT_DESCENDING ? *(const Select*)left > *(const Select*)right : *(const Select*)left < *(const Select*)right;
            }
            case(SCH_TIME):
            {
                return sortDirection == COLUMN_SORT_DESCENDING ? *(const Time*)left > *(const Time*)right : *(const Time*)left < *(const Time*)right;
            }
            case(SCH_DATE):
            {
                return sortDirection == COLUMN_SORT_DESCENDING ? *(const Date*)left > *(const Date*)right : *(const Date*)left < *(const Date*)right;
            }
        }
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
        // TEMP
        void test_setup();

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

        // TODO: make this return a (const?) pointer instead. too much copying!
        template <typename T, typename = std::enable_if<std::is_base_of<Element, T>::value>>
        T getElement(size_t column, size_t row)
        {
            return *getMutableColumn(column)->getElement<T>(row);
        }
        template <typename T, typename = std::enable_if<std::is_base_of<Element, T>::value>>
        void setElement(size_t column, size_t row, T* value, bool resort = true)
        {
            // IF the value being assigned fits the column's type, set the Element's value directly
            if (getColumn(column)->type == ((Element*)value)->getType())
            {
                std::cout << "assigned " << scheduleTypeNames.at(((Element*)value)->getType()) << std::endl;
                *getMutableColumn(column)->getElement<T>(row) = *value;
            }
            // IF the value being assigned is of a different type than the column's (i.e. the column's type was just changed and is being reset), REPLACE the pointer. Otherwise, the program will crash.
            else
            {
                std::cout << "replaced with " << scheduleTypeNames.at(((Element*)value)->getType()) << std::endl;
                getMutableColumn(column)->rows[row] = (Element*)value;
                std::cout << scheduleTypeNames.at(getMutableColumn(column)->getElement<Element>(row)->getType()) << std::endl;
            }
            ScheduleElementFlags columnFlags = getColumn(column)->flags;
            if (columnFlags & ScheduleElementFlags_Start)
            {
                *getColumnWithFlags(ScheduleElementFlags_End)->getElement<Time>(row) = (*getMutableColumn(column)->getElement<Time>(row) + *getColumnWithFlags(ScheduleElementFlags_Duration)->getElement<Time>(row));
            }
            else if (columnFlags & ScheduleElementFlags_Duration)
            {
                *getColumnWithFlags(ScheduleElementFlags_End)->getElement<Time>(row) = (*getColumnWithFlags(ScheduleElementFlags_Start)->getElement<Time>(row) + *getMutableColumn(column)->getElement<Time>(row));
            }
            else if (columnFlags & ScheduleElementFlags_End)
            {
                *getColumnWithFlags(ScheduleElementFlags_Duration)->getElement<Time>(row) = (*getMutableColumn(column)->getElement<Time>(row) - *getColumnWithFlags(ScheduleElementFlags_Start)->getElement<Time>(row));
            }

            m_schedule[column].sorted = false;
            if (resort)
            {
                sortColumns();
            }
        }
};