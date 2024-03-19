#pragma once
#include "select_container.h"
#include <time_container.h>
#include <date_container.h>
#include <weekday_container.h>
#include <algorithm>
#include <vector>
#include <any>
#include <map>
#include <cstddef>
#include <string>

// TEMP
#include <iostream>

const unsigned int ELEMENT_TEXT_MAX_LENGTH = 1024;
const unsigned int COLUMN_NAME_MAX_LENGTH = 64;
const unsigned int SELECT_OPTION_NAME_MAX_LENGTH = 16;

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

enum SCHEDULE_TYPE
{
    SCH_BOOL,
    SCH_INT,
    SCH_DOUBLE,
    SCH_TEXT,
    SCH_SELECT,
    SCH_TIME,
    SCH_DATE,
    SCH_WEEKDAY,
    SCH_LAST,
    // TODO: Enum? how would that work?
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
    std::vector<std::any> rows;
    SCHEDULE_TYPE type;
    std::string name;
    bool permanent;
    ScheduleElementFlags flags;
    COLUMN_SORT sort;
    bool sorted;
    SelectOptions selectOptions;
};

struct ColumnSortComparison 
{
    SCHEDULE_TYPE type;
    COLUMN_SORT sortDirection;

    bool operator () (const std::any& left, const std::any& right)
    {
        switch(type)
        {
            case(SCH_BOOL):
            {
                return sortDirection == COLUMN_SORT_DESCENDING ? std::any_cast<std::byte>(left) > std::any_cast<std::byte>(right) : std::any_cast<std::byte>(left) < std::any_cast<std::byte>(right);
            }
            case(SCH_INT):
            {
                return sortDirection == COLUMN_SORT_DESCENDING ? std::any_cast<int>(left) > std::any_cast<int>(right) : std::any_cast<int>(left) < std::any_cast<int>(right);
            }
            case(SCH_DOUBLE):
            {
                return sortDirection == COLUMN_SORT_DESCENDING ? std::any_cast<double>(left) > std::any_cast<double>(right) : std::any_cast<double>(left) < std::any_cast<double>(right);
            }
            case(SCH_TEXT):
            {
                return sortDirection == COLUMN_SORT_DESCENDING ? std::any_cast<std::string>(left) > std::any_cast<std::string>(right) : std::any_cast<std::string>(left) < std::any_cast<std::string>(right);
            }
            case(SCH_SELECT):
            {
                return sortDirection == COLUMN_SORT_DESCENDING ? std::any_cast<Select>(left) > std::any_cast<Select>(right) : std::any_cast<Select>(left) < std::any_cast<Select>(right);
            }
            case(SCH_TIME):
            {
                return sortDirection == COLUMN_SORT_DESCENDING ? std::any_cast<Time>(left) > std::any_cast<Time>(right) : std::any_cast<Time>(left) < std::any_cast<Time>(right);
            }
            case(SCH_DATE):
            {
                return sortDirection == COLUMN_SORT_DESCENDING ? std::any_cast<Date>(left) > std::any_cast<Date>(right) : std::any_cast<Date>(left) < std::any_cast<Date>(right);
            }
            case(SCH_WEEKDAY):
            {
                return sortDirection == COLUMN_SORT_DESCENDING ? std::any_cast<Weekday>(left) > std::any_cast<Weekday>(right) : std::any_cast<Weekday>(left) < std::any_cast<Weekday>(right);
            }
        }
    }

    // Setup the sort comparison information before using it
    void setup(SCHEDULE_TYPE _type, COLUMN_SORT _sortDirection, bool _sortByClockTime = false)
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
        std::vector<size_t> getColumnSortedNewIndices(unsigned int index);

    public:
        const std::map<SCHEDULE_TYPE, const char*> scheduleTypeNames =
        {
            {SCH_BOOL, "Bool"},
            {SCH_INT, "Integer"},
            {SCH_DOUBLE, "Decimal"},
            {SCH_TEXT, "Text"},
            {SCH_SELECT, "Select"},
            {SCH_TIME, "Time"},
            {SCH_DATE, "Date"},
            {SCH_WEEKDAY, "Weekday"},
        };
        // TEMP
        void test_setup();

        const Column* getColumn(unsigned int column);
        SelectOptions& getColumnSelectOptions(unsigned int column);
        void setColumnType(unsigned int column, SCHEDULE_TYPE type);
        void setColumnName(unsigned int column, const char* name);
        void setColumnSort(unsigned int column, COLUMN_SORT sortDirection);

        void resetColumn(unsigned int column);

        unsigned int getColumnCount();
        unsigned int getRowCount();

        void sortColumns();
        void updateColumnSelects(unsigned int index);

        void addRow(unsigned int index);
        void removeRow(unsigned int index);
        void addDefaultColumn(unsigned int index);
        void addColumn(unsigned int index, Column& column);
        void removeColumn(unsigned int column);

        // TODO: make this return a (const?) reference instead. too much copying!
        template <typename T>
        T getElement(unsigned int column, unsigned int row)
        {
            return std::any_cast<T>(m_schedule[column].rows[row]);
        }
        template <typename T>
        void setElement(unsigned int column, unsigned int row, T value, bool resort = true)
        {
            m_schedule[column].rows[row] = value;
            ScheduleElementFlags columnFlags = getColumn(column)->flags;
            if (columnFlags & ScheduleElementFlags_Start)
            {
                getColumnWithFlags(ScheduleElementFlags_End)->rows[row] = std::any_cast<Time>(getColumn(column)->rows[row]) + std::any_cast<Time>(getColumnWithFlags(ScheduleElementFlags_Duration)->rows[row]);
            }
            else if (columnFlags & ScheduleElementFlags_Duration)
            {
                getColumnWithFlags(ScheduleElementFlags_End)->rows[row] = std::any_cast<Time>(getColumnWithFlags(ScheduleElementFlags_Start)->rows[row]) + std::any_cast<Time>(getColumn(column)->rows[row]);
            }
            else if (columnFlags & ScheduleElementFlags_End)
            {
                getColumnWithFlags(ScheduleElementFlags_Duration)->rows[row] = std::any_cast<Time>(getColumn(column)->rows[row]) - std::any_cast<Time>(getColumnWithFlags(ScheduleElementFlags_Start)->rows[row]);
            }

            m_schedule[column].sorted = false;
            if (resort)
            {
                sortColumns();
            }
        }
};