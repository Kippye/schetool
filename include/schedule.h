#pragma once
#include <time_container.h>
#include <algorithm>
#include <vector>
#include <any>
#include <map>
#include <string>

// TEMP
#include <iostream>

const unsigned int ELEMENT_TEXT_MAX_LENGTH = 1024;
const unsigned int COLUMN_NAME_MAX_LENGTH = 64;

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
    SCH_TIME,
    SCH_LAST,
    // TODO: Enum? how would that work?
};

struct Column
{
    std::vector<std::any> rows;
    SCHEDULE_TYPE type;
    std::string name;
    bool permanent;
    bool displayDate;
    bool displayTime;
    bool displayWeekday;
    ScheduleElementFlags flags;
};

class Schedule
{
    /* IDEA how i could make select or multiple select ones:
    // a specific class that contains -> a vector of strings (and colours)
    // then i can just use it as a type in std::any
    // and later it won't matter what enum it is since they're all the same class */

    private:
        std::vector<Column> m_schedule = {};
        Column* getColumnWithFlags(ScheduleElementFlags flags);

    public:
        const std::map<SCHEDULE_TYPE, const char*> scheduleTypeNames =
        {
            {SCH_BOOL, "Bool"},
            {SCH_INT, "Integer"},
            {SCH_DOUBLE, "Decimal"},
            {SCH_TEXT, "Text"},
            {SCH_TIME, "Time"},
        };
        // TEMP
        void test_setup();

        const Column* getColumn(unsigned int column);
        void setColumnType(unsigned int column, SCHEDULE_TYPE type);
        void setColumnName(unsigned int column, const char* name);
        void setColumnDisplayDate(unsigned int column, bool displayDate);
        void setColumnDisplayTime(unsigned int column, bool displayTime);
        void setColumnDisplayWeekday(unsigned int column, bool displayWeekday);

        void resetColumn(unsigned int column);

        unsigned int getColumnCount();
        unsigned int getRowCount();

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
        void setElement(unsigned int column, unsigned int row, T value)
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
        }
};